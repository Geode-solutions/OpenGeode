/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/polyhedral_solid_view_builder.h>
#include <geode/mesh/core/polyhedral_solid_view.h>

#include <geode/tests/common.h>

void test_create_vertices( const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    builder.create_point( { { 0.1, 0.2, 0.3 } } );
    builder.create_point( { { 2.1, 9.4, 6.7 } } );
    builder.create_point( { { 7.5, 5.2, 6.3 } } );
    builder.create_point( { { 8.1, 1.4, 4.7 } } );
    builder.create_point( { { 4.7, 2.1, 1.3 } } );
    builder.create_point( { { 9.3, 5.3, 6.7 } } );
    builder.create_point( { { 7.5, 4.2, 2.8 } } );
    builder.create_point( { { 2.2, 3.3, 4.4 } } );
    OPENGEODE_EXCEPTION( polyhedral_solid.isolated_vertex( 0 ),
        "[Test] Vertices should be isolated before polyhedra creation" );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_vertices() == 8,
        "[Test] PolyhedralSolid should have 8 vertices" );
}

void test_create_viewed_vertices(
    const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidViewBuilder3D& builder )
{
    builder.add_viewed_vertex( 0 );
    builder.add_viewed_vertex( 1 );
    builder.add_viewed_vertex( 2 );
    builder.add_viewed_vertex( 3 );
    builder.add_viewed_vertex( 4 );
    builder.add_viewed_vertex( 5 );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_vertices() == 6,
        "[Test] PolyhedralSolidView should have 6 vertices" );
}

void test_create_viewed_polyhedra(
    const geode::PolyhedralSolidView3D& polyhedral_solid,
    geode::PolyhedralSolidViewBuilder3D& builder )
{
    builder.add_viewed_polyhedron( 0 );
    builder.add_viewed_polyhedron( 2 );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_polyhedra() == 2,
        "[Test] PolyhedralSolidView should have 2 polyhedra" );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_vertices() == 7,
        "[Test] PolyhedralSolidView should have 7 vertices" );
    OPENGEODE_EXCEPTION( polyhedral_solid.viewed_polyhedron( 1 ) == 2,
        "[Test] PolyhedralSolidView polyhedron is not correct" );

    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_vertex( { 0, 2 } ) == 2,
        "[Test] PolyhedralSolidView PolyhedronVertex is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_vertex( { 1, 3 } ) == 6,
        "[Test] PolyhedralSolidView PolyhedronVertex is not correct" );
}

void test_create_polyhedra( const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    builder.create_polyhedron(
        { 0, 1, 2, 3, 4, 5 }, { { 0, 1, 2 }, { 3, 5, 4 }, { 0, 3, 4, 1 },
                                  { 0, 2, 5, 3 }, { 1, 4, 5, 2 } } );
    builder.create_polyhedron( { 4, 5, 6, 7 },
        { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
    builder.create_polyhedron( { 3, 4, 5, 6 },
        { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_polyhedra() == 3,
        "[Test] PolyhedralSolid should have 3 polyhedra" );
    OPENGEODE_EXCEPTION( !polyhedral_solid.isolated_vertex( 0 ),
        "[Test] Vertices should not be isolated after polyhedra creation" );
}

void test_polyhedron_adjacencies(
    const geode::PolyhedralSolid3D& polyhedral_solid )
{
    OPENGEODE_EXCEPTION( !polyhedral_solid.polyhedron_adjacent( { 0, 0 } ),
        "[Test] PolyhedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_adjacent( { 0, 1 } ) == 1,
        "[Test] PolyhedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( !polyhedral_solid.polyhedron_adjacent( { 0, 2 } ),
        "[Test] PolyhedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_adjacent( { 1, 3 } ) == 0,
        "[Test] PolyhedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION(
        polyhedral_solid.polyhedra_around_vertex( 4 ).size() == 2,
        "[Test] PolyhedralSolid should have 2 polyhedra around this vertex" );
    OPENGEODE_EXCEPTION(
        polyhedral_solid.polyhedron_facets_on_border( 0 ).size() == 4,
        "[Test] First polyhedron of PolyhedralSolid should have 4 facets on "
        "border" );
}

std::unique_ptr< geode::PolyhedralSolid3D > create_solid()
{
    auto solid = geode::PolyhedralSolid3D::create();
    auto builder = geode::PolyhedralSolidBuilder3D::create( *solid );

    test_create_vertices( *solid, *builder );
    test_create_polyhedra( *solid, *builder );
    builder->compute_polyhedron_adjacencies();
    return solid;
}

void test()
{
    auto solid = create_solid();
    geode::PolyhedralSolidView3D view{ *solid };
    auto builder = geode::PolyhedralSolidViewBuilder3D::create( view );

    test_create_viewed_vertices( view, *builder );
    test_create_viewed_polyhedra( view, *builder );
    test_polyhedron_adjacencies( view );
}

OPENGEODE_TEST( "polyhedral-solid-view" )