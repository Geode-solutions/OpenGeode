/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geometry/distance.hpp>
#include <geode/geometry/nn_search.hpp>
#include <geode/geometry/point.hpp>

#include <geode/tests/common.hpp>

void first_test()
{
    const geode::NNSearch2D search{ { geode::Point2D{ { 0.1, 4.2 } },
        geode::Point2D{ { 5.9, 7.3 } }, geode::Point2D{ { 1.8, -5 } },
        geode::Point2D{ { -7.3, -1.6 } } } };

    OPENGEODE_EXCEPTION(
        search.closest_neighbor( geode::Point2D{ { 0, 0 } } ) == 0,
        "[Test] Error in closest neighbor" );
    OPENGEODE_EXCEPTION(
        search.closest_neighbor( geode::Point2D{ { 1, -4 } } ) == 2,
        "[Test] Error in closest neighbor" );

    const std::vector< geode::index_t > answer_radius{ 0, 2 };
    OPENGEODE_EXCEPTION(
        search.radius_neighbors( geode::Point2D{ { 0, 0 } }, 5.4 )
            == answer_radius,
        "[Test] Error in radius neighbors" );

    const std::vector< geode::index_t > answer_neighbors{ 2, 0 };
    OPENGEODE_EXCEPTION(
        search.neighbors( geode::Point2D{ { -1, -1 } }, 2 ) == answer_neighbors,
        "[Test] Error in neighbors" );

    const geode::Point3D p0{ { 0.1, 2.9, 5.4 } };
    const geode::Point3D p1{ { 2.4, 8.1, 7.6 } };
    const geode::Point3D p2{ { 8.1, 4.2, 3.8 } };
    const geode::Point3D p3{ { 3.1, 9.4, 9.7 } };
    const geode::NNSearch3D colocator( { p0, p0, p1, p0, p2, p1, p3 } );

    const auto colocated_info =
        colocator.colocated_index_mapping( geode::GLOBAL_EPSILON );
    OPENGEODE_EXCEPTION( colocated_info.nb_colocated_points() == 3,
        "[Test] Should be 3 colocated points" );
    const std::vector< geode::index_t > mapping_answer{ 0, 0, 1, 0, 2, 1, 3 };
    OPENGEODE_EXCEPTION( colocated_info.colocated_mapping == mapping_answer,
        "[Test] Error in colocated mapping" );
    const std::vector< geode::Point3D > points_answer{ p0, p1, p2, p3 };
    OPENGEODE_EXCEPTION( colocated_info.unique_points == points_answer,
        "[Test] Error in unique points" );
}

void second_test()
{
    geode::Logger::set_level( geode::Logger::LEVEL::debug );
    std::vector< geode::Point2D > points{
        geode::Point2D{ { 0, 0.2 } },
        geode::Point2D{ { 0.25, 0 } },
        geode::Point2D{ { -0.25, 0 } },
        geode::Point2D{ { 0, 0.8 } },
        geode::Point2D{ { 0.25, 1 } },
        geode::Point2D{ { -0.25, 1 } },
    };
    const geode::NNSearch2D colocator{ points };
    const double DISTANCE{ 0.8 };

    const auto colocated_info = colocator.colocated_index_mapping( DISTANCE );
    DEBUG( colocated_info.nb_unique_points() );
    for( const auto p : geode::Indices{ points } )
    {
        OPENGEODE_EXCEPTION( colocated_info.colocated_mapping[p]
                                 < colocated_info.unique_points.size(),
            "[Test] Wrong value of colocated_mapping (bigger than unique "
            "points size)" );
        const auto& colocated_point =
            colocated_info.unique_points[colocated_info.colocated_mapping[p]];
        OPENGEODE_EXCEPTION(
            geode::point_point_distance( points[p], colocated_point )
                <= DISTANCE,
            "[Test] Colocated point is not close enough to original point" );
    }
    for( const auto up0 : geode::Indices{ colocated_info.unique_points } )
    {
        for( const auto up1 : geode::Indices{ colocated_info.unique_points } )
        {
            if( up1 <= up0 )
            {
                continue;
            }
            OPENGEODE_EXCEPTION(
                geode::point_point_distance( colocated_info.unique_points[up0],
                    colocated_info.unique_points[up1] )
                    > DISTANCE,
                "[Test] Colocated points are too close" );
        }
    }
    OPENGEODE_EXCEPTION( colocated_info.nb_unique_points() == 2,
        "[Test] Should be 2 unique points" );
}

void test()
{
    first_test();
    second_test();
}

OPENGEODE_TEST( "nnsearch" )