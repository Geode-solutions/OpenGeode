/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/tests/common.h>

#include <absl/container/flat_hash_set.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/regular_grid.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/rasterize.h>
#include <geode/mesh/io/regular_grid_input.h>
#include <geode/mesh/io/triangulated_surface_input.h>

void test_rasterize_segment(
    const geode::RegularGrid3D& grid, const geode::Segment3D& segment )
{
    const auto cells = geode::rasterize_segment( grid, segment );
    const absl::flat_hash_set< geode::index_t > answer{ 111, 112, 121, 122, 131,
        132, 211, 212, 221, 222, 231, 232, 242, 243, 252, 253, 342, 343, 352,
        353, 363, 373 };

    OPENGEODE_EXCEPTION(
        answer.size() == cells.size(), "[Test] Wrong number of result cells" );
    for( const auto cell : cells )
    {
        OPENGEODE_EXCEPTION(
            answer.find( grid.cell_index( cell ) ) != answer.end(),
            "[Test] Wrong result cells" );
    }
}

void test_rasterize_triangle(
    const geode::RegularGrid3D& grid, const geode::Triangle3D& triangle )
{
    const auto cells = geode::rasterize_triangle( grid, triangle );
    const absl::flat_hash_set< geode::index_t > answer{ 111, 112, 121, 122, 211,
        212, 213, 221, 222, 223, 232, 233, 242, 243, 252, 253, 313, 314, 323,
        324, 333, 334, 343, 344, 352, 353, 354, 363, 364, 373, 414, 415, 424,
        425, 434, 435, 444, 445, 454, 464, 515, 516, 525, 526, 535 };
    OPENGEODE_EXCEPTION(
        answer.size() == cells.size(), "[Test] Wrong number of result cells" );
    for( const auto cell : cells )
    {
        OPENGEODE_EXCEPTION(
            answer.find( grid.cell_index( cell ) ) != answer.end(),
            "[Test] Wrong result cells" );
    }
}

void test_rasterize_degenerate_triangle(
    const geode::RegularGrid3D& grid, const geode::Triangle3D& triangle )
{
    const auto cells = geode::rasterize_triangle( grid, triangle );
    const absl::flat_hash_set< geode::index_t > answer{ 111, 112, 121, 122, 211,
        212, 221, 222, 223, 232, 233, 322, 323, 332, 333, 334, 343, 344, 433,
        434, 443, 444, 445, 454, 455, 544, 545, 554, 555, 556, 565, 566, 655,
        656, 665, 666 };
    OPENGEODE_EXCEPTION(
        answer.size() == cells.size(), "[Test] Wrong number of result cells" );
    for( const auto cell : cells )
    {
        OPENGEODE_EXCEPTION(
            answer.find( grid.cell_index( cell ) ) != answer.end(),
            "[Test] Wrong result cells" );
    }
}

void test_conservative_rasterize_segment()
{
    geode::RegularGrid2D grid{ { { 0., 0. } }, { 10, 10 }, 1 };
    geode::Point2D pt0{ { 2.5, 2.5 } };
    geode::Point2D pt1{ { 6.5, 6.5 } };
    geode::Segment2D segment{ pt0, pt1 };

    const auto cells = geode::rasterize_segment( grid, segment );
    const absl::flat_hash_set< geode::index_t > answer{ 22, 33, 44, 55, 66 };
    OPENGEODE_EXCEPTION( answer.size() == cells.size(),
        "[Test] Wrong number of result cells (rasterize_segment)" );
    for( const auto cell : cells )
    {
        OPENGEODE_EXCEPTION(
            answer.find( grid.cell_index( cell ) ) != answer.end(),
            "[Test] Wrong result cells (rasterize_segment)" );
    }
    const auto cells2 = geode::conservative_rasterize_segment( grid, segment );
    const absl::flat_hash_set< geode::index_t > answer2{ 22, 23, 32, 33, 34, 43,
        44, 45, 54, 55, 56, 65, 66 };
    OPENGEODE_EXCEPTION( answer2.size() == cells2.size(),
        "[Test] Wrong number of result cells "
        "(conservative_rasterize_segment)" );
    for( const auto cell : cells2 )
    {
        OPENGEODE_EXCEPTION(
            answer2.find( grid.cell_index( cell ) ) != answer.end(),
            "[Test] Wrong result cells (conservative_rasterize_segment)" );
    }
}

void test_conservative_rasterize_triangle()
{
    geode::RegularGrid2D grid{ { { 0., 0. } }, { 10, 10 }, 1 };
    geode::Point2D pt0{ { 2.5, 2.5 } };
    geode::Point2D pt1{ { 6.5, 6.5 } };
    geode::Point2D pt2{ { 2.5, 6.5 } };
    geode::Triangle2D triangle{ pt0, pt1, pt2 };

    const auto cells = geode::rasterize_triangle( grid, triangle );
    const absl::flat_hash_set< geode::index_t > answer{ 22, 23, 32, 33, 34, 42,
        43, 44, 45, 52, 53, 54, 55, 56, 62, 63, 64, 65, 66 };
    OPENGEODE_EXCEPTION( answer.size() == cells.size(),
        "[Test] Wrong number of result cells (rasterize_triangle)" );
    for( const auto cell : cells )
    {
        OPENGEODE_EXCEPTION(
            answer.find( grid.cell_index( cell ) ) != answer.end(),
            "[Test] Wrong result cells (rasterize_triangle)" );
    }
}

