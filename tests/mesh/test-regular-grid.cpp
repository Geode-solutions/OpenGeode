/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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
#include <geode/geometry/vector.h>

#include <geode/mesh/builder/regular_grid_solid_builder.h>
#include <geode/mesh/builder/regular_grid_surface_builder.h>
#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/regular_grid_surface.h>
#include <geode/mesh/io/regular_grid_input.h>
#include <geode/mesh/io/regular_grid_output.h>

#include <geode/tests/common.h>

void test_cell_number( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION(
        grid.nb_polyhedra() == 750, "[Test] Wrong total number of cells" );
    OPENGEODE_EXCEPTION( grid.nb_cells_in_direction( 0 ) == 5,
        "[Test] Wrong total number of cells along X" );
    OPENGEODE_EXCEPTION( grid.nb_cells_in_direction( 1 ) == 10,
        "[Test] Wrong total number of cells along Y" );
    OPENGEODE_EXCEPTION( grid.nb_cells_in_direction( 2 ) == 15,
        "[Test] Wrong total number of cells along Z" );
}

void test_cell_size( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION( grid.cell_length_in_direction( 0 ) == 1,
        "[Test] Wrong length of cells along X" );
    OPENGEODE_EXCEPTION( grid.cell_length_in_direction( 1 ) == 2,
        "[Test] Wrong length of cells along Y" );
    OPENGEODE_EXCEPTION( grid.cell_length_in_direction( 2 ) == 3,
        "[Test] Wrong length of cells along Z" );
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

    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_indices( 0 ) ) == 0,
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_indices( 1 ) ) == 1,
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_indices( 5 ) ) == 5,
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_indices( 50 ) ) == 50,
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_indices( 56 ) ) == 56,
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.cell_index( grid.cell_indices( 112 ) ) == 112,
        "[Test] Wrong cell index" );

    OPENGEODE_EXCEPTION( grid.next_cell( { 0, 0, 0 }, 0 )
                             == geode::Grid3D::CellIndices( { 1, 0, 0 } ),
        "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION(
        !grid.next_cell( { 4, 0, 0 }, 0 ), "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION(
        !grid.previous_cell( { 0, 0, 0 }, 1 ), "[Test] Wrong cell index" );
    OPENGEODE_EXCEPTION( grid.previous_cell( { 0, 0, 1 }, 2 )
                             == geode::Grid3D::CellIndices( { 0, 0, 0 } ),
        "[Test] Wrong cell index" );
}

void test_cell_on_border( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION( grid.is_cell_on_border( { { 0, 0, 0 } } ),
        "[Test] Vertex is not on border where it should be." );
    OPENGEODE_EXCEPTION( grid.is_cell_on_border( { { 4, 8, 13 } } ),
        "[Test] Vertex is not on border where it should be." );
    OPENGEODE_EXCEPTION( !grid.is_cell_on_border( { { 1, 2, 3 } } ),
        "[Test] Vertex is on border where it should not be." );
}

