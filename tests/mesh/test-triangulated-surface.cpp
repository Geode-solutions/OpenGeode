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

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/geode/geode_triangulated_surface_builder.hpp>
#include <geode/mesh/builder/surface_edges_builder.hpp>
#include <geode/mesh/core/geode/geode_triangulated_surface.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

#include <geode/tests/common.hpp>

void test_create_vertices( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    builder.create_point( geode::Point3D{ { 0.1, 0.2, 0.3 } } );
    builder.create_point( geode::Point3D{ { 2.1, 9.4, 6.7 } } );
    builder.create_point( geode::Point3D{ { 7.5, 5.2, 6.3 } } );
    builder.create_point( geode::Point3D{ { 8.1, 1.4, 4.7 } } );
    builder.create_point( geode::Point3D{ { 4.7, 2.1, 1.3 } } );
    geode::OpenGeodeMeshException::test( surface.nb_vertices() == 5,
        "TriangulatedSurface should have 5 vertices" );
}

void test_create_polygons( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    builder.create_triangles( 1 );
    builder.create_triangle( { 1, 3, 2 } );
    builder.create_polygon( { 3, 4, 2 } );
    geode::OpenGeodeMeshException::test( surface.nb_polygons() == 3,
        "TriangulatedSurface should have 3 triangles" );
    builder.set_polygon_vertex( { 0, 0 }, 0 );
    builder.set_polygon_vertex( { 0, 1 }, 1 );
    builder.set_polygon_vertex( { 0, 2 }, 2 );
}

void test_polygon_adjacencies( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    builder.compute_polygon_adjacencies();
    geode::OpenGeodeMeshException::test( !surface.polygon_adjacent( { 0, 0 } ),
        "TriangulatedSurface adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent( { 0, 1 } ) == 1,
        "TriangulatedSurface adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent( { 1, 2 } ) == 0,
        "TriangulatedSurface adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent_edge( { 0, 1 } ) == geode::PolygonEdge( 1, 2 ),
        "TriangulatedSurface adjacent index is not correct" );

    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent( { 2, 2 } ) == 1,
        "TriangulatedSurface adjacent index is not correct" );
    geode::OpenGeodeMeshException::test( !surface.polygon_adjacent( { 2, 0 } ),
        "TriangulatedSurface adjacent index is not correct" );

    surface.enable_edges();
    geode::OpenGeodeMeshException::test( surface.edges().nb_edges() == 7,
        "TriangulatedSurface should have 7 edges" );

    const auto around_1 = surface.vertices_around_vertex( 1 );
    geode::OpenGeodeMeshException::test( around_1.size() == 3,
        "There should be 3 vertices around vertex 1, not ", around_1.size() );
    for( const auto vertex_id : around_1 )
    {
        geode::OpenGeodeMeshException::test(
            vertex_id == 0 || vertex_id == 2 || vertex_id == 3,
            "Wrong vertices around vertex 1" );
    }
}

void test_permutation( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    std::vector< geode::index_t > vertex_permutation{ 4, 2, 1, 0, 3 };
    builder.permute_vertices( vertex_permutation );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 0 } ) == 3,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 1 } ) == 2,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 2 } ) == 1,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 0 } ) == 2,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 1 } ) == 4,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 2 } ) == 1,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 0 } ) == 4,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 1 } ) == 0,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 2 } ) == 1,
        "Wrong PolygonVertex after vertex permute" );

    std::vector< geode::index_t > polygon_permutation{ 2, 0, 1 };
    builder.permute_polygons( polygon_permutation );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 0 } ) == 4,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 1 } ) == 0,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 2 } ) == 1,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 0 } ) == 3,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 1 } ) == 2,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 2 } ) == 1,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 0 } ) == 2,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 1 } ) == 4,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 2 } ) == 1,
        "Wrong PolygonVertex after polygon permute" );

    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent( { 0, 2 } ) == 2,
        "Wrong Adjacency after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent( { 2, 1 } ) == 0,
        "Wrong Adjacency after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent( { 1, 1 } ) == 2,
        "Wrong Adjacency after polygon permute" );
    geode::OpenGeodeMeshException::test( !surface.polygon_adjacent( { 1, 0 } ),
        "Wrong Adjacency after polygon permute" );

    const auto polygons_2 = surface.polygons_around_vertex( 2 );
    geode::OpenGeodeMeshException::test(
        polygons_2.size() == 2, "Wrong polygons_2 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_2[0].polygon_id == 2,
        "Wrong polygons_2 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_2[0].vertex_id == 0,
        "Wrong polygons_2 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_2[1].polygon_id == 1,
        "Wrong polygons_2 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_2[1].vertex_id == 1,
        "Wrong polygons_2 after polygon permute" );

    const auto polygons_4 = surface.polygons_around_vertex( 4 );
    geode::OpenGeodeMeshException::test(
        polygons_4.size() == 2, "Wrong polygons_4 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_4[0].polygon_id == 0,
        "Wrong polygons_4 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_4[0].vertex_id == 0,
        "Wrong polygons_4 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_4[1].polygon_id == 2,
        "Wrong polygons_4 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_4[1].vertex_id == 1,
        "Wrong polygons_4 after polygon permute" );
}

