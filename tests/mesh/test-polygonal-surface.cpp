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

#include <cstring>
#include <sstream>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/geode/geode_polygonal_surface_builder.hpp>
#include <geode/mesh/builder/surface_edges_builder.hpp>
#include <geode/mesh/core/geode/geode_polygonal_surface.hpp>
#include <geode/mesh/core/internal/facet_edges_impl.hpp>
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

geode::uuid test_create_vertex_attribute(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    geode::AttributeProperties attribute_properties;
    attribute_properties.assignable = false;
    attribute_properties.interpolable = true;
    attribute_properties.transferable = true;
    geode::AttributeValues< geode::PolygonEdge > attribute_values;
    attribute_values.default_value = geode::PolygonEdge{};
    attribute_values.no_value = geode::PolygonEdge{};
    auto attribute_id =
        polygonal_surface.vertex_attribute_manager()
            .create_attribute< geode::VariableAttribute, geode::PolygonEdge >(
                "test", attribute_values, attribute_properties );
    auto attribute =
        polygonal_surface.vertex_attribute_manager()
            .find_attribute< geode::VariableAttribute, geode::PolygonEdge >(
                attribute_id );
    for( const auto v : geode::Range{ polygonal_surface.nb_vertices() } )
    {
        attribute->set_value( v, geode::PolygonEdge{ v, 0 } );
        geode::OpenGeodeMeshException::test(
            geode::PolygonEdge{} != attribute->value( v ),
            "PolygonalSurface attribute assignation is not correct" );
    }
    return attribute_id;
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

geode::uuid test_create_edge_attribute(
    const geode::PolygonalSurface3D& polygonal_surface )
{
    geode::AttributeProperties attribute_properties;
    attribute_properties.assignable = false;
    attribute_properties.interpolable = true;
    attribute_properties.transferable = true;
    geode::AttributeValues< geode::index_t > attribute_values;
    attribute_values.default_value = geode::NO_ID;
    attribute_values.no_value = geode::NO_ID;
    auto attribute_id =
        polygonal_surface.edges()
            .edge_attribute_manager()
            .create_attribute< geode::VariableAttribute, geode::index_t >(
                "edges", attribute_values, attribute_properties );
    auto attribute =
        polygonal_surface.edges()
            .edge_attribute_manager()
            .find_attribute< geode::VariableAttribute, geode::index_t >(
                attribute_id );
    for( const auto e : geode::Range{ polygonal_surface.edges().nb_edges() } )
    {
        attribute->set_value( e, e );
    }
    return attribute_id;
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
    geode::PolygonalSurfaceBuilder3D& builder,
    const geode::uuid edge_attribute_id )
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
    const auto attribute =
        polygonal_surface.edges()
            .edge_attribute_manager()
            .find_read_only_attribute< geode::index_t >( edge_attribute_id );
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
    const std::string& filename,
    const geode::uuid edge_attribute_id )
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
    const auto attribute =
        new_polygonal_surface->edges()
            .edge_attribute_manager()
            .find_read_only_attribute< geode::index_t >( edge_attribute_id );
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

void test_clone( const geode::PolygonalSurface3D& polygonal_surface,
    geode::uuid vertex_attribute_id )
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
                                .find_read_only_attribute< geode::PolygonEdge >(
                                    vertex_attribute_id );
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

void test_merge_edges_on_vertex_update()
{
    auto surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::impl_name_static() );
    surface->enable_edges();
    auto builder = geode::PolygonalSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 3 );
    auto edges_builder = builder->edges_builder();
    edges_builder.find_or_create_edge( { 0, 2 } );
    edges_builder.find_or_create_edge( { 1, 2 } );
    const std::vector< geode::index_t > old2new{ 0, 0, 2 };
    edges_builder.update_edge_vertices( old2new );
    const auto& edges = surface->edges();
    geode::OpenGeodeMeshException::test( edges.nb_edges() == 1,
        "[merge edges] Merged edges should give 1 edge, not ",
        edges.nb_edges() );
    const auto merged = edges.edge_from_vertices( { 0, 2 } );
    geode::OpenGeodeMeshException::test(
        merged && merged.value() == 0, "[merge edges] Wrong merged edge id" );
    geode::OpenGeodeMeshException::test(
        !edges.is_edge_isolated( 0 ), "[merge edges] Edge 0 is isolated" );
    edges_builder.remove_edge( { 0, 2 } );
    geode::OpenGeodeMeshException::test( !edges.is_edge_isolated( 0 ),
        "[merge edges] Merged edge should keep both counters" );

    const auto new_edge = edges_builder.find_or_create_edge( { 1, 2 } );
    geode::OpenGeodeMeshException::test( new_edge == 1,
        "[merge edges] New edge id should be 1, not ", new_edge );
    geode::OpenGeodeMeshException::test( edges.nb_edges() == 2,
        "[merge edges] Should have 2 edges, not ", edges.nb_edges() );
    geode::OpenGeodeMeshException::test(
        edges.edge_vertices( 0 ) == std::array< geode::index_t, 2 >{ 0, 2 },
        "[merge edges] Wrong vertices for edge 0" );
    geode::OpenGeodeMeshException::test(
        edges.edge_vertices( 1 ) == std::array< geode::index_t, 2 >{ 1, 2 },
        "[merge edges] Wrong vertices for edge 1" );
    geode::OpenGeodeMeshException::test(
        edges.edge_from_vertices( { 1, 2 } ) == 1,
        "[merge edges] Wrong id from vertices for edge 1" );
}

