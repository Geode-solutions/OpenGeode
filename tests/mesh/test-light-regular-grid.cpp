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

#include <cmath>
#include <filesystem>
#include <fstream>
#include <limits>

#include <absl/algorithm/container.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/coordinate_system.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/grid_builder.hpp>
#include <geode/mesh/builder/grid_coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/core/grid_coordinate_reference_system.hpp>
#include <geode/mesh/core/grid_coordinate_reference_system_manager.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/io/light_regular_grid_input.hpp>
#include <geode/mesh/io/light_regular_grid_output.hpp>

#include <geode/tests/common.hpp>

void test_cell_number( const geode::LightRegularGrid3D& grid )
{
    geode::OpenGeodeMeshException::test(
        grid.nb_cells() == 750, "Wrong total number of cells" );
    geode::OpenGeodeMeshException::test( grid.nb_cells_in_direction( 0 ) == 5,
        "Wrong total number of cells along X" );
    geode::OpenGeodeMeshException::test( grid.nb_cells_in_direction( 1 ) == 10,
        "Wrong total number of cells along Y" );
    geode::OpenGeodeMeshException::test( grid.nb_cells_in_direction( 2 ) == 15,
        "Wrong total number of cells along Z" );
}

void test_cell_size( const geode::LightRegularGrid3D& grid )
{
    geode::OpenGeodeMeshException::test(
        grid.cell_length_in_direction( 0 ) == 1,
        "Wrong length of cells along X" );
    geode::OpenGeodeMeshException::test(
        grid.cell_length_in_direction( 1 ) == 2,
        "Wrong length of cells along Y" );
    geode::OpenGeodeMeshException::test(
        grid.cell_length_in_direction( 2 ) == 3,
        "Wrong length of cells along Z" );
    geode::OpenGeodeMeshException::test(
        grid.cell_size() == 6, "Wrong cell size" );
}

