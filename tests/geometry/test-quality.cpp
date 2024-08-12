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

#include <limits>

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/quality.hpp>

#include <geode/tests/common.hpp>

void test_perfect_tetrahedron()
{
    const geode::Point3D point0{ { 0, 0, 0 } };
    const geode::Point3D point1{ { 1, 0, 0 } };
    const geode::Point3D point2{ { 0.5, std::sqrt( 3 ) / 2, 0 } };
    const geode::Point3D bary = ( point0 + point1 + point2 ) / 3;
    const auto length = geode::point_point_distance( point0, bary );
    const geode::Point3D point3 =
        bary + geode::Point3D{ { 0, 0, std::sqrt( 1 - length * length ) } };
    const geode::Tetrahedron tetra{ point0, point1, point2, point3 };
    const auto quality = geode::tetrahedron_aspect_ratio( tetra );
    OPENGEODE_EXCEPTION( quality - 1 < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio for perfect tetrahedron ", quality );
}

void test_regular_tetrahedron()
{
    const geode::Point3D point0{ { 0, 0, 0 } };
    const geode::Point3D point1{ { 1, 0, 0 } };
    const geode::Point3D point2{ { 0, 1, 0 } };
    const geode::Point3D point3{ { 0, 0, 1 } };
    const geode::Tetrahedron tetra{ point0, point1, point2, point3 };
    const auto quality = geode::tetrahedron_aspect_ratio( tetra );
    OPENGEODE_EXCEPTION( quality - 1.36603 < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio for regular tetrahedron ", quality );
}

void test_sliver_tetrahedron()
{
    const geode::Point3D point0{ { 0, 0, 0 } };
    const geode::Point3D point1{ { 1, 0, 0 } };
    const geode::Point3D point2{ { 0, 1, 0 } };
    const geode::Point3D point3{ { 1, 1, 0 } };
    const geode::Tetrahedron tetra{ point0, point1, point2, point3 };
    const auto quality = geode::tetrahedron_aspect_ratio( tetra );
    OPENGEODE_EXCEPTION( quality == std::numeric_limits< double >::max(),
        "[Test] Wrong aspect ratio for regular tetrahedron ", quality );
}

void test()
{
    test_perfect_tetrahedron();
    test_regular_tetrahedron();
    test_sliver_tetrahedron();
}

OPENGEODE_TEST( "quality" )