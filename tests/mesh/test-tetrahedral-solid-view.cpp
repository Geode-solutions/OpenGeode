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

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/tetrahedral_solid_view_builder.h>
#include <geode/mesh/core/tetrahedral_solid_view.h>

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
}

void test_create_viewed_vertices( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidViewBuilder3D& builder )
{
    builder.add_viewed_vertex( 2 );
    builder.add_viewed_vertex( 3 );
    OPENGEODE_EXCEPTION( solid.nb_vertices() == 2,
        "[Test] TetrahedralSolidView should have 2 vertices" );
    const geode::Point3D answer{ { 7.5, 5.2, 6.3 } };
    OPENGEODE_EXCEPTION( solid.point( 0 ) == answer,
        "[Test] TetrahedralSolidView point is not correct" );
}

void test_create_viewed_tetrahedra( const geode::TetrahedralSolidView3D& solid,
    geode::TetrahedralSolidViewBuilder3D& builder )
{
    builder.add_viewed_tetrahedron( 1 );
    builder.add_viewed_tetrahedron( 2 );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 2,
        "[Test] TetrahedralSolidView should have 2 polyhedra" );
    OPENGEODE_EXCEPTION( solid.nb_vertices() == 5,
        "[Test] TetrahedralSolidView should have 5 vertices" );
    OPENGEODE_EXCEPTION( solid.viewed_tetrahedron( 0 ) == 1,
        "[Test] TetrahedralSolidView tetrahedron is not correct" );
    OPENGEODE_EXCEPTION( solid.viewed_vertex( 2 ) == 1,
        "[Test] TetrahedralSolidView vertex is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_vertex( { 0, 0 } ) == 2,
        "[Test] TetrahedralSolidView polyhedron vertex is not correct" );
}

void test_isolated( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidViewBuilder3D& builder )
{
    OPENGEODE_EXCEPTION( !solid.isolated_vertex( 0 ),
        "[Test] TetrahedralSolidView isolated vertex is not correct" );
    builder.add_viewed_vertex( 0 );
    OPENGEODE_EXCEPTION( solid.isolated_vertex( 5 ),
        "[Test] TetrahedralSolidView isolated vertex is not correct" );
}

void test_polyhedron_adjacencies( const geode::TetrahedralSolid3D& solid )
{
    OPENGEODE_EXCEPTION( !solid.polyhedron_adjacent( { 0, 0 } ),
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 0, 1 } ) == 1,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 1, 3 } ) == 0,
        "[Test] TetrahedralSolid adjacent index is not correct" );
}

std::unique_ptr< geode::TetrahedralSolid3D > create_solid()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );

    test_create_vertices( *solid, *builder );
    test_create_tetrahedra( *solid, *builder );
    builder->compute_polyhedron_adjacencies();
    return solid;
}
void test()
{
    absl::flat_hash_map< geode::index_t, geode::index_t > one_map;
    auto solid = create_solid();
    geode::TetrahedralSolidView3D view{ *solid };
    auto builder = geode::TetrahedralSolidViewBuilder3D::create( view );

    test_create_viewed_vertices( view, *builder );
    test_create_viewed_tetrahedra( view, *builder );
    test_isolated( view, *builder );
    test_polyhedron_adjacencies( view );
}

OPENGEODE_TEST( "tetrahedral-solid-view" )