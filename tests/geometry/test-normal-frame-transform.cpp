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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/frame.hpp>
#include <geode/geometry/normal_frame_transform.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/tests/common.hpp>

void test_normal_frame_transform()
{
    geode::Frame2D from{ { geode::Vector< 2 >{ { 1, 0 } },
        geode::Vector2D{ { 0, 1 } } } };
    geode::Frame2D to{ { geode::Vector2D{ { 1, 0 } },
        geode::Vector2D{ { 0, 0.1 } } } };
    geode::NormalFrameTransform2D frame_transform{ from, to };
    geode::Vector2D test_vector{ { 1, 1 } };
    const auto result = frame_transform.apply( test_vector );
    const auto correct_result = geode::Vector2D{ { 1, 0.1 } };
    OPENGEODE_EXCEPTION( result == correct_result,
        "[Test] Wrong result for normal frame transform" );
    geode::Logger::info( "TEST SUCCESS" );
}

void test()
{
    test_normal_frame_transform();
}

OPENGEODE_TEST( "normal-frame-transform" )