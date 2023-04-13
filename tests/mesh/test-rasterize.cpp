/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/tests/common.h>

#include <absl/container/flat_hash_set.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/point.h>

#include <geode/mesh/builder/regular_grid_solid_builder.h>
#include <geode/mesh/builder/regular_grid_surface_builder.h>
#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/regular_grid_surface.h>
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
    auto grid = geode::RegularGrid2D::create();
    auto builder = geode::RegularGridBuilder2D::create( *grid );
    builder->initialize_grid( { { 0., 0. } }, { 10, 10 }, 1 );
    geode::Point2D pt0{ { 2.5, 2.5 } };
    geode::Point2D pt1{ { 6.5, 6.5 } };
    geode::Segment2D segment{ pt0, pt1 };

    const auto cells = geode::rasterize_segment( *grid, segment );
    const absl::flat_hash_set< geode::index_t > answer{ 22, 33, 44, 55, 66 };
    OPENGEODE_EXCEPTION( answer.size() == cells.size(),
        "[Test] Wrong number of result cells (rasterize_segment)" );
    for( const auto cell : cells )
    {
        OPENGEODE_EXCEPTION(
            answer.find( grid->cell_index( cell ) ) != answer.end(),
            "[Test] Wrong result cells (rasterize_segment)" );
    }
    const auto cells2 = geode::conservative_rasterize_segment( *grid, segment );
    const absl::flat_hash_set< geode::index_t > answer2{ 22, 23, 32, 33, 34, 43,
        44, 45, 54, 55, 56, 65, 66 };
    OPENGEODE_EXCEPTION( answer2.size() == cells2.size(),
        "[Test] Wrong number of result cells "
        "(conservative_rasterize_segment)" );
    for( const auto cell : cells2 )
    {
        OPENGEODE_EXCEPTION(
            answer2.find( grid->cell_index( cell ) ) != answer.end(),
            "[Test] Wrong result cells (conservative_rasterize_segment)" );
    }
}

void test_conservative_rasterize_triangle()
{
    auto grid = geode::RegularGrid2D::create();
    auto builder = geode::RegularGridBuilder2D::create( *grid );
    builder->initialize_grid( { { 0., 0. } }, { 10, 10 }, 1 );
    geode::Point2D pt0{ { 2.5, 2.5 } };
    geode::Point2D pt1{ { 6.5, 6.5 } };
    geode::Point2D pt2{ { 2.5, 6.5 } };
    geode::Triangle2D triangle{ pt0, pt1, pt2 };

    const auto cells = geode::rasterize_triangle( *grid, triangle );
    const absl::flat_hash_set< geode::index_t > answer{ 22, 23, 32, 33, 34, 42,
        43, 44, 45, 52, 53, 54, 55, 56, 62, 63, 64, 65, 66 };
    OPENGEODE_EXCEPTION( answer.size() == cells.size(),
        "[Test] Wrong number of result cells (rasterize_triangle)" );
    for( const auto cell : cells )
    {
        OPENGEODE_EXCEPTION(
            answer.find( grid->cell_index( cell ) ) != answer.end(),
            "[Test] Wrong result cells (rasterize_triangle)" );
    }
}

void add_cells( absl::flat_hash_set< geode::Grid3D::CellIndices >& set,
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
    auto grid = geode::RegularGrid3D::create();
    auto builder = geode::RegularGridBuilder3D::create( *grid );
    builder->initialize_grid( { { -1, -1, -1 } }, { 3, 3, 3 }, 1 );
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

    absl::flat_hash_set< geode::Grid3D::CellIndices > all_cells;
    add_cells( all_cells, *grid, triangle1 );
    add_cells( all_cells, *grid, triangle1b );
    add_cells( all_cells, *grid, triangle2 );
    add_cells( all_cells, *grid, triangle2b );
    add_cells( all_cells, *grid, triangle3 );
    add_cells( all_cells, *grid, triangle3b );
    add_cells( all_cells, *grid, triangle4 );
    add_cells( all_cells, *grid, triangle4b );
    add_cells( all_cells, *grid, triangle5 );
    add_cells( all_cells, *grid, triangle5b );
    add_cells( all_cells, *grid, triangle6 );
    add_cells( all_cells, *grid, triangle6b );

    OPENGEODE_EXCEPTION( all_cells.size() == 27,
        "[Test] Wrong number of result cells (rasterize_cube)" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto grid = geode::RegularGrid3D::create();
    auto builder = geode::RegularGridBuilder3D::create( *grid );
    builder->initialize_grid( { { 0., 0., 0. } }, { 10, 10, 10 }, 1 );
    geode::Point3D pt0{ { 2, 2, 2 } };
    geode::Point3D pt1{ { 3.55, 7.55, 3.55 } };
    geode::Point3D pt2{ { 6.55, 1.55, 5.55 } };
    geode::Point3D pt3{ { 6, 6, 6 } };
    geode::Point3D pt4{ { 4, 4, 4 } };

    test_rasterize_segment( *grid, geode::Segment3D{ pt0, pt1 } );
    test_conservative_rasterize_segment();
    test_conservative_rasterize_triangle();
    test_rasterize_triangle( *grid, geode::Triangle3D{ pt0, pt1, pt2 } );
    test_rasterize_degenerate_triangle(
        *grid, geode::Triangle3D{ pt0, pt3, pt4 } );

    test_limit();
}

OPENGEODE_TEST( "rasterize" )