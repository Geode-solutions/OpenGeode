/*
 * Copyright (c) 2019 Geode-solutions
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
#include <geode/basic/logger.h>
#include <geode/basic/point.h>

#include <geode/mesh/builder/geode_triangulated_surface_builder.h>
#include <geode/mesh/core/geode_triangulated_surface.h>
#include <geode/mesh/io/triangulated_surface_input.h>
#include <geode/mesh/io/triangulated_surface_output.h>

void test_create_vertices( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    builder.create_point( { { 0.1, 0.2, 0.3 } } );
    builder.create_point( { { 2.1, 9.4, 6.7 } } );
    builder.create_point( { { 7.5, 5.2, 6.3 } } );
    builder.create_point( { { 8.1, 1.4, 4.7 } } );
    builder.create_point( { { 4.7, 2.1, 1.3 } } );
    OPENGEODE_EXCEPTION( surface.nb_vertices() == 5,
        "[Test] TriangulatedSurface should have 5 vertices" );
}

void test_create_polygons( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    builder.create_triangle( { 0, 1, 2 } );
    builder.create_triangle( { 1, 3, 2 } );
    builder.create_polygon( { 3, 4, 2 } );
    OPENGEODE_EXCEPTION( surface.nb_polygons() == 3,
        "[Test] TriangulatedSurface should have 3 triangles" );
}

void test_polygon_adjacencies( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    builder.compute_polygon_adjacencies();
    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 0, 0 } ) == geode::NO_ID,
        "[Test] TriangulatedSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 0, 1 } ) == 1,
        "[Test] TriangulatedSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 1, 2 } ) == 0,
        "[Test] TriangulatedSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION(
        surface.polygon_adjacent_edge( { 0, 1 } ) == geode::PolygonEdge( 1, 2 ),
        "[Test] TriangulatedSurface adjacent index is not correct" );

    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 2, 2 } ) == 1,
        "[Test] TriangulatedSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 2, 0 } ) == geode::NO_ID,
        "[Test] TriangulatedSurface adjacent index is not correct" );
}

void test_delete_vertex( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    std::vector< bool > to_delete( surface.nb_vertices(), false );
    to_delete.front() = true;
    builder.delete_vertices( to_delete );
    OPENGEODE_EXCEPTION( surface.nb_vertices() == 4,
        "[Test] TriangulatedSurface should have 4 vertices" );
    geode::Point3D answer{ { 2.1, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( surface.point( 0 ) == answer,
        "[Test] TriangulatedSurface vertex coordinates are not correct" );
    OPENGEODE_EXCEPTION( surface.nb_polygons() == 2,
        "[Test] TriangulatedSurface should have 2 polygons" );
    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 1, 2 } ) == 0,
        "[Test] TriangulatedSurface adjacent index is not correct" );
}

void test_delete_polygon( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    std::vector< bool > to_delete( surface.nb_polygons(), false );
    to_delete.front() = true;
    builder.delete_polygons( to_delete );
    OPENGEODE_EXCEPTION( surface.nb_polygons() == 1,
        "[Test] TriangulatedSurface should have 1 polygon" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 0 } ) == 2,
        "[Test] TriangulatedSurface edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 1 } ) == 3,
        "[Test] TriangulatedSurface edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 2 } ) == 1,
        "[Test] TriangulatedSurface edge vertex index is not correct" );
}

void test_io(
    const geode::TriangulatedSurface3D& surface, const std::string& filename )
{
    save_triangulated_surface( surface, filename );
    auto new_surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::type_name_static() );
    load_triangulated_surface( *new_surface, filename );
}

int main()
{
    using namespace geode;

    try
    {
        auto surface = TriangulatedSurface3D::create(
            OpenGeodeTriangulatedSurface3D::type_name_static() );
        auto builder = TriangulatedSurfaceBuilder3D::create( *surface );

        test_create_vertices( *surface, *builder );
        test_create_polygons( *surface, *builder );
        test_polygon_adjacencies( *surface, *builder );
        auto base_file = "test." + surface->native_extension();
        test_io( *surface, base_file );

        test_delete_vertex( *surface, *builder );
        test_delete_polygon( *surface, *builder );

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
