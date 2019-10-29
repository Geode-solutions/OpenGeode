/*
 * Copyright (c) 2019 Geode-solutions
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
#include <geode/basic/point.h>

#include <geode/mesh/builder/geode_tetrahedral_solid_builder.h>
#include <geode/mesh/core/geode_tetrahedral_solid.h>
#include <geode/mesh/io/tetrahedral_solid_input.h>
#include <geode/mesh/io/tetrahedral_solid_output.h>

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
    builder.create_tetrahedron( { 0, 1, 2, 3 } );
    builder.create_tetrahedron( { 1, 2, 3, 4 } );
    builder.create_polyhedron( { 1, 4, 3, 5 },
        { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } );
    OPENGEODE_EXCEPTION( solid.nb_polyhedra() == 3,
        "[Test] TetrahedralSolid should have 3 tetrahedra" );
    OPENGEODE_EXCEPTION( solid.nb_facets() == 10,
        "[Test] PolyhedralSolid should have 10 facets" );
}

void test_polyhedron_adjacencies( const geode::TetrahedralSolid3D& solid,
    geode::TetrahedralSolidBuilder3D& builder )
{
    builder.compute_polyhedron_adjacencies();
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 0, 0 } ) == 1,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 0, 1 } ) == geode::NO_ID,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 1, 3 } ) == 0,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent_facet( { 0, 0 } )
                             == geode::PolyhedronFacet( 1, 3 ),
        "[Test] TetrahedralSolid adjacent index is not correct" );

    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 2, 3 } ) == 1,
        "[Test] TetrahedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 2, 0 } ) == geode::NO_ID,
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
        "[Test] TetrahedralSolid should have 2 ttrahedra" );
    OPENGEODE_EXCEPTION( solid.polyhedron_adjacent( { 1, 3 } ) == 0,
        "[Test] TetrahedralSolid adjacent index is not correct" );
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
    save_tetrahedral_solid( solid, filename );
    auto new_solid = geode::TetrahedralSolid3D::create(
        geode::OpenGeodeTetrahedralSolid3D::type_name_static() );
    load_tetrahedral_solid( *new_solid, filename );
}

void test_clone( const geode::TetrahedralSolid3D& solid )
{
    auto solid2 = solid.clone();
    OPENGEODE_EXCEPTION( solid2->nb_vertices() == 5,
        "[Test] TetrahedralSolid2 should have 5 vertices" );
    OPENGEODE_EXCEPTION( solid2->nb_facets() == 4,
        "[Test] TetrahedralSolid2 should have 4 facets" );
    OPENGEODE_EXCEPTION( solid2->nb_polyhedra() == 1,
        "[Test] TetrahedralSolid2 should have 1 polyhedron" );
}

int main()
{
    using namespace geode;

    try
    {
        auto solid = TetrahedralSolid3D::create(
            OpenGeodeTetrahedralSolid3D::type_name_static() );
        auto builder = TetrahedralSolidBuilder3D::create( *solid );

        test_create_vertices( *solid, *builder );
        test_create_tetrahedra( *solid, *builder );
        test_polyhedron_adjacencies( *solid, *builder );
        auto base_file = "test." + solid->native_extension();
        test_io( *solid, base_file );

        test_delete_vertex( *solid, *builder );
        test_delete_polyhedron( *solid, *builder );
        test_clone( *solid );

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
