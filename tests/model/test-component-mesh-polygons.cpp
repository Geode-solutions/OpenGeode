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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/component_mesh_polygons.hpp>
#include <geode/model/helpers/component_mesh_vertices.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/section_input.hpp>

#include <geode/tests/common.hpp>

void run_test_brep()
{
    auto model = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" ) );

    for( const auto& block : model.blocks() )
    {
        const auto& block_mesh = block.mesh();
        for( const auto& surface : model.surfaces() )
        {
            if( !model.is_boundary( surface, block )
                && !model.is_internal( surface, block ) )
            {
                continue;
            }
            const auto& surface_mesh = surface.mesh();
            for( const auto polygon_id :
                geode::Range{ surface_mesh.nb_polygons() } )
            {
                const auto block_facets_vertices =
                    geode::block_vertices_from_surface_polygon(
                        model, block, surface, polygon_id );
                const auto oriented_block_facets_vertices =
                    geode::oriented_block_vertices_from_surface_polygon(
                        model, block, surface, polygon_id );
                geode::OpenGeodeModelException::test(
                    block_facets_vertices.size()
                        == oriented_block_facets_vertices.nb_facets(),
                    "Different number of polyhedra for "
                    "block_vertices_from_surface_polygon and "
                    "oriented_block_vertices_from_surface_polygon functions." );
                if( model.is_boundary( surface, block ) )
                {
                    geode::OpenGeodeModelException::test(
                        block_facets_vertices.size() == 1, "",
                        block_facets_vertices.size(),
                        " polyhedra were found from boundary surface "
                        "polygon." );
                }
                else if( model.is_internal( surface, block ) )
                {
                    geode::OpenGeodeModelException::test(
                        block_facets_vertices.size() == 2, "",
                        block_facets_vertices.size(),
                        " polyhedra were found from internal surface "
                        "polygon." );
                }
                for( const auto polygon_vertex_id :
                    geode::LIndices{ block_facets_vertices[0].vertices } )
                {
                    geode::OpenGeodeModelException::test(
                        surface_mesh
                            .point( surface_mesh.polygon_vertex(
                                { polygon_id, polygon_vertex_id } ) )
                            .inexact_equal(
                                block_mesh.point( block_facets_vertices[0]
                                        .vertices[polygon_vertex_id] ) ),
                        "Point on the edge and on the surface have "
                        "different positions." );
                }
            }
        }
    }

    for( const auto& surface : model.surfaces() )
    {
        const auto& surface_mesh = surface.mesh();
        for( const auto& line : model.lines() )
        {
            if( !model.is_boundary( line, surface )
                && !model.is_internal( line, surface ) )
            {
                continue;
            }
            const auto& line_mesh = line.mesh();
            for( const auto edge_id : geode::Range{ line_mesh.nb_edges() } )
            {
                const auto surface_edge_vertices =
                    geode::surface_vertices_from_line_edge(
                        model, surface, line, edge_id );
                const auto oriented_surface_edge_vertices =
                    geode::oriented_surface_vertices_from_line_edge(
                        model, surface, line, edge_id );
                geode::OpenGeodeModelException::test(
                    surface_edge_vertices.size()
                        == oriented_surface_edge_vertices.nb_edges(),
                    "Different number of polygons for "
                    "surface_vertices_from_line_edge and "
                    "oriented_surface_vertices_from_line_edge functions." );
                if( model.is_boundary( line, surface ) )
                {
                    geode::OpenGeodeModelException::test(
                        surface_edge_vertices.size() == 1, "",
                        surface_edge_vertices.size(),
                        " polygons were found from boundary line edge." );
                }
                else if( model.is_internal( line, surface ) )
                {
                    geode::OpenGeodeModelException::test(
                        surface_edge_vertices.size() == 2, "",
                        surface_edge_vertices.size(),
                        " polygons were found from internal line edge." );
                }
                for( const auto edge_vertex_id : geode::LRange{ 2 } )
                {
                    geode::OpenGeodeModelException::test(
                        line_mesh
                            .point( line_mesh.edge_vertex(
                                { edge_id, edge_vertex_id } ) )
                            .inexact_equal(
                                surface_mesh.point( surface_edge_vertices[0]
                                        .vertices[edge_vertex_id] ) ),
                        "Point on the edge and on the surface have "
                        "different positions." );
                }
            }
        }
    }
}
void run_test_section()
{
    auto model =
        geode::load_section( absl::StrCat( geode::DATA_PATH, "quad.og_sctn" ) );

    for( const auto& surface : model.surfaces() )
    {
        const auto& surface_mesh = surface.mesh();
        for( const auto& line : model.lines() )
        {
            if( !model.is_boundary( line, surface )
                && !model.is_internal( line, surface ) )
            {
                continue;
            }
            const auto& line_mesh = line.mesh();
            for( const auto edge_id : geode::Range{ line_mesh.nb_edges() } )
            {
                const auto surface_edge_vertices =
                    geode::surface_vertices_from_line_edge(
                        model, surface, line, edge_id );
                const auto oriented_surface_edge_vertices =
                    geode::oriented_surface_vertices_from_line_edge(
                        model, surface, line, edge_id );
                geode::OpenGeodeModelException::test(
                    surface_edge_vertices.size()
                        == oriented_surface_edge_vertices.nb_edges(),
                    "Different number of polygons for "
                    "surface_vertices_from_line_edge and "
                    "oriented_surface_vertices_from_line_edge functions." );
                if( model.is_boundary( line, surface ) )
                {
                    geode::OpenGeodeModelException::test(
                        surface_edge_vertices.size() == 1, "",
                        surface_edge_vertices.size(),
                        " polygons were found from boundary line edge." );
                }
                else if( model.is_internal( line, surface ) )
                {
                    geode::OpenGeodeModelException::test(
                        surface_edge_vertices.size() == 2, "",
                        surface_edge_vertices.size(),
                        " polygons were found from internal line edge." );
                }
                for( const auto edge_vertex_id : geode::LRange{ 2 } )
                {
                    geode::OpenGeodeModelException::test(
                        line_mesh
                            .point( line_mesh.edge_vertex(
                                { edge_id, edge_vertex_id } ) )
                            .inexact_equal(
                                surface_mesh.point( surface_edge_vertices[0]
                                        .vertices[edge_vertex_id] ) ),
                        "Points on the edge and on the surface have "
                        "different positions." );
                }
            }
        }
    }
}

