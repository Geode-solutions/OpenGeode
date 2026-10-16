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

#include <random>

#include <absl/container/flat_hash_map.h>

#include <geode/tests/common.hpp>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/geode/geode_regular_grid_solid_builder.hpp>
#include <geode/mesh/builder/geode/geode_regular_grid_surface_builder.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>
#include <geode/mesh/helpers/euclidean_distance_transform.hpp>
#include <geode/mesh/io/regular_grid_output.hpp>

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
        geode::OpenGeodeMeshException::test(
            std::fabs( distance_map->value( grid->cell_index( value.first ) )
                       - value.second )
                < geode::GLOBAL_EPSILON,
            "Wrong 2D euclidean distance map" );
    }
}
void test_distance_transform_3D( double cell_length )
{
    const auto grid = geode::RegularGrid3D::create();
    const auto builder = geode::RegularGridBuilder3D::create( *grid );
    builder->initialize_grid(
        geode::Point3D{ { 0., 0., 0. } }, { 10, 10, 10 }, cell_length );
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
        geode::OpenGeodeMeshException::test(
            std::fabs( distance_map->value( grid->cell_index( value.first ) )
                       - value.second )
                < geode::GLOBAL_EPSILON,
            "Wrong 3D euclidean distance map" );
    }
}
template < geode::index_t dimension >
void check_against_brute_force( const geode::RegularGrid< dimension >& grid,
    absl::Span< const typename geode::Grid< dimension >::CellIndices > seeds,
    const geode::VariableAttribute< double >& distance_map )
{
    for( const auto cell : geode::Range{ grid.nb_cells() } )
    {
        const auto cell_indices = grid.cell_indices( cell );
        auto expected = std::numeric_limits< double >::max();
        for( const auto& seed : seeds )
        {
            double squared_distance{ 0 };
            for( const auto d : geode::LRange{ dimension } )
            {
                const auto delta = ( static_cast< double >( cell_indices[d] )
                                       - static_cast< double >( seed[d] ) )
                                   * grid.cell_length_in_direction( d );
                squared_distance += delta * delta;
            }
            expected = std::min( expected, std::sqrt( squared_distance ) );
        }
        const auto value = distance_map.value( cell );
        geode::OpenGeodeMeshException::test(
            std::fabs( value - expected ) <= 1e-9 * std::max( 1., expected ),
            "Wrong ", dimension, "D euclidean distance on random grid: ", value,
            " instead of ", expected );
        geode::OpenGeodeMeshException::test(
            distance_map.has_value( cell ) == !seeds.empty(),
            "Wrong has_value on euclidean distance map" );
    }
}

void test_random_distance_transform_2D( std::mt19937& generator )
{
    std::uniform_int_distribution< geode::index_t > size_distribution{ 1, 12 };
    std::uniform_real_distribution< double > length_distribution{ 0.1, 3. };
    std::uniform_int_distribution< geode::index_t > nb_seeds_distribution{ 0,
        4 };
    for( const auto trial : geode::Range{ 50 } )
    {
        const auto grid = geode::RegularGrid2D::create();
        const auto builder = geode::RegularGridBuilder2D::create( *grid );
        builder->initialize_grid( geode::Point2D{ { 0., 0. } },
            { size_distribution( generator ), size_distribution( generator ) },
            { geode::Vector2D{ { length_distribution( generator ), 0 } },
                geode::Vector2D{ { 0, length_distribution( generator ) } } } );
        std::vector< geode::Grid2D::CellIndices > seeds;
        const auto nb_seeds =
            trial == 0 ? 0 : nb_seeds_distribution( generator );
        for( const auto unused : geode::Range{ nb_seeds } )
        {
            geode_unused( unused );
            seeds.push_back( grid->cell_indices(
                std::uniform_int_distribution< geode::index_t >{
                    0, grid->nb_cells() - 1 }( generator ) ) );
        }
        const auto distance_map = geode::euclidean_distance_transform< 2 >(
            *grid, seeds, "random_edt" );
        check_against_brute_force< 2 >( *grid, seeds, *distance_map );
    }
}

void test_random_distance_transform_3D( std::mt19937& generator )
{
    std::uniform_int_distribution< geode::index_t > size_distribution{ 1, 9 };
    std::uniform_real_distribution< double > length_distribution{ 0.1, 3. };
    std::uniform_int_distribution< geode::index_t > nb_seeds_distribution{ 0,
        4 };
    for( const auto trial : geode::Range{ 50 } )
    {
        const auto grid = geode::RegularGrid3D::create();
        const auto builder = geode::RegularGridBuilder3D::create( *grid );
        builder->initialize_grid( geode::Point3D{ { 0., 0., 0. } },
            { size_distribution( generator ), size_distribution( generator ),
                size_distribution( generator ) },
            { geode::Vector3D{ { length_distribution( generator ), 0, 0 } },
                geode::Vector3D{ { 0, length_distribution( generator ), 0 } },
                geode::Vector3D{
                    { 0, 0, length_distribution( generator ) } } } );
        std::vector< geode::Grid3D::CellIndices > seeds;
        const auto nb_seeds =
            trial == 0 ? 0 : nb_seeds_distribution( generator );
        for( const auto unused : geode::Range{ nb_seeds } )
        {
            geode_unused( unused );
            seeds.push_back( grid->cell_indices(
                std::uniform_int_distribution< geode::index_t >{
                    0, grid->nb_cells() - 1 }( generator ) ) );
        }
        const auto distance_map = geode::euclidean_distance_transform< 3 >(
            *grid, seeds, "random_edt" );
        check_against_brute_force< 3 >( *grid, seeds, *distance_map );
    }
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_distance_transform_2D( 0.5 );
    test_distance_transform_3D( 4.8 );
    std::mt19937 generator{ 42 };
    test_random_distance_transform_2D( generator );
    test_random_distance_transform_3D( generator );
}

OPENGEODE_TEST( "euclidean distance transform" )