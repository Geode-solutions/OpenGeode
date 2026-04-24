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

#include <geode/mesh/builder/geode/geode_polyhedral_solid_builder.hpp>
#include <geode/mesh/builder/solid_edges_builder.hpp>
#include <geode/mesh/builder/solid_facets_builder.hpp>
#include <geode/mesh/core/geode/geode_polyhedral_solid.hpp>
#include <geode/mesh/core/solid_edges.hpp>
#include <geode/mesh/core/solid_facets.hpp>
#include <geode/mesh/io/polyhedral_solid_input.hpp>
#include <geode/mesh/io/polyhedral_solid_output.hpp>

#include <geode/tests/common.hpp>

void test_create_vertices( const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    builder.create_point( geode::Point3D{ { 0.1, 0.2, 0.3 } } );
    builder.create_point( geode::Point3D{ { 2.1, 9.4, 6.7 } } );
    builder.create_point( geode::Point3D{ { 7.5, 5.2, 6.3 } } );
    builder.create_point( geode::Point3D{ { 8.1, 1.4, 4.7 } } );
    builder.create_point( geode::Point3D{ { 4.7, 2.1, 1.3 } } );
    builder.create_point( geode::Point3D{ { 9.3, 5.3, 6.7 } } );
    builder.create_point( geode::Point3D{ { 7.5, 4.2, 2.8 } } );
    builder.create_point( geode::Point3D{ { 2.2, 3.3, 4.4 } } );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.is_vertex_isolated( 0 ),
        "Vertices should be isolated before polyhedra creation" );
    geode::OpenGeodeMeshException::test( polyhedral_solid.nb_vertices() == 8,
        "PolyhedralSolid should have 8 vertices" );
}

void test_bounding_box( const geode::PolyhedralSolid3D& polyhedral_solid )
{
    geode::Point3D answer_min{ { 0.1, 0.2, 0.3 } };
    geode::Point3D answer_max{ { 9.3, 9.4, 6.7 } };
    const auto bbox = polyhedral_solid.bounding_box();
    geode::OpenGeodeMeshException::test(
        bbox.min() == answer_min, "Wrong computation of bounding box (min)" );
    geode::OpenGeodeMeshException::test(
        bbox.max() == answer_max, "Wrong computation of bounding box (max)" );
}

void test_facets( const geode::PolyhedralSolid3D& polyhedral_solid )
{
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.facets().facet_from_vertices( { 0, 1, 2 } ) == 0,
        "Wrong facet from vertices" );
}

void test_edges( const geode::PolyhedralSolid3D& polyhedral_solid )
{
    geode::Point3D answer{ { 1.1, 4.8, 3.5 } };
    const auto vertices = polyhedral_solid.edges().edge_vertices( 0 );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.edge_barycenter( vertices ) == answer,
        "Wrong edge barycenter" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.edge_length( vertices ) - 11.3842 < 1e-6,
        "Wrong edge length" );
}

