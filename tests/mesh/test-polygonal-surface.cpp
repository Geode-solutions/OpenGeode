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

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/geode/geode_polygonal_surface_builder.hpp>
#include <geode/mesh/builder/surface_edges_builder.hpp>
#include <geode/mesh/core/geode/geode_polygonal_surface.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/io/polygonal_surface_input.hpp>
#include <geode/mesh/io/polygonal_surface_output.hpp>

#include <geode/tests/common.hpp>

void test_create_vertices( const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    builder.create_point( geode::Point3D{ { 0.1, 0.2, 0.3 } } );
    builder.create_point( geode::Point3D{ { 2.1, 9.4, 6.7 } } );
    builder.create_point( geode::Point3D{ { 7.5, 5.2, 6.3 } } );
    builder.create_point( geode::Point3D{ { 8.1, 1.4, 4.7 } } );
    builder.create_point( geode::Point3D{ { 4.7, 2.1, 1.3 } } );
    builder.create_point( geode::Point3D{ { 9.3, 5.3, 6.7 } } );
    builder.create_point( geode::Point3D{ { 7.5, 4.2, 2.8 } } );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.is_vertex_isolated( 0 ),
        "Vertices should be isolated before polygons creation" );
    geode::OpenGeodeMeshException::test( polygonal_surface.nb_vertices() == 7,
        "PolygonalSurface should have 7 vertices" );
}

void test_bounding_box( const geode::PolygonalSurface3D& polygonal_surface )
{
    geode::Point3D answer_min{ { 0.1, 0.2, 0.3 } };
    geode::Point3D answer_max{ { 9.3, 9.4, 6.7 } };
    geode::OpenGeodeMeshException::test(
        polygonal_surface.bounding_box().min() == answer_min,
        "Wrong computation of bounding box (min)" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.bounding_box().max() == answer_max,
        "Wrong computation of bounding box (max)" );
}

void test_create_vertex_attribute(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    auto attribute =
        polygonal_surface.vertex_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute,
                geode::PolygonEdge >( "test", geode::PolygonEdge{} );
    for( const auto v : geode::Range{ polygonal_surface.nb_vertices() } )
    {
        attribute->set_value( v, geode::PolygonEdge{ v, 0 } );
        geode::OpenGeodeMeshException::test(
            geode::PolygonEdge{} != attribute->value( v ),
            "PolygonalSurface attribute assignation is not correct" );
    }
}

void test_permutation( const geode::PolygonalSurface3D& surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    std::vector< geode::index_t > vertex_permutation{ 4, 2, 6, 1, 5, 0, 3 };
    builder.permute_vertices( vertex_permutation );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 0 } ) == 5,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 1 } ) == 3,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 2 } ) == 1,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 0 } ) == 3,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 1 } ) == 6,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 2 } ) == 0,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 3 } ) == 1,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 0 } ) == 3,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 1 } ) == 4,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 2 } ) == 2,
        "Wrong PolygonVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 3 } ) == 6,
        "Wrong PolygonVertex after vertex permute" );

    std::vector< geode::index_t > polygon_permutation{ 2, 0, 1 };
    builder.permute_polygons( polygon_permutation );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 0 } ) == 3,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 1 } ) == 4,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 2 } ) == 2,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 3 } ) == 6,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 0 } ) == 5,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 1 } ) == 3,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 1, 2 } ) == 1,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 0 } ) == 3,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 1 } ) == 6,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 2 } ) == 0,
        "Wrong PolygonVertex after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 2, 3 } ) == 1,
        "Wrong PolygonVertex after polygon permute" );

    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent( { 0, 3 } ) == 2,
        "Wrong Adjacency after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent( { 2, 0 } ) == 0,
        "Wrong Adjacency after polygon permute" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent( { 1, 1 } ) == 2,
        "Wrong Adjacency after polygon permute" );
    geode::OpenGeodeMeshException::test( !surface.polygon_adjacent( { 1, 0 } ),
        "Wrong Adjacency after polygon permute" );

    const auto polygons_3 = surface.polygons_around_vertex( 3 );
    geode::OpenGeodeMeshException::test(
        polygons_3.size() == 3, "Wrong polygons_3 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_3[0].polygon_id == 0,
        "Wrong polygons_3 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_3[0].vertex_id == 0,
        "Wrong polygons_3 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_3[1].polygon_id == 2,
        "Wrong polygons_3 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_3[1].vertex_id == 0,
        "Wrong polygons_3 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_3[2].polygon_id == 1,
        "Wrong polygons_3 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_3[2].vertex_id == 1,
        "Wrong polygons_3 after polygon permute" );

    const auto polygons_6 = surface.polygons_around_vertex( 6 );
    geode::OpenGeodeMeshException::test(
        polygons_6.size() == 2, "Wrong polygons_6 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_6[0].polygon_id == 0,
        "Wrong polygons_6 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_6[0].vertex_id == 3,
        "Wrong polygons_6 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_6[1].polygon_id == 2,
        "Wrong polygons_6 after polygon permute" );
    geode::OpenGeodeMeshException::test( polygons_6[1].vertex_id == 1,
        "Wrong polygons_6 after polygon permute" );
}

