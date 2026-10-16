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
#include <geode/basic/mapping.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/geode/geode_triangulated_surface_builder.hpp>
#include <geode/mesh/builder/surface_edges_builder.hpp>
#include <geode/mesh/core/geode/geode_triangulated_surface.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

#include <geode/tests/common.hpp>

void test_reserve_with_edges()
{
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    surface->enable_edges();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 4 );
    builder->create_triangle( { 0, 1, 2 } );
    builder->reserve_triangles( 10 );
    builder->edges_builder().reserve_edges( 10 );
    geode::OpenGeodeMeshException::test( surface->edges().nb_edges() == 3,
        "[reserve] Reserving should not create edges" );
    builder->create_triangle( { 1, 3, 2 } );
    geode::OpenGeodeMeshException::test( surface->edges().nb_edges() == 5,
        "[reserve] Wrong number of edges after reserve" );
}

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
    const auto point_attributes =
        relaod->vertex_attribute_manager().attribute_ids_matching_name(
            "points" );
    geode::OpenGeodeMeshException::test( point_attributes.has_value(),
        "TriangulatedSurface should have points attribute" );
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

void test_backward_io( const std::string& filename )
{
    const auto surface = geode::load_triangulated_surface< 3 >( filename );
    geode::OpenGeodeMeshException::test( surface->nb_vertices() == 5,
        "Backward TriangulatedSurface should have 5 vertices" );
    geode::OpenGeodeMeshException::test( surface->nb_polygons() == 3,
        "Backward TriangulatedSurface should have 3 polygons" );
    geode::OpenGeodeMeshException::test(
        surface->point( 0 ) == geode::Point3D{ { 0.1, 0.2, 0.3 } },
        "Backward TriangulatedSurface should have point ( 0.1, 0.2, 0.3 ) at "
        "index 0" );
    geode::OpenGeodeMeshException::test(
        surface->polygon_adjacent( { 0, 1 } ) == 1,
        "TriangulatedSurface adjacent index is not correct" );
    geode::OpenGeodeMeshException::test( surface->edges().nb_edges() == 7,
        "Backward TriangulatedSurface should have 7 edges" );
    for( const auto* name : { "triangle_vertices", "triangle_adjacents" } )
    {
        geode::OpenGeodeMeshException::test(
            !surface->polygon_attribute_manager()
                .attribute_ids_matching_name( name )
                .has_value(),
            "Backward TriangulatedSurface should not have legacy attribute ",
            name );
    }
    for( const auto* name :
        { "polygon_around_vertex", "polygons_around_vertex" } )
    {
        geode::OpenGeodeMeshException::test(
            !surface->vertex_attribute_manager()
                .attribute_ids_matching_name( name )
                .has_value(),
            "Backward TriangulatedSurface should not have legacy attribute ",
            name );
    }
    geode::OpenGeodeMeshException::test(
        surface->polygon_around_vertex( 0 ).value().polygon_id == 0,
        "Wrong polygon_around_vertex for backward triangulated surface" );
    geode::OpenGeodeMeshException::test(
        surface->polygons_around_vertex( 0 ).size() == 1,
        "Wrong polygons_around_vertex for backward triangulated surface" );
}
void test_clone( const geode::TriangulatedSurface3D& surface )
{
    geode::AttributeProperties attribute_properties;
    attribute_properties.assignable = false;
    attribute_properties.interpolable = true;
    attribute_properties.transferable = true;
    geode::AttributeValues< geode::index_t > attribute_values;
    attribute_values.default_value = 0;
    attribute_values.no_value = geode::NO_ID;
    auto attr_from_id =
        surface.edges()
            .edge_attribute_manager()
            .create_attribute< geode::VariableAttribute, geode::index_t >(
                "edge_id", attribute_values, attribute_properties );
    auto attr_from =
        surface.edges()
            .edge_attribute_manager()
            .find_attribute< geode::VariableAttribute, geode::index_t >(
                attr_from_id );
    for( const auto e : geode::Range{ surface.edges().nb_edges() } )
    {
        attr_from->set_value( e, e );
    }
    auto surface_clone = surface.clone();
    for( const auto v : geode::Range{ surface_clone->nb_vertices() } )
    {
        geode::OpenGeodeMeshException::test(
            surface.polygons_around_vertex( v )
                == surface_clone->polygons_around_vertex( v ),
            "Polyhedra around vertex should match during cloning" );
    }
    for( const auto v : geode::Range{ surface_clone->nb_vertices() } )
    {
        geode::OpenGeodeMeshException::test(
            surface_clone->point( v ) == surface.point( v ),
            "Wrong cloned mesh point coordinates." );
    }
    for( const auto p : geode::Range{ surface_clone->nb_polygons() } )
    {
        for( const auto e : geode::LRange{ 3 } )
        {
            for( const auto v : geode::LRange{ 2 } )
            {
                geode::OpenGeodeMeshException::test(
                    surface_clone->polygon_edge_vertex( { p, e }, v )
                        == surface.polygon_edge_vertex( { p, e }, v ),
                    "polyhedron_facet_vertex should match" );
            }
        }
    }
    geode::OpenGeodeTriangulatedSurface3D surface2{ std::move(
        *dynamic_cast< geode::OpenGeodeTriangulatedSurface3D* >(
            surface_clone.get() ) ) };
    geode::OpenGeodeMeshException::test( surface2.nb_vertices() == 5,
        "TriangulatedSurface2 should have 5 vertices" );
    geode::OpenGeodeMeshException::test( surface2.edges().nb_edges() == 5,
        "TriangulatedSurface2 should have 5 edges" );
    geode::OpenGeodeMeshException::test( surface2.nb_polygons() == 2,
        "TriangulatedSurface2 should have 2 polygons" );
    auto attr_to =
        surface2.edges()
            .edge_attribute_manager()
            .find_read_only_attribute< geode::index_t >( attr_from_id );
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

void check_no_legacy_attributes( const geode::AttributeManager& manager,
    std::initializer_list< const char* > names )
{
    for( const auto* name : names )
    {
        geode::OpenGeodeMeshException::test(
            !manager.attribute_ids_matching_name( name ).has_value(),
            "Legacy attribute ", name, " should have been removed" );
    }
}

void check_user_value(
    const geode::AttributeManager& manager, geode::index_t element )
{
    const auto ids = manager.attribute_ids_matching_name( "user_value" );
    geode::OpenGeodeMeshException::test(
        ids.has_value() && ids->size() == 1, "user_value should be kept" );
    const auto attribute =
        manager.find_read_only_attribute< double >( ids->at( 0 ) );
    geode::OpenGeodeMeshException::test(
        attribute->value( element ) == 1.5 * element,
        "Wrong user_value at element ", element );
}

void check_v18_5( const geode::TriangulatedSurface3D& surface )
{
    geode::OpenGeodeMeshException::test(
        surface.nb_vertices() == 5 && surface.nb_polygons() == 3,
        "v18_5 TriangulatedSurface has wrong number of elements" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 1 } ) == 3,
        "v18_5 TriangulatedSurface has wrong polygon vertex" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent( { 0, 1 } ) == 1,
        "v18_5 TriangulatedSurface has wrong polygon adjacent" );
    geode::OpenGeodeMeshException::test(
        surface.polygons_around_vertex( 1 ).size() == 3,
        "v18_5 TriangulatedSurface has wrong polygons around vertex" );
    geode::OpenGeodeMeshException::test( surface.edges().nb_edges() == 7,
        "v18_5 TriangulatedSurface has wrong number of edges" );
    check_user_value( surface.vertex_attribute_manager(), 4 );
    check_user_value( surface.polygon_attribute_manager(), 2 );
    check_user_value( surface.edges().edge_attribute_manager(), 6 );
    check_no_legacy_attributes( surface.vertex_attribute_manager(),
        { "polygon_around_vertex", "polygons_around_vertex" } );
    check_no_legacy_attributes( surface.polygon_attribute_manager(),
        { "triangle_vertices", "triangle_adjacents" } );
    check_no_legacy_attributes( surface.edges().edge_attribute_manager(),
        { "counter", "facet_vertices" } );
}

