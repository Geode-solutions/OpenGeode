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

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/geode/geode_tetrahedral_solid_builder.hpp>
#include <geode/mesh/builder/solid_edges_builder.hpp>
#include <geode/mesh/builder/solid_facets_builder.hpp>
#include <geode/mesh/core/geode/geode_tetrahedral_solid.hpp>
#include <geode/mesh/core/solid_edges.hpp>
#include <geode/mesh/core/solid_facets.hpp>
#include <geode/mesh/io/tetrahedral_solid_input.hpp>
#include <geode/mesh/io/tetrahedral_solid_output.hpp>

#include <geode/tests/common.hpp>

void test_create_vertices( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.create_point( geode::Point3D{ { 0.1, 0.2, 0.3 } } );
    builder.create_point( geode::Point3D{ { 2.1, 9.4, 6.7 } } );
    builder.create_point( geode::Point3D{ { 7.5, 5.2, 6.3 } } );
    builder.create_point( geode::Point3D{ { 8.1, 1.4, 4.7 } } );
    builder.create_point( geode::Point3D{ { 4.7, 2.1, 1.3 } } );
    builder.create_point( geode::Point3D{ { 1.6, 8.7, 6.1 } } );
    geode::OpenGeodeMeshException::test(
        solid.nb_vertices() == 6, "TetrahedralSolid should have 6 vertices" );
}

void test_create_tetrahedra( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.reserve_tetrahedra( 3 );
    geode::OpenGeodeMeshException::test( solid.nb_polyhedra() == 0,
        "TetrahedralSolid should have no tetrahedron" );
    builder.create_tetrahedra( 1 );
    builder.create_tetrahedron( { 1, 2, 3, 4 } );
    builder.create_polyhedron( { 1, 4, 3, 5 },
        { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } );
    geode::OpenGeodeMeshException::test( solid.nb_polyhedra() == 3,
        "TetrahedralSolid should have 3 tetrahedra" );
    builder.set_polyhedron_vertex( { 0, 0 }, 0 );
    builder.set_polyhedron_vertex( { 0, 1 }, 1 );
    builder.set_polyhedron_vertex( { 0, 2 }, 2 );
    builder.set_polyhedron_vertex( { 0, 3 }, 3 );
}

void test_polyhedron_volumes( const geode::TetrahedralSolid3D& solid )
{
    for( const auto p : geode::Range{ solid.nb_polyhedra() } )
    {
        geode::OpenGeodeMeshException::test(
            std::fabs(
                solid.polyhedron_volume( p )
                - geode::tetrahedron_signed_volume( solid.tetrahedron( p ) ) )
                < geode::GLOBAL_EPSILON,
            "Not correct tetrahedron volume computation" );
    }
}

void test_polyhedron_facet_area( const geode::TetrahedralSolid3D& solid )
{
    for( const auto p : geode::Range{ solid.nb_polyhedra() } )
    {
        for( const auto f : geode::LRange{ solid.nb_polyhedron_facets( p ) } )
        {
            auto actual = std::fabs( solid.polyhedron_facet_area( { p, f } ) );
            auto expected = geode::triangle_area( solid.triangle( { p, f } ) );
            geode::OpenGeodeMeshException::test(
                std::fabs( actual - expected ) < geode::GLOBAL_EPSILON,
                "Not correct tetrahedron facet area computation" );
        }
    }
}