void test_cell_index( const geode::LightRegularGrid3D& grid )
{
    geode::OpenGeodeMeshException::test(
        grid.cell_index( { 0, 0, 0 } ) == 0, "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_index( { 1, 0, 0 } ) == 1, "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_index( { 0, 1, 0 } ) == 5, "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_index( { 0, 0, 1 } ) == 50, "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_index( { 1, 1, 1 } ) == 56, "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_index( { 2, 2, 2 } ) == 112, "Wrong cell index" );

    geode::OpenGeodeMeshException::test(
        grid.cell_index( grid.cell_indices( 0 ) ) == 0, "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_index( grid.cell_indices( 1 ) ) == 1, "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_index( grid.cell_indices( 5 ) ) == 5, "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_index( grid.cell_indices( 50 ) ) == 50, "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_index( grid.cell_indices( 56 ) ) == 56, "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_index( grid.cell_indices( 112 ) ) == 112,
        "Wrong cell index" );

    geode::OpenGeodeMeshException::test(
        grid.next_cell( { 0, 0, 0 }, 0 )
            == geode::Grid3D::CellIndices( { 1, 0, 0 } ),
        "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        !grid.next_cell( { 4, 0, 0 }, 0 ), "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        !grid.previous_cell( { 0, 0, 0 }, 1 ), "Wrong cell index" );
    geode::OpenGeodeMeshException::test(
        grid.previous_cell( { 0, 0, 1 }, 2 )
            == geode::Grid3D::CellIndices( { 0, 0, 0 } ),
        "Wrong cell index" );
}

void test_cell_on_border( const geode::LightRegularGrid3D& grid )
{
    geode::OpenGeodeMeshException::test( grid.is_cell_on_border( { 0, 0, 0 } ),
        "Vertex is not on border where it should be." );
    geode::OpenGeodeMeshException::test( grid.is_cell_on_border( { 4, 8, 13 } ),
        "Vertex is not on border where it should be." );
    geode::OpenGeodeMeshException::test( !grid.is_cell_on_border( { 1, 2, 3 } ),
        "Vertex is on border where it should not be." );
}

void test_vertex_number( const geode::LightRegularGrid3D& grid )
{
    geode::OpenGeodeMeshException::test(
        grid.nb_grid_vertices() == 1056, "Wrong total number of vertices" );
    geode::OpenGeodeMeshException::test(
        grid.nb_vertices_in_direction( 0 ) == 6,
        "Wrong total number of vertices along X" );
    geode::OpenGeodeMeshException::test(
        grid.nb_vertices_in_direction( 1 ) == 11,
        "Wrong total number of vertices along Y" );
    geode::OpenGeodeMeshException::test(
        grid.nb_vertices_in_direction( 2 ) == 16,
        "Wrong total number of vertices along Z" );
    geode::OpenGeodeMeshException::test(
        grid.nb_cell_vertices() == 8, "Wrong number of vertices in cells" );
    geode::OpenGeodeMeshException::test( grid.nb_vertices_on_borders() == 552,
        "Wrong number of vertices on borders" );
}

void test_vertex_index( const geode::LightRegularGrid3D& grid )
{
    geode::OpenGeodeMeshException::test(
        grid.vertex_index( { 0, 0, 0 } ) == 0, "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.vertex_index( { 1, 0, 0 } ) == 1, "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.vertex_index( { 0, 1, 0 } ) == 6, "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.vertex_index( { 0, 0, 1 } ) == 66, "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.vertex_index( { 1, 1, 1 } ) == 73, "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.vertex_index( { 2, 2, 2 } ) == 146, "Wrong vertex index" );

    geode::OpenGeodeMeshException::test(
        grid.vertex_index( grid.vertex_indices( 0 ) ) == 0,
        "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.vertex_index( grid.vertex_indices( 1 ) ) == 1,
        "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.vertex_index( grid.vertex_indices( 5 ) ) == 5,
        "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.vertex_index( grid.vertex_indices( 50 ) ) == 50,
        "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.vertex_index( grid.vertex_indices( 56 ) ) == 56,
        "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.vertex_index( grid.vertex_indices( 112 ) ) == 112,
        "Wrong vertex index" );

    geode::OpenGeodeMeshException::test(
        grid.next_vertex( { 0, 0, 0 }, 0 )
            == geode::Grid3D::VertexIndices( { 1, 0, 0 } ),
        "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        !grid.next_vertex( { 5, 0, 0 }, 0 ), "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        !grid.previous_vertex( { 0, 0, 0 }, 1 ), "Wrong vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.previous_vertex( { 0, 0, 1 }, 2 )
            == geode::Grid3D::VertexIndices( { 0, 0, 0 } ),
        "Wrong vertex index" );

    geode::OpenGeodeMeshException::test(
        grid.cell_vertices( { 1, 1, 1 } )[0]
            == geode::Grid3D::VertexIndices( { 1, 1, 1 } ),
        "Wrong cell node vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_vertex_indices( { 1, 1, 1 }, 1 )
            == geode::Grid3D::VertexIndices( { 2, 1, 1 } ),
        "Wrong cell node vertex index" );
    geode::OpenGeodeMeshException::test(
        grid.cell_vertex_indices( { 1, 1, 1 }, 2 )
            == geode::Grid3D::VertexIndices( { 1, 2, 1 } ),
        "Wrong cell node vertex index" );
}

void test_vertex_on_border( const geode::LightRegularGrid3D& grid )
{
    geode::OpenGeodeMeshException::test(
        grid.is_grid_vertex_on_border( { 0, 0, 0 } ),
        "Vertex is not on border where it should be." );
    geode::OpenGeodeMeshException::test(
        grid.is_grid_vertex_on_border( { 0, 9, 0 } ),
        "Vertex is not on border where it should be." );
    geode::OpenGeodeMeshException::test(
        !grid.is_grid_vertex_on_border( { 1, 2, 3 } ),
        "Vertex is on border where it should not be." );
}

void test_cell_geometry( const geode::LightRegularGrid3D& grid )
{
    geode::OpenGeodeMeshException::test(
        grid.grid_point( { 0, 0, 0 } ) == geode::Point3D( { 1.5, 0, 1 } ),
        "Wrong point coordinates" );
    geode::OpenGeodeMeshException::test(
        grid.grid_point( { 0, 0, 1 } ) == geode::Point3D( { 1.5, 0, 4 } ),
        "Wrong point coordinates" );
    geode::OpenGeodeMeshException::test(
        grid.grid_point( { 1, 1, 1 } ) == geode::Point3D( { 2.5, 2, 4 } ),
        "Wrong point coordinates" );
    geode::OpenGeodeMeshException::test(
        grid.grid_point( { 2, 1, 4 } ) == geode::Point3D( { 3.5, 2, 13 } ),
        "Wrong point coordinates" );
}

void test_cell_query( const geode::LightRegularGrid3D& grid )
{
    geode::OpenGeodeMeshException::test(
        !grid.contains( geode::Point3D( { 0, 0, 0 } ) ),
        "Wrong result on contain: point is shown inside of grid where "
        "it is not." );
    geode::OpenGeodeMeshException::test(
        !grid.contains( geode::Point3D( { 1.5, 0, 0 } ) ),
        "Wrong result on contain: point is shown inside of grid where "
        "it is not." );
    geode::OpenGeodeMeshException::test(
        grid.cells( geode::Point3D( { 0, 0, 0 } ) ).empty(),
        "Wrong query result: point is shown inside of grid where it is "
        "not." );
    auto result = grid.cells( geode::Point3D( { 2, 2, 2 } ) );
    geode::OpenGeodeMeshException::test(
        result.size() == 2
            && result.front() == geode::Grid3D::CellIndices( { 0, 0, 0 } )
            && result.back() == geode::Grid3D::CellIndices( { 0, 1, 0 } ),
        "Wrong query result" );
    result = grid.cells( geode::Point3D( { 5, 7, 9 } ) );
    geode::OpenGeodeMeshException::test(
        result.size() == 1
            && result.front() == geode::Grid3D::CellIndices( { 3, 3, 2 } ),
        "Wrong query result" );
    result = grid.cells( geode::Point3D( { 4.5, 6, 7 - 1e-10 } ) );
    geode::OpenGeodeMeshException::test(
        result.size() == 8
            && result[0] == geode::Grid3D::CellIndices( { 2, 2, 1 } )
            && result[1] == geode::Grid3D::CellIndices( { 3, 2, 1 } )
            && result[2] == geode::Grid3D::CellIndices( { 2, 3, 1 } )
            && result[3] == geode::Grid3D::CellIndices( { 3, 3, 1 } )
            && result[4] == geode::Grid3D::CellIndices( { 2, 2, 2 } )
            && result[5] == geode::Grid3D::CellIndices( { 3, 2, 2 } )
            && result[6] == geode::Grid3D::CellIndices( { 2, 3, 2 } )
            && result[7] == geode::Grid3D::CellIndices( { 3, 3, 2 } ),
        "Wrong query result" );
    geode::Point3D near_origin_point{ { 1.5 - geode::GLOBAL_EPSILON / 2,
        -geode::GLOBAL_EPSILON / 2, 1 - geode::GLOBAL_EPSILON / 2 } };
    geode::OpenGeodeMeshException::test( grid.contains( near_origin_point ),
        "Wrong result on contain: point is shown outside of grid when "
        "it should be inside." );
    result = grid.cells( near_origin_point );
    geode::OpenGeodeMeshException::test(
        result.size() == 1
            && result.front() == geode::Grid3D::CellIndices( { 0, 0, 0 } ),
        "Wrong query result for point near origin." );
    geode::Point3D grid_furthest_point{ { 6.5 + geode::GLOBAL_EPSILON / 2,
        20 + geode::GLOBAL_EPSILON / 2, 46 + geode::GLOBAL_EPSILON / 2 } };
    geode::OpenGeodeMeshException::test( grid.contains( grid_furthest_point ),
        "Wrong result on contain: point is shown outside of grid when "
        "it should be inside." );
    result = grid.cells( grid_furthest_point );
    geode::OpenGeodeMeshException::test(
        result.size() == 1
            && result.front() == geode::Grid3D::CellIndices( { 4, 9, 14 } ),
        "Wrong query result for point near origin furthest corner." );
}

void test_boundary_box( const geode::LightRegularGrid3D& grid )
{
    const auto bbox = grid.grid_bounding_box();
    geode::Point3D min{ { 1.5, 0, 1 } };
    geode::Point3D max{ { 6.5, 20, 46 } };
    geode::OpenGeodeMeshException::test(
        bbox.min() == min, "Wrong bounding box min" );
    geode::OpenGeodeMeshException::test(
        bbox.max() == max, "Wrong bounding box max" );
}

void test_closest_vertex( const geode::LightRegularGrid3D& grid )
{
    const geode::Point3D p0{ { 1.5, 0, 1 } }; // (0,0,0)
    const geode::Point3D p1{ { 6.5, 20, 46 } }; // (5,10,15)
    const geode::Point3D p2{ { 0, -1, -1 } }; // (0,0,0)
    const geode::Point3D p3{ { 10, 30, 50 } }; // (5,10,15)
    const geode::Point3D p4{ { 3.55, 3.9, 7.5 } }; // (2,2,2)

    auto result = grid.closest_vertex( p0 );
    geode::Grid3D::VertexIndices answer{ 0, 0, 0 };
    geode::OpenGeodeMeshException::test(
        result == answer, "Wrong result for closest vertex for query p0" );

    result = grid.closest_vertex( p1 );
    answer = { 5, 10, 15 };
    geode::OpenGeodeMeshException::test(
        result == answer, "Wrong result for closest vertex for query p1" );

    result = grid.closest_vertex( p2 );
    answer = { 0, 0, 0 };
    geode::OpenGeodeMeshException::test(
        result == answer, "Wrong result for closest vertex for query p2" );

    result = grid.closest_vertex( p3 );
    answer = { 5, 10, 15 };
    geode::OpenGeodeMeshException::test(
        result == answer, "Wrong result for closest vertex for query p3" );

    result = grid.closest_vertex( p4 );
    answer = { 2, 2, 2 };
    geode::OpenGeodeMeshException::test(
        result == answer, "Wrong result for closest vertex for query p4" );
}

void test_attribute( const geode::LightRegularGrid3D& grid )
{
    geode::AttributeProperties attribute_properties;
    attribute_properties.assignable = false;
    attribute_properties.interpolable = true;
    attribute_properties.transferable = true;
    geode::AttributeValues< double > attribute_values;
    attribute_values.default_value = -1;
    attribute_values.no_value = -1;
    auto attribute_id =
        grid.cell_attribute_manager()
            .create_attribute< geode::VariableAttribute, double >(
                "test", attribute_values, attribute_properties );
    auto attribute =
        grid.cell_attribute_manager()
            .find_attribute< geode::VariableAttribute, double >( attribute_id );
    attribute->set_value( 10, 10 );
    geode::OpenGeodeMeshException::test(
        attribute->value( 0 ) == -1, "Wrong attribute value" );
    geode::OpenGeodeMeshException::test(
        attribute->value( 10 ) == 10, "Wrong attribute value" );
    geode::OpenGeodeMeshException::test(
        attribute->value( grid.nb_cells() - 1 ) == -1,
        "Wrong attribute value" );
}

void test_vertices_around_vertex( const geode::LightRegularGrid3D& grid )
{
    const auto vertex = grid.vertex_index( { 1, 1, 1 } );
    const auto around = grid.vertices_around_vertex( vertex );
    geode::OpenGeodeMeshException::test( around.size() == 6,
        "Wrong number of vertices around an inner vertex: ", around.size() );
    for( const auto& neighbor : std::array< geode::Grid3D::VertexIndices, 6 >{
             { { 0, 1, 1 }, { 2, 1, 1 }, { 1, 0, 1 }, { 1, 2, 1 }, { 1, 1, 0 },
                 { 1, 1, 2 } } } )
    {
        geode::OpenGeodeMeshException::test(
            absl::c_linear_search( around, grid.vertex_index( neighbor ) ),
            "Missing vertex around an inner vertex" );
    }
    const auto corner_around = grid.vertices_around_vertex( 0 );
    geode::OpenGeodeMeshException::test( corner_around.size() == 3,
        "Wrong number of vertices around a corner vertex" );
}

void test_set_grid_dimensions_failure()
{
    geode::LightRegularGrid3D grid{ geode::Point3D{ { 1.5, 0, 1 } },
        { 5, 10, 15 }, { 1, 2, 3 } };
    geode::GridBuilder3D builder{ grid };
    const auto check_unchanged = [&grid]( std::string_view context ) {
        geode::OpenGeodeMeshException::test(
            grid.nb_cells_in_direction( 0 ) == 5
                && grid.nb_cells_in_direction( 1 ) == 10
                && grid.nb_cells_in_direction( 2 ) == 15,
            "[Test] Cells number should be unchanged after a failed ",
            context );
        geode::OpenGeodeMeshException::test(
            grid.cell_length_in_direction( 0 ) == 1
                && grid.cell_length_in_direction( 1 ) == 2
                && grid.cell_length_in_direction( 2 ) == 3,
            "[Test] Cells length should be unchanged after a failed ",
            context );
        geode::OpenGeodeMeshException::test(
            grid.grid_point( { 1, 1, 1 } ) == geode::Point3D{ { 2.5, 2, 4 } },
            "[Test] Grid geometry should be unchanged after a failed ",
            context );
    };
    bool thrown{ false };
    try
    {
        builder.set_grid_dimensions( { 65535, 65535, 1 }, { 1, 1, 1 } );
    }
    catch( const geode::OpenGeodeException& )
    {
        thrown = true;
    }
    geode::OpenGeodeMeshException::test(
        thrown, "[Test] Too many vertices should throw" );
    check_unchanged( "vertex overflow" );
    thrown = false;
    try
    {
        builder.set_grid_dimensions( { 6, 7, 8 }, { 1, 0, 3 } );
    }
    catch( const geode::OpenGeodeException& )
    {
        thrown = true;
    }
    geode::OpenGeodeMeshException::test(
        thrown, "[Test] Null cell length should throw" );
    check_unchanged( "null length" );
}

void test_nan_query( const geode::LightRegularGrid3D& grid )
{
    const auto nan = std::numeric_limits< double >::quiet_NaN();
    const geode::Point3D query{ { nan, 2, 2 } };
    geode::OpenGeodeMeshException::test(
        !grid.contains( query ), "[Test] NaN point should not be contained" );
    geode::OpenGeodeMeshException::test(
        grid.cells( query ).empty(), "[Test] NaN point should be in no cell" );
    const auto closest = grid.closest_vertex( query );
    for( const auto d : geode::LRange{ 3 } )
    {
        geode::OpenGeodeMeshException::test(
            closest[d] < grid.nb_vertices_in_direction( d ),
            "[Test] Closest vertex of NaN point should be a valid vertex" );
    }
}

void test_clone_exact_geometry()
{
    const geode::LightRegularGrid3D grid{
        geode::Point3D{ { 1e6 + 0.1, -3e5 + 0.7, 12345.678 } }, { 3, 4, 5 },
        { geode::Vector3D{ { 0.1, 0.2, 0.3 } },
            geode::Vector3D{ { -0.7, 0.1 / 3, 0.3 } },
            geode::Vector3D{ { 1.1, 0.9, -0.3 / 7 } } }
    };
    const auto clone = grid.clone();
    for( const auto d : geode::LRange{ 3 } )
    {
        geode::OpenGeodeMeshException::test(
            clone.grid_coordinate_system().direction( d )
                == grid.grid_coordinate_system().direction( d ),
            "[Test] Cloned grid direction ", d, " should be identical: ",
            clone.grid_coordinate_system().direction( d ).string(), " vs ",
            grid.grid_coordinate_system().direction( d ).string() );
        geode::OpenGeodeMeshException::test(
            clone.cell_length_in_direction( d )
                == grid.cell_length_in_direction( d ),
            "[Test] Cloned grid cell length should be identical" );
    }
    for( const auto v : geode::Range{ grid.nb_grid_vertices() } )
    {
        geode::OpenGeodeMeshException::test(
            clone.point( v ) == grid.point( v ),
            "[Test] Cloned grid point should be identical" );
    }
}

void test_io(
    const geode::LightRegularGrid3D& grid, const std::string& filename )
{
    geode::save_light_regular_grid( grid, filename );
    const auto new_grid = geode::load_light_regular_grid< 3 >( filename );

    geode::OpenGeodeMeshException::test( new_grid.nb_grid_vertices() == 1056,
        "Reloaded LightRegularGrid should have 1056 vertices" );
    test_cell_number( new_grid );
    test_vertex_number( new_grid );
    test_cell_geometry( new_grid );
    test_boundary_box( new_grid );
}

void test_save_unwritable( const geode::LightRegularGrid3D& grid )
{
    const auto filename =
        absl::StrCat( "unwritable.", grid.native_extension() );
    std::filesystem::create_directories( filename );
    try
    {
        geode::save_light_regular_grid( grid, filename );
    }
    catch( const geode::OpenGeodeException& )
    {
        std::filesystem::remove_all( filename );
        return;
    }
    std::filesystem::remove_all( filename );
    throw geode::OpenGeodeMeshException{ nullptr,
        geode::OpenGeodeException::TYPE::internal,
        "[Test] Saving in a non writable file should throw" };
}

template < geode::index_t dimension >
bool same_coordinate_system( const geode::CoordinateSystem< dimension >& lhs,
    const geode::CoordinateSystem< dimension >& rhs )
{
    if( lhs.origin() != rhs.origin() )
    {
        return false;
    }
    for( const auto d : geode::LRange{ dimension } )
    {
        if( lhs.direction( d ) != rhs.direction( d ) )
        {
            return false;
        }
    }
    return true;
}

template < geode::index_t dimension >
void check_active_crs_is_grid_geometry(
    const geode::LightRegularGrid< dimension >& grid, std::string_view name )
{
    const auto& crs_manager = grid.grid_coordinate_reference_system_manager();
    geode::OpenGeodeMeshException::test(
        crs_manager.active_coordinate_reference_system_name() == name,
        "[Test] Wrong active grid CRS name: ",
        crs_manager.active_coordinate_reference_system_name() );
    geode::OpenGeodeMeshException::test(
        same_coordinate_system( crs_manager.active_coordinate_reference_system()
                                    .coordinate_system(),
            grid.grid_coordinate_system() ),
        "[Test] Active grid CRS should match the grid geometry" );
}

void test_coordinate_reference_systems()
{
    geode::LightRegularGrid3D grid{ geode::Point3D{ { 1.5, 0, 1 } },
        { 5, 10, 15 }, { 1, 2, 3 } };
    const auto& crs_manager = grid.grid_coordinate_reference_system_manager();
    geode::OpenGeodeMeshException::test(
        crs_manager.nb_coordinate_reference_systems() == 1,
        "[Test] A new grid should have one CRS" );
    check_active_crs_is_grid_geometry( grid, "points" );
    const auto initial_coordinate_system = grid.grid_coordinate_system();

    const geode::CoordinateSystem3D shifted{
        { geode::Vector3D{ { 0, 2, 0 } }, geode::Vector3D{ { -1, 0, 0 } },
            geode::Vector3D{ { 0, 0, 0.5 } } },
        geode::Point3D{ { 100, 200, 300 } }
    };
    geode::GridCoordinateReferenceSystemManagerBuilder3D crs_builder{ grid };
    crs_builder.register_coordinate_reference_system( "shifted",
        std::make_shared< geode::GridCoordinateReferenceSystem3D >( shifted ) );
    geode::OpenGeodeMeshException::test(
        crs_manager.nb_coordinate_reference_systems() == 2,
        "[Test] Grid should have two CRSs" );
    geode::OpenGeodeMeshException::test(
        same_coordinate_system(
            grid.grid_coordinate_system(), initial_coordinate_system ),
        "[Test] Registering a CRS should not move the grid" );

    crs_builder.set_active_coordinate_reference_system( "shifted" );
    check_active_crs_is_grid_geometry( grid, "shifted" );
    geode::OpenGeodeMeshException::test(
        same_coordinate_system( grid.grid_coordinate_system(), shifted ),
        "[Test] Grid geometry should be the active CRS one" );
    geode::OpenGeodeMeshException::test(
        grid.point( grid.vertex_index( { 1, 1, 1 } ) )
            == geode::Point3D{ { 99, 202, 300.5 } },
        "[Test] Wrong grid point in shifted CRS" );
    geode::OpenGeodeMeshException::test(
        grid.cell_length_in_direction( 0 ) == 2
            && grid.cell_length_in_direction( 1 ) == 1
            && grid.cell_length_in_direction( 2 ) == 0.5,
        "[Test] Wrong cell lengths in shifted CRS" );

    geode::GridBuilder3D{ grid }.set_grid_origin(
        geode::Point3D{ { 0, 0, 0 } } );
    check_active_crs_is_grid_geometry( grid, "shifted" );

    crs_builder.set_active_coordinate_reference_system( "points" );
    check_active_crs_is_grid_geometry( grid, "points" );
    geode::OpenGeodeMeshException::test(
        same_coordinate_system(
            grid.grid_coordinate_system(), initial_coordinate_system ),
        "[Test] Grid geometry should be restored with the first CRS" );
    geode::OpenGeodeMeshException::test(
        crs_manager.find_coordinate_reference_system( "shifted" )
                .coordinate_system()
                .origin()
            == geode::Point3D{ { 0, 0, 0 } },
        "[Test] Inactive CRS should keep the last active geometry" );

    auto clone = grid.clone();
    geode::GridCoordinateReferenceSystemManagerBuilder3D{ clone }
        .set_active_coordinate_reference_system( "shifted" );
    check_active_crs_is_grid_geometry( clone, "shifted" );
    check_active_crs_is_grid_geometry( grid, "points" );
    geode::GridBuilder3D{ clone }.set_grid_origin(
        geode::Point3D{ { 1, 1, 1 } } );
    geode::OpenGeodeMeshException::test(
        crs_manager.find_coordinate_reference_system( "shifted" )
                .coordinate_system()
                .origin()
            == geode::Point3D{ { 0, 0, 0 } },
        "[Test] Cloned grid CRSs should be independent" );

    const auto filename = absl::StrCat( "crs.", grid.native_extension() );
    geode::save_light_regular_grid( clone, filename );
    auto reloaded = geode::load_light_regular_grid< 3 >( filename );
    geode::OpenGeodeMeshException::test(
        reloaded.grid_coordinate_reference_system_manager()
                .nb_coordinate_reference_systems()
            == 2,
        "[Test] Reloaded grid should have two CRSs" );
    check_active_crs_is_grid_geometry( reloaded, "shifted" );
    geode::GridCoordinateReferenceSystemManagerBuilder3D{ reloaded }
        .set_active_coordinate_reference_system( "points" );
    geode::OpenGeodeMeshException::test(
        same_coordinate_system(
            reloaded.grid_coordinate_system(), initial_coordinate_system ),
        "[Test] Reloaded grid should keep the inactive CRS geometry" );

    crs_builder.delete_coordinate_reference_system( "points" );
    geode::OpenGeodeMeshException::test(
        crs_manager.active_coordinate_reference_system_name().empty(),
        "[Test] Deleting the active CRS should leave no active CRS" );
    geode::OpenGeodeMeshException::test(
        same_coordinate_system(
            grid.grid_coordinate_system(), initial_coordinate_system ),
        "[Test] Deleting the active CRS should not move the grid" );
}

void test_backward_io()
{
    const auto grid3d = geode::load_light_regular_grid< 3 >(
        absl::StrCat( geode::DATA_PATH, "backward_io/v19_8/v19_8.og_lrgd3d" ) );
    test_cell_number( grid3d );
    test_cell_geometry( grid3d );
    check_active_crs_is_grid_geometry( grid3d, "points" );
    geode::OpenGeodeMeshException::test(
        grid3d.grid_coordinate_reference_system_manager()
                .nb_coordinate_reference_systems()
            == 1,
        "[Test] Legacy grid should have one CRS" );
    const auto attribute_ids =
        grid3d.cell_attribute_manager().attribute_ids_matching_name(
            "cell_value" );
    geode::OpenGeodeMeshException::test(
        attribute_ids && attribute_ids->size() == 1,
        "[Test] Legacy grid should keep its cell attribute" );
    geode::OpenGeodeMeshException::test(
        grid3d.cell_attribute_manager()
                .find_attribute< geode::VariableAttribute, double >(
                    attribute_ids->front() )
                ->value( 10 )
            == 42,
        "[Test] Legacy grid should keep its cell attribute values" );

    const auto grid2d = geode::load_light_regular_grid< 2 >(
        absl::StrCat( geode::DATA_PATH, "backward_io/v19_8/v19_8.og_lrgd2d" ) );
    check_active_crs_is_grid_geometry( grid2d, "points" );
    geode::OpenGeodeMeshException::test(
        grid2d.point( grid2d.vertex_index( { 1, 1 } ) )
            == geode::Point2D{ { 0.5, 4 } },
        "[Test] Wrong legacy 2D grid point" );

    const auto filename =
        absl::StrCat( "backward.", grid2d.native_extension() );
    geode::save_light_regular_grid( grid2d, filename );
    const auto reloaded = geode::load_light_regular_grid< 2 >( filename );
    check_active_crs_is_grid_geometry( reloaded, "points" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();

    const geode::LightRegularGrid3D grid{ geode::Point3D{ { 1.5, 0, 1 } },
        { 5, 10, 15 }, { 1, 2, 3 } };
    test_cell_number( grid );
    test_cell_index( grid );
    test_vertex_number( grid );
    test_vertex_index( grid );
    test_vertex_on_border( grid );
    test_cell_geometry( grid );
    test_cell_query( grid );
    test_boundary_box( grid );
    test_closest_vertex( grid );
    test_attribute( grid );
    test_vertices_around_vertex( grid );
    test_set_grid_dimensions_failure();
    test_nan_query( grid );
    test_clone_exact_geometry();
    test_io( grid, absl::StrCat( "test.", grid.native_extension() ) );
    test_save_unwritable( grid );
    test_coordinate_reference_systems();
    test_backward_io();
}

OPENGEODE_TEST( "light-regular-grid" )
