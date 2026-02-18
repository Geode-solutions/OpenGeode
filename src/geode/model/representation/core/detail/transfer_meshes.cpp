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

#include <geode/model/representation/core/detail/transfer_meshes.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
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
    void remove_corner_meshes_in_mapping( const Model& model,
        typename Model::Builder& builder,
        const geode::ModelCopyMapping& mapping )
    {
        for( const auto& [in, _] :
            mapping.at( geode::Corner< Model::dim >::component_type_static() )
                .in2out_map() )
        {
            builder.update_corner_mesh(
                model.corner( in ), geode::PointSet< Model::dim >::create() );
        }
    }

    template < typename Model >
    void remove_line_meshes_in_mapping( const Model& model,
        typename Model::Builder& builder,
        const geode::ModelCopyMapping& mapping )
    {
        for( const auto& in2out :
            mapping.at( geode::Line< Model::dim >::component_type_static() )
                .in2out_map() )
        {
            builder.update_line_mesh( model.line( in2out.first ),
                geode::EdgedCurve< Model::dim >::create() );
        }
    }

    template < typename Model >
    void remove_surface_meshes_in_mapping( const Model& model,
        typename Model::Builder& builder,
        const geode::ModelCopyMapping& mapping )
    {
        for( const auto& in2out :
            mapping.at( geode::Surface< Model::dim >::component_type_static() )
                .in2out_map() )
        {
            builder.update_surface_mesh( model.surface( in2out.first ),
                geode::SurfaceMesh< Model::dim >::create() );
        }
    }

    template < typename Model >
    void remove_block_meshes_in_mapping( const Model& model,
        typename Model::Builder& builder,
        const geode::ModelCopyMapping& mapping )
    {
        for( const auto& in2out :
            mapping.at( geode::Block< Model::dim >::component_type_static() )
                .in2out_map() )
        {
            builder.update_block_mesh( model.block( in2out.first ),
                geode::SolidMesh< Model::dim >::create() );
        }
    }

    template < typename Model >
    absl::FixedArray< geode::index_t > save_mesh_unique_vertices(
        const Model& model,
        geode::index_t nb_vertices,
        const geode::ComponentID& component_id )
    {
        absl::FixedArray< geode::index_t > unique_vertices( nb_vertices );
        for( const auto v : geode::Range{ nb_vertices } )
        {
            unique_vertices[v] = model.unique_vertex( { component_id, v } );
        }
        return unique_vertices;
    }

    template < typename ModelBuilder >
    void set_mesh_unique_vertices( ModelBuilder& builder,
        absl::Span< const geode::index_t > unique_vertices,
        const geode::ComponentID& component_id,
        geode::index_t first_new_unique_vertex )
    {
        for( const auto v : geode::Indices{ unique_vertices } )
        {
            if( unique_vertices[v] != geode::NO_ID )
            {
                builder.set_unique_vertex( { component_id, v },
                    first_new_unique_vertex + unique_vertices[v] );
            }
        }
    }

    template < typename Model >
    void set_corner_meshes_in_mapping( const Model& model,
        typename Model::Builder& builder,
        const Model& other,
        typename Model::Builder& other_builder,
        const geode::ModelCopyMapping& mapping,
        geode::index_t first_new_unique_vertex_id )
    {
        for( const auto& [corner_id, other_corner_id] :
            mapping.at( geode::Corner< Model::dim >::component_type_static() )
                .in2out_map() )
        {
            const auto& other_corner = other.corner( other_corner_id );
            const auto corner_unique_vertices = save_mesh_unique_vertices(
                other, other_corner.mesh().nb_vertices(),
                other_corner.component_id() );
            const auto& corner = model.corner( corner_id );
            builder.update_corner_mesh(
                corner, other_builder.steal_corner_mesh( other_corner_id ) );
            auto mesh_builder = builder.corner_mesh_builder( corner_id );
            mesh_builder->set_id( corner.id() );
            mesh_builder->set_name( corner.name() );
            set_mesh_unique_vertices( builder, corner_unique_vertices,
                corner.component_id(), first_new_unique_vertex_id );
        }
    }

    template < typename Model >
    void set_line_meshes_in_mapping( const Model& model,
        typename Model::Builder& builder,
        const Model& other,
        typename Model::Builder& other_builder,
        const geode::ModelCopyMapping& mapping,
        geode::index_t first_new_unique_vertex_id )
    {
        for( const auto& [line_id, other_line_id] :
            mapping.at( geode::Line< Model::dim >::component_type_static() )
                .in2out_map() )
        {
            const auto& other_line = other.line( other_line_id );
            const auto line_unique_vertices = save_mesh_unique_vertices( other,
                other_line.mesh().nb_vertices(), other_line.component_id() );
            const auto& line = model.line( line_id );
            builder.update_line_mesh(
                line, other_builder.steal_line_mesh( other_line_id ) );
            auto mesh_builder = builder.line_mesh_builder( line_id );
            mesh_builder->set_id( line.id() );
            mesh_builder->set_name( line.name() );
            set_mesh_unique_vertices( builder, line_unique_vertices,
                line.component_id(), first_new_unique_vertex_id );
        }
    }

    template < typename Model >
    void set_surface_meshes_in_mapping( const Model& model,
        typename Model::Builder& builder,
        const Model& other,
        typename Model::Builder& other_builder,
        const geode::ModelCopyMapping& mapping,
        geode::index_t first_new_unique_vertex_id )
    {
        for( const auto& [surface_id, other_surface_id] :
            mapping.at( geode::Surface< Model::dim >::component_type_static() )
                .in2out_map() )
        {
            const auto& other_surface = other.surface( other_surface_id );
            const auto surface_unique_vertices = save_mesh_unique_vertices(
                other, other_surface.mesh().nb_vertices(),
                other_surface.component_id() );
            const auto& surface = model.surface( surface_id );
            builder.update_surface_mesh(
                surface, other_builder.steal_surface_mesh( other_surface_id ) );
            auto mesh_builder = builder.surface_mesh_builder( surface_id );
            mesh_builder->set_id( surface.id() );
            mesh_builder->set_name( surface.name() );
            set_mesh_unique_vertices( builder, surface_unique_vertices,
                surface.component_id(), first_new_unique_vertex_id );
        }
    }

    template < typename Model >
    void set_block_meshes_in_mapping( const Model& model,
        typename Model::Builder& builder,
        const Model& other,
        typename Model::Builder& other_builder,
        const geode::ModelCopyMapping& mapping,
        geode::index_t first_new_unique_vertex_id )
    {
        for( const auto& [block_id, other_block_id] :
            mapping.at( geode::Block< Model::dim >::component_type_static() )
                .in2out_map() )
        {
            const auto& other_block = other.block( other_block_id );
            const auto block_unique_vertices = save_mesh_unique_vertices( other,
                other_block.mesh().nb_vertices(), other_block.component_id() );
            const auto& block = model.block( block_id );
            builder.update_block_mesh(
                block, other_builder.steal_block_mesh( other_block_id ) );
            auto mesh_builder = builder.block_mesh_builder( block_id );
            mesh_builder->set_id( block.id() );
            mesh_builder->set_name( block.name() );
            set_mesh_unique_vertices( builder, block_unique_vertices,
                block.component_id(), first_new_unique_vertex_id );
        }
    }
} // namespace

