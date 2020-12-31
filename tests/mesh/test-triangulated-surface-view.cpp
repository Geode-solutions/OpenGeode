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

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/triangulated_surface_view_builder.h>
#include <geode/mesh/core/triangulated_surface_view.h>

#include <geode/tests/common.h>

void test_create_vertices( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    builder.create_point( { { 0.1, 0.2, 0.3 } } );
    builder.create_point( { { 2.1, 9.4, 6.7 } } );
    builder.create_point( { { 7.5, 5.2, 6.3 } } );
    builder.create_point( { { 8.1, 1.4, 4.7 } } );
    builder.create_point( { { 4.7, 2.1, 1.3 } } );
    OPENGEODE_EXCEPTION( surface.nb_vertices() == 5,
        "[Test] TriangulatedSurface should have 5 vertices" );
}

void test_create_viewed_vertices(
    const geode::TriangulatedSurfaceView3D& surface,
    geode::TriangulatedSurfaceViewBuilder3D& builder )
{
    builder.add_viewed_vertex( 1 );
    builder.add_viewed_vertex( 2 );
    builder.add_viewed_vertex( 3 );
    OPENGEODE_EXCEPTION( surface.nb_vertices() == 3,
        "[Test] TriangulatedSurfaceView should have 3 vertices" );
    const geode::Point3D answer{ { 2.1, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( surface.point( 0 ) == answer,
        "[Test] TriangulatedSurfaceView point is not correct" );
}

void test_create_viewed_polygons(
    const geode::TriangulatedSurfaceView3D& surface,
    geode::TriangulatedSurfaceViewBuilder3D& builder )
{
    builder.add_viewed_triangle( 1 );
    builder.add_viewed_triangle( 2 );
    OPENGEODE_EXCEPTION( surface.nb_polygons() == 2,
        "[Test] TriangulatedSurfaceView should have 2 polygons" );
    OPENGEODE_EXCEPTION( surface.nb_vertices() == 4,
        "[Test] TriangulatedSurfaceView should have 4 vertices" );
    OPENGEODE_EXCEPTION( surface.viewed_triangle( 0 ) == 1,
        "[Test] TriangulatedSurfaceView triangle is not correct" );

    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 2 } ) == 1,
        "[Test] TriangulatedSurfaceView PolygonVertex is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 1, 1 } ) == 3,
        "[Test] TriangulatedSurfaceView PolygonVertex is not correct" );
}

void test_create_polygons( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    builder.create_triangles( 1 );
    builder.create_triangle( { 1, 3, 2 } );
    builder.create_polygon( { 3, 4, 2 } );
    OPENGEODE_EXCEPTION( surface.nb_polygons() == 3,
        "[Test] TriangulatedSurface should have 3 triangles" );
    builder.set_polygon_vertex( { 0, 0 }, 0 );
    builder.set_polygon_vertex( { 0, 1 }, 1 );
    builder.set_polygon_vertex( { 0, 2 }, 2 );
}

void test_polygon_adjacencies( const geode::TriangulatedSurface3D& surface )
{
    OPENGEODE_EXCEPTION( !surface.polygon_adjacent( { 0, 0 } ),
        "[Test] TriangulatedSurfaceView adjacent index is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 0, 1 } ) == 1,
        "[Test] TriangulatedSurfaceView adjacent index is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 1, 2 } ) == 0,
        "[Test] TriangulatedSurfaceView adjacent index is not correct" );
    OPENGEODE_EXCEPTION(
        surface.polygon_adjacent_edge( { 0, 1 } ) == geode::PolygonEdge( 1, 2 ),
        "[Test] TriangulatedSurfaceView adjacent index is not correct" );
}

std::unique_ptr< geode::TriangulatedSurface3D > create_surface()
{
    auto surface = geode::TriangulatedSurface3D::create();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );

    test_create_vertices( *surface, *builder );
    test_create_polygons( *surface, *builder );
    builder->compute_polygon_adjacencies();
    return surface;
}

void test()
{
    auto surface = create_surface();
    geode::TriangulatedSurfaceView3D view{ *surface };
    auto builder = geode::TriangulatedSurfaceViewBuilder3D::create( view );

    test_create_viewed_vertices( view, *builder );
    test_create_viewed_polygons( view, *builder );
    test_polygon_adjacencies( view );
}

OPENGEODE_TEST( "triangulated-surface-view" )