void test_delete_polygon( const geode::TriangulatedSurface3D& surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    std::vector< bool > to_delete( surface.nb_polygons(), false );
    to_delete.front() = true;
    builder.delete_polygons( to_delete );
    geode::OpenGeodeMeshException::test( surface.nb_polygons() == 2,
        "TriangulatedSurface should have 2 polygons" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 0 } ) == 3,
        "TriangulatedSurface edge vertex index is not correct" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 1 } ) == 2,
        "TriangulatedSurface edge vertex index is not correct" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 2 } ) == 1,
        "TriangulatedSurface edge vertex index is not correct" );
    builder.edges_builder().delete_isolated_edges();
    geode::OpenGeodeMeshException::test( surface.edges().nb_edges() == 5,
        "TriangulatedSurface should have 5 edges" );
}

void test_io(
    const geode::TriangulatedSurface3D& surface, std::string_view filename )
{
    geode::save_triangulated_surface( surface, filename );
    const auto relaod = geode::load_triangulated_surface< 3 >( filename );
    geode_unused( relaod );
    const auto surface2 = geode::load_triangulated_surface< 3 >(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static(), filename );
    for( const auto vertex_id : geode::Range{ surface.nb_vertices() } )
    {
        geode::OpenGeodeMeshException::test(
            surface.point( vertex_id )
                .inexact_equal( surface2->point( vertex_id ) ),
            "Wrong reloaded mesh point coordinates." );
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
    geode::OpenGeodeMeshException::test( surface2.nb_vertices() == 5,
        "TriangulatedSurface2 should have 5 vertices" );
    geode::OpenGeodeMeshException::test( surface2.edges().nb_edges() == 5,
        "TriangulatedSurface2 should have 5 edges" );
    geode::OpenGeodeMeshException::test( surface2.nb_polygons() == 2,
        "TriangulatedSurface2 should have 2 polygons" );
    auto attr_to = surface2.edges()
                       .edge_attribute_manager()
                       .find_attribute< geode::index_t >( "edge_id" );
    for( const auto e : geode::Range{ surface.edges().nb_edges() } )
    {
        geode::OpenGeodeMeshException::test(
            attr_from->value( e ) == attr_to->value( e ),
            "Error in edge attribute transfer during cloning" );
    }
}

void test_delete_all( const geode::TriangulatedSurface3D& triangulated_surface,
    geode::TriangulatedSurfaceBuilder3D& builder )
{
    builder.delete_isolated_vertices();
    geode::OpenGeodeMeshException::test(
        triangulated_surface.nb_vertices() == 3,
        "[Test]TriangulatedSurface should have 3 vertices" );
    geode::OpenGeodeMeshException::test(
        triangulated_surface.edges().nb_edges() == 3,
        "[Test]TriangulatedSurface should have 3 edges" );
    geode::OpenGeodeMeshException::test(
        triangulated_surface.nb_polygons() == 1,
        "[Test]TriangulatedSurface should have 1 polygon" );

    std::vector< bool > to_delete( triangulated_surface.nb_polygons(), true );
    builder.delete_polygons( to_delete );
    geode::OpenGeodeMeshException::test(
        triangulated_surface.nb_vertices() == 3,
        "[Test]TriangulatedSurface should have 3 vertices" );
    builder.edges_builder().delete_isolated_edges();
    geode::OpenGeodeMeshException::test(
        triangulated_surface.edges().nb_edges() == 0,
        "[Test]TriangulatedSurface should have 0 edge" );
    geode::OpenGeodeMeshException::test(
        triangulated_surface.nb_polygons() == 0,
        "[Test]TriangulatedSurface should have 0 polygon" );
    geode::OpenGeodeMeshException::test(
        triangulated_surface.polygons_around_vertex( 0 ).empty(),
        "No more polygon around vertices" );

    builder.delete_isolated_vertices();
    geode::OpenGeodeMeshException::test(
        triangulated_surface.nb_vertices() == 0,
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