void test_polyhedron_adjacencies( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.compute_polyhedron_adjacencies();
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_adjacent( { 0, 0 } ) == 1,
        "TetrahedralSolid adjacent index is not correct" );
    geode::OpenGeodeMeshException::test( !solid.polyhedron_adjacent( { 0, 1 } ),
        "TetrahedralSolid adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_adjacent( { 1, 3 } ) == 0,
        "TetrahedralSolid adjacent index is not correct" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_adjacent_facet( { 0, 0 } )
            == geode::PolyhedronFacet( 1, 3 ),
        "TetrahedralSolid adjacent index is not correct" );

    geode::OpenGeodeMeshException::test(
        solid.polyhedron_adjacent( { 2, 3 } ) == 1,
        "TetrahedralSolid adjacent index is not correct" );
    geode::OpenGeodeMeshException::test( !solid.polyhedron_adjacent( { 2, 0 } ),
        "TetrahedralSolid adjacent index is not correct" );

    solid.enable_edges();
    solid.enable_facets();
    geode::OpenGeodeMeshException::test( solid.facets().nb_facets() == 10,
        "TetrahedralSolid should have 10 facets" );
    geode::OpenGeodeMeshException::test( solid.edges().nb_edges() == 12,
        "TetrahedralSolid should have 12 edges" );

    const auto around_0 = solid.vertices_around_vertex( 0 );
    geode::OpenGeodeMeshException::test( around_0.size() == 3,
        "There should be 3 vertices around vertex 0, not ", around_0.size() );
    for( const auto vertex_id : around_0 )
    {
        geode::OpenGeodeMeshException::test(
            vertex_id == 1 || vertex_id == 2 || vertex_id == 3,
            "Wrong vertices around vertex 0." );
    }
    const auto around_1 = solid.vertices_around_vertex( 1 );
    geode::OpenGeodeMeshException::test( around_1.size() == 5,
        "There should be 5 vertices around vertex 1, not ", around_1.size() );
    for( const auto vertex_id : around_1 )
    {
        geode::OpenGeodeMeshException::test(
            vertex_id == 0 || vertex_id == 2 || vertex_id == 3 || vertex_id == 4
                || vertex_id == 5,
            "Wrong vertices around vertex 1." );
    }
}

void test_is_on_border( const geode::TetrahedralSolid3D& solid )
{
    for( const auto v : geode::Range{ solid.nb_vertices() } )
    {
        geode::OpenGeodeMeshException::test( solid.is_vertex_on_border( v ),
            "All vertices should be on border" );
    }
    for( const auto e : geode::Range{ solid.edges().nb_edges() } )
    {
        geode::OpenGeodeMeshException::test(
            solid.is_edge_on_border( solid.edges().edge_vertices( e ) ),
            "All edges should be on border" );
    }
}

void test_permutation( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    std::vector< geode::index_t > vertex_permutation{ 4, 2, 1, 5, 0, 3 };
    builder.permute_vertices( vertex_permutation );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 0 } ) == 4,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 1 } ) == 2,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 2 } ) == 1,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 3 } ) == 5,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 0 } ) == 2,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 1 } ) == 1,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 2 } ) == 5,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 3 } ) == 0,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 0 } ) == 2,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 1 } ) == 0,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 2 } ) == 5,
        "Wrong PolyhedronVertex after vertex permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 3 } ) == 3,
        "Wrong PolyhedronVertex after vertex permute" );

    std::vector< geode::index_t > polyhedron_permutation{ 2, 0, 1 };
    builder.permute_polyhedra( polyhedron_permutation );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 0 } ) == 2,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 1 } ) == 0,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 2 } ) == 5,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 3 } ) == 3,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 0 } ) == 4,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 1 } ) == 2,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 2 } ) == 1,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 1, 3 } ) == 5,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 0 } ) == 2,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 1 } ) == 1,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 2 } ) == 5,
        "Wrong PolyhedronVertex after polyhedron permute" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 3 } ) == 0,
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
        solid.polyhedron_adjacent( { 1, 0 } ) == 2,
        "Wrong Adjacency after polyhedron permute" );

    const auto polyhedra_5 = solid.polyhedra_around_vertex( 5 );
    geode::OpenGeodeMeshException::test(
        polyhedra_5.size() == 3, "Wrong polyhedra_5 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_5[0].polyhedron_id == 0,
        "Wrong polyhedra_5 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_5[0].vertex_id == 2,
        "Wrong polyhedra_5 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_5[1].polyhedron_id == 2,
        "Wrong polyhedra_5 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_5[1].vertex_id == 2,
        "Wrong polyhedra_5 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_5[2].polyhedron_id == 1,
        "Wrong polyhedra_5 after polyhedron permute" );
    geode::OpenGeodeMeshException::test( polyhedra_5[2].vertex_id == 3,
        "Wrong polyhedra_5 after polyhedron permute" );
}

