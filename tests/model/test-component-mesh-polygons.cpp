/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>
#include <geode/basic/range.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/helpers/component_mesh_polygons.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>
#include <geode/model/representation/io/brep_input.h>
#include <geode/model/representation/io/section_input.h>

#include <geode/tests/common.h>

void run_test_brep()
{
    auto model = geode::load_brep(
        absl::StrCat( geode::data_path, "test_mesh3.og_brep" ) );

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
                OPENGEODE_EXCEPTION(
                    block_facets_vertices.size()
                        == oriented_block_facets_vertices.nb_facets(),
                    "[Test] Different number of polyhedra for "
                    "block_vertices_from_surface_polygon and "
                    "oriented_block_vertices_from_surface_polygon functions." );
                if( model.is_boundary( surface, block ) )
                {
                    OPENGEODE_EXCEPTION( block_facets_vertices.size() == 1,
                        "[Test] ", block_facets_vertices.size(),
                        " polyhedra were found from boundary surface "
                        "polygon." );
                }
                else if( model.is_internal( surface, block ) )
                {
                    OPENGEODE_EXCEPTION( block_facets_vertices.size() == 2,
                        "[Test] ", block_facets_vertices.size(),
                        " polyhedra were found from internal surface "
                        "polygon." );
                }
                for( const auto polygon_vertex_id :
                    geode::LIndices{ block_facets_vertices[0].vertices } )
                {
                    OPENGEODE_EXCEPTION(
                        surface_mesh
                            .point( surface_mesh.polygon_vertex(
                                { polygon_id, polygon_vertex_id } ) )
                            .inexact_equal( block_mesh.point(
                                block_facets_vertices[0]
                                    .vertices[polygon_vertex_id] ) ),
                        "[Test] Point on the edge and on the surface have "
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
                OPENGEODE_EXCEPTION(
                    surface_edge_vertices.size()
                        == oriented_surface_edge_vertices.nb_edges(),
                    "[Test] Different number of polygons for "
                    "surface_vertices_from_line_edge and "
                    "oriented_surface_vertices_from_line_edge functions." );
                if( model.is_boundary( line, surface ) )
                {
                    OPENGEODE_EXCEPTION( surface_edge_vertices.size() == 1,
                        "[Test] ", surface_edge_vertices.size(),
                        " polygons were found from boundary line edge." );
                }
                else if( model.is_internal( line, surface ) )
                {
                    OPENGEODE_EXCEPTION( surface_edge_vertices.size() == 2,
                        "[Test] ", surface_edge_vertices.size(),
                        " polygons were found from internal line edge." );
                }
                for( const auto edge_vertex_id : geode::LRange{ 2 } )
                {
                    OPENGEODE_EXCEPTION(
                        line_mesh
                            .point( line_mesh.edge_vertex(
                                { edge_id, edge_vertex_id } ) )
                            .inexact_equal( surface_mesh.point(
                                surface_edge_vertices[0]
                                    .vertices[edge_vertex_id] ) ),
                        "[Test] Point on the edge and on the surface have "
                        "different positions." );
                }
            }
        }
    }
}
void run_test_section()
{
    auto model =
        geode::load_section( absl::StrCat( geode::data_path, "quad.og_sctn" ) );

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
                OPENGEODE_EXCEPTION(
                    surface_edge_vertices.size()
                        == oriented_surface_edge_vertices.nb_edges(),
                    "[Test] Different number of polygons for "
                    "surface_vertices_from_line_edge and "
                    "oriented_surface_vertices_from_line_edge functions." );
                if( model.is_boundary( line, surface ) )
                {
                    OPENGEODE_EXCEPTION( surface_edge_vertices.size() == 1,
                        "[Test] ", surface_edge_vertices.size(),
                        " polygons were found from boundary line edge." );
                }
                else if( model.is_internal( line, surface ) )
                {
                    OPENGEODE_EXCEPTION( surface_edge_vertices.size() == 2,
                        "[Test] ", surface_edge_vertices.size(),
                        " polygons were found from internal line edge." );
                }
                for( const auto edge_vertex_id : geode::LRange{ 2 } )
                {
                    OPENGEODE_EXCEPTION(
                        line_mesh
                            .point( line_mesh.edge_vertex(
                                { edge_id, edge_vertex_id } ) )
                            .inexact_equal( surface_mesh.point(
                                surface_edge_vertices[0]
                                    .vertices[edge_vertex_id] ) ),
                        "[Test] Points on the edge and on the surface have "
                        "different positions." );
                }
            }
        }
    }
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    run_test_brep();
    run_test_section();
}

OPENGEODE_TEST( "convert-mesh-polygons" )