void test_create_polygons( const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    builder.create_polygon( { 0, 1, 2 } );
    builder.create_polygon( { 1, 3, 4, 2 } );
    builder.create_polygon( { 1, 5, 6, 3 } );
    geode::OpenGeodeMeshException::test(
        !polygonal_surface.is_vertex_isolated( 0 ),
        "Vertices should not be isolated after polygons creation" );
    geode::OpenGeodeMeshException::test( polygonal_surface.nb_polygons() == 3,
        "PolygonalSurface should have 3 polygons" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.edges().nb_edges() == 9,
        "PolygonalSurface should have 9 edges" );
    geode::PolygonVertices answer{ 1, 3, 4, 2 };
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygon_vertices( 1 ) == answer,
        "Wrong polygon vertices list" );
}

void test_create_edge_attribute(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    auto attribute = polygonal_surface.edges()
                         .edge_attribute_manager()
                         .find_or_create_attribute< geode::VariableAttribute,
                             geode::index_t >( "test", geode::NO_ID );
    for( const auto e : geode::Range{ polygonal_surface.edges().nb_edges() } )
    {
        attribute->set_value( e, e );
    }
}

void test_polygon_adjacencies(
    const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    builder.compute_polygon_adjacencies();
    geode::OpenGeodeMeshException::test(
        !polygonal_surface.polygon_adjacent( { 0, 0 } ),
        "PolygonalSurface adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygon_adjacent( { 0, 1 } ) == 1,
        "PolygonalSurface adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygon_adjacent( { 1, 3 } ) == 0,
        "PolygonalSurface adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygon_adjacent_edge( { 0, 1 } )
            == geode::PolygonEdge( 1, 3 ),
        "PolygonalSurface adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.edges().edge_from_vertices(
            polygonal_surface.polygon_edge_vertices( { 0, 1 } ) )
            == polygonal_surface.edges().edge_from_vertices(
                polygonal_surface.polygon_edge_vertices( { 1, 3 } ) ),
        "PolygonalSurface edge indices is not correct" );

    geode::OpenGeodeMeshException::test(
        !polygonal_surface.polygon_adjacent( { 2, 0 } ),
        "PolygonalSurface adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygon_adjacent( { 2, 3 } ) == 1,
        "PolygonalSurface adjacent index is not correct" );

    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygons_around_vertex( 1 ).size() == 3,
        "PolygonalSurface should have 3 polygons around this vertex" );
}

void test_polygon_edges_on_borders(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    const auto result = polygonal_surface.polygon_edges_on_border( 0 );
    geode::OpenGeodeMeshException::test( result.size() == 2,
        "Number of polygon edges on border index is not correct" );
    geode::OpenGeodeMeshException::test(
        absl::c_find( result, geode::PolygonEdge( 0, 0 ) ) != result.end()
            && absl::c_find( result, geode::PolygonEdge( 0, 2 ) )
                   != result.end(),
        "Polygon edge indices on border index is not correct" );
}

void test_previous_next_on_border(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    geode::OpenGeodeMeshException::test(
        polygonal_surface.previous_on_border( { 0, 0 } )
            == geode::PolygonEdge( 0, 2 ),
        "Previous edge on border index is not correct" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.next_on_border( { 0, 0 } )
            == geode::PolygonEdge( 2, 0 ),
        "Next edge on border index is not correct" );
}

