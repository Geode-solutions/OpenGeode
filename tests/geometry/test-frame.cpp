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

#include <geode/geometry/frame.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/tests/common.hpp>

void test_elongation_directions()
{
    const geode::Frame3D frame{ { geode::Vector3D{ { 1e-3, 0, 0 } },
        geode::Vector3D{ { 0, 0, 10 } }, geode::Vector3D{ { 0, 1, 0 } } } };
    geode::OpenGeodeGeometryException::test(
        frame.max_elongation_direction() == 1,
        "Wrong max elongation direction" );
    geode::OpenGeodeGeometryException::test(
        frame.min_elongation_direction() == 0,
        "Wrong min elongation direction" );
}

void test()
{
    test_elongation_directions();
}

OPENGEODE_TEST( "frame" )
