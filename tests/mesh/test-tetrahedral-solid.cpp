/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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
    OPENGEODE_EXCEPTION( solid.nb_vertices() == 6,
        "[Test] TetrahedralSolid should have 6 vertices" );
}

void test_create_tetrahedra( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.reserve_tetrahedra( 3 );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 0,
        "[Test] TetrahedralSolid should have no tetrahedron" );
    builder.create_tetrahedra( 1 );
    builder.create_tetrahedron( { 1, 2, 3, 4 } );
    builder.create_polyhedron( { 1, 4, 3, 5 },
        { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 3,
        "[Test] TetrahedralSolid should have 3 tetrahedra" );
    builder.set_polyhedron_vertex( { 0, 0 }, 0 );
    builder.set_polyhedron_vertex( { 0, 1 }, 1 );
    builder.set_polyhedron_vertex( { 0, 2 }, 2 );
    builder.set_polyhedron_vertex( { 0, 3 }, 3 );
}

void test_polyhedron_volumes( const geode::TetrahedralSolid3D& solid )
{
    for( const auto p : geode::Range{ solid.nb_polyhedra() } )
    {
        OPENGEODE_EXCEPTION( std::fabs( solid.polyhedron_volume( p )
                                        - geode::tetrahedron_signed_volume(
                                            solid.tetrahedron( p ) ) )
                                 < geode::GLOBAL_EPSILON,
            "[Test] Not correct tetrahedron volume computation" );
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
            OPENGEODE_EXCEPTION(
                std::fabs( actual - expected ) < geode::GLOBAL_EPSILON,
                "[Test] Not correct tetrahedron facet area computation" );
        }
    }
}

void test_polyhedron_adjacencies( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.compute_polyhedron_adjacencies();
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 0, 0 } ) == 1,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( !solid.polyhedron_adjacent( { 0, 1 } ),
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 1, 3 } ) == 0,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent_facet( { 0, 0 } )
                             == geode::PolyhedronFacet( 1, 3 ),
        "[Test] TetrahedralSolid adjacent index is not correct" );

    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 2, 3 } ) == 1,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( !solid.polyhedron_adjacent( { 2, 0 } ),
        "[Test] TetrahedralSolid adjacent index is not correct" );

    solid.enable_edges();
    solid.enable_facets();
    OPENGEODE_EXCEPTION( solid.facets().nb_facets() == 10,
        "[Test] TetrahedralSolid should have 10 facets" );
    OPENGEODE_EXCEPTION( solid.edges().nb_edges() == 12,
        "[Test] TetrahedralSolid should have 12 edges" );

    const auto around_0 = solid.vertices_around_vertex( 0 );
    OPENGEODE_EXCEPTION( around_0.size() == 3,
        "[Test] There should be 3 vertices around vertex 0, not ",
        around_0.size() );
    for( const auto vertex_id : around_0 )
    {
        OPENGEODE_EXCEPTION( vertex_id == 1 || vertex_id == 2 || vertex_id == 3,
            "[Test] Wrong vertices around vertex 0." );
    }
    const auto around_1 = solid.vertices_around_vertex( 1 );
    OPENGEODE_EXCEPTION( around_1.size() == 5,
        "[Test] There should be 5 vertices around vertex 1, not ",
        around_1.size() );
    for( const auto vertex_id : around_1 )
    {
        OPENGEODE_EXCEPTION( vertex_id == 0 || vertex_id == 2 || vertex_id == 3
                                 || vertex_id == 4 || vertex_id == 5,
            "[Test] Wrong vertices around vertex 1." );
    }
}

void test_is_on_border( const geode::TetrahedralSolid3D& solid )
{
    for( const auto v : geode::Range{ solid.nb_vertices() } )
    {
        OPENGEODE_EXCEPTION( solid.is_vertex_on_border( v ),
            "[Test] All vertices should be on border" );
    }
    for( const auto e : geode::Range{ solid.edges().nb_edges() } )
    {
        OPENGEODE_EXCEPTION(
            solid.is_edge_on_border( solid.edges().edge_vertices( e ) ),
            "[Test] All edges should be on border" );
    }
}