void test_polygon_edge_requests(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    geode::OpenGeodeMeshException::test(
        polygonal_surface.edge_length(
            polygonal_surface.edges().edge_vertices( 0 ) )
            == std::sqrt( 2 * 2 + 9.2 * 9.2 + 6.4 * 6.4 ),
        "Polygon edge length is not correct" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.edge_barycenter(
            polygonal_surface.edges().edge_vertices( 0 ) )
            == geode::Point3D( { 1.1, 4.8, 3.5 } ),
        "Polygon edge barycenter is not correct" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygon_edge_vertex( { 0, 0 }, 1 ) == 1,
        "Polygon edge vertex index is not correct" );
    auto polygon_edge = polygonal_surface.polygon_edge_from_vertices( 3, 5 );
    geode::OpenGeodeMeshException::test(
        !polygon_edge, "Polygon edge from vertices is not correct" );
    polygon_edge = polygonal_surface.polygon_edge_from_vertices( 0, 1 );
    geode::OpenGeodeMeshException::test(
        polygon_edge.has_value(), "Polygon edge from vertices is not correct" );
    geode::OpenGeodeMeshException::test(
        polygon_edge == geode::PolygonEdge( 0, 0 ),
        "Polygon edge from vertices is not correct" );
    polygon_edge = polygonal_surface.polygon_edge_from_vertices( 1, 0 );
    geode::OpenGeodeMeshException::test(
        !polygon_edge, "Polygon edge from vertices is not correct" );
    const auto polygon_edges =
        polygonal_surface.polygons_from_edge_vertices( { { 1, 2 } } );
    geode::OpenGeodeMeshException::test( polygon_edges.size() == 2,
        "Wrong number of polygon edges from vertices are not correct." );
    geode::OpenGeodeMeshException::test(
        ( polygon_edges[0] == geode::PolygonEdge( 0, 1 )
            && polygon_edges[1] == geode::PolygonEdge( 1, 3 ) )
            || ( polygon_edges[1] == geode::PolygonEdge( 0, 1 )
                 && polygon_edges[0] == geode::PolygonEdge( 1, 3 ) ),
        "Polygon edges from vertices are not correct." );
}

void test_delete_polygon( const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    std::vector< bool > to_delete( polygonal_surface.nb_polygons(), false );
    to_delete.front() = true;
    builder.delete_polygons( to_delete );
    geode::OpenGeodeMeshException::test( polygonal_surface.nb_polygons() == 2,
        "PolygonalSurface should have 2 polygons" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygon_vertex( { 0, 0 } ) == 5,
        "PolygonalSurface edge vertex index is not correct" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygon_vertex( { 0, 1 } ) == 3,
        "PolygonalSurface edge vertex index is not correct" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygon_vertex( { 0, 2 } ) == 1,
        "PolygonalSurface edge vertex index is not correct" );
    const auto isol_edge =
        polygonal_surface.edges().edge_from_vertices( { 2, 6 } ).value();
    geode::OpenGeodeMeshException::test(
        polygonal_surface.edges().is_edge_isolated( isol_edge ),
        "Edge should be isolated after polygon deletion" );
    builder.edges_builder().delete_isolated_edges();
    geode::OpenGeodeMeshException::test(
        polygonal_surface.edges().nb_edges() == 6,
        "PolygonalSurface should have 6 edges" );
    const auto attribute = polygonal_surface.edges()
                               .edge_attribute_manager()
                               .find_attribute< geode::index_t >( "test" );
    for( const auto e : geode::Range{ 6 } )
    {
        geode::OpenGeodeMeshException::test( attribute->value( e ) == e,
            "Update of edge attributes after "
            "polygon deletion is not correct" );
    }
    geode::OpenGeodeMeshException::test(
        polygonal_surface.edges().edge_from_vertices(
            polygonal_surface.polygon_edge_vertices( { 0, 0 } ) )
            == 0,
        "Edge index for PolygonEdge( 0, 0 ) is not correct" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.edges().edge_from_vertices(
            polygonal_surface.polygon_edge_vertices( { 0, 2 } ) )
            == 2,
        "Edge index for PolygonEdge( 0, 2 ) is not correct" );
}

void test_polygon_barycenter(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    const geode::Point3D answer( { 5.6, 4.525, 4.75 } );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygon_barycenter( 1 ) == answer,
        "PolygonalSurface polygon barycenter is not correct" );
}

