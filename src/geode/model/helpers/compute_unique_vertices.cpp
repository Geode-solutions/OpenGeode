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

#include <geode/model/helpers/compute_unique_vertices.hpp>

#include <geode/geometry/nn_search.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < typename Model >
    std::vector< geode::Point< Model::dim > > get_all_points_base(
        const Model& model )
    {
        std::vector< geode::Point< Model::dim > > points;
        for( const auto& corner : model.corners() )
        {
            for( const auto v : geode::Range{ corner.mesh().nb_vertices() } )
            {
                points.emplace_back( corner.mesh().point( v ) );
            }
        }
        for( const auto& line : model.lines() )
        {
            for( const auto v : geode::Range{ line.mesh().nb_vertices() } )
            {
                points.emplace_back( line.mesh().point( v ) );
            }
        }
        for( const auto& surface : model.surfaces() )
        {
            for( const auto v : geode::Range{ surface.mesh().nb_vertices() } )
            {
                points.emplace_back( surface.mesh().point( v ) );
            }
        }
        return points;
    }

    std::vector< geode::Point2D > get_all_points( const geode::Section& model )
    {
        return get_all_points_base< geode::Section >( model );
    }

    std::vector< geode::Point3D > get_all_points( const geode::BRep& model )
    {
        auto points = get_all_points_base< geode::BRep >( model );
        for( const auto& block : model.blocks() )
        {
            for( const auto v : geode::Range{ block.mesh().nb_vertices() } )
            {
                points.emplace_back( block.mesh().point( v ) );
            }
        }
        return points;
    }

    template < typename Model >
    const geode::Point< Model::dim >& get_point_base(
        const Model& model, const geode::ComponentMeshVertex& cmv )
    {
        if( cmv.component_id.type()
            == geode::Surface< Model::dim >::component_type_static() )
        {
            const auto& surface = model.surface( cmv.component_id.id() );
            return surface.mesh().point( cmv.vertex );
        }
        if( cmv.component_id.type()
            == geode::Line< Model::dim >::component_type_static() )
        {
            const auto& line = model.line( cmv.component_id.id() );
            return line.mesh().point( cmv.vertex );
        }
        if( cmv.component_id.type()
            == geode::Corner< Model::dim >::component_type_static() )
        {
            const auto& corner = model.corner( cmv.component_id.id() );
            return corner.mesh().point( cmv.vertex );
        }
        throw geode::OpenGeodeModelException{ nullptr,
            geode::OpenGeodeException::TYPE::data,
            "[compute_unique_vertices::get_point] Unknown component type: ",
            cmv.component_id.string() };
        const auto& corner = model.corner( cmv.component_id.id() );
        return corner.mesh().point( cmv.vertex );
    }

    const geode::Point2D& get_point(
        const geode::Section& model, const geode::ComponentMeshVertex& cmv )
    {
        return get_point_base< geode::Section >( model, cmv );
    }

    const geode::Point3D& get_point(
        const geode::BRep& model, const geode::ComponentMeshVertex& cmv )
    {
        if( cmv.component_id.type()
            == geode::Block< 3 >::component_type_static() )
        {
            const auto& block = model.block( cmv.component_id.id() );
            return block.mesh().point( cmv.vertex );
        }
        return get_point_base< geode::BRep >( model, cmv );
    }

    template < typename Model >
    void set_unique_vertices_base( const Model& model,
        typename Model::Builder& builder,
        absl::Span< const geode::index_t > initial_uv_correspondance,
        const geode::NNSearch< Model::dim >& unique_nns )
    {
        for( const auto& corner : model.corners() )
        {
            for( const auto v : geode::Range{ corner.mesh().nb_vertices() } )
            {
                geode::ComponentMeshVertex cmv{ corner.component_id(), v };
                if( model.unique_vertex( { corner.component_id(), v } )
                    != geode::NO_ID )
                {
                    continue;
                }
                const auto& point = corner.mesh().point( v );
                const auto unique_vertex_id =
                    unique_nns.closest_neighbor( point );
                builder.set_unique_vertex(
                    cmv, initial_uv_correspondance[unique_vertex_id] );
            }
        }
        for( const auto& line : model.lines() )
        {
            for( const auto v : geode::Range{ line.mesh().nb_vertices() } )
            {
                geode::ComponentMeshVertex cmv{ line.component_id(), v };
                if( model.unique_vertex( cmv ) != geode::NO_ID )
                {
                    continue;
                }
                const auto& point = line.mesh().point( v );
                const auto unique_vertex_id =
                    unique_nns.closest_neighbor( point );
                builder.set_unique_vertex(
                    cmv, initial_uv_correspondance[unique_vertex_id] );
            }
        }
        for( const auto& surface : model.surfaces() )
        {
            for( const auto v : geode::Range{ surface.mesh().nb_vertices() } )
            {
                geode::ComponentMeshVertex cmv{ surface.component_id(), v };
                if( model.unique_vertex( cmv ) != geode::NO_ID )
                {
                    continue;
                }
                const auto& point = surface.mesh().point( v );
                const auto unique_vertex_id =
                    unique_nns.closest_neighbor( point );
                builder.set_unique_vertex(
                    cmv, initial_uv_correspondance[unique_vertex_id] );
            }
        }
    }

    void set_unique_vertices( const geode::Section& model,
        geode::SectionBuilder& builder,
        absl::Span< const geode::index_t > initial_uv_correspondance,
        const geode::NNSearch2D& unique_nns )
    {
        set_unique_vertices_base(
            model, builder, initial_uv_correspondance, unique_nns );
    }

    void set_unique_vertices( const geode::BRep& model,
        geode::BRepBuilder& builder,
        absl::Span< const geode::index_t > initial_uv_correspondance,
        const geode::NNSearch3D& unique_nns )
    {
        set_unique_vertices_base(
            model, builder, initial_uv_correspondance, unique_nns );
        for( const auto& block : model.blocks() )
        {
            for( const auto v : geode::Range{ block.mesh().nb_vertices() } )
            {
                geode::ComponentMeshVertex cmv{ block.component_id(), v };
                if( model.unique_vertex( cmv ) != geode::NO_ID )
                {
                    continue;
                }
                const auto& point = block.mesh().point( v );
                const auto unique_vertex_id =
                    unique_nns.closest_neighbor( point );
                builder.set_unique_vertex(
                    cmv, initial_uv_correspondance[unique_vertex_id] );
            }
        }
    }

    template < typename Model >
    std::vector< geode::index_t > compute_initial_uv_correspondance(
        const Model& model, const geode::NNSearch< Model::dim >& unique_nns )
    {
        std::vector< geode::index_t > initial_uv_correspondance(
            unique_nns.nb_points(), geode::NO_ID );
        for( const auto uv : geode::Range{ model.nb_unique_vertices() } )
        {
            const auto& cmv = model.component_mesh_vertices( uv ).front();
            const auto& point = get_point( model, cmv );
            initial_uv_correspondance[unique_nns.closest_neighbor( point )] =
                uv;
        }
        geode::index_t new_uv = model.nb_unique_vertices();
        for( auto& uv : initial_uv_correspondance )
        {
            if( uv == geode::NO_ID )
            {
                uv = new_uv++;
            }
        }
        return initial_uv_correspondance;
    }

} // namespace

namespace geode
{
    template < typename Model >
    void compute_model_unique_vertices(
        const Model& model, typename Model::Builder& builder )
    {
        auto all_vertices = get_all_points( model );
        NNSearch< Model::dim > nns{ all_vertices };
        const auto colocated_info =
            nns.colocated_index_mapping( GLOBAL_EPSILON );
        const auto nb_initial_unique_vertices = model.nb_unique_vertices();
        NNSearch< Model::dim > unique_nns{ colocated_info.unique_points };
        const auto initial_uv_correspondance =
            compute_initial_uv_correspondance( model, unique_nns );
        builder.create_unique_vertices(
            colocated_info.nb_unique_points() - nb_initial_unique_vertices );
        set_unique_vertices(
            model, builder, initial_uv_correspondance, unique_nns );
    }

    template void opengeode_model_api compute_model_unique_vertices(
        const BRep&, BRepBuilder& );
    template void opengeode_model_api compute_model_unique_vertices(
        const Section&, SectionBuilder& );
} // namespace geode