void test_create_polyhedra( const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    builder.create_polyhedron(
        { 0, 1, 2, 3, 4, 5 }, { { 0, 1, 2 }, { 3, 5, 4 }, { 0, 3, 4, 1 },
                                  { 0, 2, 5, 3 }, { 1, 4, 5, 2 } } );
    builder.create_polyhedron( { 3, 4, 5, 6 },
        { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
    builder.create_polyhedron( { 4, 5, 6, 7 },
        { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
    geode::OpenGeodeMeshException::test( polyhedral_solid.nb_polyhedra() == 3,
        "PolyhedralSolid should have 3 polyhedra" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.facets().nb_facets() == 11,
        "PolyhedralSolid should have 11 facets" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.edges().nb_edges() == 15,
        "PolyhedralSolid should have 15 edges" );
    geode::OpenGeodeMeshException::test(
        !polyhedral_solid.is_vertex_isolated( 0 ),
        "Vertices should not be isolated after polyhedra creation" );

    geode::OpenGeodeMeshException::test(
        polyhedral_solid.edges().edge_from_vertices(
            polyhedral_solid.polyhedron_facet_edge_vertices( { { 0, 1 }, 2 } ) )
            == 4,
        "Wrong edge index get from PolyhedronFacetEdge" );
    geode::OpenGeodeMeshException::test(
        !polyhedral_solid.vertex_in_polyhedron( 0, 7 ),
        "Wrong vertex in polyhedron" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.vertex_in_polyhedron( 1, 5 ).has_value(),
        "Wrong vertex in polyhedron" );
    geode::PolyhedronVertices answer{ 3, 4, 5, 6 };
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_vertices( 1 ) == answer,
        "Wrong polyhedron vertices list" );
}

void test_create_facet_attribute(
    const geode::PolyhedralSolid3D& polyhedral_solid )
{
    auto attribute = polyhedral_solid.facets()
                         .facet_attribute_manager()
                         .find_or_create_attribute< geode::VariableAttribute,
                             geode::index_t >( "test", geode::NO_ID );
    for( const auto f : geode::Range{ polyhedral_solid.facets().nb_facets() } )
    {
        attribute->set_value( f, f );
    }
}

void test_create_edge_attribute(
    const geode::PolyhedralSolid3D& polyhedral_solid )
{
    auto attribute = polyhedral_solid.edges()
                         .edge_attribute_manager()
                         .find_or_create_attribute< geode::VariableAttribute,
                             geode::index_t >( "test", geode::NO_ID );
    for( const auto e : geode::Range{ polyhedral_solid.edges().nb_edges() } )
    {
        const auto& vertices = polyhedral_solid.edges().edge_vertices( e );
        attribute->set_value( e, vertices[0] + vertices[1] );
    }
    geode::OpenGeodeMeshException::test(
        attribute->value( 0 ) == 1, "Wrong value for attribute on edge 0" );
    geode::OpenGeodeMeshException::test(
        attribute->value( 1 ) == 3, "Wrong value for attribute on edge 1" );
}

void test_polyhedron_adjacencies(
    const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    builder.compute_polyhedron_adjacencies();
    geode::OpenGeodeMeshException::test(
        !polyhedral_solid.polyhedron_adjacent( { 0, 0 } ),
        "PolyhedralSolid adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_adjacent( { 0, 1 } ) == 1,
        "PolyhedralSolid adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        !polyhedral_solid.polyhedron_adjacent( { 0, 2 } ),
        "PolyhedralSolid adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_adjacent( { 1, 0 } ) == 2,
        "PolyhedralSolid adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_adjacent( { 1, 3 } ) == 0,
        "PolyhedralSolid adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_adjacent( { 2, 3 } ) == 1,
        "PolyhedralSolid adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedra_around_vertex( 4 ).size() == 3,
        "PolyhedralSolid should have 3 polyhedra around this vertex" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_facets_on_border( 0 ).size() == 4,
        "First polyhedron of PolyhedralSolid should have 4 facets on "
        "border" );

    auto edge_id =
        polyhedral_solid.edges().edge_from_vertices( { 5, 4 } ).value();
    geode::OpenGeodeMeshException::test(
        edge_id == 8, "Wrong edge index from vertices" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedra_around_edge( { 5, 4 } ).size() == 3,
        "PolyhedralSolid should have 3 polyhedra around this edge" );
    const auto facet_id = polyhedral_solid.facets().facet_from_vertices(
        polyhedral_solid.polyhedron_facet_vertices( { 1, 0 } ) );
    const auto& polyhedra = polyhedral_solid.polyhedra_from_facet_vertices(
        polyhedral_solid.facets().facet_vertices( facet_id.value() ) );
    geode::OpenGeodeMeshException::test(
        polyhedra.size() == 2, "Wrong number of polyhedra from facet" );
    geode::OpenGeodeMeshException::test(
        absl::c_find( polyhedra, geode::PolyhedronFacet{ 1, 0 } )
            != polyhedra.end(),
        "Polyhedra from facet should contain (1, 0)" );
    geode::OpenGeodeMeshException::test(
        absl::c_find( polyhedra, geode::PolyhedronFacet{ 2, 3 } )
            != polyhedra.end(),
        "Polyhedra from facet should contain (2, 3)" );
}

void test_permutation( const geode::PolyhedralSolid3D& solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    std::vector< geode::index_t > vertex_permutation{ 4, 2, 6, 1, 5, 0, 7, 3 };
    builder.permute_vertices( vertex_permutation );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 0 } ) == 5,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 1 } ) == 3,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 2 } ) == 1,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 3 } ) == 7,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 4 } ) == 0,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 5 } ) == 4,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 0 } ) == 7,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 1 } ) == 0,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 2 } ) == 4,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 3 } ) == 2,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 0 } ) == 0,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 1 } ) == 4,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 2 } ) == 2,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 3 } ) == 6,
        "Wrong PolyhedronVertex after vertex permute" );

    std::vector< geode::index_t > polyhedron_permutation{ 2, 0, 1 };
    builder.permute_polyhedra( polyhedron_permutation );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 0 } ) == 0,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 1 } ) == 4,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 2 } ) == 2,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 3 } ) == 6,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 0 } ) == 5,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 1 } ) == 3,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 2 } ) == 1,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 3 } ) == 7,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 4 } ) == 0,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 5 } ) == 4,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 0 } ) == 7,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 1 } ) == 0,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 2 } ) == 4,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 3 } ) == 2,
        "Wrong PolyhedronVertex after polyhedron permute" );

    geode::OpenGeodeMeshException::test( !solid.polyhedron_adjacent( { 0, 2 } ),
        "Wrong Adjacency after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_adjacent( { 2, 3 } ) == 1,
        "Wrong Adjacency after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_adjacent( { 0, 3 } ) == 2,
        "Wrong Adjacency after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_adjacent( { 1, 1 } ) == 2,
        "Wrong Adjacency after polyhedron permute" );

    const auto polyhedra_0 = solid.polyhedra_around_vertex( 0 );
    geode::OpenGeodeMeshException::test(
        polyhedra_0.size() == 3, "Wrong polyhedra_0 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_0[0].polyhedron_id == 0,
        "Wrong polyhedra_0 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_0[0].vertex_id == 0,
        "Wrong polyhedra_0 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_0[1].polyhedron_id == 2,
        "Wrong polyhedra_0 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_0[1].vertex_id == 1,
        "Wrong polyhedra_0 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_0[2].polyhedron_id == 1,
        "Wrong polyhedra_0 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_0[2].vertex_id == 4,
        "Wrong polyhedra_0 after polyhedron permute" );
}