void test_permutation( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    std::vector< geode::index_t > vertex_permutation{ 4, 2, 1, 5, 0, 3 };
    builder.permute_vertices( vertex_permutation );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 0 } ) == 4,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 1 } ) == 2,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 2 } ) == 1,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 3 } ) == 5,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 0 } ) == 2,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 1 } ) == 1,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 2 } ) == 5,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 3 } ) == 0,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 0 } ) == 2,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 1 } ) == 0,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 2 } ) == 5,
        "[Test] Wrong PolyhedronVertex after vertex permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 3 } ) == 3,
        "[Test] Wrong PolyhedronVertex after vertex permute" );

    std::vector< geode::index_t > polyhedron_permutation{ 2, 0, 1 };
    builder.permute_polyhedra( polyhedron_permutation );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 0 } ) == 2,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 1 } ) == 0,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 2 } ) == 5,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 3 } ) == 3,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 0 } ) == 4,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 1 } ) == 2,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 2 } ) == 1,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 1, 3 } ) == 5,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 0 } ) == 2,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 1 } ) == 1,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 2 } ) == 5,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 2, 3 } ) == 0,
        "[Test] Wrong PolyhedronVertex after polyhedron permute" );

    OPENGEODE_EXCEPTION( !solid.polyhedron_adjacent( { 0, 2 } ),
        "[Test] Wrong Adjacency after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 2, 3 } ) == 1,
        "[Test] Wrong Adjacency after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 0, 3 } ) == 2,
        "[Test] Wrong Adjacency after polyhedron permute" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 1, 0 } ) == 2,
        "[Test] Wrong Adjacency after polyhedron permute" );

    const auto polyhedra_5 = solid.polyhedra_around_vertex( 5 );
    OPENGEODE_EXCEPTION( polyhedra_5.size() == 3,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[0].polyhedron_id == 0,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[0].vertex_id == 2,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[1].polyhedron_id == 2,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[1].vertex_id == 2,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[2].polyhedron_id == 1,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
    OPENGEODE_EXCEPTION( polyhedra_5[2].vertex_id == 3,
        "[Test] Wrong polyhedra_5 after polyhedron permute" );
}

void test_delete_polyhedron( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    std::vector< bool > to_delete( solid.nb_polyhedra(), false );
    to_delete.front() = true;
    builder.delete_polyhedra( to_delete );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 2,
        "[Test] TetrahedralSolid should have 2 polyhedra" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 0 } ) == 4,
        "[Test] TetrahedralSolid facet vertex index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 1 } ) == 2,
        "[Test] TetrahedralSolid facet vertex index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 2 } ) == 1,
        "[Test] TetrahedralSolid facet vertex index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 3 } ) == 5,
        "[Test] TetrahedralSolid facet vertex index is not correct" );
}

void test_io(
    const geode::TetrahedralSolid3D& solid, const std::string& filename )
{
    geode::save_tetrahedral_solid( solid, filename );
    const auto reloaded = geode::load_tetrahedral_solid< 3 >( filename );
    geode_unused( reloaded );
    const auto new_solid = geode::load_tetrahedral_solid< 3 >(
        geode::OpenGeodeTetrahedralSolid3D::impl_name_static(), filename );
    OPENGEODE_EXCEPTION( new_solid->nb_vertices() == 6,
        "[Test] Reloaded TetrahedralSolid should have 6 vertices" );
    for( const auto vertex_id : geode::Range{ solid.nb_vertices() } )
    {
        OPENGEODE_EXCEPTION(
            solid.point( vertex_id )
                .inexact_equal( new_solid->point( vertex_id ) ),
            "[Test] Wrong reloaded mesh point coordinates." );
    }
    OPENGEODE_EXCEPTION( new_solid->facets().nb_facets() == 10,
        "[Test] Reloaded TetrahedralSolid should have 10 facets" );
    OPENGEODE_EXCEPTION( new_solid->edges().nb_edges() == 12,
        "[Test] Reloaded TetrahedralSolid should have 12 edges" );
    OPENGEODE_EXCEPTION( new_solid->nb_polyhedra() == 3,
        "[Test] Reloaded TetrahedralSolid should have 3 polyhedra" );
}

