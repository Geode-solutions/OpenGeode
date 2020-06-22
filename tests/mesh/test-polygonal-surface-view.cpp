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

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>
#include <geode/geometry/vector.h>

#include <geode/mesh/builder/polygonal_surface_view_builder.h>
#include <geode/mesh/core/polygonal_surface_view.h>

#include <geode/tests/common.h>

void test_create_vertices( const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    builder.create_point( { { 0.1, 0.2, 0.3 } } );
    builder.create_point( { { 2.1, 9.4, 6.7 } } );
    builder.create_point( { { 7.5, 5.2, 6.3 } } );
    builder.create_point( { { 8.1, 1.4, 4.7 } } );
    builder.create_point( { { 4.7, 2.1, 1.3 } } );
    builder.create_point( { { 9.3, 5.3, 6.7 } } );
    builder.create_point( { { 7.5, 4.2, 2.8 } } );
    OPENGEODE_EXCEPTION( polygonal_surface.isolated_vertex( 0 ),
        "[Test] Vertices should be isolated before polygons creation" );
    OPENGEODE_EXCEPTION( polygonal_surface.nb_vertices() == 7,
        "[Test] PolygonalSurface should have 7 vertices" );
}

void test_create_viewed_vertices( const geode::PolygonalSurfaceView3D& surface,
    geode::PolygonalSurfaceViewBuilder3D& builder )
{
    builder.add_viewed_vertex( 1 );
    builder.add_viewed_vertex( 2 );
    builder.add_viewed_vertex( 3 );
    builder.add_viewed_vertex( 4 );
    OPENGEODE_EXCEPTION( surface.nb_vertices() == 4,
        "[Test] PolygonalSurfaceView should have 4 vertices" );
    const geode::Point3D answer{ { 8.1, 1.4, 4.7 } };
    OPENGEODE_EXCEPTION( surface.point( 2 ) == answer,
        "[Test] PolygonalSurfaceView point is not correct" );
}

void test_create_viewed_polygons( const geode::PolygonalSurfaceView3D& surface,
    geode::PolygonalSurfaceViewBuilder3D& builder )
{
    builder.add_viewed_polygon( 1 );
    builder.add_viewed_polygon( 2 );
    OPENGEODE_EXCEPTION( surface.nb_polygons() == 2,
        "[Test] PolygonalSurfaceView should have 2 polygons" );
    OPENGEODE_EXCEPTION( surface.nb_vertices() == 6,
        "[Test] PolygonalSurfaceView should have 6 vertices" );
    OPENGEODE_EXCEPTION( surface.nb_edges() == 7,
        "[Test] PolygonalSurfaceView should have 7 edges" );
    OPENGEODE_EXCEPTION( surface.viewed_polygon( 0 ) == 1,
        "[Test] PolygonalSurfaceView polygon is not correct" );

    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 2 } ) == 3,
        "[Test] PolygonalSurfaceView PolygonVertex is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 1, 1 } ) == 4,
        "[Test] PolygonalSurfaceView PolygonVertex is not correct" );

    OPENGEODE_EXCEPTION( !surface.isolated_edge( 4 ),
        "[Test]  PolygonalSurfaceView isolated edge is not correct" );
}

void test_create_polygons( const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    builder.create_polygon( { 0, 1, 2 } );
    builder.create_polygon( { 1, 3, 4, 2 } );
    builder.create_polygon( { 1, 5, 6, 3 } );
    OPENGEODE_EXCEPTION( !polygonal_surface.isolated_vertex( 0 ),
        "[Test] Vertices should not be isolated after polygons creation" );
    OPENGEODE_EXCEPTION( polygonal_surface.nb_polygons() == 3,
        "[Test] PolygonalSurface should have 3 polygons" );
    OPENGEODE_EXCEPTION( polygonal_surface.nb_edges() == 9,
        "[Test] PolygonalSurface should have 9 edges" );
}

void test_polygon_adjacencies(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_adjacent( { 0, 0 } ) == 1,
        "[Test] PolygonalSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( !polygonal_surface.polygon_adjacent( { 0, 1 } ),
        "[Test] PolygonalSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_adjacent( { 1, 3 } ) == 0,
        "[Test] PolygonalSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_adjacent_edge( { 0, 0 } )
                             == geode::PolygonEdge( 1, 3 ),
        "[Test] PolygonalSurface adjacent index is not correct" );

    OPENGEODE_EXCEPTION(
        polygonal_surface.polygons_around_vertex( 0 ).size() == 2,
        "[Test] PolygonalSurface polygons around vertex index is not correct" );
    const auto polys = polygonal_surface.polygons_around_vertex( 1 );
    OPENGEODE_EXCEPTION( polys.size() == 1,
        "[Test] PolygonalSurface polygons around vertex index is not correct" );
    OPENGEODE_EXCEPTION( polys.front().polygon_id == 0,
        "[Test] PolygonalSurface polygons around vertex index is not correct" );
}

std::unique_ptr< geode::PolygonalSurface3D > create_surface()
{
    auto surface = geode::PolygonalSurface3D::create();
    auto builder = geode::PolygonalSurfaceBuilder3D::create( *surface );

    test_create_vertices( *surface, *builder );
    test_create_polygons( *surface, *builder );
    builder->compute_polygon_adjacencies();
    return surface;
}

void test()
{
    auto surface = create_surface();
    geode::PolygonalSurfaceView3D view{ *surface };
    auto builder = geode::PolygonalSurfaceViewBuilder3D::create( view );

    test_create_viewed_vertices( view, *builder );
    test_create_viewed_polygons( view, *builder );
    test_polygon_adjacencies( view );
}

OPENGEODE_TEST( "polygonal-surface-view" )