void test_delete_polyhedra( const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    std::vector< bool > to_delete( polyhedral_solid.nb_polyhedra(), false );
    to_delete.front() = true;
    builder.delete_polyhedra( to_delete );
    geode::OpenGeodeMeshException::test( polyhedral_solid.nb_polyhedra() == 2,
        "PolyhedralSolid should have  polyhedra" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_vertex( { 0, 0 } ) == 5,
        "PolyhedralSolid vertex index is not correct" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_vertex( { 0, 1 } ) == 3,
        "PolyhedralSolid vertex index is not correct" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_vertex( { 0, 2 } ) == 1,
        "PolyhedralSolid vertex index is not correct" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_vertex( { 0, 3 } ) == 7,
        "PolyhedralSolid vertex index is not correct" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.facets().nb_facets() == 11,
        "PolyhedralSolid should have 11 facets" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.edges().nb_edges() == 15,
        "PolyhedralSolid should have 15 edges" );
    builder.edges_builder().delete_isolated_edges();
    builder.facets_builder().delete_isolated_facets();
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.facets().nb_facets() == 8,
        "PolyhedralSolid should have 8 facets" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.edges().nb_edges() == 12,
        "PolyhedralSolid should have 12 edges" );
    auto attribute = polyhedral_solid.edges()
                         .edge_attribute_manager()
                         .find_attribute< geode::index_t >( "test" );
    geode::OpenGeodeMeshException::test( attribute->value( 0 ) == 1,
        "Wrong value for attribute on edge 0 after vertex deletion" );
    geode::OpenGeodeMeshException::test( attribute->value( 1 ) == 3,
        "Wrong value for attribute on edge 1 after vertex deletion" );
}

