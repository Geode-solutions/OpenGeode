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

#include <geode/basic/algorithm.h>
#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>
#include <geode/basic/point.h>
#include <geode/basic/vector.h>

#include <geode/mesh/builder/geode_polygonal_surface_builder.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/io/polygonal_surface_input.h>
#include <geode/mesh/io/polygonal_surface_output.h>

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
    OPENGEODE_EXCEPTION( polygonal_surface.nb_vertices() == 7,
        "PolygonalSurface should have 7 vertices" );
}

void test_create_vertex_attribute(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    auto attribute =
        polygonal_surface.vertex_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, double >(
                "test" );
    for( geode::index_t v = 0; v < polygonal_surface.nb_vertices(); v++ )
    {
        attribute->value( v ) = v;
    }
}

void test_delete_vertex( const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    std::vector< bool > to_delete( polygonal_surface.nb_vertices(), false );
    to_delete.front() = true;
    builder.delete_vertices( to_delete );
    OPENGEODE_EXCEPTION( polygonal_surface.nb_vertices() == 6,
        "PolygonalSurface should have 6 vertices" );
    geode::Point3D answer{ { 2.1, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( polygonal_surface.point( 0 ) == answer,
        "PolygonalSurface vertex coordinates are not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.nb_polygons() == 2,
        "PolygonalSurface should have 2 polygons" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_adjacent( { 1, 3 } ) == 0,
        "PolygonalSurface adjacent index is not correct" );
}

void test_create_polygons( const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    builder.create_polygon( { 0, 1, 2 } );
    builder.create_polygon( { 1, 3, 4, 2 } );
    builder.create_polygon( { 1, 5, 6, 3 } );
    OPENGEODE_EXCEPTION( polygonal_surface.nb_polygons() == 3,
        "PolygonalSurface should have 3 polygons" );
}

void test_polygon_adjacencies(
    const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    builder.compute_polygon_adjacencies();
    OPENGEODE_EXCEPTION(
        polygonal_surface.polygon_adjacent( { 0, 0 } ) == geode::NO_ID,
        "PolygonalSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_adjacent( { 0, 1 } ) == 1,
        "PolygonalSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_adjacent( { 1, 3 } ) == 0,
        "PolygonalSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_adjacent_edge( { 0, 1 } )
                             == geode::PolygonEdge( 1, 3 ),
        "PolygonalSurface adjacent index is not correct" );

    OPENGEODE_EXCEPTION(
        polygonal_surface.polygon_adjacent( { 2, 0 } ) == geode::NO_ID,
        "PolygonalSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_adjacent( { 2, 3 } ) == 1,
        "PolygonalSurface adjacent index is not correct" );

    OPENGEODE_EXCEPTION(
        polygonal_surface.polygons_around_vertex( 1 ).size() == 3,
        "PolygonalSurface should have 3 polygons around this vertex" );
}

void test_polygon_edges_on_borders(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    auto result = polygonal_surface.polygon_edges_on_border( 0 );
    OPENGEODE_EXCEPTION( result.size() == 2,
        "Number of polygon edges on border index is not correct" );
    OPENGEODE_EXCEPTION( contain( result, geode::PolygonEdge( 0, 0 ) )
                             && contain( result, geode::PolygonEdge( 0, 2 ) ),
        "Polygon edge indices on border index is not correct" );
}

void test_previous_next_on_border(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    OPENGEODE_EXCEPTION( polygonal_surface.previous_on_border( { 0, 0 } )
                             == geode::PolygonEdge( 0, 2 ),
        "Previous edge on border index is not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.next_on_border( { 0, 0 } )
                             == geode::PolygonEdge( 2, 0 ),
        "Next edge on border index is not correct" );
}

void test_polygon_edge_requests(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_edge_length( { 0, 0 } )
                             == std::sqrt( 2 * 2 + 9.2 * 9.2 + 6.4 * 6.4 ),
        "Polygon edge length is not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_edge_barycenter( { 0, 0 } )
                             == geode::Point3D( { 1.1, 4.8, 3.5 } ),
        "Polygon edge barycenter is not correct" );
    OPENGEODE_EXCEPTION(
        polygonal_surface.polygon_edge_vertex( { 0, 0 }, 1 ) == 1,
        "Polygon edge vertex index is not correct" );
    bool found;
    geode::PolygonEdge polygon_edge;
    std::tie( found, std::ignore ) =
        polygonal_surface.polygon_edge_from_vertices( 3, 5 );
    OPENGEODE_EXCEPTION( !found, "Polygon edge from vertices is not correct" );
    std::tie( found, polygon_edge ) =
        polygonal_surface.polygon_edge_from_vertices( 0, 1 );
    OPENGEODE_EXCEPTION( found, "Polygon edge from vertices is not correct" );
    OPENGEODE_EXCEPTION( polygon_edge == geode::PolygonEdge( 0, 0 ),
        "Polygon edge from vertices is not correct" );
    std::tie( found, std::ignore ) =
        polygonal_surface.polygon_edge_from_vertices( 1, 0 );
    OPENGEODE_EXCEPTION( !found, "Polygon edge from vertices is not correct" );
}

void test_delete_polygon( const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    std::vector< bool > to_delete( polygonal_surface.nb_polygons(), false );
    to_delete.front() = true;
    builder.delete_polygons( to_delete );
    OPENGEODE_EXCEPTION( polygonal_surface.nb_polygons() == 1,
        "PolygonalSurface should have 1 polygon" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_vertex( { 0, 0 } ) == 0,
        "PolygonalSurface edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_vertex( { 0, 1 } ) == 4,
        "PolygonalSurface edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_vertex( { 0, 2 } ) == 5,
        "PolygonalSurface edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_vertex( { 0, 3 } ) == 2,
        "PolygonalSurface edge vertex index is not correct" );
}

void test_polygon_barycenter(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    geode::Point3D answer( { 5.6, 4.525, 4.75 } );
    OPENGEODE_EXCEPTION( polygonal_surface.polygon_barycenter( 1 ) == answer,
        "PolygonalSurface polygon barycenter is not correct" );
}

void test_polygon_area()
{
    auto polygonal_surface = geode::PolygonalSurface2D::create(
        geode::OpenGeodePolygonalSurface2D::type_name_static() );
    auto builder =
        geode::PolygonalSurfaceBuilder2D::create( *polygonal_surface );
    double a{ 6.0 };
    double b{ 8.0 };
    double c{ 4.0 };

    builder->create_point( { { 0.0, 0.0 } } );
    builder->create_point( { { a, 0.0 } } );
    builder->create_point( { { b, c } } );
    builder->create_point( { { 0.0, c } } );

    builder->create_polygon( { 0, 1, 2, 3 } );

    double answer{ 28. };

    OPENGEODE_EXCEPTION( polygonal_surface->polygon_area( 0 ) == answer,
        "PolygonalSurface polygon area is not correct" );
}

void test_polygon_normal()
{
    auto polygonal_surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::type_name_static() );
    auto builder =
        geode::PolygonalSurfaceBuilder3D::create( *polygonal_surface );
    double a{ 6.0 };
    double b{ 8.0 };
    double c{ 4.0 };

    builder->create_point( { { 0.0, 0.0, 0.0 } } );
    builder->create_point( { { a, 0.0, 0.0 } } );
    builder->create_point( { { b, c, 0.0 } } );
    builder->create_point( { { 0.0, c, 0.0 } } );

    builder->create_polygon( { 0, 1, 2, 3 } );

    geode::Vector3D answer{ { 0., 0., 1. } };

    OPENGEODE_EXCEPTION( polygonal_surface->polygon_normal( 0 ) == answer,
        "PolygonalSurface polygon normal is not correct" );
}

