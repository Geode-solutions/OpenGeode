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

#include <absl/container/flat_hash_map.h>

#include <geode/tests/common.h>

#include <geode/basic/logger.h>

#include <geode/mesh/builder/geode_regular_grid_solid_builder.h>
#include <geode/mesh/builder/geode_regular_grid_surface_builder.h>
#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/regular_grid_surface.h>
#include <geode/mesh/helpers/euclidean_distance_transform.h>
#include <geode/mesh/io/regular_grid_output.h>

void test_distance_transform_2D( double cell_length )
{
    const auto grid = geode::RegularGrid2D::create();
    const auto builder = geode::RegularGridBuilder2D::create( *grid );
    builder->initialize_grid( { { 0., 0. } }, { 10, 10 }, cell_length );
    const std::array< const geode::GridCellIndices2D, 2 > objects_raster{
        { { 0, 0 }, { 9, 9 } }
    };
    const auto distance_map = geode::euclidean_distance_transform< 2 >(
        *grid, objects_raster, "test_edt" );
    const absl::flat_hash_map< geode::GridCellIndices2D, double > values{
        { geode::GridCellIndices2D{ 0, 1 }, cell_length * 1. },
        { geode::GridCellIndices2D{ 0, 2 }, cell_length * 2. },
        { geode::GridCellIndices2D{ 0, 3 }, cell_length * 3. },
        { geode::GridCellIndices2D{ 0, 4 }, cell_length * 4. },
        { geode::GridCellIndices2D{ 0, 5 }, cell_length * 5. },
        { geode::GridCellIndices2D{ 0, 6 }, cell_length * 6. },
        { geode::GridCellIndices2D{ 0, 7 }, cell_length * 7. },
        { geode::GridCellIndices2D{ 0, 8 }, cell_length * 8. },
        { geode::GridCellIndices2D{ 0, 9 }, cell_length * 9. },
        { geode::GridCellIndices2D{ 1, 0 }, cell_length * 1. },
        { geode::GridCellIndices2D{ 2, 0 }, cell_length * 2. },
        { geode::GridCellIndices2D{ 3, 0 }, cell_length * 3. },
        { geode::GridCellIndices2D{ 4, 0 }, cell_length * 4. },
        { geode::GridCellIndices2D{ 5, 0 }, cell_length * 5. },
        { geode::GridCellIndices2D{ 6, 0 }, cell_length * 6. },
        { geode::GridCellIndices2D{ 7, 0 }, cell_length * 7. },
        { geode::GridCellIndices2D{ 8, 0 }, cell_length * 8. },
        { geode::GridCellIndices2D{ 9, 0 }, cell_length * 9. },
        { geode::GridCellIndices2D{ 0, 0 }, cell_length * 0. },
        { geode::GridCellIndices2D{ 1, 1 }, cell_length * std::sqrt( 2 ) },
        { geode::GridCellIndices2D{ 2, 2 }, cell_length * std::sqrt( 8 ) },
        { geode::GridCellIndices2D{ 3, 3 }, cell_length * std::sqrt( 18 ) },
        { geode::GridCellIndices2D{ 4, 4 }, cell_length * std::sqrt( 32 ) },
        { geode::GridCellIndices2D{ 5, 5 }, cell_length * std::sqrt( 32 ) },
        { geode::GridCellIndices2D{ 6, 6 }, cell_length * std::sqrt( 18 ) },
        { geode::GridCellIndices2D{ 7, 7 }, cell_length * std::sqrt( 8 ) },
        { geode::GridCellIndices2D{ 8, 8 }, cell_length * std::sqrt( 2 ) },
        { geode::GridCellIndices2D{ 9, 9 }, cell_length * 0. },
        { geode::GridCellIndices2D{ 8, 1 }, cell_length * std::sqrt( 65 ) },
        { geode::GridCellIndices2D{ 7, 2 }, cell_length * std::sqrt( 53 ) },
        { geode::GridCellIndices2D{ 6, 3 }, cell_length * std::sqrt( 45 ) },
        { geode::GridCellIndices2D{ 5, 4 }, cell_length * std::sqrt( 41 ) },
        { geode::GridCellIndices2D{ 4, 5 }, cell_length * std::sqrt( 41 ) },
        { geode::GridCellIndices2D{ 3, 6 }, cell_length * std::sqrt( 45 ) },
        { geode::GridCellIndices2D{ 2, 7 }, cell_length * std::sqrt( 53 ) },
        { geode::GridCellIndices2D{ 1, 8 }, cell_length * std::sqrt( 65 ) }
    };
    for( const auto value : values )
    {
        OPENGEODE_EXCEPTION(
            std::fabs( distance_map->value( grid->cell_index( value.first ) )
                       - value.second )
                < 0.0001,
            "[Test] Wrong 2D euclidean distance map" );
    }
}
void test_distance_transform_3D( double cell_length )
{
    const auto grid = geode::RegularGrid3D::create();
    const auto builder = geode::RegularGridBuilder3D::create( *grid );
    builder->initialize_grid( { { 0., 0., 0. } }, { 10, 10, 10 }, cell_length );
    const std::array< const geode::GridCellIndices3D, 2 > objects_raster{
        { { 0, 0, 0 }, { 9, 9, 9 } }
    };
    const auto distance_map = geode::euclidean_distance_transform< 3 >(
        *grid, objects_raster, "test_edt" );
    const absl::flat_hash_map< geode::GridCellIndices3D, double > values{
        { geode::GridCellIndices3D{ 0, 9, 0 }, cell_length * 9. },
        { geode::GridCellIndices3D{ 0, 9, 9 }, cell_length * 9. },
        { geode::GridCellIndices3D{ 0, 0, 9 }, cell_length * 9. },
        { geode::GridCellIndices3D{ 9, 0, 0 }, cell_length * 9. },
        { geode::GridCellIndices3D{ 9, 0, 9 }, cell_length * 9. },
        { geode::GridCellIndices3D{ 9, 9, 0 }, cell_length * 9. },
        { geode::GridCellIndices3D{ 0, 0, 0 }, cell_length * 0. },
        { geode::GridCellIndices3D{ 1, 1, 1 }, cell_length * std::sqrt( 3 ) },
        { geode::GridCellIndices3D{ 2, 2, 2 }, cell_length * std::sqrt( 12 ) },
        { geode::GridCellIndices3D{ 3, 3, 3 }, cell_length * std::sqrt( 27 ) },
        { geode::GridCellIndices3D{ 4, 4, 4 }, cell_length * std::sqrt( 48 ) },
        { geode::GridCellIndices3D{ 5, 5, 5 }, cell_length * std::sqrt( 48 ) },
        { geode::GridCellIndices3D{ 6, 6, 6 }, cell_length * std::sqrt( 27 ) },
        { geode::GridCellIndices3D{ 7, 7, 7 }, cell_length * std::sqrt( 12 ) },
        { geode::GridCellIndices3D{ 8, 8, 8 }, cell_length * std::sqrt( 3 ) },
        { geode::GridCellIndices3D{ 9, 9, 9 }, cell_length * 0. }
    };
    for( const auto value : values )
    {
        OPENGEODE_EXCEPTION(
            std::fabs( distance_map->value( grid->cell_index( value.first ) )
                       - value.second )
                < 0.0001,
            "[Test] Wrong 3D euclidean distance map" );
    }
}
void test()
{
    geode::OpenGeodeMesh::initialize();
    test_distance_transform_2D( 0.5 );
    test_distance_transform_3D( 4.8 );
}

OPENGEODE_TEST( "euclidean distance transform" )