void test_edge_is_opposite()
{
    auto surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::impl_name_static() );
    surface->enable_edges();
    auto builder = geode::PolygonalSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 3 );
    const auto edge_id =
        builder->edges_builder().find_or_create_edge( { 2, 1 } );
    const auto& edges = surface->edges();
    // Edges are stored in a canonical orientation
    geode::OpenGeodeMeshException::test( edges.is_opposite( edge_id, { 1, 2 } ),
        "[is_opposite] Edge should match its canonical orientation" );
    geode::OpenGeodeMeshException::test( edges.is_opposite( edge_id, { 2, 1 } ),
        "[is_opposite] Edge should match its canonical orientation" );
    geode::OpenGeodeMeshException::test(
        !edges.is_opposite( edge_id, { 0, 2 } ),
        "[is_opposite] Different edge detected as opposite" );
}

using FacetEdges = geode::internal::FacetEdgesImpl< 3 >;

std::string serialize_edges( const FacetEdges& edges )
{
    std::ostringstream output{ std::ios::binary };
    geode::TContext context{};
    geode::BitseryExtensions::register_serialize_pcontext(
        std::get< 0 >( context ) );
    geode::Serializer archive{ context, output };
    archive.object( edges );
    archive.adapter().flush();
    return output.str();
}

void deserialize_edges( const std::string& buffer, FacetEdges& edges )
{
    std::istringstream input{ buffer, std::ios::binary };
    geode::TContext context{};
    geode::BitseryExtensions::register_deserialize_pcontext(
        std::get< 0 >( context ) );
    geode::Deserializer archive{ context, input };
    archive.object( edges );
}

constexpr std::array< geode::index_t, 2 > EDGE_A{ 1000001, 1000002 };
constexpr std::array< geode::index_t, 2 > EDGE_B{ 1000003, 1000004 };

void check_repaired_edges( const std::string& buffer,
    geode::index_t expected_counter,
    std::string_view case_name )
{
    FacetEdges edges{ geode::BITSERY::constructor };
    deserialize_edges( buffer, edges );
    geode::OpenGeodeMeshException::test(
        edges.edge_attribute_manager().nb_elements() == 2, "[corrupted edges] ",
        case_name, ": should have 2 edges, not ",
        edges.edge_attribute_manager().nb_elements() );
    const auto edge_a = edges.find_edge( EDGE_A );
    const auto edge_b = edges.find_edge( EDGE_B );
    geode::OpenGeodeMeshException::test(
        edge_a && edge_b && edge_a.value() != edge_b.value()
            && edge_a.value() < 2 && edge_b.value() < 2,
        "[corrupted edges] ", case_name, ": wrong edge ids" );
    geode::OpenGeodeMeshException::test(
        edges.edge_vertices( edge_a.value() ) == EDGE_A
            && edges.edge_vertices( edge_b.value() ) == EDGE_B,
        "[corrupted edges] ", case_name, ": wrong edge vertices" );
    for( const auto& [edge, vertices] :
        { std::pair{ edge_a.value(), EDGE_A }, { edge_b.value(), EDGE_B } } )
    {
        for( const auto i : geode::Range{ expected_counter } )
        {
            geode::OpenGeodeMeshException::test(
                !edges.is_edge_isolated( edge ), "[corrupted edges] ",
                case_name, ": edge ", edge, " isolated after ", i,
                " removals" );
            edges.remove_edge( vertices );
        }
        geode::OpenGeodeMeshException::test( edges.is_edge_isolated( edge ),
            "[corrupted edges] ", case_name, ": edge ", edge,
            " should be isolated" );
    }
}

