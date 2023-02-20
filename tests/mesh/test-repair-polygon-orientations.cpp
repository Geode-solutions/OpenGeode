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

#include <geode/geometry/point.h>

#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/surface_edges.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/mesh/helpers/repair_polygon_orientations.h>

#include <geode/tests/common.h>

std::unique_ptr< geode::SurfaceMesh2D > build_bad_orientation_surface()
{
    auto surface = geode::SurfaceMesh2D::create();
    auto builder = geode::SurfaceMeshBuilder2D::create( *surface );
    builder->create_vertices( 12 );
    builder->set_point( 0, { { 0, 0 } } );
    builder->set_point( 1, { { 1, 0 } } );
    builder->set_point( 2, { { 1, 1 } } );
    builder->set_point( 3, { { 2, 0 } } );
    builder->set_point( 4, { { 2, 1 } } );
    builder->set_point( 5, { { 3, 2 } } );
    builder->set_point( 6, { { 3, 1 } } );
    builder->set_point( 7, { { 4, 1 } } );
    builder->set_point( 8, { { 3, 0 } } );
    builder->set_point( 9, { { 4, 0 } } );
    builder->set_point( 10, { { 4, 2 } } );
    builder->set_point( 11, { { 4, 3 } } );
    builder->create_polygon( { 0, 2, 1 } ); // -
    builder->create_polygon( { 1, 3, 2 } ); // +
    builder->create_polygon( { 2, 4, 3 } ); // -
    builder->create_polygon( { 4, 6, 5 } ); // +
    builder->create_polygon( { 5, 7, 6 } ); // -
    builder->create_polygon( { 5, 7, 11 } ); // +
    builder->create_polygon( { 6, 8, 9, 7 } ); // +
    builder->create_polygon( { 9, 7, 11, 10 } ); // -
    builder->create_polygon( { 2, 4, 5 } ); // +
    builder->compute_polygon_adjacencies();
    return surface;
}

void check_vertices_order( const geode::SurfaceMesh2D& surface,
    geode::index_t polygon_id,
    absl::Span< const geode::index_t > vertices_id )
{
    for( const auto v :
        geode::LRange{ surface.nb_polygon_vertices( polygon_id ) } )
    {
        OPENGEODE_EXCEPTION(
            surface.polygon_vertex( { polygon_id, v } ) == vertices_id[v],
            "[Test] Wrong vertex for PolygonVertex ( ", polygon_id, ", ", v,
            " )" );
    }
}

void check_adjacents( const geode::SurfaceMesh2D& surface,
    geode::index_t polygon_id,
    absl::Span< const geode::index_t > adjacents_id )
{
    for( const auto e :
        geode::LRange{ surface.nb_polygon_edges( polygon_id ) } )
    {
        if( adjacents_id[e] == geode::NO_ID )
        {
            OPENGEODE_EXCEPTION( surface.is_edge_on_border( { polygon_id, e } ),
                "[Test] Wrong edge on border for PolygonEdge ( ", polygon_id,
                ", ", e, " )" );
        }
        else
        {
            OPENGEODE_EXCEPTION(
                surface.polygon_adjacent( { polygon_id, e } ).value()
                    == adjacents_id[e],
                "[Test] Wrong adjacent for PolygonEdge ( ", polygon_id, ", ", e,
                " )" );
        }
    }
}

void check_repaired_surface( const geode::SurfaceMesh2D& surface )
{
    check_vertices_order( surface, 0, { 0, 1, 2 } );
    check_adjacents( surface, 0, { geode::NO_ID, 1, geode::NO_ID } );

    check_vertices_order( surface, 1, { 1, 3, 2 } );
    check_adjacents( surface, 1, { geode::NO_ID, 2, 0 } );

    check_vertices_order( surface, 2, { 2, 3, 4 } );
    check_adjacents( surface, 2, { 1, geode::NO_ID, 8 } );

    check_vertices_order( surface, 3, { 4, 6, 5 } );
    check_adjacents( surface, 3, { geode::NO_ID, 4, 8 } );

    check_vertices_order( surface, 4, { 5, 6, 7 } );
    check_adjacents( surface, 4, { 3, 6, 5 } );

    check_vertices_order( surface, 5, { 5, 7, 11 } );
    check_adjacents( surface, 5, { 4, 7, geode::NO_ID } );

    check_vertices_order( surface, 6, { 6, 8, 9, 7 } );
    check_adjacents( surface, 6, { geode::NO_ID, geode::NO_ID, 7, 4 } );

    check_vertices_order( surface, 7, { 9, 10, 11, 7 } );
    check_adjacents( surface, 7, { geode::NO_ID, geode::NO_ID, 5, 6 } );

    check_vertices_order( surface, 8, { 2, 4, 5 } );
    check_adjacents( surface, 8, { 2, 3, geode::NO_ID } );
}

absl::FixedArray< std::array< geode::index_t, 2 > > get_edges(
    const geode::SurfaceMesh2D& surface )
{
    absl::FixedArray< std::array< geode::index_t, 2 > > edges(
        surface.edges().nb_edges() );
    for( const auto e : geode::Range{ surface.edges().nb_edges() } )
    {
        edges[e] = surface.edges().edge_vertices( e );
    }
    return edges;
}

void compare_edges(
    absl::Span< const std::array< geode::index_t, 2 > > edges_before,
    absl::Span< const std::array< geode::index_t, 2 > > edges_after )
{
    OPENGEODE_EXCEPTION( edges_before.size() == edges_after.size(),
        "[Test] Number of edges has been modified" );
    for( const auto e : geode::Range{ edges_before.size() } )
    {
        OPENGEODE_EXCEPTION( edges_before[e] == edges_after[e], "[Test] Edge ",
            e, " has been modified" );
    }
}

void test()
{
    geode::OpenGeodeMesh::initialize();
    auto wrong_surface = build_bad_orientation_surface();
    wrong_surface->enable_edges();
    const auto edges_before = get_edges( *wrong_surface );
    geode::repair_polygon_orientations( *wrong_surface );
    check_repaired_surface( *wrong_surface );
    const auto edges_after = get_edges( *wrong_surface );
    compare_edges( edges_before, edges_after );
}

OPENGEODE_TEST( "repair-polygon-orientations" )