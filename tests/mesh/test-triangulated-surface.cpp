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

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/geode/geode_triangulated_surface_builder.h>
#include <geode/mesh/builder/surface_edges_builder.h>
#include <geode/mesh/core/geode/geode_triangulated_surface.h>
#include <geode/mesh/core/surface_edges.h>
#include <geode/mesh/io/triangulated_surface_input.h>
#include <geode/mesh/io/triangulated_surface_output.h>

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

void test_polygon_adjacencies( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    builder.compute_polygon_adjacencies();
    OPENGEODE_EXCEPTION( !surface.polygon_adjacent( { 0, 0 } ),
        "[Test] TriangulatedSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 0, 1 } ) == 1,
        "[Test] TriangulatedSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 1, 2 } ) == 0,
        "[Test] TriangulatedSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION(
        surface.polygon_adjacent_edge( { 0, 1 } ) == geode::PolygonEdge( 1, 2 ),
        "[Test] TriangulatedSurface adjacent index is not correct" );

    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 2, 2 } ) == 1,
        "[Test] TriangulatedSurface adjacent index is not correct" );
    OPENGEODE_EXCEPTION( !surface.polygon_adjacent( { 2, 0 } ),
        "[Test] TriangulatedSurface adjacent index is not correct" );

    surface.enable_edges();
    OPENGEODE_EXCEPTION( surface.edges().nb_edges() == 7,
        "[Test] TriangulatedSurface should have 7 edges" );
}

void test_permutation( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    std::vector< geode::index_t > vertex_permutation{ 4, 2, 1, 0, 3 };
    builder.permute_vertices( vertex_permutation );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 0 } ) == 3,
        "[Test] Wrong PolygonVertex after vertex permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 1 } ) == 2,
        "[Test] Wrong PolygonVertex after vertex permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 2 } ) == 1,
        "[Test] Wrong PolygonVertex after vertex permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 1, 0 } ) == 2,
        "[Test] Wrong PolygonVertex after vertex permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 1, 1 } ) == 4,
        "[Test] Wrong PolygonVertex after vertex permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 1, 2 } ) == 1,
        "[Test] Wrong PolygonVertex after vertex permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 2, 0 } ) == 4,
        "[Test] Wrong PolygonVertex after vertex permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 2, 1 } ) == 0,
        "[Test] Wrong PolygonVertex after vertex permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 2, 2 } ) == 1,
        "[Test] Wrong PolygonVertex after vertex permute" );

    std::vector< geode::index_t > polygon_permutation{ 2, 0, 1 };
    builder.permute_polygons( polygon_permutation );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 0 } ) == 4,
        "[Test] Wrong PolygonVertex after polygon permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 1 } ) == 0,
        "[Test] Wrong PolygonVertex after polygon permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 2 } ) == 1,
        "[Test] Wrong PolygonVertex after polygon permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 1, 0 } ) == 3,
        "[Test] Wrong PolygonVertex after polygon permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 1, 1 } ) == 2,
        "[Test] Wrong PolygonVertex after polygon permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 1, 2 } ) == 1,
        "[Test] Wrong PolygonVertex after polygon permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 2, 0 } ) == 2,
        "[Test] Wrong PolygonVertex after polygon permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 2, 1 } ) == 4,
        "[Test] Wrong PolygonVertex after polygon permute" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 2, 2 } ) == 1,
        "[Test] Wrong PolygonVertex after polygon permute" );

    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 0, 2 } ) == 2,
        "[Test] Wrong Adjacency after polygon permute" );
    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 2, 1 } ) == 0,
        "[Test] Wrong Adjacency after polygon permute" );
    OPENGEODE_EXCEPTION( surface.polygon_adjacent( { 1, 1 } ) == 2,
        "[Test] Wrong Adjacency after polygon permute" );
    OPENGEODE_EXCEPTION( !surface.polygon_adjacent( { 1, 0 } ),
        "[Test] Wrong Adjacency after polygon permute" );

    const auto polygons_2 = surface.polygons_around_vertex( 2 );
    OPENGEODE_EXCEPTION( polygons_2.size() == 2,
        "[Test] Wrong polygons_2 after polygon permute" );
    OPENGEODE_EXCEPTION( polygons_2[0].polygon_id == 2,
        "[Test] Wrong polygons_2 after polygon permute" );
    OPENGEODE_EXCEPTION( polygons_2[0].vertex_id == 0,
        "[Test] Wrong polygons_2 after polygon permute" );
    OPENGEODE_EXCEPTION( polygons_2[1].polygon_id == 1,
        "[Test] Wrong polygons_2 after polygon permute" );
    OPENGEODE_EXCEPTION( polygons_2[1].vertex_id == 1,
        "[Test] Wrong polygons_2 after polygon permute" );

    const auto polygons_4 = surface.polygons_around_vertex( 4 );
    OPENGEODE_EXCEPTION( polygons_4.size() == 2,
        "[Test] Wrong polygons_4 after polygon permute" );
    OPENGEODE_EXCEPTION( polygons_4[0].polygon_id == 0,
        "[Test] Wrong polygons_4 after polygon permute" );
    OPENGEODE_EXCEPTION( polygons_4[0].vertex_id == 0,
        "[Test] Wrong polygons_4 after polygon permute" );
    OPENGEODE_EXCEPTION( polygons_4[1].polygon_id == 2,
        "[Test] Wrong polygons_4 after polygon permute" );
    OPENGEODE_EXCEPTION( polygons_4[1].vertex_id == 1,
        "[Test] Wrong polygons_4 after polygon permute" );
}