void test_delete_polyhedron( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    std::vector< bool > to_delete( solid.nb_polyhedra(), false );
    to_delete.front() = true;
    builder.delete_polyhedra( to_delete );
    geode::OpenGeodeMeshException::test(
        solid.nb_polyhedra() == 2, "TetrahedralSolid should have 2 polyhedra" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 0 } ) == 4,
        "TetrahedralSolid facet vertex index is not correct" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 1 } ) == 2,
        "TetrahedralSolid facet vertex index is not correct" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 2 } ) == 1,
        "TetrahedralSolid facet vertex index is not correct" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 0, 3 } ) == 5,
        "TetrahedralSolid facet vertex index is not correct" );
}

void test_io(
    const geode::TetrahedralSolid3D& solid, const std::string& filename )
{
    geode::save_tetrahedral_solid( solid, filename );
    const auto reloaded = geode::load_tetrahedral_solid< 3 >( filename );
    geode_unused( reloaded );
    const auto new_solid = geode::load_tetrahedral_solid< 3 >(
        geode::OpenGeodeTetrahedralSolid3D::impl_name_static(), filename );
    geode::OpenGeodeMeshException::test( new_solid->nb_vertices() == 6,
        "Reloaded TetrahedralSolid should have 6 vertices" );
    for( const auto vertex_id : geode::Range{ solid.nb_vertices() } )
    {
        geode::OpenGeodeMeshException::test(
            solid.point( vertex_id )
                .inexact_equal( new_solid->point( vertex_id ) ),
            "Wrong reloaded mesh point coordinates." );
    }
    geode::OpenGeodeMeshException::test( new_solid->facets().nb_facets() == 10,
        "Reloaded TetrahedralSolid should have 10 facets" );
    geode::OpenGeodeMeshException::test( new_solid->edges().nb_edges() == 12,
        "Reloaded TetrahedralSolid should have 12 edges" );
    geode::OpenGeodeMeshException::test( new_solid->nb_polyhedra() == 3,
        "Reloaded TetrahedralSolid should have 3 polyhedra" );
}

void test_backward_io( const std::string& filename )
{
    const auto solid = geode::load_tetrahedral_solid< 3 >( filename );
    geode::OpenGeodeMeshException::test( solid->nb_vertices() == 6,
        "Backward TetrahedralSolid should have 6 vertices" );
    geode::OpenGeodeMeshException::test( solid->nb_polyhedra() == 3,
        "Backward TetrahedralSolid should have 3 polyhedra" );
    geode::OpenGeodeMeshException::test( solid->edges().nb_edges() == 12,
        "Backward TetrahedralSolid should have 12 edges" );
    geode::OpenGeodeMeshException::test( solid->facets().nb_facets() == 10,
        "Backward TetrahedralSolid should have 10 facets" );
    for( const auto* name :
        { "polyhedron_around_vertex", "polyhedra_around_vertex" } )
    {
        geode::OpenGeodeMeshException::test(
            !solid->vertex_attribute_manager()
                .attribute_ids_matching_name( name )
                .has_value(),
            "Backward TetrahedralSolid should not have legacy attribute ",
            name );
    }
    for( const auto* name :
        { "tetrahedron_vertices", "tetrahedron_adjacents" } )
    {
        geode::OpenGeodeMeshException::test(
            !solid->polyhedron_attribute_manager()
                .attribute_ids_matching_name( name )
                .has_value(),
            "Backward TetrahedralSolid should not have legacy attribute ",
            name );
    }
    test_is_on_border( *solid );
}