void test_vertex_number( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION(
        grid.nb_vertices() == 1056, "[Test] Wrong total number of vertices" );
    OPENGEODE_EXCEPTION( grid.nb_vertices_in_direction( 0 ) == 6,
        "[Test] Wrong total number of vertices along X" );
    OPENGEODE_EXCEPTION( grid.nb_vertices_in_direction( 1 ) == 11,
        "[Test] Wrong total number of vertices along Y" );
    OPENGEODE_EXCEPTION( grid.nb_vertices_in_direction( 2 ) == 16,
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

    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_indices( 0 ) ) == 0,
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_indices( 1 ) ) == 1,
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_indices( 5 ) ) == 5,
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_indices( 50 ) ) == 50,
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_indices( 56 ) ) == 56,
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.vertex_index( grid.vertex_indices( 112 ) ) == 112,
        "[Test] Wrong vertex index" );

    OPENGEODE_EXCEPTION( grid.next_vertex( { 0, 0, 0 }, 0 )
                             == geode::Grid3D::VertexIndices( { 1, 0, 0 } ),
        "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION(
        !grid.next_vertex( { 5, 0, 0 }, 0 ), "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION(
        !grid.previous_vertex( { 0, 0, 0 }, 1 ), "[Test] Wrong vertex index" );
    OPENGEODE_EXCEPTION( grid.previous_vertex( { 0, 0, 1 }, 2 )
                             == geode::Grid3D::VertexIndices( { 0, 0, 0 } ),
        "[Test] Wrong vertex index" );

    OPENGEODE_EXCEPTION( grid.cell_vertices( { 1, 1, 1 } )[0]
                             == geode::Grid3D::VertexIndices( { 1, 1, 1 } ),
        "[Test] Wrong cell node vertex index" );
    OPENGEODE_EXCEPTION( grid.cell_vertex_indices( { 1, 1, 1 }, 1 )
                             == geode::Grid3D::VertexIndices( { 2, 1, 1 } ),
        "[Test] Wrong cell node vertex index" );
    OPENGEODE_EXCEPTION( grid.cell_vertex_indices( { 1, 1, 1 }, 2 )
                             == geode::Grid3D::VertexIndices( { 1, 2, 1 } ),
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
    const auto pt0 = grid.point( grid.vertex_index( { 0, 0, 0 } ) );
    OPENGEODE_EXCEPTION( pt0 == geode::Point3D( { 1.5, 0, 1 } ),
        "[Test] Wrong point coordinates: ", pt0.string(),
        " instead of [1.5, 0, 1]." );
    const auto pt1 = grid.point( grid.vertex_index( { 0, 0, 1 } ) );
    OPENGEODE_EXCEPTION( pt1 == geode::Point3D( { 1.5, -3, 1 } ),
        "[Test] Wrong point coordinates: ", pt1.string(),
        " instead of [1.5, -3, 1]." );
    const auto pt2 = grid.point( grid.vertex_index( { 1, 1, 1 } ) );
    OPENGEODE_EXCEPTION( pt2 == geode::Point3D( { -0.5, -3, 2 } ),
        "[Test] Wrong point coordinates: ", pt2.string(),
        " instead of [-0.5, -3, 2]." );
    const auto pt3 = grid.point( grid.vertex_index( { 2, 1, 4 } ) );
    OPENGEODE_EXCEPTION( pt3 == geode::Point3D( { -0.5, -12, 3 } ),
        "[Test] Wrong point coordinates: ", pt3.string(),
        " instead of [-0.5, -12, 3]." );
    const auto pt4 =
        grid.polyhedron_barycenter( grid.cell_index( { 2, 1, 4 } ) );
    OPENGEODE_EXCEPTION( pt4 == geode::Point3D( { -1.5, -13.5, 3.5 } ),
        "[Test] Wrong point coordinates: ", pt4.string(),
        " instead of [-1.5, -13.5, 3.5]." );
}

