/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

void test_cell_size( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION(
        grid.cell_length( 0 ) == 1, "[Test] Wrong length of cells along X" );
    OPENGEODE_EXCEPTION(
        grid.cell_length( 1 ) == 2, "[Test] Wrong length of cells along Y" );
    OPENGEODE_EXCEPTION(
        grid.cell_length( 2 ) == 3, "[Test] Wrong length of cells along Z" );
    OPENGEODE_EXCEPTION( grid.cell_size() == 6, "[Test] Wrong cell size" );
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
                             == geode::GridCellIndex3D( { 1, 0, 0 } ),
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION(
        !grid.next_cell( { 4, 0, 0 }, 0 ), "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION(
        !grid.previous_cell( { 0, 0, 0 }, 1 ), "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.previous_cell( { 0, 0, 1 }, 2 )
                             == geode::GridCellIndex3D( { 0, 0, 0 } ),
        "[Test] Wrong cell index" );
}

void test_vertex_number( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION(
        grid.nb_vertices() == 1056, "[Test] Wrong total number of vertices" );
    OPENGEODE_EXCEPTION( grid.nb_vertices( 0 ) == 6,
        "[Test] Wrong total number of vertices along X" );
    OPENGEODE_EXCEPTION( grid.nb_vertices( 1 ) == 11,
        "[Test] Wrong total number of vertices along Y" );
    OPENGEODE_EXCEPTION( grid.nb_vertices( 2 ) == 16,
        "[Test] Wrong total number of vertices along Z" );
    OPENGEODE_EXCEPTION( grid.nb_cell_vertices() == 8,
        "[Test] Wrong number of vertices in cells" );
    OPENGEODE_EXCEPTION( grid.nb_vertices_on_borders() == 552,
        "[Test] Wrong number of vertices on borders" );
}

void test_vertex_index( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION(
        grid.vertex_index( { 0, 0, 0 } ) == 0, "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION(
        grid.vertex_index( { 1, 0, 0 } ) == 1, "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION(
        grid.vertex_index( { 0, 1, 0 } ) == 6, "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION(
        grid.vertex_index( { 0, 0, 1 } ) == 66, "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION(
        grid.vertex_index( { 1, 1, 1 } ) == 73, "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION(
        grid.vertex_index( { 2, 2, 2 } ) == 146, "[Test] Wrong vertex index" );

    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_index( 0 ) ) == 0,
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_index( 1 ) ) == 1,
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_index( 5 ) ) == 5,
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_index( 50 ) ) == 50,
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_index( 56 ) ) == 56,
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_index( 112 ) ) == 112,
        "[Test] Wrong vertex index" );

    OPENGEODE_EXCEPTION( grid.next_vertex( { 0, 0, 0 }, 0 )
                             == geode::GridVertexIndex3D( { 1, 0, 0 } ),
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION(
        !grid.next_vertex( { 5, 0, 0 }, 0 ), "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION(
        !grid.previous_vertex( { 0, 0, 0 }, 1 ), "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.previous_vertex( { 0, 0, 1 }, 2 )
                             == geode::GridVertexIndex3D( { 0, 0, 0 } ),
        "[Test] Wrong vertex index" );

    OPENGEODE_EXCEPTION( grid.cell_vertices( { 1, 1, 1 } )[0]
                             == geode::GridVertexIndex3D( { 1, 1, 1 } ),
        "[Test] Wrong cell node vertex index" );
    OPENGEODE_EXCEPTION( grid.cell_vertex_indices( { 1, 1, 1 }, 1 )
                             == geode::GridVertexIndex3D( { 2, 1, 1 } ),
        "[Test] Wrong cell node vertex index" );
    OPENGEODE_EXCEPTION( grid.cell_vertex_indices( { 1, 1, 1 }, 2 )
                             == geode::GridVertexIndex3D( { 1, 2, 1 } ),
        "[Test] Wrong cell node vertex index" );
    OPENGEODE_EXCEPTION( grid.cell_vertex_index( { 1, 1, 1 }, 7 ) == 146,
        "[Test] Wrong cell node vertex index" );
}

void test_vertex_on_border( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION(
        grid.is_vertex_on_border( grid.vertex_index( { { 0, 0, 0 } } ) ),
        "[Test] Vertex is not on border where it should be." );
    OPENGEODE_EXCEPTION(
        grid.is_vertex_on_border( grid.vertex_index( { { 0, 9, 0 } } ) ),
        "[Test] Vertex is not on border where it should be." );
    OPENGEODE_EXCEPTION(
        !grid.is_vertex_on_border( grid.vertex_index( { { 1, 2, 3 } } ) ),
        "[Test] Vertex is on border where it should not be." );
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
    OPENGEODE_EXCEPTION(
        grid.cell_barycenter( { 2, 1, 4 } ) == geode::Point3D( { 4, 3, 14.5 } ),
        "[Test] Wrong point coordinates" );
}

