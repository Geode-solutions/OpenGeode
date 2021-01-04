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

#include <fstream>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/regular_grid.h>
#include <geode/mesh/io/regular_grid_input.h>
#include <geode/mesh/io/regular_grid_output.h>

#include <geode/tests/common.h>

void test_cell_number( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION(
        grid.nb_cells() == 750, "[Test] Wrong total number of cells" );
    OPENGEODE_EXCEPTION(
        grid.nb_cells( 0 ) == 5, "[Test] Wrong total number of cells along X" );
    OPENGEODE_EXCEPTION( grid.nb_cells( 1 ) == 10,
        "[Test] Wrong total number of cells along Y" );
    OPENGEODE_EXCEPTION( grid.nb_cells( 2 ) == 15,
        "[Test] Wrong total number of cells along Z" );
}

void test_cell_index( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION(
        grid.cell_index( { 0, 0, 0 } ) == 0, "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION(
        grid.cell_index( { 1, 0, 0 } ) == 1, "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION(
        grid.cell_index( { 0, 1, 0 } ) == 5, "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION(
        grid.cell_index( { 0, 0, 1 } ) == 50, "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION(
        grid.cell_index( { 1, 1, 1 } ) == 56, "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION(
        grid.cell_index( { 2, 2, 2 } ) == 112, "[Test] Wrong cell index" );

    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_index( 0 ) ) == 0,
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_index( 1 ) ) == 1,
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_index( 5 ) ) == 5,
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_index( 50 ) ) == 50,
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_index( 56 ) ) == 56,
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_index( 112 ) ) == 112,
        "[Test] Wrong cell index" );

    OPENGEODE_EXCEPTION( grid.next_cell( { 0, 0, 0 }, 0 )
                             == geode::RegularGrid3D::Index( { 1, 0, 0 } ),
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION(
        !grid.next_cell( { 4, 0, 0 }, 0 ), "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION(
        !grid.previous_cell( { 0, 0, 0 }, 1 ), "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.previous_cell( { 0, 0, 1 }, 2 )
                             == geode::RegularGrid3D::Index( { 0, 0, 0 } ),
        "[Test] Wrong cell index" );
}

void test_cell_geometry( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION(
        grid.point( { 0, 0, 0 } ) == geode::Point3D( { 1.5, 0, 1 } ),
        "[Test] Wrong point coordinates" );
    OPENGEODE_EXCEPTION(
        grid.point( { 0, 0, 1 } ) == geode::Point3D( { 1.5, 0, 4 } ),
        "[Test] Wrong point coordinates" );
    OPENGEODE_EXCEPTION(
        grid.point( { 1, 1, 1 } ) == geode::Point3D( { 2.5, 2, 4 } ),
        "[Test] Wrong point coordinates" );
    OPENGEODE_EXCEPTION(
        grid.point( { 2, 1, 4 } ) == geode::Point3D( { 3.5, 2, 13 } ),
        "[Test] Wrong point coordinates" );
}

void test_cell_query( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION( !grid.cell( geode::Point3D( { 0, 0, 0 } ) ),
        "[Test] Wrong query result" );
    auto result = grid.cell( geode::Point3D( { 2, 2, 2 } ) ).value();
    OPENGEODE_EXCEPTION(
        result.size() == 2
            && result.front() == geode::RegularGrid3D::Index( { 0, 0, 0 } )
            && result.back() == geode::RegularGrid3D::Index( { 0, 1, 0 } ),
        "[Test] Wrong query result" );
    result = grid.cell( geode::Point3D( { 5, 7, 9 } ) ).value();
    OPENGEODE_EXCEPTION(
        result.size() == 1
            && result.front() == geode::RegularGrid3D::Index( { 3, 3, 2 } ),
        "[Test] Wrong query result" );
    result = grid.cell( geode::Point3D( { 4.5, 6, 7 - 1e-10 } ) ).value();
    OPENGEODE_EXCEPTION(
        result.size() == 8
            && result[0] == geode::RegularGrid3D::Index( { 2, 2, 1 } )
            && result[1] == geode::RegularGrid3D::Index( { 3, 2, 1 } )
            && result[2] == geode::RegularGrid3D::Index( { 2, 3, 1 } )
            && result[3] == geode::RegularGrid3D::Index( { 3, 3, 1 } )
            && result[4] == geode::RegularGrid3D::Index( { 2, 2, 2 } )
            && result[5] == geode::RegularGrid3D::Index( { 3, 2, 2 } )
            && result[6] == geode::RegularGrid3D::Index( { 2, 3, 2 } )
            && result[7] == geode::RegularGrid3D::Index( { 3, 3, 2 } ),
        "[Test] Wrong query result" );
}

void test_boundary_box( const geode::RegularGrid3D& grid )
{
    const auto bbox = grid.bounding_box();
    geode::Point3D min{ { 1.5, 0, 1 } };
    geode::Point3D max{ { 7.5, 22, 49 } };
    OPENGEODE_EXCEPTION( bbox.min() == min, "[Test] Wrong bounding box min" );
    OPENGEODE_EXCEPTION( bbox.max() == max, "[Test] Wrong bounding box max" );
}

void test_clone( geode::RegularGrid3D& grid )
{
    const auto attribute_name = "int_attribute";
    auto attribute =
        grid.cell_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, int >(
                attribute_name, 0 );
    for( const auto c : geode::Range{ grid.nb_cells() } )
    {
        attribute->set_value( c, 2 * c );
    }
    const auto clone = grid.clone();
    OPENGEODE_EXCEPTION(
        clone.origin() == grid.origin(), "[Test] Wrong clone origin" );
    OPENGEODE_EXCEPTION(
        clone.nb_cells() == grid.nb_cells(), "[Test] Wrong clone nb_cells" );
    OPENGEODE_EXCEPTION(
        clone.cell_attribute_manager().attribute_exists( attribute_name ),
        "[Test] Clone missing attribute" );
    const auto clone_attribute =
        clone.cell_attribute_manager().find_attribute< int >( attribute_name );
    for( const auto c : geode::TRange< int >{ clone.nb_cells() } )
    {
        OPENGEODE_EXCEPTION( clone_attribute->value( c ) == 2 * c,
            "[Test] Wrong clone attribute" );
    }
}

void test_io( const geode::RegularGrid3D& grid, absl::string_view filename )
{
    geode::save_regular_grid( grid, filename );
    geode::load_regular_grid< 3 >( filename );
}

void test()
{
    geode::RegularGrid3D grid{ { { 1.5, 0, 1 } }, { 5, 10, 15 }, { 1, 2, 3 } };
    test_cell_number( grid );
    test_cell_index( grid );
    test_cell_geometry( grid );
    test_cell_query( grid );
    test_boundary_box( grid );
    test_clone( grid );
    test_io( grid, absl::StrCat( "test.", grid.native_extension() ) );
}

OPENGEODE_TEST( "regular-grid" )
