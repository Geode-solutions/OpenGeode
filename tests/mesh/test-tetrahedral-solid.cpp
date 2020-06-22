/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/geode_tetrahedral_solid_builder.h>
#include <geode/mesh/core/geode_tetrahedral_solid.h>
#include <geode/mesh/io/tetrahedral_solid_input.h>
#include <geode/mesh/io/tetrahedral_solid_output.h>

#include <geode/tests/common.h>

void test_create_vertices( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.create_point( { { 0.1, 0.2, 0.3 } } );
    builder.create_point( { { 2.1, 9.4, 6.7 } } );
    builder.create_point( { { 7.5, 5.2, 6.3 } } );
    builder.create_point( { { 8.1, 1.4, 4.7 } } );
    builder.create_point( { { 4.7, 2.1, 1.3 } } );
    builder.create_point( { { 1.6, 8.7, 6.1 } } );
    OPENGEODE_EXCEPTION( solid.nb_vertices() == 6,
        "[Test] TetrahedralSolid should have 6 vertices" );
}

void test_create_tetrahedra( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.reserve_tetrahedra( 3 );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 0,
        "[Test] TetrahedralSolid should have no tetrahedron" );
    builder.create_tetrahedra( 1 );
    builder.create_tetrahedron( { 1, 2, 3, 4 } );
    builder.create_polyhedron( { 1, 4, 3, 5 },
        { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 3,
        "[Test] TetrahedralSolid should have 3 tetrahedra" );
    builder.set_polyhedron_vertex( { 0, 0 }, 0 );
    builder.set_polyhedron_vertex( { 0, 1 }, 1 );
    builder.set_polyhedron_vertex( { 0, 2 }, 2 );
    builder.set_polyhedron_vertex( { 0, 3 }, 3 );
    builder.delete_isolated_edges();
    builder.delete_isolated_facets();
    OPENGEODE_EXCEPTION( solid.nb_facets() == 10,
        "[Test] TetrahedralSolid should have 10 facets" );
    OPENGEODE_EXCEPTION( solid.nb_edges() == 12,
        "[Test] TetrahedralSolid should have 12 edges" );
}

void test_polyhedron_adjacencies( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.compute_polyhedron_adjacencies();
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 0, 0 } ) == 1,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( !solid.polyhedron_adjacent( { 0, 1 } ),
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 1, 3 } ) == 0,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent_facet( { 0, 0 } )
                             == geode::PolyhedronFacet( 1, 3 ),
        "[Test] TetrahedralSolid adjacent index is not correct" );

    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 2, 3 } ) == 1,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( !solid.polyhedron_adjacent( { 2, 0 } ),
        "[Test] TetrahedralSolid adjacent index is not correct" );
}

void test_delete_vertex( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    std::vector< bool > to_delete( solid.nb_vertices(), false );
    to_delete.front() = true;
    builder.delete_vertices( to_delete );
    OPENGEODE_EXCEPTION( solid.nb_vertices() == 5,
        "[Test] TetrahedralSolid should have 5 vertices" );
    geode::Point3D answer{ { 2.1, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( solid.point( 0 ) == answer,
        "[Test] TetrahedralSolid vertex coordinates are not correct" );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 2,
        "[Test] TetrahedralSolid should have 2 tetrahedra" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 1, 3 } ) == 0,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    builder.delete_isolated_edges();
    builder.delete_isolated_facets();
    OPENGEODE_EXCEPTION( solid.nb_facets() == 7,
        "[Test] TetrahedralSolid should have 7 facets" );
    OPENGEODE_EXCEPTION(
        solid.nb_edges() == 9, "[Test] TetrahedralSolid should have 9 edges" );
}

void test_delete_polyhedron( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    std::vector< bool > to_delete( solid.nb_polyhedra(), false );
    to_delete.front() = true;
    builder.delete_polyhedra( to_delete );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 1,
        "[Test] TetrahedralSolid should have 1 polyhedron" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 0 } ) == 0,
        "[Test] TetrahedralSolid facet vertex index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 1 } ) == 3,
        "[Test] TetrahedralSolid facet vertex index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 2 } ) == 2,
        "[Test] TetrahedralSolid facet vertex index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 3 } ) == 4,
        "[Test] TetrahedralSolid facet vertex index is not correct" );
}

void test_io(
    const geode::TetrahedralSolid3D& solid, const std::string& filename )
{
    geode::save_tetrahedral_solid( solid, filename );
    geode::load_tetrahedral_solid< 3 >( filename );
    const auto new_solid = geode::load_tetrahedral_solid< 3 >(
        geode::OpenGeodeTetrahedralSolid3D::impl_name_static(), filename );
    OPENGEODE_EXCEPTION( new_solid->nb_vertices() == 6,
        "[Test] Reloaded TetrahedralSolid should have 6 vertices" );
    OPENGEODE_EXCEPTION( new_solid->nb_facets() == 10,
        "[Test] Reloaded TetrahedralSolid should have 10 facets" );
    OPENGEODE_EXCEPTION( new_solid->nb_edges() == 12,
        "[Test] Reloaded TetrahedralSolid should have 12 edges" );
    OPENGEODE_EXCEPTION( new_solid->nb_polyhedra() == 3,
        "[Test] Reloaded TetrahedralSolid should have 3 polyhedra" );
}