void test_cell_query( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION( !grid.cell( geode::Point3D( { 0, 0, 0 } ) ),
        "[Test] Wrong query result" );
    auto result = grid.cell( geode::Point3D( { 2, 2, 2 } ) ).value();
    OPENGEODE_EXCEPTION(
        result.size() == 2
            && result.front() == geode::GridCellIndex3D( { 0, 0, 0 } )
            && result.back() == geode::GridCellIndex3D( { 0, 1, 0 } ),
        "[Test] Wrong query result" );
    result = grid.cell( geode::Point3D( { 5, 7, 9 } ) ).value();
    OPENGEODE_EXCEPTION(
        result.size() == 1
            && result.front() == geode::GridCellIndex3D( { 3, 3, 2 } ),
        "[Test] Wrong query result" );
    result = grid.cell( geode::Point3D( { 4.5, 6, 7 - 1e-10 } ) ).value();
    OPENGEODE_EXCEPTION(
        result.size() == 8 && result[0] == geode::GridCellIndex3D( { 2, 2, 1 } )
            && result[1] == geode::GridCellIndex3D( { 3, 2, 1 } )
            && result[2] == geode::GridCellIndex3D( { 2, 3, 1 } )
            && result[3] == geode::GridCellIndex3D( { 3, 3, 1 } )
            && result[4] == geode::GridCellIndex3D( { 2, 2, 2 } )
            && result[5] == geode::GridCellIndex3D( { 3, 2, 2 } )
            && result[6] == geode::GridCellIndex3D( { 2, 3, 2 } )
            && result[7] == geode::GridCellIndex3D( { 3, 3, 2 } ),
        "[Test] Wrong query result" );
}

void test_boundary_box( const geode::RegularGrid3D& grid )
{
    const auto bbox = grid.bounding_box();
    geode::Point3D min{ { 1.5, 0, 1 } };
    geode::Point3D max{ { 6.5, 20, 46 } };
    OPENGEODE_EXCEPTION( bbox.min() == min, "[Test] Wrong bounding box min" );
    OPENGEODE_EXCEPTION( bbox.max() == max, "[Test] Wrong bounding box max" );
}

void test_clone( geode::RegularGrid3D& grid )
{
    const auto attribute_name = "int_attribute";
    const auto attribute_name_d = "double_attribute";
    auto attribute =
        grid.cell_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, int >(
                attribute_name, 0 );
    auto attribute_d =
        grid.vertex_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, double >(
                attribute_name_d, 0 );
    for( const auto c : geode::Range{ grid.nb_cells() } )
    {
        attribute->set_value( c, 2 * c );
    }
    for( const auto c : geode::Range{ grid.nb_vertices() } )
    {
        attribute_d->set_value( c, 2 * c );
    }
    const auto clone = grid.clone();
    OPENGEODE_EXCEPTION(
        clone.origin() == grid.origin(), "[Test] Wrong clone origin" );
    OPENGEODE_EXCEPTION(
        clone.cell_attribute_manager().attribute_exists( attribute_name ),
        "[Test] Clone missing attribute" );
    OPENGEODE_EXCEPTION(
        clone.vertex_attribute_manager().attribute_exists( attribute_name_d ),
        "[Test] Clone missing attribute" );
    const auto clone_attribute =
        clone.cell_attribute_manager().find_attribute< int >( attribute_name );
    for( const auto c : geode::TRange< int >{ clone.nb_cells() } )
    {
        OPENGEODE_EXCEPTION( clone_attribute->value( c ) == 2 * c,
            "[Test] Wrong clone attribute" );
    }
    const auto clone_attribute_d =
        clone.vertex_attribute_manager().find_attribute< double >(
            attribute_name_d );
    for( const auto c : geode::TRange< int >{ clone.nb_vertices() } )
    {
        OPENGEODE_EXCEPTION( clone_attribute_d->value( c ) == 2 * c,
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
    test_vertex_number( grid );
    test_vertex_index( grid );
    test_vertex_on_border( grid );
    test_cell_geometry( grid );
    test_cell_query( grid );
    test_boundary_box( grid );
    test_clone( grid );
    test_io( grid, absl::StrCat( "test.", grid.native_extension() ) );
}

OPENGEODE_TEST( "regular-grid" )
