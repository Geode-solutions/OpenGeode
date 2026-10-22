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

#include <geode/model/helpers/external_time_series_helpers.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/mapping.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

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
    // Point set vertex matched with each element of each Block, NO_ID if none
    using BlocksMatches =
        absl::flat_hash_map< geode::uuid, std::vector< geode::index_t > >;

    struct BlockPolyhedron
    {
        geode::uuid block_id;
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
            "[ExternalTimeSeriesHelpers::apply_to] No Block element to match "
            "the ",
            point_set_name, " with" );
        const auto tolerance = matching_tolerance( point_set );
        const geode::NNSearch3D search{ std::move( element_points ) };
        for( const auto vertex : geode::Range{ point_set.nb_vertices() } )
        {
            const auto& point = point_set.point( vertex );
            const auto element = search.closest_neighbor( point );
            const auto distance =
                geode::point_point_distance( point, search.point( element ) );
            geode::OpenGeodeModelException::check_exception(
                distance <= tolerance, nullptr,
                geode::OpenGeodeException::TYPE::data,
                "[ExternalTimeSeriesHelpers::apply_to] Point ", vertex,
                " of the ", point_set_name, " [", point.string(),
                "] is at distance ", distance,
                " from the closest Block element, more than the tolerance ",
                tolerance );
            matches[element] = vertex;
        }
        return matches;
    }

    // Matching is done on the model unique vertices so that the colocated
    // vertices of several Blocks get the same values
    std::vector< geode::index_t > match_unique_vertices(
        const geode::BRep& model, const geode::PointSet3D& unique_vertices )
    {
        std::vector< geode::Point3D > points;
        std::vector< geode::index_t > model_unique_vertices;
        std::vector< bool > visited( model.nb_unique_vertices(), false );
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            for( const auto vertex : geode::Range{ mesh.nb_vertices() } )
            {
                const auto unique_vertex =
                    model.unique_vertex( { block.component_id(), vertex } );
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

    BlocksMatches match_block_vertices(
        const geode::BRep& model, const geode::PointSet3D& unique_vertices )
    {
        const auto unique_matches =
            match_unique_vertices( model, unique_vertices );
        BlocksMatches matches;
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            auto& block_matches = matches[block.id()];
            block_matches.resize( mesh.nb_vertices(), geode::NO_ID );
            for( const auto vertex : geode::Range{ mesh.nb_vertices() } )
            {
                const auto unique_vertex =
                    model.unique_vertex( { block.component_id(), vertex } );
                if( unique_vertex != geode::NO_ID )
                {
                    block_matches[vertex] = unique_matches[unique_vertex];
                }
            }
        }
        return matches;
    }

    BlocksMatches match_block_polyhedra(
        const geode::BRep& model, const geode::PointSet3D& cell_barycenters )
    {
        BlocksMatches matches;
        std::vector< geode::Point3D > barycenters;
        std::vector< BlockPolyhedron > polyhedra;
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            matches[block.id()].resize( mesh.nb_polyhedra(), geode::NO_ID );
            for( const auto polyhedron : geode::Range{ mesh.nb_polyhedra() } )
            {
                barycenters.push_back(
                    mesh.polyhedron_barycenter( polyhedron ) );
                polyhedra.push_back( { block.id(), polyhedron } );
            }
        }
        const auto point_matches = match_points(
            cell_barycenters, "cell barycenters", std::move( barycenters ) );
        for( const auto index : geode::Indices{ polyhedra } )
        {
            const auto& block_polyhedron = polyhedra[index];
            matches[block_polyhedron.block_id][block_polyhedron.polyhedron] =
                point_matches[index];
        }
        return matches;
    }

    void apply_matches( const geode::AttributeManager& from,
        absl::Span< const geode::index_t > matches,
        geode::AttributeManager& target )
    {
        geode::GenericMapping< geode::index_t > point2element;
        for( const auto element : geode::Indices{ matches } )
        {
            if( matches[element] != geode::NO_ID )
            {
                point2element.map( matches[element], element );
            }
        }
        if( point2element.size_in() == 0 )
        {
            return;
        }
        delete_replaced_steps( from, target );
        target.import( from, point2element );
    }
} // namespace

namespace geode
{
    class ExternalTimeSeriesHelpers::Impl
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
            for( const auto& block : model.blocks() )
            {
                const auto& mesh = block.mesh();
                apply_matches( unique_vertices_->vertex_attribute_manager(),
                    vertex_matches.at( block.id() ),
                    mesh.vertex_attribute_manager() );
                apply_matches( cell_barycenters_->vertex_attribute_manager(),
                    polyhedron_matches.at( block.id() ),
                    mesh.polyhedron_attribute_manager() );
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
                "[ExternalTimeSeriesHelpers] Null ", name, " point set" );
        }

    private:
        std::unique_ptr< PointSet3D > unique_vertices_;
        std::unique_ptr< PointSet3D > cell_barycenters_;
    };

    ExternalTimeSeriesHelpers::ExternalTimeSeriesHelpers() = default;

    ExternalTimeSeriesHelpers::ExternalTimeSeriesHelpers(
        ExternalTimeSeriesHelpers&& ) noexcept = default;

    ExternalTimeSeriesHelpers& ExternalTimeSeriesHelpers::operator=(
        ExternalTimeSeriesHelpers&& ) noexcept = default;

    ExternalTimeSeriesHelpers::~ExternalTimeSeriesHelpers() = default;

    void ExternalTimeSeriesHelpers::set_unique_vertices(
        std::unique_ptr< PointSet3D > vertices )
    {
        impl_->set_unique_vertices( std::move( vertices ) );
    }

    void ExternalTimeSeriesHelpers::set_cell_barycenters(
        std::unique_ptr< PointSet3D > barycenters )
    {
        impl_->set_cell_barycenters( std::move( barycenters ) );
    }

    void ExternalTimeSeriesHelpers::apply_to( BRep& model ) const
    {
        impl_->apply_to( model );
    }

    template < typename Archive >
    void ExternalTimeSeriesHelpers::serialize( Archive& archive )
    {
        archive.ext( *this, Growable< Archive, ExternalTimeSeriesHelpers >{
                                { []( Archive& local_archive,
                                      ExternalTimeSeriesHelpers& helpers ) {
                                    local_archive.object( helpers.impl_ );
                                } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, ExternalTimeSeriesHelpers );
} // namespace geode