void test_clone( const geode::TetrahedralSolid3D& solid )
{
    geode::AttributeProperties attribute_properties;
    attribute_properties.assignable = false;
    attribute_properties.interpolable = true;
    attribute_properties.transferable = true;
    geode::AttributeValues< geode::index_t > attribute_values;
    attribute_values.default_value = 0;
    attribute_values.no_value = geode::NO_ID;
    auto attr_from_id =
        solid.facets()
            .facet_attribute_manager()
            .create_attribute< geode::VariableAttribute, geode::index_t >(
                "facet_id", attribute_values, attribute_properties );
    auto attr_from =
        solid.facets()
            .facet_attribute_manager()
            .find_attribute< geode::VariableAttribute, geode::index_t >(
                attr_from_id );
    for( const auto f : geode::Range{ solid.facets().nb_facets() } )
    {
        attr_from->set_value( f, f );
    }
    auto attr_edge_from_id =
        solid.edges()
            .edge_attribute_manager()
            .create_attribute< geode::VariableAttribute, geode::index_t >(
                "edge_id", attribute_values, attribute_properties );
    auto attr_edge_from =
        solid.edges()
            .edge_attribute_manager()
            .find_attribute< geode::VariableAttribute, geode::index_t >(
                attr_edge_from_id );
    for( const auto e : geode::Range{ solid.edges().nb_edges() } )
    {
        attr_edge_from->set_value( e, e );
    }

    const auto solid2 = solid.clone();
    geode::OpenGeodeTetrahedralSolid3D solid4{ std::move(
        *dynamic_cast< geode::OpenGeodeTetrahedralSolid3D* >(
            solid2.get() ) ) };
    const auto attr_to =
        solid4.facets()
            .facet_attribute_manager()
            .find_read_only_attribute< geode::index_t >( attr_from_id );
    for( const auto v : geode::Range{ solid4.nb_vertices() } )
    {
        geode::OpenGeodeMeshException::test(
            solid4.point( v ) == solid.point( v ),
            "Wrong cloned mesh point coordinates." );
    }
    for( const auto p : geode::Range{ solid4.nb_polyhedra() } )
    {
        for( const auto f : geode::LRange{ 4 } )
        {
            for( const auto v : geode::LRange{ 3 } )
            {
                geode::OpenGeodeMeshException::test(
                    solid4.polyhedron_facet_vertex( { { p, f }, v } )
                        == solid.polyhedron_facet_vertex( { { p, f }, v } ),
                    "polyhedron_facet_vertex should match" );
            }
        }
    }
    for( const auto f : geode::Range{ solid.facets().nb_facets() } )
    {
        geode::OpenGeodeMeshException::test(
            attr_from->value( f ) == attr_to->value( f ),
            "Error in facet attribute transfer during cloning" );
        const auto from_vertices = solid.facets().facet_vertices( f );
        const auto to_vertices = solid4.facets().facet_vertices( f );
        for( const auto v : geode::Indices{ from_vertices } )
        {
            geode::OpenGeodeMeshException::test(
                from_vertices[v] == to_vertices[v],
                "Error in facet vertices transfer during cloning" );
        }
    }
    for( const auto v : geode::Range{ solid4.nb_vertices() } )
    {
        geode::OpenGeodeMeshException::test(
            solid.polyhedra_around_vertex( v )
                == solid4.polyhedra_around_vertex( v ),
            "Polyhedra around vertex should match during cloning" );
    }
    const auto attr_edge_to =
        solid4.edges()
            .edge_attribute_manager()
            .find_read_only_attribute< geode::index_t >( attr_edge_from_id );
    for( const auto e : geode::Range{ solid.edges().nb_edges() } )
    {
        geode::OpenGeodeMeshException::test(
            attr_edge_from->value( e ) == attr_edge_to->value( e ),
            "Error in edge attribute transfer during cloning" );
    }
    geode::OpenGeodeMeshException::test(
        solid4.nb_vertices() == 6, "TetrahedralSolid2 should have 6 vertices" );
    geode::OpenGeodeMeshException::test(
        solid4.facets().nb_facets() == solid.facets().nb_facets(),
        "TetrahedralSolid2 should have same number of facets" );
    geode::OpenGeodeMeshException::test( solid4.nb_polyhedra() == 2,
        "TetrahedralSolid2 should have 2 polyhedra" );
}