void test_cell_query( const geode::RegularGrid3D& grid )
{
    OPENGEODE_EXCEPTION( !grid.contains( geode::Point3D( { 0, 0, 0 } ) ),
        "[Test] Wrong result on contain: point [0,0,0] is shown inside of grid "
        "where "
        "it is not." );
    OPENGEODE_EXCEPTION( !grid.contains( geode::Point3D( { 1.5, 0, 0 } ) ),
        "[Test] Wrong result on contain: point [1.5,0,0] is shown inside of "
        "grid where "
        "it is not." );
    OPENGEODE_EXCEPTION( grid.cells( geode::Point3D( { 0, 0, 0 } ) ).empty(),
        "[Test] Wrong query result: point is shown inside of grid where it is "
        "not." );
    auto result = grid.cells( geode::Point3D( { -0.5, -1.5, 1.5 } ) );
    OPENGEODE_EXCEPTION(
        result.size() == 2
            && result.front() == geode::Grid3D::CellIndices( { 0, 0, 0 } )
            && result.back() == geode::Grid3D::CellIndices( { 0, 1, 0 } ),
        "[Test] Wrong query result" );
    result = grid.cells( geode::Point3D( { -5.5, -8, 4.5 } ) );
    OPENGEODE_EXCEPTION(
        result.size() == 1
            && result.front() == geode::Grid3D::CellIndices( { 3, 3, 2 } ),
        "[Test] Wrong query result" );
    result = grid.cells( geode::Point3D( { -4.5, -6 - 1e-10, 4 } ) );
    OPENGEODE_EXCEPTION(
        result.size() == 8
            && result[0] == geode::Grid3D::CellIndices( { 2, 2, 1 } )
            && result[1] == geode::Grid3D::CellIndices( { 3, 2, 1 } )
            && result[2] == geode::Grid3D::CellIndices( { 2, 3, 1 } )
            && result[3] == geode::Grid3D::CellIndices( { 3, 3, 1 } )
            && result[4] == geode::Grid3D::CellIndices( { 2, 2, 2 } )
            && result[5] == geode::Grid3D::CellIndices( { 3, 2, 2 } )
            && result[6] == geode::Grid3D::CellIndices( { 2, 3, 2 } )
            && result[7] == geode::Grid3D::CellIndices( { 3, 3, 2 } ),
        "[Test] Wrong query result" );
    geode::Point3D near_origin_point{ { 1.5 + geode::global_epsilon / 2,
        geode::global_epsilon / 2, 1 - geode::global_epsilon / 2 } };
    OPENGEODE_EXCEPTION( grid.contains( near_origin_point ),
        "[Test] Wrong result on contain: point is shown outside of grid when "
        "it should be inside." );
    result = grid.cells( near_origin_point );
    OPENGEODE_EXCEPTION(
        result.size() == 1
            && result.front() == geode::Grid3D::CellIndices( { 0, 0, 0 } ),
        "[Test] Wrong query result for point near origin." );
    geode::Point3D grid_furthest_point{ { -18.5 - geode::global_epsilon / 2,
        -45 - geode::global_epsilon / 2, 6 + geode::global_epsilon / 2 } };
    OPENGEODE_EXCEPTION( grid.contains( grid_furthest_point ),
        "[Test] Wrong result on contain: point is shown outside of grid when "
        "it should be inside." );
    result = grid.cells( grid_furthest_point );
    OPENGEODE_EXCEPTION(
        result.size() == 1
            && result.front() == geode::Grid3D::CellIndices( { 4, 9, 14 } ),
        "[Test] Wrong query result for point near origin furthest corner." );
}

void test_bounding_box( const geode::RegularGrid3D& grid )
{
    const auto bbox = grid.bounding_box();
    geode::Point3D min{ { -18.5, -45, 1 } };
    geode::Point3D max{ { 1.5, 0, 6 } };
    OPENGEODE_EXCEPTION( bbox.min() == min,
        "[Test] Wrong bounding box min: ", bbox.min().string(), " instead of ",
        min.string() );
    OPENGEODE_EXCEPTION( bbox.max() == max,
        "[Test] Wrong bounding box max: ", bbox.max().string(), " instead of ",
        max.string() );
}

void test_closest_vertex( const geode::RegularGrid3D& grid )
{
    const geode::Point3D p0{ { 1.5, 0, 1 } }; // (0,0,0)
    const geode::Point3D p1{ { -18.5, -45, 6 } }; // (5,10,15)
    const geode::Point3D p2{ { 2, 1, 0 } }; // (0,0,0)
    const geode::Point3D p3{ { -20, -50, 10 } }; // (5,10,15)
    const geode::Point3D p4{ { -2.55, -5.9, 3 } }; // (2,2,2)

    auto result = grid.closest_vertex( p0 );
    geode::Grid3D::VertexIndices answer{ 0, 0, 0 };
    OPENGEODE_ASSERT( result == answer,
        "[Test] Wrong result for closest vertex for query p0" );

    result = grid.closest_vertex( p1 );
    answer = { 5, 10, 15 };
    OPENGEODE_ASSERT( result == answer,
        "[Test] Wrong result for closest vertex for query p1" );

    result = grid.closest_vertex( p2 );
    answer = { 0, 0, 0 };
    OPENGEODE_ASSERT( result == answer,
        "[Test] Wrong result for closest vertex for query p2" );

    result = grid.closest_vertex( p3 );
    answer = { 5, 10, 15 };
    OPENGEODE_ASSERT( result == answer,
        "[Test] Wrong result for closest vertex for query p3" );

    result = grid.closest_vertex( p4 );
    answer = { 2, 2, 2 };
    OPENGEODE_ASSERT( result == answer,
        "[Test] Wrong result for closest vertex for query p4" );
}