std::string as_bytes( geode::index_t value )
{
    std::string bytes( sizeof( geode::index_t ), 0 );
    std::memcpy( bytes.data(), &value, sizeof( geode::index_t ) );
    return bytes;
}

void test_corrupted_edges_deserialization()
{
    FacetEdges edges;
    edges.find_or_create_edge( EDGE_A );
    edges.find_or_create_edge( EDGE_B );
    const auto buffer = serialize_edges( edges );
    check_repaired_edges( buffer, 1, "valid" );

    // Edge counters are serialized last: [size = 2][counter 0][counter 1]
    constexpr auto COUNTERS_SIZE = 1 + 2 * sizeof( geode::index_t );
    geode::OpenGeodeMeshException::test(
        buffer.size() > COUNTERS_SIZE
            && buffer[buffer.size() - COUNTERS_SIZE] == 2,
        "[corrupted edges] Unexpected serialized layout" );
    const auto prefix = buffer.substr( 0, buffer.size() - COUNTERS_SIZE );
    const auto one_counter = as_bytes( 1 );

    // Only one counter: one edge index is out of bounds
    check_repaired_edges(
        absl::StrCat( prefix, std::string( 1, 1 ), one_counter ), 1,
        "missing counter" );

    // Three counters for two edges: the unindexed counter is added to every
    // edge
    check_repaired_edges( absl::StrCat( prefix, std::string( 1, 3 ),
                              one_counter, one_counter, one_counter ),
        2, "extra counter" );

    // Both edges share the same index: each map entry is serialized as
    // [vertices][edge index]
    const auto edge_b_entry = absl::StrCat(
        as_bytes( EDGE_B[0] ), as_bytes( EDGE_B[1] ), as_bytes( 1 ) );
    auto shared_index = buffer;
    const auto edge_b_position = shared_index.find( edge_b_entry );
    geode::OpenGeodeMeshException::test( edge_b_position != std::string::npos,
        "[corrupted edges] Unexpected serialized edge layout" );
    shared_index.replace( edge_b_position + 2 * sizeof( geode::index_t ),
        sizeof( geode::index_t ), as_bytes( 0 ) );
    check_repaired_edges( shared_index, 2, "shared index" );
}

void test_texture( const geode::PolygonalSurface3D& polygonal_surface )
{
    auto manager = polygonal_surface.texture_manager();
    manager.find_or_create_texture( "texture" );
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

void check_v18_5( const geode::PolygonalSurface3D& surface )
{
    geode::OpenGeodeMeshException::test(
        surface.nb_vertices() == 5 && surface.nb_polygons() == 2,
        "v18_5 PolygonalSurface has wrong number of elements" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_vertex( { 0, 2 } ) == 3,
        "v18_5 PolygonalSurface has wrong polygon vertex" );
    geode::OpenGeodeMeshException::test(
        surface.polygon_adjacent( { 0, 1 } ) == 1,
        "v18_5 PolygonalSurface has wrong polygon adjacent" );
    geode::OpenGeodeMeshException::test(
        surface.polygons_around_vertex( 1 ).size() == 2,
        "v18_5 PolygonalSurface has wrong polygons around vertex" );
    geode::OpenGeodeMeshException::test( surface.edges().nb_edges() == 6,
        "v18_5 PolygonalSurface has wrong number of edges" );
    check_user_value( surface.vertex_attribute_manager(), 4 );
    check_user_value( surface.polygon_attribute_manager(), 1 );
    check_user_value( surface.edges().edge_attribute_manager(), 5 );
    check_no_legacy_attributes( surface.vertex_attribute_manager(),
        { "polygon_around_vertex", "polygons_around_vertex" } );
    check_no_legacy_attributes( surface.edges().edge_attribute_manager(),
        { "counter", "facet_vertices" } );
}

void test_backward_v18_5()
{
    const auto surface = geode::load_polygonal_surface< 3 >(
        absl::StrCat( geode::DATA_PATH, "backward_io/v18_5/v18_5.og_psf3d" ) );
    check_v18_5( *surface );
    const auto filename =
        absl::StrCat( "v18_5_reload.", surface->native_extension() );
    geode::save_polygonal_surface( *surface, filename );
    check_v18_5( *geode::load_polygonal_surface< 3 >( filename ) );
}

void test_create_invalid_polygons()
{
    auto surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::impl_name_static() );
    auto builder = geode::PolygonalSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 300 );
    const auto expect_throw = [&builder, &surface](
                                  absl::Span< const geode::index_t > vertices,
                                  std::string_view message ) {
        bool has_thrown{ false };
        try
        {
            builder->create_polygon( vertices );
        }
        catch( const geode::OpenGeodeException& /*unused*/ )
        {
            has_thrown = true;
        }
        geode::OpenGeodeMeshException::test(
            has_thrown, "[create_polygon] Should throw with ", message );
        geode::OpenGeodeMeshException::test( surface->nb_polygons() == 0,
            "[create_polygon] Failed creation should not add a polygon (",
            message, ")" );
        geode::OpenGeodeMeshException::test( surface->is_vertex_isolated( 0 ),
            "[create_polygon] Failed creation should not link vertices (",
            message, ")" );
    };
    expect_throw( {}, "no vertex" );
    std::vector< geode::index_t > too_many( geode::NO_LID + 1 );
    absl::c_iota( too_many, 0 );
    expect_throw( too_many, "too many vertices" );
    expect_throw( { 0, 1, 300 }, "an invalid vertex" );
    std::vector< geode::index_t > largest( geode::NO_LID - 1 );
    absl::c_iota( largest, 0 );
    builder->create_polygon( largest );
    geode::OpenGeodeMeshException::test(
        surface->nb_polygon_vertices( 0 ) == largest.size(),
        "[create_polygon] Wrong number of polygon vertices" );
}