void add_cells( absl::flat_hash_set< geode::GridCellIndices3D >& set,
    const geode::RegularGrid3D& grid,
    const geode::Triangle3D& triangle )
{
    for( const auto& cell : geode::rasterize_triangle( grid, triangle ) )
    {
        set.insert( cell );
    }
}

void test_limit()
{
    const geode::RegularGrid3D grid{ { { -1, -1, -1 } }, { 3, 3, 3 }, 1 };
    const geode::Point3D pt000{ { 0, 0, 0 } };
    const geode::Point3D pt001{ { 0, 0, 1 } };
    const geode::Point3D pt010{ { 0, 1, 0 } };
    const geode::Point3D pt011{ { 0, 1, 1 } };
    const geode::Point3D pt100{ { 1, 0, 0 } };
    const geode::Point3D pt101{ { 1, 0, 1 } };
    const geode::Point3D pt110{ { 1, 1, 0 } };
    const geode::Point3D pt111{ { 1, 1, 1 } };
    const geode::Triangle3D triangle1{ pt000, pt010, pt101 };
    const geode::Triangle3D triangle1b{ pt000, pt101, pt100 };
    const geode::Triangle3D triangle2{ pt010, pt011, pt111 };
    const geode::Triangle3D triangle2b{ pt010, pt111, pt101 };
    const geode::Triangle3D triangle3{ pt000, pt100, pt110 };
    const geode::Triangle3D triangle3b{ pt000, pt110, pt001 };
    const geode::Triangle3D triangle4{ pt001, pt110, pt111 };
    const geode::Triangle3D triangle4b{ pt001, pt111, pt011 };
    const geode::Triangle3D triangle5{ pt000, pt001, pt011 };
    const geode::Triangle3D triangle5b{ pt000, pt011, pt010 };
    const geode::Triangle3D triangle6{ pt100, pt101, pt111 };
    const geode::Triangle3D triangle6b{ pt100, pt111, pt110 };

    absl::flat_hash_set< geode::GridCellIndices3D > all_cells;
    add_cells( all_cells, grid, triangle1 );
    add_cells( all_cells, grid, triangle1b );
    add_cells( all_cells, grid, triangle2 );
    add_cells( all_cells, grid, triangle2b );
    add_cells( all_cells, grid, triangle3 );
    add_cells( all_cells, grid, triangle3b );
    add_cells( all_cells, grid, triangle4 );
    add_cells( all_cells, grid, triangle4b );
    add_cells( all_cells, grid, triangle5 );
    add_cells( all_cells, grid, triangle5b );
    add_cells( all_cells, grid, triangle6 );
    add_cells( all_cells, grid, triangle6b );

    OPENGEODE_EXCEPTION( all_cells.size() == 27,
        "[Test] Wrong number of result cells (rasterize_cube)" );
}

void test_limit2()
{
    const auto grid_ptr = geode::load_regular_grid< 3 >(
        absl::StrCat( geode::data_path, "grid.og_rgd3d" ) );
    const auto& grid = *grid_ptr;
    const auto triangle_surf = geode::load_triangulated_surface< 3 >(
        absl::StrCat( geode::data_path, "triangle.og_tsf3d" ) );
    const geode::Triangle3D triangle{ triangle_surf->point( 0 ),
        triangle_surf->point( 1 ), triangle_surf->point( 2 ) };

    const auto cells = geode::rasterize_triangle( grid, triangle );
    const absl::flat_hash_set< geode::index_t > answer{ 23361, 23395, 24720,
        24721, 24754, 24755, 26079, 26080, 26081, 26113, 26114, 26115 };

    OPENGEODE_EXCEPTION( answer.size() == cells.size(),
        "[Test] Wrong number of result cells (rasterize_triangle)" );
    for( const auto cell : cells )
    {
        OPENGEODE_EXCEPTION(
            answer.find( grid.cell_index( cell ) ) != answer.end(),
            "[Test] Wrong result cells (rasterize_triangle)" );
    }
}

void test()
{
    geode::RegularGrid3D grid{ { { 0., 0., 0. } }, { 10, 10, 10 }, 1 };
    geode::Point3D pt0{ { 2, 2, 2 } };
    geode::Point3D pt1{ { 3.55, 7.55, 3.55 } };
    geode::Point3D pt2{ { 6.55, 1.55, 5.55 } };
    geode::Point3D pt3{ { 6, 6, 6 } };
    geode::Point3D pt4{ { 4, 4, 4 } };

    test_rasterize_segment( grid, geode::Segment3D{ pt0, pt1 } );
    test_conservative_rasterize_segment();
    test_conservative_rasterize_triangle();
    test_rasterize_triangle( grid, geode::Triangle3D{ pt0, pt1, pt2 } );
    test_rasterize_degenerate_triangle(
        grid, geode::Triangle3D{ pt0, pt3, pt4 } );

    test_limit();
    test_limit2();

    geode::Logger::info( "TEST SUCCESS" );
}

OPENGEODE_TEST( "rasterize" )