void test_delete_all( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.delete_isolated_vertices();
    geode::OpenGeodeMeshException::test(
        solid.nb_vertices() == 5, "TetrahedralSolid should have 5 vertices" );
    geode::OpenGeodeMeshException::test( solid.facets().nb_facets() == 7,
        "TetrahedralSolid should have 4 facets" );
    geode::OpenGeodeMeshException::test( solid.nb_polyhedra() == 2,
        "TetrahedralSolid should have 1 polyhedron" );

    std::vector< bool > to_delete( solid.nb_polyhedra(), true );
    builder.delete_polyhedra( to_delete );
    geode::OpenGeodeMeshException::test(
        solid.nb_vertices() == 5, "TetrahedralSolid should have 5 vertices" );
    geode::OpenGeodeMeshException::test( solid.facets().nb_facets() == 7,
        "TetrahedralSolid should have 7 facet" );
    geode::OpenGeodeMeshException::test( solid.nb_polyhedra() == 0,
        "TetrahedralSolid should have 0 polyhedron" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedra_around_vertex( 0 ).empty(),
        "No more polyhedron around vertices" );

    std::vector< bool > f_to_delete( solid.facets().nb_facets(), true );
    builder.facets_builder().delete_facets( f_to_delete );
    geode::OpenGeodeMeshException::test( solid.facets().nb_facets() == 0,
        "TetrahedralSolid should have 0 facet" );

    builder.delete_isolated_vertices();
    geode::OpenGeodeMeshException::test(
        solid.nb_vertices() == 0, "TetrahedralSolid should have 0 vertex" );
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

void check_v18_5( const geode::TetrahedralSolid3D& solid )
{
    geode::OpenGeodeMeshException::test(
        solid.nb_vertices() == 6 && solid.nb_polyhedra() == 3,
        "v18_5 TetrahedralSolid has wrong number of elements" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( { 2, 2 } ) == 4,
        "v18_5 TetrahedralSolid has wrong polyhedron vertex" );
    geode::index_t nb_adjacents{ 0 };
    for( const auto p : geode::Range{ solid.nb_polyhedra() } )
    {
        for( const auto f : geode::LRange{ 4 } )
        {
            if( solid.polyhedron_adjacent( { p, f } ) )
            {
                nb_adjacents++;
            }
        }
    }
    geode::OpenGeodeMeshException::test(
        nb_adjacents == 4, "v18_5 TetrahedralSolid has wrong adjacencies" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedra_around_vertex( 5 ).size() == 3
            && solid.polyhedra_around_vertex( 0 ).size() == 1,
        "v18_5 TetrahedralSolid has wrong polyhedra around vertex" );
    geode::OpenGeodeMeshException::test( solid.edges().nb_edges() == 12,
        "v18_5 TetrahedralSolid has wrong number of edges" );
    geode::OpenGeodeMeshException::test( solid.facets().nb_facets() == 10,
        "v18_5 TetrahedralSolid has wrong number of facets" );
    check_user_value( solid.vertex_attribute_manager(), 5 );
    check_user_value( solid.polyhedron_attribute_manager(), 2 );
    check_user_value( solid.edges().edge_attribute_manager(), 11 );
    check_user_value( solid.facets().facet_attribute_manager(), 9 );
    check_no_legacy_attributes( solid.vertex_attribute_manager(),
        { "polyhedron_around_vertex", "polyhedra_around_vertex" } );
    check_no_legacy_attributes( solid.polyhedron_attribute_manager(),
        { "tetrahedron_vertices", "tetrahedron_adjacents" } );
    check_no_legacy_attributes( solid.edges().edge_attribute_manager(),
        { "counter", "facet_vertices" } );
    check_no_legacy_attributes( solid.facets().facet_attribute_manager(),
        { "counter", "facet_vertices" } );
}

void test_backward_v18_5()
{
    const auto solid = geode::load_tetrahedral_solid< 3 >(
        absl::StrCat( geode::DATA_PATH, "backward_io/v18_5/v18_5.og_tso3d" ) );
    check_v18_5( *solid );
    const auto filename =
        absl::StrCat( "v18_5_reload.", solid->native_extension() );
    geode::save_tetrahedral_solid( *solid, filename );
    check_v18_5( *geode::load_tetrahedral_solid< 3 >( filename ) );
}

std::unique_ptr< geode::TetrahedralSolid3D > create_solid_with_points(
    geode::index_t nb_points )
{
    auto solid = geode::TetrahedralSolid3D::create(
        geode::OpenGeodeTetrahedralSolid3D::impl_name_static() );
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    for( const auto p : geode::Range{ nb_points } )
    {
        builder->create_point( geode::Point3D{ { static_cast< double >( p % 3 ),
            static_cast< double >( p % 5 ), static_cast< double >( p ) } } );
    }
    return solid;
}

void check_vertex_link(
    const geode::TetrahedralSolid3D& solid, geode::index_t vertex )
{
    const auto polyhedron_vertex = solid.polyhedron_around_vertex( vertex );
    geode::OpenGeodeMeshException::test( polyhedron_vertex.has_value(),
        "Vertex ", vertex, " should not be isolated" );
    geode::OpenGeodeMeshException::test(
        solid.polyhedron_vertex( polyhedron_vertex.value() ) == vertex,
        "Vertex ", vertex, " is linked to a wrong polyhedron vertex" );
}

void test_set_polyhedron_vertex_relink()
{
    auto solid = create_solid_with_points( 6 );
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 1, 2, 3, 4 } );
    builder->compute_polyhedron_adjacencies();
    const auto around = solid->polyhedron_around_vertex( 1 );
    geode::OpenGeodeMeshException::test(
        around.has_value() && around->polyhedron_id == 1,
        "Vertex 1 should be linked to tetrahedron 1" );
    builder->set_polyhedron_vertex( around.value(), 5 );
    check_vertex_link( *solid, 1 );
    check_vertex_link( *solid, 5 );
    geode::OpenGeodeMeshException::test(
        solid->polyhedron_around_vertex( 1 )->polyhedron_id == 0,
        "Vertex 1 should now be linked to tetrahedron 0" );
}

