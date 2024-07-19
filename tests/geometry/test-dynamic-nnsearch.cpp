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

#include <geode/basic/logger.hpp>

#include <geode/geometry/dynamic_nn_search.hpp>
#include <geode/geometry/point.hpp>

#include <geode/tests/common.hpp>

void test()
{
    geode::DynamicNNSearch2D search{ { geode::Point2D{ { 1.0, 0.0 } },
        geode::Point2D{ { -1.0, 0.0 } }, geode::Point2D{ { 2.0, 0.0 } },
        geode::Point2D{ { 3.0, 0.0 } } } };

    // TEST WITH THE RADIUS NEIGHBORS METHOD.
    // with radius = 1.1
    const double radius = 1.1;
    const auto result =
        search.radius_neighbors( geode::Point2D{ { 0, 0 } }, radius );
    OPENGEODE_EXCEPTION( search.nb_points() == 4,
        "[Test] Error wrong number of initial Points" );
    OPENGEODE_EXCEPTION( result.size() == 2,
        "[Test] Error wrong number of Points within the radius 1.1" );
    OPENGEODE_EXCEPTION(
        ( search.point( result.at( 0 ) ) == geode::Point2D{ { 1.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result.at( 1 ) ) == geode::Point2D{ { -1.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );

    // with radius = 1.0
    const double radius2 = 1.0;
    const auto result2 =
        search.radius_neighbors( geode::Point2D{ { 0, 0 } }, radius2 );
    OPENGEODE_EXCEPTION( search.nb_points() == 4,
        "[Test] Error wrong number of initial Points" );
    OPENGEODE_EXCEPTION( result2.size() == 0,
        "[Test] Error wrong number of Points within the radius 1.0" );

    // with radius = 2.1
    const double radius3 = 2.1;
    const auto result3 =
        search.radius_neighbors( geode::Point2D{ { 0, 0 } }, radius3 );
    OPENGEODE_EXCEPTION( search.nb_points() == 4,
        "[Test] Error wrong number of initial Points" );
    OPENGEODE_EXCEPTION( result3.size() == 3,
        "[Test] Error wrong number of Points within the radius 2.1" );
    OPENGEODE_EXCEPTION(
        ( search.point( result3.at( 0 ) ) == geode::Point2D{ { 1.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result3.at( 1 ) ) == geode::Point2D{ { -1.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result3.at( 2 ) ) == geode::Point2D{ { 2.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );

    // with radius = 3.1
    const double radius4 = 3.1;
    const auto result4 =
        search.radius_neighbors( geode::Point2D{ { 0, 0 } }, radius4 );
    OPENGEODE_EXCEPTION( search.nb_points() == 4,
        "[Test] Error wrong number of initial Points" );
    OPENGEODE_EXCEPTION( result4.size() == 4,
        "[Test] Error wrong number of Points within the radius" );
    OPENGEODE_EXCEPTION(
        ( search.point( result4.at( 0 ) ) == geode::Point2D{ { 1.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result4.at( 1 ) ) == geode::Point2D{ { -1.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result4.at( 2 ) ) == geode::Point2D{ { 2.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result4.at( 3 ) ) == geode::Point2D{ { 3.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );

    // test with the add point method
    // with pt1(0.1,0.1)
    geode::Point2D pt1{ { 0.1, 0.1 } };
    search.addPoint( pt1 );
    const auto result5 =
        search.radius_neighbors( geode::Point2D{ { 0, 0 } }, radius4 );
    OPENGEODE_EXCEPTION( search.nb_points() == 5,
        "[Test] Error wrong number of initial Points" );
    OPENGEODE_EXCEPTION( result5.size() == 5,
        "[Test] Error wrong number of Points within the radius" );
    OPENGEODE_EXCEPTION(
        ( search.point( result5.at( 0 ) ) == geode::Point2D{ { 0.1, 0.1 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result5.at( 1 ) ) == geode::Point2D{ { 1.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result5.at( 2 ) ) == geode::Point2D{ { -1.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result5.at( 3 ) ) == geode::Point2D{ { 2.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result5.at( 4 ) ) == geode::Point2D{ { 3.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );

    // with pt2(10,10)
    geode::Point2D pt2{ { 10, 10 } };
    search.addPoint( pt2 );
    const auto result6 =
        search.radius_neighbors( geode::Point2D{ { 0, 0 } }, radius4 );
    OPENGEODE_EXCEPTION( search.nb_points() == 6,
        "[Test] Error wrong number of initial Points" );
    OPENGEODE_EXCEPTION( result6.size() == 5,
        "[Test] Error wrong number of Points within the radius" );
    OPENGEODE_EXCEPTION(
        ( search.point( result6.at( 0 ) ) == geode::Point2D{ { 0.1, 0.1 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result6.at( 1 ) ) == geode::Point2D{ { 1.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result6.at( 2 ) ) == geode::Point2D{ { -1.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result6.at( 3 ) ) == geode::Point2D{ { 2.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
    OPENGEODE_EXCEPTION(
        ( search.point( result6.at( 4 ) ) == geode::Point2D{ { 3.0, 0.0 } } ),
        "[Test] Error wrong Point coordinate" );
}

OPENGEODE_TEST( "dynamic-nnsearch" )