namespace geode
{
    namespace detail
    {
        void transfer_brep_meshes( const BRep& brep,
            BRepBuilder& brep_builder,
            BRep&& other,
            const ModelCopyMapping& mapping )
        {
            remove_corner_meshes_in_mapping( brep, brep_builder, mapping );
            remove_line_meshes_in_mapping( brep, brep_builder, mapping );
            remove_surface_meshes_in_mapping( brep, brep_builder, mapping );
            remove_block_meshes_in_mapping( brep, brep_builder, mapping );
            brep_builder.delete_isolated_vertices();
            const auto first_new_unique_vertex_id =
                brep_builder.create_unique_vertices(
                    other.nb_unique_vertices() );
            BRepBuilder other_builder{ other };
            set_corner_meshes_in_mapping( brep, brep_builder, other,
                other_builder, mapping, first_new_unique_vertex_id );
            set_line_meshes_in_mapping( brep, brep_builder, other,
                other_builder, mapping, first_new_unique_vertex_id );
            set_surface_meshes_in_mapping( brep, brep_builder, other,
                other_builder, mapping, first_new_unique_vertex_id );
            set_block_meshes_in_mapping( brep, brep_builder, other,
                other_builder, mapping, first_new_unique_vertex_id );
        }

        void transfer_section_meshes( const Section& section,
            SectionBuilder& section_builder,
            Section&& other,
            const ModelCopyMapping& mapping )
        {
            remove_corner_meshes_in_mapping(
                section, section_builder, mapping );
            remove_line_meshes_in_mapping( section, section_builder, mapping );
            remove_surface_meshes_in_mapping(
                section, section_builder, mapping );
            section_builder.delete_isolated_vertices();
            const auto first_new_unique_vertex_id =
                section_builder.create_unique_vertices(
                    other.nb_unique_vertices() );
            SectionBuilder other_builder{ other };
            set_corner_meshes_in_mapping( section, section_builder, other,
                other_builder, mapping, first_new_unique_vertex_id );
            set_line_meshes_in_mapping( section, section_builder, other,
                other_builder, mapping, first_new_unique_vertex_id );
            set_surface_meshes_in_mapping( section, section_builder, other,
                other_builder, mapping, first_new_unique_vertex_id );
        }
    } // namespace detail
} // namespace geode