void test_non_manifold_facet_adjacencies()
{
    auto solid = create_solid_with_points( 6 );
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 0, 1, 2, 4 } );
    builder->create_tetrahedron( { 0, 1, 2, 5 } );
    builder->compute_polyhedron_adjacencies();
    for( const auto t : geode::Range{ 3 } )
    {
        geode::OpenGeodeMeshException::test(
            solid->polyhedron_facets_on_border( t ).size() == 4, "Tetrahedron ",
            t,
            " should have all its facets on border: facet (0,1,2) is "
            "shared by 3 tetrahedra" );
    }
}

void test_delete_polyhedron_at_pinched_vertex()
{
    auto solid = create_solid_with_points( 7 );
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 0, 4, 5, 6 } );
    builder->compute_polyhedron_adjacencies();
    std::vector< bool > to_delete{ false, true };
    builder->delete_polyhedra( to_delete );
    geode::OpenGeodeMeshException::test( solid->nb_polyhedra() == 1,
        "Solid should have 1 tetrahedron after deletion" );
    check_vertex_link( *solid, 0 );
    for( const auto v : geode::Range{ 4, 7 } )
    {
        geode::OpenGeodeMeshException::test( solid->is_vertex_isolated( v ),
            "Vertex ", v, " should be isolated after deletion" );
    }
}

void test_permute_vertices_with_isolated_vertex()
{
    std::vector< geode::index_t > permutation{ 0, 1, 2, 3, 4 };
    do
    {
        auto solid = create_solid_with_points( 5 );
        auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
        builder->create_tetrahedron( { 1, 2, 3, 4 } );
        const auto old2new = builder->permute_vertices( permutation );
        for( const auto old_v : geode::Range{ 5 } )
        {
            const auto new_v = old2new[old_v];
            if( old_v == 0 )
            {
                geode::OpenGeodeMeshException::test(
                    solid->is_vertex_isolated( new_v ),
                    "Isolated vertex should stay isolated after "
                    "permutation" );
            }
            else
            {
                check_vertex_link( *solid, new_v );
            }
        }
    } while( absl::c_next_permutation( permutation ) );
}

void test_replace_vertices_chained()
{
    auto solid = create_solid_with_points( 9 );
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_tetrahedron( { 0, 3, 4, 5 } );
    builder->create_tetrahedron( { 1, 6, 7, 8 } );
    geode::GenericMapping< geode::index_t > mapping;
    mapping.map( 0, 1 );
    mapping.map( 1, 2 );
    builder->replace_vertices( mapping );
    geode::OpenGeodeMeshException::test(
        solid->polyhedron_vertex( { 0, 0 } ) == 1
            && solid->polyhedron_vertex( { 1, 0 } ) == 2,
        "Wrong polyhedron vertices after replace_vertices" );
    geode::OpenGeodeMeshException::test( solid->is_vertex_isolated( 0 ),
        "Vertex 0 should be isolated after replace_vertices" );
    check_vertex_link( *solid, 1 );
    check_vertex_link( *solid, 2 );
}

