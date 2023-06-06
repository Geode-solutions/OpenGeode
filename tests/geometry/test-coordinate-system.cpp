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

#include <geode/basic/logger.h>

#include <geode/geometry/coordinate_system.h>
#include <geode/geometry/point.h>

#include <geode/tests/common.h>

void check_point( const geode::CoordinateSystem2D& coord_system,
    const geode::Point2D& input,
    const geode::Point2D& output )
{
    const auto coordinates = coord_system.coordinates( input );
    OPENGEODE_EXCEPTION(
        coordinates.inexact_equal( output ), "[Test] Wrong coordinates" );
    const auto global_point = coord_system.global_coordinates( coordinates );
    OPENGEODE_EXCEPTION( input.inexact_equal( global_point ),
        "[Test] Wrong global coordinates" );
}

void acute_system()
{
    geode::Logger::info( "[Test] Acute system" );
    const geode::CoordinateSystem2D coord_system{
        { geode::Vector2D{ { 1, 0 } }, geode::Vector2D{ { 1, 1 } } },
        { { 1, 1 } }
    };
    check_point( coord_system, { { 0, 1 } }, { { -1, 0 } } );
    check_point( coord_system, { { 0, 0 } }, { { 0, -1 } } );
    check_point( coord_system, { { -1, 0 } }, { { -1, -1 } } );
    check_point( coord_system, { { 2, 0 } }, { { 2, -1 } } );
}

void optuse_system()
{
    geode::Logger::info( "[Test] Optuse system" );
    const geode::CoordinateSystem2D coord_system{
        { geode::Vector2D{ { 1, 0 } }, geode::Vector2D{ { -1, 1 } } },
        { { 1, 1 } }
    };
    check_point( coord_system, { { 0, 1 } }, { { -1, 0 } } );
    check_point( coord_system, { { 0, 0 } }, { { -2, -1 } } );
    check_point( coord_system, { { -1, 0 } }, { { -3, -1 } } );
    check_point( coord_system, { { 2, 0 } }, { { 0, -1 } } );
}

void test()
{
    acute_system();
    optuse_system();
}

OPENGEODE_TEST( "coordinate-system" )