void test_clone( const geode::RegularGrid3D& grid )
{
    const auto attribute_name = "int_attribute";
    const auto attribute_name_d = "double_attribute";
    auto attribute =
        grid.polyhedron_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, int >(
                attribute_name, 0 );
    auto attribute_d =
        grid.vertex_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, double >(
                attribute_name_d, 0 );
    for( const auto c : geode::Range{ grid.nb_polyhedra() } )
    {
        attribute->set_value( c, 2 * c );
    }
    for( const auto c : geode::Range{ grid.nb_vertices() } )
    {
        attribute_d->set_value( c, 2 * c );
    }
    const auto clone = grid.clone();
    OPENGEODE_EXCEPTION( clone->nb_vertices() == grid.nb_vertices(),
        "[Test] Clone wrong number of vertices" );
    OPENGEODE_EXCEPTION( clone->nb_vertices_in_direction( 0 )
                             == grid.nb_vertices_in_direction( 0 ),
        "[Test] Clone wrong number of vertices in direction 0" );
    OPENGEODE_EXCEPTION( clone->nb_vertices_in_direction( 1 )
                             == grid.nb_vertices_in_direction( 1 ),
        "[Test] Clone wrong number of vertices in direction 1" );
    OPENGEODE_EXCEPTION( clone->nb_vertices_in_direction( 2 )
                             == grid.nb_vertices_in_direction( 2 ),
        "[Test] Clone wrong number of vertices in direction 2" );

    OPENGEODE_EXCEPTION( clone->nb_cells() == grid.nb_cells(),
        "[Test] Clone wrong number of cells" );
    OPENGEODE_EXCEPTION(
        clone->nb_cells_in_direction( 0 ) == grid.nb_cells_in_direction( 0 ),
        "[Test] Clone wrong number of cells in direction 0" );
    OPENGEODE_EXCEPTION(
        clone->nb_cells_in_direction( 1 ) == grid.nb_cells_in_direction( 1 ),
        "[Test] Clone wrong number of cells in direction 1" );
    OPENGEODE_EXCEPTION(
        clone->nb_cells_in_direction( 2 ) == grid.nb_cells_in_direction( 2 ),
        "[Test] Clone wrong number of cells in direction 2" );

    OPENGEODE_EXCEPTION( clone->cell_length_in_direction( 0 )
                             == grid.cell_length_in_direction( 0 ),
        "[Test] Clone wrong cell length in direction 0" );
    OPENGEODE_EXCEPTION( clone->cell_length_in_direction( 1 )
                             == grid.cell_length_in_direction( 1 ),
        "[Test] Clone wrong cell length in direction 1" );
    OPENGEODE_EXCEPTION( clone->cell_length_in_direction( 2 )
                             == grid.cell_length_in_direction( 2 ),
        "[Test] Clone wrong cell length in direction 2" );

    OPENGEODE_EXCEPTION( clone->polyhedron_attribute_manager().attribute_exists(
                             attribute_name ),
        "[Test] Clone missing attribute" );
    OPENGEODE_EXCEPTION(
        clone->vertex_attribute_manager().attribute_exists( attribute_name_d ),
        "[Test] Clone missing attribute" );
    const auto clone_attribute =
        clone->polyhedron_attribute_manager().find_attribute< int >(
            attribute_name );
    for( const auto c : geode::TRange< int >{ clone->nb_polyhedra() } )
    {
        OPENGEODE_EXCEPTION( clone_attribute->value( c ) == 2 * c,
            "[Test] Wrong clone attribute" );
    }
    const auto clone_attribute_d =
        clone->vertex_attribute_manager().find_attribute< double >(
            attribute_name_d );
    for( const auto c : geode::TRange< int >{ clone->nb_vertices() } )
    {
        OPENGEODE_EXCEPTION( clone_attribute_d->value( c ) == 2 * c,
            "[Test] Wrong clone attribute" );
    }
}

void test_io( const geode::RegularGrid3D& grid, absl::string_view filename )
{
    geode::save_regular_grid( grid, filename );
    const auto reload = geode::load_regular_grid< 3 >( filename );
    OPENGEODE_EXCEPTION( grid.nb_cells() == reload->nb_cells(),
        "[Test] Wrong reload nb_cells()" );
    OPENGEODE_EXCEPTION(
        grid.nb_cells_in_direction( 0 ) == reload->nb_cells_in_direction( 0 ),
        "[Test] Wrong reload nb_cells_in_direction(0)" );
    OPENGEODE_EXCEPTION(
        grid.nb_cells_in_direction( 1 ) == reload->nb_cells_in_direction( 1 ),
        "[Test] Wrong reload nb_cells_in_direction(1)" );
    OPENGEODE_EXCEPTION(
        grid.nb_cells_in_direction( 2 ) == reload->nb_cells_in_direction( 2 ),
        "[Test] Wrong reload nb_cells_in_direction(2)" );
}

