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

#include <geode/geometry/vector.h>

#include <geode/mesh/builder/geode/geode_regular_grid_solid_builder.h>
#include <geode/mesh/builder/geode/geode_regular_grid_surface_builder.h>
#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/regular_grid_surface.h>
#include <geode/mesh/helpers/euclidean_distance_transform.h>
#include <geode/mesh/io/regular_grid_output.h>

void test_distance_transform_2D( double cell_length )
{
    const auto grid = geode::RegularGrid2D::create();
    const auto builder = geode::RegularGridBuilder2D::create( *grid );
    builder->initialize_grid(
        geode::Point2D{ { 0., 0. } }, { 10, 10 }, cell_length );
    const std::array< const geode::Grid2D::CellIndices, 2 > objects_raster{
        { { 0, 0 }, { 9, 9 } }
    };
    const auto distance_map = geode::euclidean_distance_transform< 2 >(
        *grid, objects_raster, "test_edt" );
    const absl::flat_hash_map< geode::Grid2D::CellIndices, double > values{
        { geode::Grid2D::CellIndices{ 0, 1 }, cell_length * 1. },
        { geode::Grid2D::CellIndices{ 0, 2 }, cell_length * 2. },
        { geode::Grid2D::CellIndices{ 0, 3 }, cell_length * 3. },
        { geode::Grid2D::CellIndices{ 0, 4 }, cell_length * 4. },
        { geode::Grid2D::CellIndices{ 0, 5 }, cell_length * 5. },
        { geode::Grid2D::CellIndices{ 0, 6 }, cell_length * 6. },
        { geode::Grid2D::CellIndices{ 0, 7 }, cell_length * 7. },
        { geode::Grid2D::CellIndices{ 0, 8 }, cell_length * 8. },
        { geode::Grid2D::CellIndices{ 0, 9 }, cell_length * 9. },
        { geode::Grid2D::CellIndices{ 1, 0 }, cell_length * 1. },
        { geode::Grid2D::CellIndices{ 2, 0 }, cell_length * 2. },
        { geode::Grid2D::CellIndices{ 3, 0 }, cell_length * 3. },
        { geode::Grid2D::CellIndices{ 4, 0 }, cell_length * 4. },
        { geode::Grid2D::CellIndices{ 5, 0 }, cell_length * 5. },
        { geode::Grid2D::CellIndices{ 6, 0 }, cell_length * 6. },
        { geode::Grid2D::CellIndices{ 7, 0 }, cell_length * 7. },
        { geode::Grid2D::CellIndices{ 8, 0 }, cell_length * 8. },
        { geode::Grid2D::CellIndices{ 9, 0 }, cell_length * 9. },
        { geode::Grid2D::CellIndices{ 0, 0 }, cell_length * 0. },
        { geode::Grid2D::CellIndices{ 1, 1 }, cell_length * std::sqrt( 2 ) },
        { geode::Grid2D::CellIndices{ 2, 2 }, cell_length * std::sqrt( 8 ) },
        { geode::Grid2D::CellIndices{ 3, 3 }, cell_length * std::sqrt( 18 ) },
        { geode::Grid2D::CellIndices{ 4, 4 }, cell_length * std::sqrt( 32 ) },
        { geode::Grid2D::CellIndices{ 5, 5 }, cell_length * std::sqrt( 32 ) },
        { geode::Grid2D::CellIndices{ 6, 6 }, cell_length * std::sqrt( 18 ) },
        { geode::Grid2D::CellIndices{ 7, 7 }, cell_length * std::sqrt( 8 ) },
        { geode::Grid2D::CellIndices{ 8, 8 }, cell_length * std::sqrt( 2 ) },
        { geode::Grid2D::CellIndices{ 9, 9 }, cell_length * 0. },
        { geode::Grid2D::CellIndices{ 8, 1 }, cell_length * std::sqrt( 65 ) },
        { geode::Grid2D::CellIndices{ 7, 2 }, cell_length * std::sqrt( 53 ) },
        { geode::Grid2D::CellIndices{ 6, 3 }, cell_length * std::sqrt( 45 ) },
        { geode::Grid2D::CellIndices{ 5, 4 }, cell_length * std::sqrt( 41 ) },
        { geode::Grid2D::CellIndices{ 4, 5 }, cell_length * std::sqrt( 41 ) },
        { geode::Grid2D::CellIndices{ 3, 6 }, cell_length * std::sqrt( 45 ) },
        { geode::Grid2D::CellIndices{ 2, 7 }, cell_length * std::sqrt( 53 ) },
        { geode::Grid2D::CellIndices{ 1, 8 }, cell_length * std::sqrt( 65 ) }
    };
    for( const auto value : values )
    {
        OPENGEODE_EXCEPTION(
            std::fabs( distance_map->value( grid->cell_index( value.first ) )
                       - value.second )
                < geode::global_epsilon,
            "[Test] Wrong 2D euclidean distance map" );
    }
}
void test_distance_transform_3D( double cell_length )
{
    const auto grid = geode::RegularGrid3D::create();
    const auto builder = geode::RegularGridBuilder3D::create( *grid );
    builder->initialize_grid( { { 0., 0., 0. } }, { 10, 10, 10 }, cell_length );
    const std::array< const geode::Grid3D::CellIndices, 2 > objects_raster{
        { { 0, 0, 0 }, { 9, 9, 9 } }
    };
    const auto distance_map = geode::euclidean_distance_transform< 3 >(
        *grid, objects_raster, "test_edt" );
    const absl::flat_hash_map< geode::Grid3D::CellIndices, double > values{
        { geode::Grid3D::CellIndices{ 0, 9, 0 }, cell_length * 9. },
        { geode::Grid3D::CellIndices{ 0, 9, 9 }, cell_length * 9. },
        { geode::Grid3D::CellIndices{ 0, 0, 9 }, cell_length * 9. },
        { geode::Grid3D::CellIndices{ 9, 0, 0 }, cell_length * 9. },
        { geode::Grid3D::CellIndices{ 9, 0, 9 }, cell_length * 9. },
        { geode::Grid3D::CellIndices{ 9, 9, 0 }, cell_length * 9. },
        { geode::Grid3D::CellIndices{ 0, 0, 0 }, cell_length * 0. },
        { geode::Grid3D::CellIndices{ 1, 1, 1 }, cell_length * std::sqrt( 3 ) },
        { geode::Grid3D::CellIndices{ 2, 2, 2 },
            cell_length * std::sqrt( 12 ) },
        { geode::Grid3D::CellIndices{ 3, 3, 3 },
            cell_length * std::sqrt( 27 ) },
        { geode::Grid3D::CellIndices{ 4, 4, 4 },
            cell_length * std::sqrt( 48 ) },
        { geode::Grid3D::CellIndices{ 5, 5, 5 },
            cell_length * std::sqrt( 48 ) },
        { geode::Grid3D::CellIndices{ 6, 6, 6 },
            cell_length * std::sqrt( 27 ) },
        { geode::Grid3D::CellIndices{ 7, 7, 7 },
            cell_length * std::sqrt( 12 ) },
        { geode::Grid3D::CellIndices{ 8, 8, 8 }, cell_length * std::sqrt( 3 ) },
        { geode::Grid3D::CellIndices{ 9, 9, 9 }, cell_length * 0. }
    };
    for( const auto value : values )
    {
        OPENGEODE_EXCEPTION(
            std::fabs( distance_map->value( grid->cell_index( value.first ) )
                       - value.second )
                < geode::global_epsilon,
            "[Test] Wrong 3D euclidean distance map" );
    }
}
void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_distance_transform_2D( 0.5 );
    test_distance_transform_3D( 4.8 );
}

OPENGEODE_TEST( "euclidean distance transform" )