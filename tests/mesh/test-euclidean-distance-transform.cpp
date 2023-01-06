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

#include <geode/basic/logger.h>

#include <geode/mesh/builder/geode_regular_grid_surface_builder.h>
#include <geode/mesh/core/regular_grid_surface.h>
#include <geode/mesh/helpers/euclidean_distance_transform.h>

void test_approximated_distance_transform_2D()
{
    auto grid = geode::RegularGrid2D::create();
    auto builder = geode::RegularGridBuilder2D::create( *grid );
    builder->initialize_grid( { { 0., 0. } }, { 10, 10 }, 1 );

    std::vector< geode::GridCellIndices2D > objects_raster;
    objects_raster.push_back( geode::GridCellIndices2D{ 0, 0 } );
    std::string name( "test_edt" );
    auto ed_map = geode::approximated_euclidean_distance_transform(
        *grid, objects_raster, name );

    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 0, 0 } ) )
            == 0.,
        "[Test] Wrong initit distance" );
    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 9, 9 } ) )
            == 18.,
        "[Test] Wrong initit distance 9.9. " );
    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 0, 9 } ) )
            == 9.,
        "[Test] Wrong initit distance 0.9. " );
    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 9, 0 } ) )
            == 9.,
        "[Test] Wrong initit distance 9.0. " );
}
void test_square_distance_transform_2D()
{
    auto grid = geode::RegularGrid2D::create();
    auto builder = geode::RegularGridBuilder2D::create( *grid );
    builder->initialize_grid( { { 0., 0. } }, { 10, 10 }, 1 );

    std::vector< geode::GridCellIndices2D > objects_raster;
    objects_raster.push_back( geode::GridCellIndices2D{ 0, 0 } );
    std::string name( "test_edt" );
    auto ed_map = geode::euclidean_squared_distance_transform(
        *grid, objects_raster, name );

    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 0, 0 } ) )
            == 0.,
        "[Test] Wrong initit distance" );
    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 9, 9 } ) )
            == 162.,
        "[Test] Wrong initit distance 9.9. " );
    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 0, 9 } ) )
            == 81,
        "[Test] Wrong initit distance 0.9. " );
    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 9, 0 } ) )
            == 81,
        "[Test] Wrong initit distance 9.0. " );
}
void test_distance_transform_2D()
{
    auto grid = geode::RegularGrid2D::create();
    auto builder = geode::RegularGridBuilder2D::create( *grid );
    builder->initialize_grid( { { 0., 0. } }, { 10, 10 }, 1 );

    std::vector< geode::GridCellIndices2D > objects_raster;
    objects_raster.push_back( geode::GridCellIndices2D{ 0, 0 } );
    std::string name( "test_edt" );
    auto ed_map =
        geode::euclidean_distance_transform( *grid, objects_raster, name );

    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 0, 0 } ) )
            == 0.,
        "[Test] Wrong initit distance" );
    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 9, 9 } ) )
                - 12.72792
            < 0.001,
        "[Test] Wrong initit distance 9.9. " );
    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 0, 9 } ) )
            == 9,
        "[Test] Wrong initit distance 0.9. " );
    OPENGEODE_EXCEPTION(
        ed_map->value( grid->cell_index( geode::GridCellIndices2D{ 9, 0 } ) )
            == 9,
        "[Test] Wrong initit distance 9.0. " );
}
void test()
{
    geode::OpenGeodeMesh::initialize();
    test_approximated_distance_transform_2D();
    test_square_distance_transform_2D();
    test_distance_transform_2D();
}

OPENGEODE_TEST( "euclidean distance transform" )