void test_polygon_area()
{
    auto polygonal_surface = geode::PolygonalSurface2D::create(
        geode::OpenGeodePolygonalSurface2D::impl_name_static() );
    auto builder =
        geode::PolygonalSurfaceBuilder2D::create( *polygonal_surface );
    const double a{ 6.0 };
    const double b{ 8.0 };
    const double c{ 4.0 };

    builder->create_point( geode::Point2D{ { 0.0, 0.0 } } );
    builder->create_point( geode::Point2D{ { a, 0.0 } } );
    builder->create_point( geode::Point2D{ { b, c } } );
    builder->create_point( geode::Point2D{ { 0.0, c } } );

    builder->create_polygon( { 0, 1, 2, 3 } );

    const double answer{ 28. };
    geode::OpenGeodeMeshException::test(
        polygonal_surface->polygon_area( 0 ) == answer,
        "PolygonalSurface polygon area is not correct" );
}

void test_polygon_normal()
{
    auto polygonal_surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::impl_name_static() );
    auto builder =
        geode::PolygonalSurfaceBuilder3D::create( *polygonal_surface );
    const double a{ 6.0 };
    const double b{ 8.0 };
    const double c{ 4.0 };

    builder->create_point( geode::Point3D{ { 0.0, 0.0, 0.0 } } );
    builder->create_point( geode::Point3D{ { a, 0.0, 0.0 } } );
    builder->create_point( geode::Point3D{ { b, c, 0.0 } } );
    builder->create_point( geode::Point3D{ { 0.0, c, 0.0 } } );

    builder->create_polygon( { 0, 1, 2, 3 } );

    const geode::Vector3D answer{ { 0., 0., 1. } };
    geode::OpenGeodeMeshException::test(
        polygonal_surface->polygon_normal( 0 ).value() == answer,
        "PolygonalSurface polygon normal is not correct" );
}

void test_polygon_vertex_normal()
{
    auto polygonal_surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::impl_name_static() );
    auto builder =
        geode::PolygonalSurfaceBuilder3D::create( *polygonal_surface );

    builder->create_point( geode::Point3D{ { 0.0, 0.0, 0.0 } } );
    builder->create_point( geode::Point3D{ { 1.0, 0.0, 0.0 } } );
    builder->create_point( geode::Point3D{ { 0.0, 1.0, 1.0 } } );
    builder->create_point( geode::Point3D{ { 0.0, -1.0, 1.0 } } );

    builder->create_polygon( { 0, 1, 2 } );
    builder->create_polygon( { 0, 3, 1 } );
    builder->compute_polygon_adjacencies();

    const geode::Vector3D answer{ { 0., 0., 1. } };
    geode::OpenGeodeMeshException::test(
        polygonal_surface->polygon_vertex_normal( 0 ).value() == answer,
        "PolygonalSurface polygon vertex normal is not correct" );
}

void test_io( const geode::PolygonalSurface3D& polygonal_surface,
    const std::string& filename )
{
    geode::save_polygonal_surface( polygonal_surface, filename );
    const auto reloaded = geode::load_polygonal_surface< 3 >( filename );
    geode_unused( reloaded );
    const auto new_polygonal_surface = geode::load_polygonal_surface< 3 >(
        geode::OpenGeodePolygonalSurface3D::impl_name_static(), filename );

    geode::OpenGeodeMeshException::test(
        new_polygonal_surface->nb_vertices() == 7,
        "Reloaded PolygonalSurface should have 7 vertices" );
    for( const auto vertex_id :
        geode::Range{ polygonal_surface.nb_vertices() } )
    {
        geode::OpenGeodeMeshException::test(
            polygonal_surface.point( vertex_id )
                .inexact_equal( new_polygonal_surface->point( vertex_id ) ),
            "Wrong reloaded mesh point coordinates." );
    }
    geode::OpenGeodeMeshException::test(
        new_polygonal_surface->edges().nb_edges() == 9,
        "Reloaded PolygonalSurface should have 9 edges" );
    geode::OpenGeodeMeshException::test(
        new_polygonal_surface->nb_polygons() == 3,
        "Reloaded PolygonalSurface should have 3 polygons" );
    geode::OpenGeodeMeshException::test(
        new_polygonal_surface->edges().edge_from_vertices( { 1, 0 } )
            == polygonal_surface.edges().edge_from_vertices( { 1, 0 } ),
        "Reloaded PolygonalSurface has wrong polygon edge index" );
    const auto attribute = new_polygonal_surface->edges()
                               .edge_attribute_manager()
                               .find_attribute< geode::index_t >( "test" );
    for( const auto e :
        geode::Range{ new_polygonal_surface->edges().nb_edges() } )
    {
        geode::OpenGeodeMeshException::test( attribute->value( e ) == e,
            "Reloaded PolygonalSurface has "
            "wrong attributes on its edges" );
    }
    auto manager = new_polygonal_surface->texture_manager();
    auto texture_names = manager.texture_names();
    geode::OpenGeodeMeshException::test( texture_names.size() == 1,
        "Reloaded PolygonalSurface has wrong number of textures" );
    geode::OpenGeodeMeshException::test( texture_names[0] == "texture",
        "Reloaded PolygonalSurface has wrong texture name" );
}