void test_io( const geode::PolyhedralSolid3D& polyhedral_solid,
    const std::string& filename )
{
    geode::save_polyhedral_solid( polyhedral_solid, filename );
    const auto reloaded = geode::load_polyhedral_solid< 3 >( filename );
    geode_unused( reloaded );
    const auto new_polyhedral_solid = geode::load_polyhedral_solid< 3 >(
        geode::OpenGeodePolyhedralSolid3D::impl_name_static(), filename );

    geode::OpenGeodeMeshException::test(
        new_polyhedral_solid->nb_vertices() == 8,
        "Reloaded PolyhedralSolid should have 8 vertices" );
    for( const auto vertex_id : geode::Range{ polyhedral_solid.nb_vertices() } )
    {
        geode::OpenGeodeMeshException::test(
            polyhedral_solid.point( vertex_id )
                .inexact_equal( new_polyhedral_solid->point( vertex_id ) ),
            "Wrong reloaded mesh point coordinates." );
    }
    geode::OpenGeodeMeshException::test(
        new_polyhedral_solid->facets().nb_facets() == 11,
        "Reloaded PolyhedralSolid should have 11 facets" );
    geode::OpenGeodeMeshException::test(
        new_polyhedral_solid->edges().nb_edges() == 15,
        "Reloaded PolyhedralSolid should have 15 edges" );
    geode::OpenGeodeMeshException::test(
        new_polyhedral_solid->nb_polyhedra() == 3,
        "Reloaded PolyhedralSolid should have 3 polyhedra" );
    geode::OpenGeodeMeshException::test(
        new_polyhedral_solid->facets().facet_from_vertices(
            new_polyhedral_solid->polyhedron_facet_vertices( { 1, 0 } ) )
            == polyhedral_solid.facets().facet_from_vertices(
                polyhedral_solid.polyhedron_facet_vertices( { 1, 0 } ) ),
        "Reloaded PolyhedralSolid has wrong polyhedron facet index" );
    auto attribute = new_polyhedral_solid->facets()
                         .facet_attribute_manager()
                         .find_attribute< geode::index_t >( "test" );
    for( auto f : geode::Range{ new_polyhedral_solid->facets().nb_facets() } )
    {
        geode::OpenGeodeMeshException::test( attribute->value( f ) == f,
            "Reloaded PolyhedralSolid has "
            "wrong attributes on its facets" );
    }
    auto manager = new_polyhedral_solid->texture_manager();
    auto texture_names = manager.texture_names();
    geode::OpenGeodeMeshException::test( texture_names.size() == 1,
        "Reloaded PolyhedralSolid has wrong number of textures" );
    geode::OpenGeodeMeshException::test( texture_names[0] == "texture",
        "Reloaded PolyhedralSolid has wrong texture name" );
}

void test_backward_io( const std::string& filename )
{
    const auto new_polyhedral_solid =
        geode::load_polyhedral_solid< 3 >( filename );

    geode::OpenGeodeMeshException::test(
        new_polyhedral_solid->nb_vertices() == 8,
        "Backward PolyhedralSolid should have 8 vertices" );
    geode::OpenGeodeMeshException::test(
        new_polyhedral_solid->facets().nb_facets() == 11,
        "Backward PolyhedralSolid should have 11 facets" );
    geode::OpenGeodeMeshException::test(
        new_polyhedral_solid->edges().nb_edges() == 15,
        "Backward PolyhedralSolid should have 15 edges" );
    geode::OpenGeodeMeshException::test(
        new_polyhedral_solid->nb_polyhedra() == 3,
        "Backward PolyhedralSolid should have 3 polyhedra" );
    geode::OpenGeodeMeshException::test(
        new_polyhedral_solid->polyhedra_around_vertex( 4 ).size() == 3,
        "Backward PolyhedralSolid has wrong polyhedra around vertex" );
}

void test_barycenters()
{
    auto polyhedral_solid = geode::PolyhedralSolid3D::create(
        geode::OpenGeodePolyhedralSolid3D::impl_name_static() );
    auto builder = geode::PolyhedralSolidBuilder3D::create( *polyhedral_solid );
    const double o{ 0.0 };
    const double a{ 0.6 };
    const double b{ 2.4 };
    const double c{ 1.8 };
    builder->create_point( geode::Point3D{ { o, o, o } } );
    builder->create_point( geode::Point3D{ { a, o, o } } );
    builder->create_point( geode::Point3D{ { o, o, c } } );
    builder->create_point( geode::Point3D{ { o, b, o } } );
    builder->create_point( geode::Point3D{ { a, b, o } } );
    builder->create_point( geode::Point3D{ { o, b, c } } );
    builder->create_polyhedron(
        { 0, 1, 2, 3, 4, 5 }, { { 0, 1, 2 }, { 0, 1, 4, 3 }, { 1, 2, 5, 4 },
                                  { 0, 3, 5, 2 }, { 3, 4, 5 } } );
    const geode::Point3D answer_facet_barycenter{ { a / 3., 0, c / 3. } };
    geode::OpenGeodeMeshException::test(
        polyhedral_solid->facet_barycenter(
            polyhedral_solid->polyhedron_facet_vertices( { 0, 0 } ) )
            == answer_facet_barycenter,
        "PolyhedralSolid facet_barycenter is not correct" );
    const geode::Point3D answer_polyhedron_barycenter{ { a / 3., 0.5 * b,
        c / 3. } };
    geode::OpenGeodeMeshException::test(
        polyhedral_solid->polyhedron_barycenter( 0 )
            == answer_polyhedron_barycenter,
        "PolyhedralSolid polyhedron barycenter is not correct" );
}