void test_adjacencies2D()
{
    auto grid = geode::RegularGrid2D::create();
    auto builder = geode::RegularGridBuilder2D::create( *grid );
    builder->initialize_grid( { { 0., 0. } }, { 10, 10 }, 1 );
    for( const auto p : geode::Range{ grid->nb_polygons() } )
    {
        for( const auto ee : geode::LRange{ grid->nb_polygon_edges( p ) } )
        {
            grid->polygon_adjacent_edge( { p, ee } );
        }
    }
}

void test_around_vertex( const geode::RegularGrid3D& grid )
{
    for( const auto v : geode::Range{ grid.nb_vertices() } )
    {
        const auto vertex = grid.vertex_indices( v );
        geode::index_t nb_polyhedra{ 8 };
        for( const auto d : geode::LRange{ 3 } )
        {
            if( vertex[d] == 0
                || vertex[d] == grid.nb_vertices_in_direction( d ) - 1 )
            {
                nb_polyhedra /= 2;
            }
        }
        const auto nb_polyhedra_around =
            grid.polyhedra_around_vertex( v ).size();
        OPENGEODE_EXCEPTION( nb_polyhedra_around == nb_polyhedra,
            "[Test] Wrong number of polyhedra around vertex ", v, "(",
            nb_polyhedra_around, "/", nb_polyhedra, ")" );
    }
    const auto around_0 = grid.vertices_around_vertex( 0 );
    OPENGEODE_EXCEPTION( around_0.size() == 3,
        "[Test] There should be 3 vertices around vertex 0, not ",
        around_0.size() );
    OPENGEODE_EXCEPTION( around_0.contains( 1 ) && around_0.contains( 6 )
                             && around_0.contains( 66 ),
        "[Test] Wrong value of vertices around vertex 0." );
    const auto around_73 = grid.vertices_around_vertex( 73 );
    OPENGEODE_EXCEPTION( around_73.size() == 6,
        "[Test] There should be 6 vertices around vertex 73, not ",
        around_73.size() );
    OPENGEODE_EXCEPTION(
        around_73.contains( 72 ) && around_73.contains( 74 )
            && around_73.contains( 67 ) && around_73.contains( 79 )
            && around_73.contains( 7 ) && around_73.contains( 139 ),
        "[Test] Wrong value of vertices around vertex 73." );
}

void test_grid( const geode::RegularGrid3D& grid )
{
    test_cell_number( grid );
    test_cell_index( grid );
    test_vertex_number( grid );
    test_vertex_index( grid );
    test_vertex_on_border( grid );
    test_around_vertex( grid );
    test_cell_geometry( grid );
    test_cell_query( grid );
    test_bounding_box( grid );
    test_closest_vertex( grid );
    test_clone( grid );
    test_io( grid, absl::StrCat( "test.", grid.native_extension() ) );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();

    auto grid = geode::RegularGrid3D::create();
    auto builder = geode::RegularGridBuilder3D::create( *grid );
    builder->initialize_grid( { { 1.5, 0, 1 } }, { 5, 10, 15 },
        { geode::Vector3D{ { 0, 0, 1 } }, geode::Vector3D{ { -2, 0, 0 } },
            geode::Vector3D{ { 0, -3, 0 } } } );
    test_grid( *grid );

    auto grid_v12 = geode::load_regular_grid< 3 >(
        absl::StrCat( geode::data_path, "test_v12.og_rgd3d" ) );
    auto builder_v12 = geode::RegularGridBuilder3D::create( *grid_v12 );
    builder_v12->update_origin_and_directions( { { 1.5, 0, 1 } },
        { geode::Vector3D{ { 0, 0, 1 } }, geode::Vector3D{ { -2, 0, 0 } },
            geode::Vector3D{ { 0, -3, 0 } } } );
    test_grid( *grid_v12 );

    test_adjacencies2D();
}

OPENGEODE_TEST( "regular-grid" )
