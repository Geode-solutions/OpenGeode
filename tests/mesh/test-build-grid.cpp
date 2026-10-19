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
#include <geode/basic/logger.hpp>

#include <geode/basic/logger.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/helpers/build_grid.hpp>

#include <geode/tests/common.hpp>

void test_2d()
{
    const geode::index_t max_nb_cells{ 100 };
    geode::BoundingBox2D bbox;
    bbox.add_point( geode::Point2D{ { 0, 0 } } );
    bbox.add_point( geode::Point2D{ { 1, 1 } } );
    const auto grid =
        geode::build_grid_from_bbox_target_length_and_maximum_cell_number(
            bbox, 0.01, max_nb_cells );
    DEBUG( grid.nb_cells() );
    DEBUG( grid.nb_cells_in_direction( 0 ) );
    DEBUG( grid.nb_cells_in_direction( 1 ) );
    DEBUG( grid.cell_length_in_direction( 0 ) );
    DEBUG( grid.cell_length_in_direction( 1 ) );
    geode::OpenGeodeMeshException::test(
        grid.nb_cells() <= max_nb_cells, "Too much cells in built grid" );

    bbox.add_point( geode::Point2D{ { 1, 10 } } );
    const auto grid2 =
        geode::build_grid_from_bbox_target_length_and_maximum_cell_number(
            bbox, 0.01, max_nb_cells );
    DEBUG( grid2.nb_cells() );
    DEBUG( grid2.nb_cells_in_direction( 0 ) );
    DEBUG( grid2.nb_cells_in_direction( 1 ) );
    DEBUG( grid2.cell_length_in_direction( 0 ) );
    DEBUG( grid2.cell_length_in_direction( 1 ) );
    geode::OpenGeodeMeshException::test(
        grid2.nb_cells() <= max_nb_cells, "Too much cells in built grid2" );

    bbox.add_point( geode::Point2D{ { 3, 10 } } );
    const auto grid3 =
        geode::build_grid_from_bbox_target_length_and_maximum_cell_number(
            bbox, 0.01, max_nb_cells );
    DEBUG( grid3.nb_cells() );
    DEBUG( grid3.nb_cells_in_direction( 0 ) );
    DEBUG( grid3.nb_cells_in_direction( 1 ) );
    DEBUG( grid3.cell_length_in_direction( 0 ) );
    DEBUG( grid3.cell_length_in_direction( 1 ) );
    geode::OpenGeodeMeshException::test(
        grid3.nb_cells() <= max_nb_cells, "Too much cells in built grid3" );
}

void check_grid_3d( const geode::BoundingBox3D& bbox,
    double target_cell_length,
    geode::index_t max_nb_cells )
{
    const auto grid =
        geode::build_grid_from_bbox_target_length_and_maximum_cell_number(
            bbox, target_cell_length, max_nb_cells );
    geode::Logger::info( "Grid ", grid.nb_cells_in_direction( 0 ), " x ",
        grid.nb_cells_in_direction( 1 ), " x ", grid.nb_cells_in_direction( 2 ),
        " = ", grid.nb_cells(), " cells (max ", max_nb_cells, ")" );
    geode::OpenGeodeMeshException::test( grid.nb_cells() <= max_nb_cells,
        "Too much cells in built 3D grid: ", grid.nb_cells(), " > ",
        max_nb_cells );
    geode::OpenGeodeMeshException::test( grid.nb_cells() >= max_nb_cells / 8,
        "Too few cells in built 3D grid: ", grid.nb_cells() );
    const auto diagonal = bbox.diagonal();
    for( const auto d : geode::LRange{ 3 } )
    {
        if( diagonal.value( d ) < geode::GLOBAL_EPSILON )
        {
            continue;
        }
        geode::OpenGeodeMeshException::test(
            std::fabs( grid.nb_cells_in_direction( d )
                           * grid.cell_length_in_direction( d )
                       - diagonal.value( d ) )
                < 1e-6 * diagonal.value( d ),
            "Built 3D grid does not cover the bbox in direction ", d );
    }
}

void test_3d()
{
    geode::BoundingBox3D thin_bbox;
    thin_bbox.add_point( geode::Point3D{ { 0, 0, 0 } } );
    thin_bbox.add_point( geode::Point3D{ { 1000, 1000, 1 } } );
    check_grid_3d( thin_bbox, 1, 10000 );

    geode::BoundingBox3D bbox;
    bbox.add_point( geode::Point3D{ { 0, 0, 0 } } );
    bbox.add_point( geode::Point3D{ { 10, 10, 0.5 } } );
    check_grid_3d( bbox, 0.37, 1000 );
    check_grid_3d( bbox, 0.01, 1000 );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_2d();
    test_3d();
}

OPENGEODE_TEST( "build-grid" )