void test_backward_v18_5()
{
    const auto surface = geode::load_triangulated_surface< 3 >(
        absl::StrCat( geode::DATA_PATH, "backward_io/v18_5/v18_5.og_tsf3d" ) );
    check_v18_5( *surface );
    const auto filename =
        absl::StrCat( "v18_5_reload.", surface->native_extension() );
    geode::save_triangulated_surface( *surface, filename );
    check_v18_5( *geode::load_triangulated_surface< 3 >( filename ) );
}

void test_set_polygon_vertex_keeps_old_vertex_link()
{
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 5 );
    builder->create_triangle( { 0, 2, 3 } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->compute_polygon_adjacencies();
    builder->set_polygon_vertex( { 1, 0 }, 4 );
    const auto around_0 = surface->polygon_around_vertex( 0 );
    geode::OpenGeodeMeshException::test(
        around_0 && surface->polygon_vertex( around_0.value() ) == 0,
        "[set_polygon_vertex] Old vertex should be linked to a polygon "
        "still using it" );
    const auto around_4 = surface->polygon_around_vertex( 4 );
    geode::OpenGeodeMeshException::test(
        around_4 && surface->polygon_vertex( around_4.value() ) == 4,
        "[set_polygon_vertex] New vertex should be linked to the modified "
        "polygon" );
}