void test_normals()
{
    auto polyhedral_solid = geode::PolyhedralSolid3D::create(
        geode::OpenGeodePolyhedralSolid3D::impl_name_static() );
    auto builder = geode::PolyhedralSolidBuilder3D::create( *polyhedral_solid );
    const double o{ 0.0 };
    const double a{ 0.6 };
    const double b{ 2.4 };
    const double c{ 1.8 };
    builder->create_point( geode::Point3D{ { o, b, -c } } );
    builder->create_point( geode::Point3D{ { o, o, o } } );
    builder->create_point( geode::Point3D{ { a, o, o } } );
    builder->create_point( geode::Point3D{ { a, b, o } } );
    builder->create_point( geode::Point3D{ { o, b, o } } );
    builder->create_point( geode::Point3D{ { o, b, c } } );
    builder->create_polyhedron(
        { 0, 1, 2, 3, 4 }, { { 1, 2, 0 }, { 1, 2, 3, 4 }, { 2, 3, 0 },
                               { 3, 4, 0 }, { 4, 1, 0 } } );
    builder->create_polyhedron(
        { 5, 1, 4, 3, 2 }, { { 1, 2, 0 }, { 1, 2, 3, 4 }, { 2, 3, 0 },
                               { 3, 4, 0 }, { 4, 1, 0 } } );

    const geode::Point3D answer_facet_normal{ { 0, 0, 1 } };
    const auto polyhedron_facet_normal0 =
        polyhedral_solid->polyhedron_facet_normal( { 0, 1 } ).value();
    geode::OpenGeodeMeshException::test(
        polyhedron_facet_normal0 == answer_facet_normal,
        "PolyhedralSolid polyhedron_facet_normal is not correct (0, "
        "1)" );

    const auto polyhedron_facet_normal1 =
        polyhedral_solid->polyhedron_facet_normal( { 1, 1 } ).value();
    geode::OpenGeodeMeshException::test(
        polyhedron_facet_normal1 == answer_facet_normal * -1.,
        "PolyhedralSolid polyhedron_facet_normal is not correct (1, "
        "1)" );
}

void test_create_vertex_attribute(
    const geode::PolyhedralSolid3D& polyhedral_solid )
{
    auto attribute = polyhedral_solid.vertex_attribute_manager()
                         .find_or_create_attribute< geode::VariableAttribute,
                             geode::PolyhedronFacetVertex >(
                             "test", geode::PolyhedronFacetVertex{} );
    for( const auto v : geode::Range{ polyhedral_solid.nb_vertices() } )
    {
        attribute->set_value( v, geode::PolyhedronFacetVertex{ { v, 0 }, 1 } );
    }
}

void test_clone( const geode::PolyhedralSolid3D& polyhedral_solid )
{
    const auto polyhedral_solid_clone = polyhedral_solid.clone();
    geode::OpenGeodePolyhedralSolid3D polyhedral_solid2{ std::move(
        *dynamic_cast< geode::OpenGeodePolyhedralSolid3D* >(
            polyhedral_solid_clone.get() ) ) };
    geode::OpenGeodeMeshException::test( polyhedral_solid2.nb_vertices() == 8,
        "PolyhedralSolid2 should have 8 vertices" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid2.facets().nb_facets() == 8,
        "[Test]PolyhedralSolid2 should have 8 facets" );
    geode::OpenGeodeMeshException::test( polyhedral_solid2.nb_polyhedra() == 2,
        "PolyhedralSolid2 should have 2 polyhedra" );

    const auto attribute2 =
        polyhedral_solid2.vertex_attribute_manager()
            .find_attribute< geode::PolyhedronFacetVertex >( "test" );
    std::vector< geode::PolyhedronFacetVertex > att_answer{ { { 4, 0 }, 1 },
        { { 2, 0 }, 1 }, { { 6, 0 }, 1 }, { { 1, 0 }, 1 }, { { 5, 0 }, 1 },
        { { 0, 0 }, 1 }, { { 7, 0 }, 1 }, { { 3, 0 }, 1 } };
    for( const auto v : geode::Range{ polyhedral_solid2.nb_vertices() } )
    {
        geode::OpenGeodeMeshException::test(
            attribute2->value( v ) == att_answer[v],
            "PolyhedralSolid2 attribute is not correct" );
    }
}