void test_delete_polygon( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    std::vector< bool > to_delete( surface.nb_polygons(), false );
    to_delete.front() = true;
    builder.delete_polygons( to_delete );
    OPENGEODE_EXCEPTION( surface.nb_polygons() == 2,
        "[Test] TriangulatedSurface should have 2 polygons" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 0 } ) == 3,
        "[Test] TriangulatedSurface edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 1 } ) == 2,
        "[Test] TriangulatedSurface edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( surface.polygon_vertex( { 0, 2 } ) == 1,
        "[Test] TriangulatedSurface edge vertex index is not correct" );
    builder.edges_builder().delete_isolated_edges();
    OPENGEODE_EXCEPTION( surface.edges().nb_edges() == 5,
        "[Test] TriangulatedSurface should have 5 edges" );
}

void test_io(
    const geode::TriangulatedSurface3D& surface, absl::string_view filename )
{
    geode::save_triangulated_surface( surface, filename );
    geode::load_triangulated_surface< 3 >( filename );
    const auto reload = geode::load_triangulated_surface< 3 >(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static(), filename );
    for( const auto vertex_id : geode::Range{ surface.nb_vertices() } )
    {
        OPENGEODE_EXCEPTION( surface.point( vertex_id )
                                 .inexact_equal( reload->point( vertex_id ) ),
            "[Test] Wrong reloaded mesh point coordinates." );
    }
}

void test_clone( const geode::TriangulatedSurface3D& surface )
{
    auto attr_from = surface.edges()
                         .edge_attribute_manager()
                         .find_or_create_attribute< geode::VariableAttribute,
                             geode::index_t >( "edge_id", 0 );
    for( const auto e : geode::Range{ surface.edges().nb_edges() } )
    {
        attr_from->set_value( e, e );
    }
    auto surface_clone = surface.clone();
    geode::OpenGeodeTriangulatedSurface3D surface2{ std::move(
        *dynamic_cast< geode::OpenGeodeTriangulatedSurface3D* >(
            surface_clone.get() ) ) };
    OPENGEODE_EXCEPTION( surface2.nb_vertices() == 5,
        "[Test] TriangulatedSurface2 should have 5 vertices" );
    OPENGEODE_EXCEPTION( surface2.edges().nb_edges() == 5,
        "[Test] TriangulatedSurface2 should have 5 edges" );
    OPENGEODE_EXCEPTION( surface2.nb_polygons() == 2,
        "[Test] TriangulatedSurface2 should have 2 polygons" );
    auto attr_to = surface2.edges()
                       .edge_attribute_manager()
                       .find_attribute< geode::index_t >( "edge_id" );
    for( const auto e : geode::Range{ surface.edges().nb_edges() } )
    {
        OPENGEODE_EXCEPTION( attr_from->value( e ) == attr_to->value( e ),
            "[Test] Error in edge attribute transfer during cloning" );
    }
}

void test_delete_all( const geode::TriangulatedSurface3D& triangulated_surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    builder.delete_isolated_vertices();
    OPENGEODE_EXCEPTION( triangulated_surface.nb_vertices() == 3,
        "[Test]TriangulatedSurface should have 3 vertices" );
    OPENGEODE_EXCEPTION( triangulated_surface.edges().nb_edges() == 3,
        "[Test]TriangulatedSurface should have 3 edges" );
    OPENGEODE_EXCEPTION( triangulated_surface.nb_polygons() == 1,
        "[Test]TriangulatedSurface should have 1 polygon" );

    std::vector< bool > to_delete( triangulated_surface.nb_polygons(), true );
    builder.delete_polygons( to_delete );
    OPENGEODE_EXCEPTION( triangulated_surface.nb_vertices() == 3,
        "[Test]TriangulatedSurface should have 3 vertices" );
    builder.edges_builder().delete_isolated_edges();
    OPENGEODE_EXCEPTION( triangulated_surface.edges().nb_edges() == 0,
        "[Test]TriangulatedSurface should have 0 edge" );
    OPENGEODE_EXCEPTION( triangulated_surface.nb_polygons() == 0,
        "[Test]TriangulatedSurface should have 0 polygon" );
    OPENGEODE_EXCEPTION(
        triangulated_surface.polygons_around_vertex( 0 ).empty(),
        "[Test] No more polygon around vertices" );

    builder.delete_isolated_vertices();
    OPENGEODE_EXCEPTION( triangulated_surface.nb_vertices() == 0,
        "[Test]TriangulatedSurface should have 0 vertex" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );

    test_create_vertices( *surface, *builder );
    test_create_polygons( *surface, *builder );
    test_polygon_adjacencies( *surface, *builder );
    test_io( *surface, absl::StrCat( "test.", surface->native_extension() ) );

    test_permutation( *surface, *builder );
    test_delete_polygon( *surface, *builder );
    test_clone( *surface );
}

OPENGEODE_TEST( "triangulated-surface" )