void test_create_polygon_invalid_size()
{
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 4 );
    bool has_thrown{ false };
    try
    {
        builder->create_polygon( { 0, 1, 2, 3 } );
    }
    catch( const geode::OpenGeodeException& /*unused*/ )
    {
        has_thrown = true;
    }
    geode::OpenGeodeMeshException::test( has_thrown,
        "[create_polygon] Creating a quad in a TriangulatedSurface should "
        "throw" );
    geode::OpenGeodeMeshException::test( surface->nb_polygons() == 0,
        "[create_polygon] Failed creation should not add a polygon" );
    geode::OpenGeodeMeshException::test( surface->is_vertex_isolated( 0 ),
        "[create_polygon] Failed creation should not link vertices" );
}

void test_large_fan_around_vertex()
{
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    constexpr geode::index_t NB_TRIANGLES{ 1500 };
    builder->create_vertices( NB_TRIANGLES + 2 );
    for( const auto t : geode::Range{ NB_TRIANGLES } )
    {
        builder->create_triangle( { 0, t + 1, t + 2 } );
    }
    builder->compute_polygon_adjacencies();
    geode::OpenGeodeMeshException::test(
        surface->polygons_around_vertex( 0 ).size() == NB_TRIANGLES,
        "[polygons_around_vertex] Wrong number of polygons in a large fan" );
    geode::OpenGeodeMeshException::test( surface->is_vertex_on_border( 0 ),
        "[polygons_around_vertex] Fan center should be on border" );
}

void test_polygons_around_vertex_after_adjacencies()
{
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 4 );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 1, 3, 2 } );
    geode::OpenGeodeMeshException::test(
        surface->polygons_around_vertex( 1 ).size() == 1,
        "[polygons_around_vertex] Wrong number of polygons before "
        "adjacencies" );
    builder->compute_polygon_adjacencies();
    geode::OpenGeodeMeshException::test(
        surface->polygons_around_vertex( 1 ).size() == 2,
        "[polygons_around_vertex] Wrong number of polygons after "
        "adjacencies" );
}

void test_next_on_border_with_flipped_orientation()
{
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 4 );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 1, 2, 3 } );
    builder->compute_polygon_adjacencies();
    geode::OpenGeodeMeshException::test(
        surface->next_on_border( { 0, 0 } ) == geode::PolygonEdge{ 1, 2 },
        "[next_on_border] Wrong next border edge with flipped orientation" );
    geode::OpenGeodeMeshException::test(
        surface->previous_on_border( { 0, 2 } ) == geode::PolygonEdge{ 1, 1 },
        "[previous_on_border] Wrong previous border edge with flipped "
        "orientation" );
}

void test_incremental_adjacencies_with_edges()
{
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    surface->enable_edges();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 5 );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 1, 0, 3 } );
    builder->create_triangle( { 0, 1, 4 } );
    const std::array< geode::index_t, 2 > first{ 0, 1 };
    builder->compute_polygon_adjacencies( first );
    const std::array< geode::index_t, 2 > second{ 1, 2 };
    builder->compute_polygon_adjacencies( second );
    // Same result as a full computation: the non-manifold edge 0-1 is
    // unconnected for the three triangles
    for( const auto t : geode::Range{ 3 } )
    {
        geode::OpenGeodeMeshException::test(
            !surface->polygon_adjacent( { t, 0 } ),
            "[compute_polygon_adjacencies] Non-manifold edge should be "
            "unconnected for polygon ",
            t );
    }
}