void test_backward_io( const std::string& filename )
{
    const auto new_polygonal_surface = geode::load_polygonal_surface< 3 >(
        geode::OpenGeodePolygonalSurface3D::impl_name_static(), filename );

    geode::OpenGeodeMeshException::test(
        new_polygonal_surface->nb_vertices() == 7,
        "Reloaded PolygonalSurface should have 7 vertices" );
    geode::OpenGeodeMeshException::test(
        new_polygonal_surface->edges().nb_edges() == 9,
        "Reloaded PolygonalSurface should have 9 edges" );
    geode::OpenGeodeMeshException::test(
        new_polygonal_surface->nb_polygons() == 3,
        "Reloaded PolygonalSurface should have 3 polygons" );

    geode::OpenGeodeMeshException::test(
        new_polygonal_surface->polygons_around_vertex( 1 ).size() == 3,
        "Backward polygons around failed" );
    geode::OpenGeodeMeshException::test(
        new_polygonal_surface->polygons_around_vertex( 2 ).size() == 2,
        "Backward polygons around failed" );
}

void test_clone( const geode::PolygonalSurface3D& polygonal_surface )
{
    const auto polygonal_surface_clone = polygonal_surface.clone();
    geode::OpenGeodePolygonalSurface3D polygonal_surface2{ std::move(
        *dynamic_cast< geode::OpenGeodePolygonalSurface3D* >(
            polygonal_surface_clone.get() ) ) };
    geode::OpenGeodeMeshException::test( polygonal_surface2.nb_vertices() == 7,
        "PolygonalSurface2 should have 7 vertices" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface2.edges().nb_edges() == 6,
        "PolygonalSurface2 should have 6 edges" );
    geode::OpenGeodeMeshException::test( polygonal_surface2.nb_polygons() == 2,
        "PolygonalSurface2 should have 2 polygons" );

    const auto attribute2 = polygonal_surface2.vertex_attribute_manager()
                                .find_attribute< geode::PolygonEdge >( "test" );
    std::vector< geode::PolygonEdge > att_answer{ { 4, 0 }, { 2, 0 }, { 6, 0 },
        { 1, 0 }, { 5, 0 }, { 0, 0 }, { 3, 0 } };
    for( const auto v : geode::Range{ polygonal_surface2.nb_vertices() } )
    {
        const geode::PolygonEdge answer{ v, 0 };
        geode::OpenGeodeMeshException::test(
            attribute2->value( v ) == att_answer[v],
            "PolygonalSurface2 attribute is not correct" );
    }
}

void test_set_polygon_vertex(
    const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    builder.set_polygon_vertex( { 0, 0 }, 2 );
    builder.edges_builder().delete_isolated_edges();
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygon_vertex( { 0, 0 } ) == 2,
        "PolygonVertex after set_polygon_vertex is wrong" );

    const auto vertices = polygonal_surface.edges().edge_vertices( 1 );
    geode::OpenGeodeMeshException::test( vertices[0] == 3 && vertices[1] == 6,
        "Edge vertices after set_polygon_vertex is wrong" );
}

void test_replace_vertex( const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    const auto att = polygonal_surface.edges()
                         .edge_attribute_manager()
                         .find_attribute< geode::index_t >( "counter" );
    const auto new_id = builder.create_vertex();
    const auto polygons_around = polygonal_surface.polygons_around_vertex( 1 );
    builder.replace_vertex( 1, new_id );
    for( const auto& pv : polygons_around )
    {
        geode::OpenGeodeMeshException::test(
            polygonal_surface.polygon_vertex( pv ) == new_id,
            "PolygonVertex after replace_vertex is wrong" );
    }
    geode::OpenGeodeMeshException::test(
        polygonal_surface.is_vertex_isolated( 1 ),
        "Isolated vertex after replace_vertex is wrong" );
    builder.replace_vertex( new_id, 1 );
    for( const auto& pv : polygons_around )
    {
        geode::OpenGeodeMeshException::test(
            polygonal_surface.polygon_vertex( pv ) == 1,
            "PolygonVertex after second replace_vertex is wrong" );
    }
    builder.delete_isolated_vertices();
    builder.edges_builder().delete_isolated_edges();
    geode::OpenGeodeMeshException::test(
        polygonal_surface.nb_vertices() == new_id,
        "Revert after replace_vertex is wrong" );
}

