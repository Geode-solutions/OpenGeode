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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>

#include <geode/tests/common.hpp>

void test()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b_ref{ { 0.5, 0.1, 0.0 } };
    const geode::Point3D b{ { 0.6, 0.3 * geode::GLOBAL_EPSILON, 0.0 } };
    const geode::Point3D c{ { 1.0, 0.0, 0.0 } };

    const geode::Vector3D answer{ { 0, 0, -1 } };

    const geode::Triangle3D triangle_ref{ a, b_ref, c };
    const auto normal_ref = triangle_ref.normal();
    geode::OpenGeodeGeometryException::test( normal_ref.has_value(),
        "Normal of reference triangle should be calculated" );
    geode::OpenGeodeGeometryException::test(
        normal_ref.value() == answer, "Wrong normal of reference triangle" );
    const auto pivot_ref = triangle_ref.pivot();
    geode::OpenGeodeGeometryException::test( pivot_ref.has_value(),
        "Pivot of reference triangle should be calculated" );
    geode::OpenGeodeGeometryException::test(
        pivot_ref.value() == 0, "Wrong pivot of reference triangle" );

    const geode::Triangle3D triangle2{ a, b, c };
    const auto normal2 = triangle2.normal();
    geode::OpenGeodeGeometryException::test(
        !normal2, "Normal of triangle2 should not be calculated" );
    const auto pivot2 = triangle2.pivot();
    geode::OpenGeodeGeometryException::test(
        !pivot2, "Pivot of triangle2 should not be calculated" );
}

OPENGEODE_TEST( "distance" )