void test_set_polyhedron_vertex(
    const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    builder.set_polyhedron_vertex( { 0, 2 }, 2 );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.facets().is_facet_isolated( 0 ),
        "Facet should be isolated before clean" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.edges().is_edge_isolated( 1 ),
        "Edge should be isolated before clean" );
    builder.facets_builder().delete_isolated_facets();
    builder.edges_builder().delete_isolated_edges();
    geode::OpenGeodeMeshException::test(
        !polyhedral_solid.facets().is_facet_isolated( 0 ),
        "Facet should not be isolated after clean" );
    geode::OpenGeodeMeshException::test(
        !polyhedral_solid.edges().is_edge_isolated( 1 ),
        "Edge should not be isolated after clean" );

    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_vertex( { 0, 2 } ) == 2,
        "PolyhedronVertex after set_polyhedron_vertex is wrong" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedron_facet_vertex( { { 0, 1 }, 1 } ) == 4,
        "PolyhedronFacetVertex after set_polyhedron_vertex is "
        "wrong" );
}

void test_delete_all( const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    std::vector< bool > to_delete( polyhedral_solid.nb_polyhedra(), true );
    builder.delete_polyhedra( to_delete );
    geode::OpenGeodeMeshException::test( polyhedral_solid.nb_vertices() == 8,
        "PolyhedralSolid should have 8 vertices" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.is_vertex_isolated( 0 ),
        "Vertices should be isolated after polyhedra deletion" );
    geode::OpenGeodeMeshException::test( polyhedral_solid.nb_polyhedra() == 0,
        "PolyhedralSolid should have 0 polyhedron" );
    builder.edges_builder().delete_isolated_edges();
    builder.facets_builder().delete_isolated_facets();
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.facets().nb_facets() == 0,
        "PolyhedralSolid should have 0 facet" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.edges().nb_edges() == 0,
        "PolyhedralSolid should have 0 edge" );
    geode::OpenGeodeMeshException::test(
        polyhedral_solid.polyhedra_around_vertex( 0 ).empty(),
        "No more polyhedra around vertices" );

    builder.delete_isolated_vertices();
    geode::OpenGeodeMeshException::test( polyhedral_solid.nb_vertices() == 0,
        "[Test]PolyhedralSolid should have 0 vertex" );
}

void test_texture( const geode::PolyhedralSolid3D& polyhedral_solid )
{
    auto manager = polyhedral_solid.texture_manager();
    manager.find_or_create_texture( "texture" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto polyhedral_solid = geode::PolyhedralSolid3D::create(
        geode::OpenGeodePolyhedralSolid3D::impl_name_static() );
    polyhedral_solid->enable_edges();
    polyhedral_solid->enable_facets();
    auto builder = geode::PolyhedralSolidBuilder3D::create( *polyhedral_solid );

    test_create_vertices( *polyhedral_solid, *builder );
    test_create_vertex_attribute( *polyhedral_solid );
    test_create_polyhedra( *polyhedral_solid, *builder );
    test_create_facet_attribute( *polyhedral_solid );
    test_create_edge_attribute( *polyhedral_solid );
    test_edges( *polyhedral_solid );
    test_facets( *polyhedral_solid );
    test_polyhedron_adjacencies( *polyhedral_solid, *builder );
    test_texture( *polyhedral_solid );

    test_io( *polyhedral_solid,
        absl::StrCat( "test.", polyhedral_solid->native_extension() ) );
    test_backward_io( absl::StrCat(
        geode::DATA_PATH, "test_v7.", polyhedral_solid->native_extension() ) );
    test_backward_io( absl::StrCat(
        geode::DATA_PATH, "test_v12.", polyhedral_solid->native_extension() ) );

    test_permutation( *polyhedral_solid, *builder );
    test_delete_polyhedra( *polyhedral_solid, *builder );
    test_clone( *polyhedral_solid );
    test_set_polyhedron_vertex( *polyhedral_solid, *builder );
    test_delete_all( *polyhedral_solid, *builder );

    test_barycenters();
    test_normals();
}

OPENGEODE_TEST( "polyhedral-solid" )