void run_test_polygon_without_unique_vertex()
{
    auto model = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" ) );
    geode::BRepBuilder builder{ model };
    const auto& block = *model.blocks().begin();
    const auto& surface = *model.boundaries( block ).begin();
    const geode::ComponentMeshVertex surface_cmv{ surface.component_id(),
        surface.mesh().polygon_vertex( { 0, 0 } ) };
    builder.unset_unique_vertex(
        surface_cmv, model.unique_vertex( surface_cmv ) );
    const auto polygons = geode::component_mesh_polygons( model, surface, 0 );
    geode::OpenGeodeModelException::test(
        polygons.surface_polygons.empty() && polygons.block_polygons.empty(),
        "No component mesh polygon should be found from a polygon with a "
        "vertex without unique vertex" );
    geode::OpenGeodeModelException::test(
        geode::block_vertices_from_surface_polygon( model, block, surface, 0 )
            .empty(),
        "No block facet should be found from a polygon with a vertex without "
        "unique vertex" );
    geode::OpenGeodeModelException::test(
        geode::oriented_block_vertices_from_surface_polygon(
            model, block, surface, 0 )
                .nb_facets()
            == 0,
        "No oriented block facet should be found from a polygon with a "
        "vertex without unique vertex" );

    const auto& line = *model.boundaries( surface ).begin();
    const geode::ComponentMeshVertex line_cmv{ line.component_id(),
        line.mesh().edge_vertex( { 0, 0 } ) };
    builder.unset_unique_vertex( line_cmv, model.unique_vertex( line_cmv ) );
    geode::OpenGeodeModelException::test(
        geode::surface_vertices_from_line_edge( model, surface, line, 0 )
            .empty(),
        "No surface edge should be found from a line edge with a vertex "
        "without unique vertex" );
    geode::OpenGeodeModelException::test(
        geode::oriented_surface_vertices_from_line_edge(
            model, surface, line, 0 )
                .nb_edges()
            == 0,
        "No oriented surface edge should be found from a line edge with a "
        "vertex without unique vertex" );
}

void run_test_degenerate_polygon()
{
    const auto model = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" ) );
    const auto& surface = *model.surfaces().begin();
    auto unique_vertices = geode::polygon_unique_vertices( model, surface, 0 );
    unique_vertices[1] = unique_vertices[0];
    const auto polygons =
        geode::component_mesh_polygons( model, unique_vertices );
    geode::OpenGeodeModelException::test(
        polygons.surface_polygons.empty() && polygons.block_polygons.empty(),
        "No component mesh polygon should be found from degenerate unique "
        "vertices" );
    const auto& cmvs = model.component_mesh_vertices( unique_vertices[0] );
    geode::OpenGeodeModelException::test(
        geode::component_mesh_vertex_pairs( cmvs, cmvs ).empty(),
        "No component mesh vertex pair should be found from the same unique "
        "vertex" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    run_test_brep();
    run_test_section();
    run_test_polygon_without_unique_vertex();
    run_test_degenerate_polygon();
}

OPENGEODE_TEST( "convert-mesh-polygons" )