void test_polygon_vertex_normal_with_coincident_points()
{
    auto polygonal_surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::impl_name_static() );
    auto builder =
        geode::PolygonalSurfaceBuilder3D::create( *polygonal_surface );

    builder->create_point( geode::Point3D{ { 0.0, 0.0, 0.0 } } );
    builder->create_point( geode::Point3D{ { 0.0, 0.0, 0.0 } } );
    builder->create_point( geode::Point3D{ { 1.0, 0.0, 0.0 } } );
    builder->create_point( geode::Point3D{ { 0.0, 1.0, 0.0 } } );
    builder->create_point( geode::Point3D{ { -1.0, 1.0, 0.0 } } );

    builder->create_polygon( { 0, 1, 2, 3 } );
    builder->create_polygon( { 0, 3, 4 } );
    builder->compute_polygon_adjacencies();

    const geode::Vector3D answer{ { 0., 0., 1. } };
    const auto normal = polygonal_surface->polygon_vertex_normal( 0 );
    geode::OpenGeodeMeshException::test(
        normal && normal->inexact_equal( answer ),
        "PolygonalSurface polygon vertex normal with coincident points is "
        "not correct" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_backward_v18_5();
    auto polygonal_surface = geode::PolygonalSurface3D::create(
        geode::OpenGeodePolygonalSurface3D::impl_name_static() );
    polygonal_surface->enable_edges();
    auto builder =
        geode::PolygonalSurfaceBuilder3D::create( *polygonal_surface );

    test_create_vertices( *polygonal_surface, *builder );
    test_bounding_box( *polygonal_surface );
    const auto vertex_attribute_id =
        test_create_vertex_attribute( *polygonal_surface );
    test_create_polygons( *polygonal_surface, *builder );
    const auto edge_attribute_id =
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
        absl::StrCat( "test.", polygonal_surface->native_extension() ),
        edge_attribute_id );
    test_backward_io( absl::StrCat( geode::DATA_PATH, "backward_io/v7/test_v7.",
        polygonal_surface->native_extension() ) );
    test_backward_io( absl::StrCat( geode::DATA_PATH,
        "backward_io/v12/test_v12.", polygonal_surface->native_extension() ) );
    test_backward_io( absl::StrCat( geode::DATA_PATH, "backward_io/v17/v17.",
        polygonal_surface->native_extension() ) );
    test_permutation( *polygonal_surface, *builder );
    test_replace_vertex( *polygonal_surface, *builder );
    test_delete_polygon( *polygonal_surface, *builder, edge_attribute_id );
    test_clone( *polygonal_surface, vertex_attribute_id );
    test_set_polygon_vertex( *polygonal_surface, *builder );
    test_delete_all( *polygonal_surface, *builder );

    test_non_manifold_surface();
    test_create_invalid_polygons();
    test_polygon_vertex_normal_with_coincident_points();
    test_merge_edges_on_vertex_update();
    test_corrupted_edges_deserialization();
    test_edge_is_opposite();
}

OPENGEODE_TEST( "polygonal-surface" )