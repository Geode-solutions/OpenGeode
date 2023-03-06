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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>
#include <geode/geometry/vector.h>

#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/distance.h>

#include <geode/tests/common.h>

void test()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b_ref{ { 0.5, 0.1, 0.0 } };
    const geode::Point3D b{ { 0.6, 0.3 * geode::global_epsilon, 0.0 } };
    const geode::Point3D c{ { 1.0, 0.0, 0.0 } };

    const geode::Vector3D answer{ { 0, 0, -1 } };

    const geode::Triangle3D triangle_ref{ a, b_ref, c };
    const auto normal_ref = triangle_ref.normal();
    OPENGEODE_EXCEPTION( normal_ref,
        "[Test] Normal of reference triangle should be calculated" );
    OPENGEODE_EXCEPTION( normal_ref.value() == answer,
        "[Test] Wrong normal of reference triangle" );
    const auto pivot_ref = triangle_ref.pivot();
    OPENGEODE_EXCEPTION(
        pivot_ref, "[Test] Pivot of reference triangle should be calculated" );
    OPENGEODE_EXCEPTION(
        pivot_ref.value() == 0, "[Test] Wrong pivot of reference triangle" );

    const geode::Triangle3D triangle2{ a, b, c };
    const auto normal2 = triangle2.normal();
    OPENGEODE_EXCEPTION(
        !normal2, "[Test] Normal of triangle2 should not be calculated" );
    const auto pivot2 = triangle2.pivot();
    OPENGEODE_EXCEPTION(
        !pivot2, "[Test] Pivot of triangle2 should not be calculated" );
}

OPENGEODE_TEST( "distance" )