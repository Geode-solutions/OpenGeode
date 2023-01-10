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

#include <absl/container/flat_hash_map.h>

#include <geode/tests/common.h>

#include <geode/basic/logger.h>

#include <geode/mesh/builder/geode_regular_grid_solid_builder.h>
#include <geode/mesh/builder/geode_regular_grid_surface_builder.h>

#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/regular_grid_surface.h>

#include <geode/mesh/helpers/euclidean_distance_transform.h>

void test_distance_transform_2D()
{
    auto grid = geode::RegularGrid2D::create();
    auto builder = geode::RegularGridBuilder2D::create( *grid );
    builder->initialize_grid( { { 0., 0. } }, { 10, 10 }, 1 );

    std::vector< geode::GridCellIndices2D > objects_raster;
    objects_raster.push_back( geode::GridCellIndices2D{ 0, 0 } );
    objects_raster.push_back( geode::GridCellIndices2D{ 9, 9 } );
    std::string name( "test_edt" );
    auto ed_map =
        geode::euclidean_distance_transform( *grid, objects_raster, name );

    const absl::flat_hash_map< geode::GridCellIndices2D, double > values{
        { geode::GridCellIndices2D{ 0, 9 }, 9. },
        { geode::GridCellIndices2D{ 9, 0 }, 9. },
        { geode::GridCellIndices2D{ 0, 0 }, 0. },
        { geode::GridCellIndices2D{ 1, 1 }, 1.41421 },
        { geode::GridCellIndices2D{ 2, 2 }, 2.82843 },
        { geode::GridCellIndices2D{ 3, 3 }, 4.24264 },
        { geode::GridCellIndices2D{ 4, 4 }, 5.65685 },
        { geode::GridCellIndices2D{ 5, 5 }, 5.65685 },
        { geode::GridCellIndices2D{ 6, 6 }, 4.24264 },
        { geode::GridCellIndices2D{ 7, 7 }, 2.82843 },
        { geode::GridCellIndices2D{ 8, 8 }, 1.41421 },
        { geode::GridCellIndices2D{ 9, 9 }, 0. }
    };
    for( const auto value : values )
    {
        OPENGEODE_EXCEPTION(
            ed_map->value( grid->cell_index( value.first ) ) - value.second
                < 0.0001,
            "[Test] Wrong 2D euclidean distance map" );
    }
}
void test_distance_transform_3D()
{
    auto grid = geode::RegularGrid3D::create();
    auto builder = geode::RegularGridBuilder3D::create( *grid );
    builder->initialize_grid( { { 0., 0., 0. } }, { 10, 10, 10 }, 1 );

    std::vector< geode::GridCellIndices3D > objects_raster;
    objects_raster.push_back( geode::GridCellIndices3D{ 0, 0, 0 } );
    objects_raster.push_back( geode::GridCellIndices3D{ 9, 9, 9 } );
    std::string name( "test_edt" );
    auto ed_map =
        geode::euclidean_distance_transform( *grid, objects_raster, name );

    const absl::flat_hash_map< geode::GridCellIndices3D, double > values{
        { geode::GridCellIndices3D{ 0, 9, 0 }, 9. },
        { geode::GridCellIndices3D{ 0, 9, 9 }, 9. },
        { geode::GridCellIndices3D{ 0, 0, 9 }, 9. },
        { geode::GridCellIndices3D{ 9, 0, 0 }, 9. },
        { geode::GridCellIndices3D{ 9, 0, 9 }, 9. },
        { geode::GridCellIndices3D{ 9, 9, 0 }, 9. },
        { geode::GridCellIndices3D{ 0, 0, 0 }, 0. },
        { geode::GridCellIndices3D{ 1, 1, 1 }, 1.73205 },
        { geode::GridCellIndices3D{ 2, 2, 2 }, 3.4641 },
        { geode::GridCellIndices3D{ 3, 3, 3 }, 5.19615 },
        { geode::GridCellIndices3D{ 4, 4, 4 }, 6.9282 },
        { geode::GridCellIndices3D{ 5, 5, 5 }, 6.9282 },
        { geode::GridCellIndices3D{ 6, 6, 6 }, 5.19615 },
        { geode::GridCellIndices3D{ 7, 7, 7 }, 3.4641 },
        { geode::GridCellIndices3D{ 8, 8, 8 }, 1.73205 },
        { geode::GridCellIndices3D{ 9, 9, 9 }, 0. }
    };
    for( const auto value : values )
    {
        OPENGEODE_EXCEPTION(
            ed_map->value( grid->cell_index( value.first ) ) - value.second
                < 0.0001,
            "[Test] Wrong 3D euclidean distance map" );
    }
}
void test()
{
    geode::OpenGeodeMesh::initialize();
    test_distance_transform_2D();
    test_distance_transform_3D();
}

OPENGEODE_TEST( "euclidean distance transform" )