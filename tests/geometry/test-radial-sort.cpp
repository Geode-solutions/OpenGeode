/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/point.h>
#include <geode/geometry/radial_sort.h>

#include <geode/tests/common.h>

void test_1()
{
    geode::Point3D s0{ { 0, 1, 0 } };
    geode::Point3D s1{ { 0, 0, 0 } };
    geode::Point3D a{ { 1, 0, 0 } };
    geode::Point3D b{ { 1, 1, 1 } };
    geode::Point3D c{ { 0, 0, 1 } };
    geode::Point3D d{ { -1, 1, 1 } };
    geode::Point3D e{ { -1, 0, 0 } };
    geode::Point3D f{ { -1, 1, -1 } };
    geode::Point3D g{ { 0, 0, -1 } };
    geode::Point3D h{ { 1, 1, -1 } };

    const auto sorted =
        geode::radial_sort( { s0, s1 }, { d, g, f, c, e, a, b, h } );
    const absl::FixedArray< geode::index_t > answer{ 1, 2, 4, 0, 3, 6, 5, 7 };
    OPENGEODE_EXCEPTION( sorted == answer, "[Test] Wrong radial sorting" );
}

void test_2()
{
    geode::Point3D s0{ { 0, 0, 1 } };
    geode::Point3D s1{ { 0, 1, 1 } };
    geode::Point3D a{ { 0, 0.5, 1.5 } };
    geode::Point3D b{ { 0, 0, 0.5 } };
    geode::Point3D c{ { 1, 1, 1 } };

    const auto sorted = geode::radial_sort( { s0, s1 }, { a, b, a, b, c, c } );
    const absl::FixedArray< geode::index_t > answer{ 5, 4, 2, 0, 1, 3 };
    OPENGEODE_EXCEPTION( sorted == answer, "[Test] Wrong radial sorting" );
}

void test()
{
    test_1();
    test_2();

    geode::Logger::info( "TEST SUCCESS" );
}

OPENGEODE_TEST( "radial-sort" )