void test_polygon_vertex_normal()
{
    auto polygonal_surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::type_name_static() );
    auto builder =
        geode::PolygonalSurfaceBuilder3D::create( *polygonal_surface );

    builder->create_point( { { 0.0, 0.0, 0.0 } } );
    builder->create_point( { { 1.0, 0.0, 0.0 } } );
    builder->create_point( { { 0.0, 1.0, 1.0 } } );
    builder->create_point( { { 0.0, -1.0, 1.0 } } );

    builder->create_polygon( { 0, 1, 2 } );
    builder->create_polygon( { 0, 3, 1 } );

    DEBUG(polygonal_surface->polygon_normal(0) );
    DEBUG(polygonal_surface->polygon_normal(1) );
    DEBUG(polygonal_surface->polygon_vertex_normal(0) );

    geode::Vector3D answer{ { 0., 0., 1. } };

    OPENGEODE_EXCEPTION( polygonal_surface->polygon_vertex_normal( 0 ) == answer,
        "PolygonalSurface polygon vertex normal is not correct" );
}

void test_io( const geode::PolygonalSurface3D& polygonal_surface,
    const std::string& filename )
{
    save_polygonal_surface( polygonal_surface, filename );
    auto new_polygonal_surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::type_name_static() );
    load_polygonal_surface( *new_polygonal_surface, filename );
}

int main()
{
    using namespace geode;

    try
    {
        auto polygonal_surface = PolygonalSurface3D::create(
            OpenGeodePolygonalSurface3D::type_name_static() );
        auto builder = PolygonalSurfaceBuilder3D::create( *polygonal_surface );

        test_create_vertices( *polygonal_surface, *builder );
        test_create_vertex_attribute( *polygonal_surface );
        test_create_polygons( *polygonal_surface, *builder );
        test_polygon_adjacencies( *polygonal_surface, *builder );
        test_polygon_edges_on_borders( *polygonal_surface );
        test_previous_next_on_border( *polygonal_surface );
        test_polygon_edge_requests( *polygonal_surface );
        test_polygon_barycenter( *polygonal_surface );
        test_polygon_area();
        test_polygon_normal();
        test_polygon_vertex_normal();

        auto base_file = "test." + polygonal_surface->native_extension();
        test_io( *polygonal_surface, base_file );

        test_delete_vertex( *polygonal_surface, *builder );
        test_delete_polygon( *polygonal_surface, *builder );

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