void test_backward_io( const std::string& filename )
{
    const auto new_solid = geode::load_tetrahedral_solid< 3 >(
        geode::OpenGeodeTetrahedralSolid3D::impl_name_static(), filename );
    OPENGEODE_EXCEPTION( new_solid->nb_vertices() == 6,
        "[Test] Reloaded TetrahedralSolid should have 6 vertices" );
    OPENGEODE_EXCEPTION( new_solid->nb_facets() == 10,
        "[Test] Reloaded TetrahedralSolid should have 10 facets" );
    OPENGEODE_EXCEPTION( new_solid->nb_edges() == 12,
        "[Test] Reloaded TetrahedralSolid should have 12 edges" );
    OPENGEODE_EXCEPTION( new_solid->nb_polyhedra() == 3,
        "[Test] Reloaded TetrahedralSolid should have 3 polyhedra" );
}

void test_clone( const geode::TetrahedralSolid3D& solid )
{
    auto attr_from = solid.facet_attribute_manager()
                         .find_or_create_attribute< geode::VariableAttribute,
                             geode::index_t >( "facet_id", 0 );
    for( const auto f : geode::Range{ solid.nb_facets() } )
    {
        attr_from->set_value( f, f );
    }
    auto attr_edge_from =
        solid.edge_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute,
                geode::index_t >( "edge_id", 0 );
    for( const auto e : geode::Range{ solid.nb_edges() } )
    {
        attr_edge_from->set_value( e, e );
    }
    const auto solid2 = solid.clone();
    const auto attr_to =
        solid2->facet_attribute_manager().find_attribute< geode::index_t >(
            "facet_id" );
    for( const auto f : geode::Range{ solid.nb_facets() } )
    {
        OPENGEODE_EXCEPTION( attr_from->value( f ) == attr_to->value( f ),
            "[Test] Error in facet attribute transfer during cloning" );
        const auto from_vertices = solid.facet_vertices( f );
        const auto to_vertices = solid2->facet_vertices( f );
        for( const auto v : geode::Range{ from_vertices.size() } )
        {
            OPENGEODE_EXCEPTION( from_vertices[v] == to_vertices[v],
                "[Test] Error in facet vertices transfer during cloning" );
        }
    }
    const auto attr_edge_to =
        solid2->edge_attribute_manager().find_attribute< geode::index_t >(
            "edge_id" );
    for( const auto e : geode::Range{ solid.nb_edges() } )
    {
        OPENGEODE_EXCEPTION(
            attr_edge_from->value( e ) == attr_edge_to->value( e ),
            "[Test] Error in edge attribute transfer during cloning" );
    }
    OPENGEODE_EXCEPTION( solid2->nb_vertices() == 5,
        "[Test] TetrahedralSolid2 should have 5 vertices" );
    OPENGEODE_EXCEPTION( solid2->nb_facets() == 4,
        "[Test] TetrahedralSolid2 should have 4 facets" );
    OPENGEODE_EXCEPTION( solid2->nb_polyhedra() == 1,
        "[Test] TetrahedralSolid2 should have 1 polyhedron" );
}

void test_delete_all( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.delete_isolated_vertices();
    OPENGEODE_EXCEPTION( solid.nb_vertices() == 5,
        "[Test]TetrahedralSolid should have 5 vertices" );
    OPENGEODE_EXCEPTION(
        solid.nb_facets() == 4, "[Test]TetrahedralSolid should have 4 facets" );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 1,
        "[Test]TetrahedralSolid should have 1 polyhedron" );

    std::vector< bool > to_delete( solid.nb_polyhedra(), true );
    builder.delete_polyhedra( to_delete );
    OPENGEODE_EXCEPTION( solid.nb_vertices() == 5,
        "[Test]TetrahedralSolid should have 5 vertices" );
    OPENGEODE_EXCEPTION(
        solid.nb_facets() == 0, "[Test]TetrahedralSolid should have 0 facet" );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 0,
        "[Test]TetrahedralSolid should have 0 polyhedron" );
    OPENGEODE_EXCEPTION( solid.polyhedra_around_vertex( 0 ).empty(),
        "[Test] No more polyhedron around vertices" );

    builder.delete_isolated_vertices();
    OPENGEODE_EXCEPTION( solid.nb_vertices() == 0,
        "[Test]TetrahedralSolid should have 0 vertex" );
}

void test()
{
    auto solid = geode::TetrahedralSolid3D::create(
        geode::OpenGeodeTetrahedralSolid3D::impl_name_static() );
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );

    test_create_vertices( *solid, *builder );
    test_create_tetrahedra( *solid, *builder );
    test_polyhedron_adjacencies( *solid, *builder );
    test_io( *solid, absl::StrCat( "test.", solid->native_extension() ) );
    test_backward_io( absl::StrCat(
        geode::data_path, "/test_v1.", solid->native_extension() ) );

    test_delete_vertex( *solid, *builder );
    test_delete_polyhedron( *solid, *builder );
    test_clone( *solid );
}

OPENGEODE_TEST( "tetrahedral-solid" )