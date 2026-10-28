/*
 * Copyright (c) 2019 - 2026 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <geode/model/representation/core/external_time_series.hpp>

#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

#include <absl/algorithm/container.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/mapping.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/nn_search.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/vertex_identifier.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace
{
    // One index per element of each Block, Blocks in model.blocks() order
    using BlocksIndices = std::vector< std::vector< geode::index_t > >;

    struct BlockPolyhedron
    {
        geode::index_t block;
        geode::index_t polyhedron;
    };

    void delete_replaced_steps( const geode::AttributeManager& from,
        geode::AttributeManager& target,
        std::string_view name )
    {
        for( const auto& step : from.time_steps( name ) )
        {
            const auto attribute =
                from.find_generic_attribute( step.attribute_id );
            if( !attribute->properties().transferable )
            {
                continue;
            }
            for( const auto& existing_step : target.time_steps( name ) )
            {
                if( existing_step.time == step.time )
                {
                    target.delete_attribute( existing_step.attribute_id );
                }
            }
        }
    }

    void delete_replaced_steps(
        const geode::AttributeManager& from, geode::AttributeManager& target )
    {
        for( const auto& name : from.time_series_names() )
        {
            delete_replaced_steps( from, target, name );
        }
    }

    // Coordinates are rounded relatively to their magnitude
    double matching_tolerance( const geode::PointSet3D& point_set )
    {
        const auto box = point_set.bounding_box();
        double largest_coordinate{ 1 };
        for( const auto& point : { box.min(), box.max() } )
        {
            for( const auto axis : geode::LRange{ 3 } )
            {
                largest_coordinate = std::max(
                    largest_coordinate, std::fabs( point.value( axis ) ) );
            }
        }
        return geode::GLOBAL_EPSILON * largest_coordinate;
    }

    // Each point is matched with the closest element, elements without
    // point (e.g. outside of the simulated domain) are not matched.
    // Matching is rejected when ambiguous: several points matched with the
    // same element, or a point within the tolerance of several elements.
    // Returns the point matched with each element, NO_ID if none.
    std::vector< geode::index_t > match_points(
        const geode::PointSet3D& point_set,
        std::string_view point_set_name,
        std::vector< geode::Point3D > element_points )
    {
        std::vector< geode::index_t > matches(
            element_points.size(), geode::NO_ID );
        if( point_set.nb_vertices() == 0 )
        {
            return matches;
        }
        geode::OpenGeodeModelException::check_exception(
            !element_points.empty(), nullptr,
            geode::OpenGeodeException::TYPE::data,
            "[ExternalTimeSeries::apply_to] No Block element to match the ",
            point_set_name, " with" );
        const auto tolerance = matching_tolerance( point_set );
        const geode::NNSearch3D search{ std::move( element_points ) };
        for( const auto vertex : geode::Range{ point_set.nb_vertices() } )
        {
            const auto& point = point_set.point( vertex );
            const auto closest = search.neighbors( point, 2 );
            const auto element = closest.front();
            const auto distance =
                geode::point_point_distance( point, search.point( element ) );
            geode::OpenGeodeModelException::check_exception(
                distance <= tolerance, nullptr,
                geode::OpenGeodeException::TYPE::data,
                "[ExternalTimeSeries::apply_to] Point ", vertex, " of the ",
                point_set_name, " [", point.string(), "] is at distance ",
                distance,
                " from the closest Block element, more than the tolerance ",
                tolerance );
            geode::OpenGeodeModelException::check_exception(
                closest.size() < 2
                    || geode::point_point_distance(
                           point, search.point( closest[1] ) )
                           > tolerance,
                nullptr, geode::OpenGeodeException::TYPE::data,
                "[ExternalTimeSeries::apply_to] Point ", vertex, " of the ",
                point_set_name, " [", point.string(),
                "] is within the tolerance ", tolerance,
                " of several Block elements" );
            geode::OpenGeodeModelException::check_exception(
                matches[element] == geode::NO_ID, nullptr,
                geode::OpenGeodeException::TYPE::data,
                "[ExternalTimeSeries::apply_to] Points ", matches[element],
                " and ", vertex, " of the ", point_set_name,
                " are matched with the same Block element [",
                search.point( element ).string(), "]" );
            matches[element] = vertex;
        }
        return matches;
    }

    // Unique vertex of each Block vertex, NO_ID if none
    BlocksIndices block_unique_vertices( const geode::BRep& model )
    {
        BlocksIndices unique_vertices;
        unique_vertices.reserve( model.nb_blocks() );
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            auto& block_vertices =
                unique_vertices.emplace_back( mesh.nb_vertices() );
            for( const auto vertex : geode::Range{ mesh.nb_vertices() } )
            {
                block_vertices[vertex] =
                    model.unique_vertex( { block.component_id(), vertex } );
            }
        }
        return unique_vertices;
    }

    // Point matched with each model unique vertex, NO_ID if none
    std::vector< geode::index_t > match_unique_vertices(
        const geode::BRep& model,
        const BlocksIndices& block_unique_vertices,
        const geode::PointSet3D& unique_vertices )
    {
        std::vector< geode::Point3D > points;
        std::vector< geode::index_t > model_unique_vertices;
        std::vector< bool > visited( model.nb_unique_vertices(), false );
        geode::index_t block_index{ 0 };
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            const auto& block_vertices = block_unique_vertices[block_index++];
            for( const auto vertex : geode::Indices{ block_vertices } )
            {
                const auto unique_vertex = block_vertices[vertex];
                if( unique_vertex == geode::NO_ID || visited[unique_vertex] )
                {
                    continue;
                }
                visited[unique_vertex] = true;
                points.push_back( mesh.point( vertex ) );
                model_unique_vertices.push_back( unique_vertex );
            }
        }
        const auto point_matches = match_points(
            unique_vertices, "unique vertices", std::move( points ) );
        std::vector< geode::index_t > matches(
            model.nb_unique_vertices(), geode::NO_ID );
        for( const auto index : geode::Indices{ model_unique_vertices } )
        {
            matches[model_unique_vertices[index]] = point_matches[index];
        }
        return matches;
    }

    // Matching is done on the model unique vertices so that the colocated
    // vertices of several Blocks get the same values.
    // Returns the point matched with each Block vertex, NO_ID if none.
    BlocksIndices match_block_vertices(
        const geode::BRep& model, const geode::PointSet3D& unique_vertices )
    {
        auto matches = block_unique_vertices( model );
        const auto unique_matches =
            match_unique_vertices( model, matches, unique_vertices );
        for( auto& block_matches : matches )
        {
            for( auto& match : block_matches )
            {
                if( match != geode::NO_ID )
                {
                    match = unique_matches[match];
                }
            }
        }
        return matches;
    }

    // Returns the point matched with each Block polyhedron, NO_ID if none
    BlocksIndices match_block_polyhedra(
        const geode::BRep& model, const geode::PointSet3D& cell_barycenters )
    {
        BlocksIndices matches;
        matches.reserve( model.nb_blocks() );
        std::vector< geode::Point3D > barycenters;
        std::vector< BlockPolyhedron > polyhedra;
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            const auto block_index =
                static_cast< geode::index_t >( matches.size() );
            matches.emplace_back( mesh.nb_polyhedra(), geode::NO_ID );
            for( const auto polyhedron : geode::Range{ mesh.nb_polyhedra() } )
            {
                barycenters.push_back(
                    mesh.polyhedron_barycenter( polyhedron ) );
                polyhedra.push_back( { block_index, polyhedron } );
            }
        }
        const auto point_matches = match_points(
            cell_barycenters, "cell barycenters", std::move( barycenters ) );
        for( const auto index : geode::Indices{ polyhedra } )
        {
            const auto& block_polyhedron = polyhedra[index];
            matches[block_polyhedron.block][block_polyhedron.polyhedron] =
                point_matches[index];
        }
        return matches;
    }

    // Element filled by each point, std::nullopt if a point fills several
    // elements
    std::optional< std::vector< geode::index_t > > point_to_element(
        geode::index_t nb_points, absl::Span< const geode::index_t > matches )
    {
        std::vector< geode::index_t > point2element( nb_points, geode::NO_ID );
        for( const auto element : geode::Indices{ matches } )
        {
            const auto point = matches[element];
            if( point == geode::NO_ID )
            {
                continue;
            }
            if( point2element[point] != geode::NO_ID )
            {
                return std::nullopt;
            }
            point2element[point] = element;
        }
        return point2element;
    }

    // A point fills several elements of a Block only when its unique vertex
    // has several vertices in the Block mesh (e.g. mesh cut along an
    // internal Surface): a GenericMapping is then needed
    geode::GenericMapping< geode::index_t > point_to_elements(
        absl::Span< const geode::index_t > matches )
    {
        geode::GenericMapping< geode::index_t > point2elements;
        for( const auto element : geode::Indices{ matches } )
        {
            if( matches[element] != geode::NO_ID )
            {
                point2elements.map( matches[element], element );
            }
        }
        return point2elements;
    }

    // Importing with a point2element array iterates over all the points: it
    // is only worth it when the Block is matched with most of them, the
    // GenericMapping only stores the matched ones
    void apply_matches( const geode::AttributeManager& from,
        absl::Span< const geode::index_t > matches,
        geode::AttributeManager& target )
    {
        const auto nb_matched = static_cast< geode::index_t >(
            absl::c_count_if( matches, []( geode::index_t point ) {
                return point != geode::NO_ID;
            } ) );
        if( nb_matched == 0 )
        {
            return;
        }
        delete_replaced_steps( from, target );
        if( 2 * nb_matched >= from.nb_elements() )
        {
            if( const auto point2element =
                    point_to_element( from.nb_elements(), matches ) )
            {
                target.import( from, point2element.value() );
                return;
            }
        }
        target.import( from, point_to_elements( matches ) );
    }
} // namespace

namespace geode
{
    class ExternalTimeSeries::Impl
    {
        friend class bitsery::Access;

    public:
        Impl()
            : unique_vertices_{ PointSet3D::create() },
              cell_barycenters_{ PointSet3D::create() }
        {
        }

        void set_unique_vertices( std::unique_ptr< PointSet3D > vertices )
        {
            check_point_set( vertices.get(), "unique vertices" );
            unique_vertices_ = std::move( vertices );
        }

        void set_cell_barycenters( std::unique_ptr< PointSet3D > barycenters )
        {
            check_point_set( barycenters.get(), "cell barycenters" );
            cell_barycenters_ = std::move( barycenters );
        }

        void apply_to( const BRep& model ) const
        {
            const auto vertex_matches =
                match_block_vertices( model, *unique_vertices_ );
            const auto polyhedron_matches =
                match_block_polyhedra( model, *cell_barycenters_ );
            index_t block_index{ 0 };
            for( const auto& block : model.blocks() )
            {
                const auto& mesh = block.mesh();
                apply_matches( unique_vertices_->vertex_attribute_manager(),
                    vertex_matches[block_index],
                    mesh.vertex_attribute_manager() );
                apply_matches( cell_barycenters_->vertex_attribute_manager(),
                    polyhedron_matches[block_index],
                    mesh.polyhedron_attribute_manager() );
                block_index++;
            }
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext(
                *this, Growable< Archive, Impl >{
                           { []( Archive& local_archive, Impl& impl ) {
                               local_archive.ext( impl.unique_vertices_,
                                   bitsery::ext::StdSmartPtr{} );
                               local_archive.ext( impl.cell_barycenters_,
                                   bitsery::ext::StdSmartPtr{} );
                           } } } );
        }

        static void check_point_set(
            const PointSet3D* point_set, std::string_view name )
        {
            OpenGeodeModelException::check_exception( point_set != nullptr,
                nullptr, OpenGeodeException::TYPE::data,
                "[ExternalTimeSeries] Null ", name, " point set" );
        }

    private:
        std::unique_ptr< PointSet3D > unique_vertices_;
        std::unique_ptr< PointSet3D > cell_barycenters_;
    };

    ExternalTimeSeries::ExternalTimeSeries() = default;

    ExternalTimeSeries::ExternalTimeSeries(
        ExternalTimeSeries&& ) noexcept = default;

    ExternalTimeSeries& ExternalTimeSeries::operator=(
        ExternalTimeSeries&& ) noexcept = default;

    ExternalTimeSeries::~ExternalTimeSeries() = default;

    void ExternalTimeSeries::set_unique_vertices(
        std::unique_ptr< PointSet3D > vertices )
    {
        impl_->set_unique_vertices( std::move( vertices ) );
    }

    void ExternalTimeSeries::set_cell_barycenters(
        std::unique_ptr< PointSet3D > barycenters )
    {
        impl_->set_cell_barycenters( std::move( barycenters ) );
    }

    void ExternalTimeSeries::apply_to( BRep& model ) const
    {
        impl_->apply_to( model );
    }

    template < typename Archive >
    void ExternalTimeSeries::serialize( Archive& archive )
    {
        archive.ext( *this, Growable< Archive, ExternalTimeSeries >{
                                { []( Archive& local_archive,
                                      ExternalTimeSeries& time_series ) {
                                    local_archive.object( time_series.impl_ );
                                } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, ExternalTimeSeries );
} // namespace geode