void test_clone( const geode::TetrahedralSolid3D& solid )
{
    auto attr_from = solid.facets()
                         .facet_attribute_manager()
                         .find_or_create_attribute< geode::VariableAttribute,
                             geode::index_t >( "facet_id", 0 );
    for( const auto f : geode::Range{ solid.facets().nb_facets() } )
    {
        attr_from->set_value( f, f );
    }
    auto attr_edge_from =
        solid.edges()
            .edge_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute,
                geode::index_t >( "edge_id", 0 );
    for( const auto e : geode::Range{ solid.edges().nb_edges() } )
    {
        attr_edge_from->set_value( e, e );
    }

    const auto solid2 = solid.clone();
    geode::OpenGeodeTetrahedralSolid3D solid4{ std::move(
        *dynamic_cast< geode::OpenGeodeTetrahedralSolid3D* >(
            solid2.get() ) ) };
    const auto attr_to = solid4.facets()
                             .facet_attribute_manager()
                             .find_attribute< geode::index_t >( "facet_id" );
    for( const auto f : geode::Range{ solid.facets().nb_facets() } )
    {
        OPENGEODE_EXCEPTION( attr_from->value( f ) == attr_to->value( f ),
            "[Test] Error in facet attribute transfer during cloning" );
        const auto from_vertices = solid.facets().facet_vertices( f );
        const auto to_vertices = solid4.facets().facet_vertices( f );
        for( const auto v : geode::Indices{ from_vertices } )
        {
            OPENGEODE_EXCEPTION( from_vertices[v] == to_vertices[v],
                "[Test] Error in facet vertices transfer during cloning" );
        }
    }
    const auto attr_edge_to =
        solid4.edges()
            .edge_attribute_manager()
            .find_attribute< geode::index_t >( "edge_id" );
    for( const auto e : geode::Range{ solid.edges().nb_edges() } )
    {
        OPENGEODE_EXCEPTION(
            attr_edge_from->value( e ) == attr_edge_to->value( e ),
            "[Test] Error in edge attribute transfer during cloning" );
    }
    OPENGEODE_EXCEPTION( solid4.nb_vertices() == 6,
        "[Test] TetrahedralSolid2 should have 6 vertices" );
    OPENGEODE_EXCEPTION(
        solid4.facets().nb_facets() == solid.facets().nb_facets(),
        "[Test] TetrahedralSolid2 should have same number of facets" );
    OPENGEODE_EXCEPTION( solid4.nb_polyhedra() == 2,
        "[Test] TetrahedralSolid2 should have 2 polyhedra" );
}

void test_delete_all( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.delete_isolated_vertices();
    OPENGEODE_EXCEPTION( solid.nb_vertices() == 5,
        "[Test] TetrahedralSolid should have 5 vertices" );
    OPENGEODE_EXCEPTION( solid.facets().nb_facets() == 7,
        "[Test] TetrahedralSolid should have 4 facets" );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 2,
        "[Test] TetrahedralSolid should have 1 polyhedron" );

    std::vector< bool > to_delete( solid.nb_polyhedra(), true );
    builder.delete_polyhedra( to_delete );
    OPENGEODE_EXCEPTION( solid.nb_vertices() == 5,
        "[Test] TetrahedralSolid should have 5 vertices" );
    OPENGEODE_EXCEPTION( solid.facets().nb_facets() == 7,
        "[Test] TetrahedralSolid should have 7 facet" );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 0,
        "[Test] TetrahedralSolid should have 0 polyhedron" );
    OPENGEODE_EXCEPTION( solid.polyhedra_around_vertex( 0 ).empty(),
        "[Test] No more polyhedron around vertices" );

    std::vector< bool > f_to_delete( solid.facets().nb_facets(), true );
    builder.facets_builder().delete_facets( f_to_delete );
    OPENGEODE_EXCEPTION( solid.facets().nb_facets() == 0,
        "[Test] TetrahedralSolid should have 0 facet" );

    builder.delete_isolated_vertices();
    OPENGEODE_EXCEPTION( solid.nb_vertices() == 0,
        "[Test] TetrahedralSolid should have 0 vertex" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
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

    test_permutation( *solid, *builder );
    test_delete_polyhedron( *solid, *builder );
    test_clone( *solid );
    test_delete_all( *solid, *builder );
}

OPENGEODE_TEST( "tetrahedral-solid" )