void test_create_invalid_polyhedron()
{
    auto solid = create_solid_with_points( 5 );
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    const auto check_invalid = [&solid, &builder](
                                   absl::Span< const geode::index_t > vertices,
                                   const std::string& message ) {
        bool thrown{ false };
        try
        {
            builder->create_polyhedron( vertices, {} );
        }
        catch( const geode::OpenGeodeException& )
        {
            thrown = true;
        }
        geode::OpenGeodeMeshException::test(
            thrown, "Creating ", message, " should throw" );
        geode::OpenGeodeMeshException::test( solid->nb_polyhedra() == 1,
            "Creating ", message, " should not add a polyhedron" );
        geode::OpenGeodeMeshException::test(
            solid->polyhedron_around_vertex( 0 )->polyhedron_id == 0,
            "Creating ", message, " should not change vertex links" );
    };
    check_invalid( std::vector< geode::index_t >{ 0, 1, 4 }, "3 vertices" );
    check_invalid(
        std::vector< geode::index_t >{ 0, 1, 2, 4, 3 }, "5 vertices" );
    check_invalid( std::vector< geode::index_t >{ 0, 1, 2, 42 },
        "an invalid vertex index" );
}

void test_degenerated_tetrahedron_with_repeated_vertex()
{
    auto solid = geode::TetrahedralSolid3D::create(
        geode::OpenGeodeTetrahedralSolid3D::impl_name_static() );
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_point( geode::Point3D{ { 0, 0, 10 } } );
    builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
    builder->create_point( geode::Point3D{ { 1, 0, 0 } } );
    builder->create_point( geode::Point3D{ { 0, 1, 0 } } );
    builder->create_tetrahedron( { 1, 2, 3, 1 } );
    builder->create_tetrahedron( { 1, 2, 3, 0 } );
    geode::OpenGeodeMeshException::test(
        solid->polyhedron_minimum_height( 0 ) == 0.,
        "Tetrahedron with repeated vertex should have a null height" );
    geode::OpenGeodeMeshException::test( solid->is_polyhedron_degenerated( 0 ),
        "Tetrahedron with repeated vertex should be degenerated" );
    geode::OpenGeodeMeshException::test(
        std::fabs(
            solid->polyhedron_minimum_height( 1 ) - 1. / std::sqrt( 2.01 ) )
            < geode::GLOBAL_EPSILON,
        "Wrong minimum height of tetrahedron 1" );
    geode::OpenGeodeMeshException::test( !solid->is_polyhedron_degenerated( 1 ),
        "Tetrahedron 1 should not be degenerated" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_backward_v18_5();
    auto solid = geode::TetrahedralSolid3D::create(
        geode::OpenGeodeTetrahedralSolid3D::impl_name_static() );
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );

    test_create_vertices( *solid, *builder );
    test_create_tetrahedra( *solid, *builder );
    test_polyhedron_volumes( *solid );
    test_polyhedron_facet_area( *solid );
    test_polyhedron_adjacencies( *solid, *builder );
    test_is_on_border( *solid );
    test_io( *solid, absl::StrCat( "test.", solid->native_extension() ) );
    test_backward_io( absl::StrCat(
        geode::DATA_PATH, "backward_io/v17/v17.", solid->native_extension() ) );
    test_permutation( *solid, *builder );
    test_delete_polyhedron( *solid, *builder );
    test_clone( *solid );
    test_delete_all( *solid, *builder );
    test_set_polyhedron_vertex_relink();
    test_non_manifold_facet_adjacencies();
    test_delete_polyhedron_at_pinched_vertex();
    test_permute_vertices_with_isolated_vertex();
    test_replace_vertices_chained();
    test_create_invalid_polyhedron();
    test_degenerated_tetrahedron_with_repeated_vertex();
}

OPENGEODE_TEST( "tetrahedral-solid" )