void test_delete_polygon_at_pinched_vertex()
{
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 5 );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 0, 3, 4 } );
    builder->compute_polygon_adjacencies();
    builder->delete_polygons( { false, true } );
    geode::OpenGeodeMeshException::test( !surface->is_vertex_isolated( 0 ),
        "[delete_polygons] Pinched vertex should not be isolated" );
    geode::OpenGeodeMeshException::test(
        surface->polygon_vertex( surface->polygon_around_vertex( 0 ).value() )
            == 0,
        "[delete_polygons] Pinched vertex wrongly linked" );
    builder->delete_isolated_vertices();
    geode::OpenGeodeMeshException::test( surface->nb_vertices() == 3,
        "[delete_polygons] Wrong number of vertices after deleting isolated "
        "vertices" );
}

void test_permute_vertices_with_isolated_vertex()
{
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 4 );
    builder->create_triangle( { 1, 2, 3 } );
    builder->compute_polygon_adjacencies();
    builder->permute_vertices( { 2, 0, 1, 3 } );
    geode::OpenGeodeMeshException::test( surface->is_vertex_isolated( 1 ),
        "[permute_vertices] New vertex 1 should be isolated" );
    for( const geode::index_t v : { 0, 2, 3 } )
    {
        const auto around = surface->polygon_around_vertex( v );
        geode::OpenGeodeMeshException::test(
            around && surface->polygon_vertex( around.value() ) == v,
            "[permute_vertices] Wrong link for vertex ", v );
    }
}

void test_replace_vertices_chained_mapping()
{
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 7 );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 3, 4, 5 } );
    geode::GenericMapping< geode::index_t > mapping;
    mapping.map( 0, 3 );
    mapping.map( 3, 6 );
    builder->replace_vertices( mapping );
    geode::OpenGeodeMeshException::test( surface->is_vertex_isolated( 0 ),
        "[replace_vertices] Vertex 0 should be isolated" );
    for( const geode::index_t v : { 3, 6 } )
    {
        const auto around = surface->polygon_around_vertex( v );
        geode::OpenGeodeMeshException::test(
            around && surface->polygon_vertex( around.value() ) == v,
            "[replace_vertices] Wrong link for vertex ", v );
    }
}

void test_polygon_vertex_edge_hash()
{
    const std::hash< geode::PolygonVertex > vertex_hash;
    geode::OpenGeodeMeshException::test(
        vertex_hash( { 1, 2 } ) != vertex_hash( { 2, 1 } ),
        "[hash] Symmetric PolygonVertex should not collide" );
    geode::OpenGeodeMeshException::test(
        vertex_hash( { 1, 1 } ) != vertex_hash( { 2, 2 } ),
        "[hash] Diagonal PolygonVertex should not collide" );
    const std::hash< geode::PolygonEdge > edge_hash;
    geode::OpenGeodeMeshException::test(
        edge_hash( { 1, 2 } ) != edge_hash( { 2, 1 } ),
        "[hash] Symmetric PolygonEdge should not collide" );
    geode::OpenGeodeMeshException::test(
        edge_hash( { 1, 1 } ) != edge_hash( { 2, 2 } ),
        "[hash] Diagonal PolygonEdge should not collide" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_backward_v18_5();
    auto surface = geode::TriangulatedSurface3D::create(
        geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );

    test_create_vertices( *surface, *builder );
    test_create_polygons( *surface, *builder );
    test_polygon_adjacencies( *surface, *builder );
    test_io( *surface, absl::StrCat( "test.", surface->native_extension() ) );
    test_backward_io( absl::StrCat( geode::DATA_PATH, "backward_io/v17/v17.",
        surface->native_extension() ) );
    test_permutation( *surface, *builder );
    test_delete_polygon( *surface, *builder );
    test_clone( *surface );
    test_set_polygon_vertex_keeps_old_vertex_link();
    test_create_polygon_invalid_size();
    test_large_fan_around_vertex();
    test_polygons_around_vertex_after_adjacencies();
    test_next_on_border_with_flipped_orientation();
    test_incremental_adjacencies_with_edges();
    test_delete_polygon_at_pinched_vertex();
    test_permute_vertices_with_isolated_vertex();
    test_replace_vertices_chained_mapping();
    test_polygon_vertex_edge_hash();
    test_reserve_with_edges();
}

OPENGEODE_TEST( "triangulated-surface" )