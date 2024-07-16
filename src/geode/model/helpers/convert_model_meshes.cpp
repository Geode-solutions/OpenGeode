/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/model/helpers/convert_model_meshes.hpp>

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/convert_solid_mesh.hpp>
#include <geode/mesh/helpers/convert_surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < typename Model, typename Mesh >
    absl::FixedArray< geode::index_t > save_unique_vertices( const Model& model,
        const Mesh& mesh,
        const geode::ComponentID& component_id )
    {
        const auto nb_vertices = mesh.nb_vertices();
        absl::FixedArray< geode::index_t > unique_vertices( nb_vertices );
        for( const auto v : geode::Range{ nb_vertices } )
        {
            unique_vertices[v] = model.unique_vertex( { component_id, v } );
        }
        return unique_vertices;
    }

    template < typename Builder >
    void set_unique_vertices( Builder& builder,
        absl::Span< const geode::index_t > unique_vertices,
        const geode::ComponentID& component_id )
    {
        for( const auto v : geode::Indices{ unique_vertices } )
        {
            if( unique_vertices[v] != geode::NO_ID )
            {
                builder.set_unique_vertex(
                    { component_id, v }, unique_vertices[v] );
            }
        }
    }

    template < typename Model >
    void do_convert_surface( const Model& model,
        typename Model::Builder& builder,
        const geode::Surface< Model::dim >& surface,
        const geode::MeshType& mesh_type )
    {
        const auto& mesh = surface.mesh();
        if( mesh.type_name() == mesh_type )
        {
            return;
        }
        const auto unique_vertices =
            save_unique_vertices( model, mesh, surface.component_id() );
        if( mesh_type
            == geode::TriangulatedSurface< Model::dim >::type_name_static() )
        {
            auto tri_surface =
                geode::convert_surface_mesh_into_triangulated_surface( mesh );
            OPENGEODE_EXCEPTION( tri_surface,
                "[do_convert_surface] Cannot convert SurfaceMesh "
                "to TriangulatedSurface" );
            builder.update_surface_mesh(
                surface, std::move( tri_surface ).value() );
        }
        else if( mesh_type
                 == geode::PolygonalSurface< Model::dim >::type_name_static() )
        {
            builder.update_surface_mesh( surface,
                std::move( geode::convert_surface_mesh_into_polygonal_surface(
                    mesh ) ) );
        }
        set_unique_vertices( builder, unique_vertices, surface.component_id() );
    }

    template < typename Model >
    void do_convert_surfaces_to_triangulated(
        const Model& model, typename Model::Builder& builder )
    {
        for( const auto& surface : model.surfaces() )
        {
            do_convert_surface( model, builder, surface,
                geode::TriangulatedSurface< Model::dim >::type_name_static() );
        }
    }

    template < typename Model >
    void do_triangulate_surfaces(
        Model& model, typename Model::Builder& builder )
    {
        for( const auto& surface : model.surfaces() )
        {
            const auto& mesh = surface.mesh();
            geode::triangulate_surface_mesh(
                mesh, *builder.surface_mesh_builder( surface.id() ) );
        }
    }

    void do_convert_block( const geode::BRep& model,
        geode::BRepBuilder& builder,
        const geode::Block3D& block,
        const geode::MeshType& mesh_type )
    {
        OPENGEODE_EXCEPTION(
            mesh_type == geode::TetrahedralSolid3D::type_name_static()
                || mesh_type == geode::HybridSolid3D::type_name_static(),
            "[do_convert_block] You can only convert block mesh to "
            "TetrahedralSolid3D or HybridSolid3D, not ",
            mesh_type.get() );
        const auto& mesh = block.mesh();
        if( mesh.type_name() == mesh_type )
        {
            return;
        }
        const auto unique_vertices =
            save_unique_vertices( model, mesh, block.component_id() );
        if( mesh_type == geode::TetrahedralSolid3D::type_name_static() )
        {
            auto tet_solid =
                geode::convert_solid_mesh_into_tetrahedral_solid( mesh );
            OPENGEODE_EXCEPTION( tet_solid, "[do_convert_block] Cannot convert "
                                            "SolidMesh to TetrahedralSolid" );
            builder.update_block_mesh( block, std::move( tet_solid ).value() );
        }
        else if( mesh_type == geode::HybridSolid3D::type_name_static() )
        {
            auto hybrid_solid =
                geode::convert_solid_mesh_into_hybrid_solid( mesh );
            OPENGEODE_EXCEPTION( hybrid_solid,
                "[do_convert_block] Cannot convert SolidMesh to HybridSolid" );
            builder.update_block_mesh(
                block, std::move( hybrid_solid ).value() );
        }
        set_unique_vertices( builder, unique_vertices, block.component_id() );
    }
} // namespace

namespace geode
{
    void convert_surface_mesh( const Section& model,
        SectionBuilder& builder,
        const geode::Surface2D& surface,
        const geode::MeshType& mesh_type )
    {
        do_convert_surface( model, builder, surface, mesh_type );
    }

    void convert_surface_mesh( const BRep& model,
        BRepBuilder& builder,
        const geode::Surface3D& surface,
        const geode::MeshType& mesh_type )
    {
        do_convert_surface( model, builder, surface, mesh_type );
    }

    void convert_surface_meshes_into_triangulated_surfaces( BRep& brep )
    {
        BRepBuilder builder{ brep };
        convert_surface_meshes_into_triangulated_surfaces( brep, builder );
    }

    void convert_surface_meshes_into_triangulated_surfaces(
        const BRep& brep, BRepBuilder& builder )
    {
        do_convert_surfaces_to_triangulated( brep, builder );
    }

    void convert_surface_meshes_into_triangulated_surfaces(
        const Section& section, SectionBuilder& builder )
    {
        do_convert_surfaces_to_triangulated( section, builder );
    }

    void convert_block_mesh( const BRep& model,
        BRepBuilder& builder,
        const Block3D& block,
        const MeshType& new_mesh_type )
    {
        do_convert_block( model, builder, block, new_mesh_type );
    }

    void convert_block_meshes_into_tetrahedral_solids(
        const BRep& brep, BRepBuilder& builder )
    {
        for( const auto& block : brep.blocks() )
        {
            do_convert_block(
                brep, builder, block, TetrahedralSolid3D::type_name_static() );
        }
    }

    void triangulate_surface_meshes( const BRep& brep, BRepBuilder& builder )
    {
        do_triangulate_surfaces( brep, builder );
    }

    void triangulate_surface_meshes(
        const Section& section, SectionBuilder& builder )
    {
        do_triangulate_surfaces( section, builder );
    }
} // namespace geode
