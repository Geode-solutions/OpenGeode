/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/model/helpers/convert_model_meshes.h>

#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/convert_solid_mesh.h>
#include <geode/mesh/helpers/convert_surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

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
        for( const auto v : geode::Range{ unique_vertices.size() } )
        {
            builder.set_unique_vertex(
                { component_id, v }, unique_vertices[v] );
        }
    }

    template < typename Model, typename ModelBuilder >
    void do_convert_surfaces( Model& model, ModelBuilder& builder )
    {
        for( const auto& surface : model.surfaces() )
        {
            const auto& mesh = surface.mesh();
            const auto unique_vertices =
                save_unique_vertices( model, mesh, surface.component_id() );
            auto tri_surface =
                geode::convert_surface_mesh_into_triangulated_surface( mesh );
            OPENGEODE_EXCEPTION( tri_surface,
                "[convert_surface_meshes_into_triangulated_surfaces] Cannot "
                "convert SurfaceMesh to TriangulatedSurface" );
            builder.update_surface_mesh(
                surface, std::move( tri_surface ).value() );
            set_unique_vertices(
                builder, unique_vertices, surface.component_id() );
        }
    }

    void do_convert_blocks( geode::BRep& model, geode::BRepBuilder& builder )
    {
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            const auto unique_vertices =
                save_unique_vertices( model, mesh, block.component_id() );
            auto tet_solid =
                geode::convert_solid_mesh_into_tetrahedral_solid( mesh );
            OPENGEODE_EXCEPTION( tet_solid,
                "[convert_block_meshes_into_tetrahedral_solids] Cannot "
                "convert SolidMesh to TetrahedralSolid" );
            builder.update_block_mesh( block, std::move( tet_solid ).value() );
            set_unique_vertices(
                builder, unique_vertices, block.component_id() );
        }
    }
} // namespace

namespace geode
{
    void convert_surface_meshes_into_triangulated_surfaces( BRep& brep )
    {
        BRepBuilder brep_builder{ brep };
        do_convert_surfaces( brep, brep_builder );
    }

    void convert_surface_meshes_into_triangulated_surfaces( Section& section )
    {
        SectionBuilder section_builder{ section };
        do_convert_surfaces( section, section_builder );
    }

    void convert_block_meshes_into_tetrahedral_solids( BRep& brep )
    {
        BRepBuilder brep_builder{ brep };
        do_convert_blocks( brep, brep_builder );
    }

    // template void opengeode_model_api
    //     convert_surface_meshes_into_triangulated_surfaces( Section& );
    // template void opengeode_model_api
    //     convert_surface_meshes_into_triangulated_surfaces( BRep& );

} // namespace geode