void test_delete_all( const geode::PolygonalSurface3D& polygonal_surface,
    geode::PolygonalSurfaceBuilder3D& builder )
{
    std::vector< bool > to_delete( polygonal_surface.nb_polygons(), true );
    builder.delete_polygons( to_delete );
    geode::OpenGeodeMeshException::test( polygonal_surface.nb_vertices() == 7,
        "PolygonalSurface should have 7 vertices" );
    geode::OpenGeodeMeshException::test( polygonal_surface.nb_polygons() == 0,
        "PolygonalSurface should have 0 polygon" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.polygons_around_vertex( 0 ).empty(),
        "No more polygon around vertices" );

    builder.delete_isolated_vertices();
    builder.edges_builder().delete_isolated_edges();
    geode::OpenGeodeMeshException::test( polygonal_surface.nb_vertices() == 0,
        "PolygonalSurface should have 0 vertex" );
    geode::OpenGeodeMeshException::test(
        polygonal_surface.edges().nb_edges() == 0,
        "PolygonalSurface should have 0 edge" );
}

void test_non_manifold_surface()
{
    auto surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::impl_name_static() );
    auto builder = geode::PolygonalSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 10 );
    builder->create_polygon( { 0, 1, 2, 3 } );
    builder->create_polygon( { 2, 3, 4, 5 } );
    builder->create_polygon( { 7, 6, 3, 2 } );
    builder->create_polygon( { 9, 2, 3, 8 } );
    builder->compute_polygon_adjacencies();
    for( const auto p : geode::Range{ surface->nb_polygons() } )
    {
        for( const auto e : geode::LRange{ surface->nb_polygon_edges( p ) } )
        {
            geode::PolygonEdge edge{ p, e };
            geode::OpenGeodeMeshException::test(
                surface->is_edge_on_border( edge ),
                "Wrong adjacency computations on non-manifold surface" );
        }
    }
}

void test_texture( const geode::PolygonalSurface3D& polygonal_surface )
{
    auto manager = polygonal_surface.texture_manager();
    manager.find_or_create_texture( "texture" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto polygonal_surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::impl_name_static() );
    polygonal_surface->enable_edges();
    auto builder =
        geode::PolygonalSurfaceBuilder3D::create( *polygonal_surface );

    test_create_vertices( *polygonal_surface, *builder );
    test_bounding_box( *polygonal_surface );
    test_create_vertex_attribute( *polygonal_surface );
    test_create_polygons( *polygonal_surface, *builder );
    test_create_edge_attribute( *polygonal_surface );
    test_polygon_adjacencies( *polygonal_surface, *builder );
    test_polygon_edges_on_borders( *polygonal_surface );
    test_previous_next_on_border( *polygonal_surface );
    test_polygon_edge_requests( *polygonal_surface );
    test_polygon_barycenter( *polygonal_surface );
    test_polygon_area();
    test_polygon_normal();
    test_polygon_vertex_normal();
    test_texture( *polygonal_surface );

    test_io( *polygonal_surface,
        absl::StrCat( "test.", polygonal_surface->native_extension() ) );
    test_backward_io( absl::StrCat(
        geode::DATA_PATH, "test_v7.", polygonal_surface->native_extension() ) );
    test_backward_io( absl::StrCat( geode::DATA_PATH, "test_v12.",
        polygonal_surface->native_extension() ) );

    test_permutation( *polygonal_surface, *builder );
    test_replace_vertex( *polygonal_surface, *builder );
    test_delete_polygon( *polygonal_surface, *builder );
    test_clone( *polygonal_surface );
    test_set_polygon_vertex( *polygonal_surface, *builder );
    test_delete_all( *polygonal_surface, *builder );

    test_non_manifold_surface();
}

OPENGEODE_TEST( "polygonal-surface" )