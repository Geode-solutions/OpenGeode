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

#include <limits>

#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
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
    const auto quality2 = geode::tetrahedron_volume_to_edge_ratio( tetra );
    const auto quality3 = geode::tetrahedron_collapse_aspect_ratio( tetra );
    const auto quality4 = geode::tetrahedron_volume_to_facet_ratio( tetra );
    OPENGEODE_EXCEPTION( std::fabs( quality - 1 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio for perfect tetrahedron ", quality );
    OPENGEODE_EXCEPTION( std::fabs( quality2 - 1 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio 2 for perfect tetrahedron ", quality2 );
    OPENGEODE_EXCEPTION( std::fabs( quality3 - 1 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio 3 for perfect tetrahedron ", quality3 );
    OPENGEODE_EXCEPTION( std::fabs( quality4 - 1 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio 4 for perfect tetrahedron ", quality4 );
}

void test_regular_tetrahedron()
{
    const geode::Point3D point0{ { 0, 0, 0 } };
    const geode::Point3D point1{ { 1, 0, 0 } };
    const geode::Point3D point2{ { 0, 1, 0 } };
    const geode::Point3D point3{ { 0, 0, 1 } };
    const geode::Tetrahedron tetra{ point0, point1, point2, point3 };
    const auto quality = geode::tetrahedron_aspect_ratio( tetra );
    const auto quality2 = geode::tetrahedron_volume_to_edge_ratio( tetra );
    const auto quality3 = geode::tetrahedron_collapse_aspect_ratio( tetra );
    const auto quality4 = geode::tetrahedron_volume_to_facet_ratio( tetra );
    OPENGEODE_EXCEPTION(
        std::fabs( quality - 1.366026 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio for regular tetrahedron ", quality );
    OPENGEODE_EXCEPTION( std::fabs( quality2 - 0.7698 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio 2 for regular tetrahedron ", quality2 );
    OPENGEODE_EXCEPTION( std::fabs( quality3 - 2 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio 3 for regular tetrahedron ", quality3 );
    OPENGEODE_EXCEPTION(
        std::fabs( quality4 - 0.732051 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio 4 for regular tetrahedron ", quality4 );
}

void test_sliver_tetrahedron()
{
    const geode::Point3D point0{ { 0, 0, 0 } };
    const geode::Point3D point1{ { 1, 0, 0 } };
    const geode::Point3D point2{ { 0, 1, 0 } };
    const geode::Point3D point3{ { 1, 1, 0 } };
    const geode::Tetrahedron tetra{ point0, point1, point2, point3 };
    const auto quality = geode::tetrahedron_aspect_ratio( tetra );
    const auto quality2 = geode::tetrahedron_volume_to_edge_ratio( tetra );
    const auto quality3 = geode::tetrahedron_collapse_aspect_ratio( tetra );
    const auto quality4 = geode::tetrahedron_volume_to_facet_ratio( tetra );
    OPENGEODE_EXCEPTION( quality == std::numeric_limits< double >::max(),
        "[Test] Wrong aspect ratio for sliver tetrahedron ", quality );
    OPENGEODE_EXCEPTION( std::fabs( quality2 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio 2 for sliver tetrahedron ", quality2 );
    OPENGEODE_EXCEPTION( quality3 == std::numeric_limits< double >::max(),
        "[Test] Wrong aspect ratio 3 for sliver tetrahedron ", quality3 );
    OPENGEODE_EXCEPTION( quality4 == 0,
        "[Test] Wrong aspect ratio 4 for sliver tetrahedron ", quality4 );
}

void test_other_tetrahedron()
{
    const geode::Point3D point0{ { 0, 0, 0 } };
    const geode::Point3D point1{ { 4, 0, 0 } };
    const geode::Point3D point2{ { 2, 1, 0 } };
    const geode::Point3D point3{ { 2, 1, 1 } };
    const geode::Tetrahedron tetra{ point0, point1, point2, point3 };
    const auto quality = geode::tetrahedron_aspect_ratio( tetra );
    const auto quality2 = geode::tetrahedron_volume_to_edge_ratio( tetra );
    const auto quality3 = geode::tetrahedron_collapse_aspect_ratio( tetra );
    const auto quality4 = geode::tetrahedron_volume_to_facet_ratio( tetra );

    OPENGEODE_EXCEPTION(
        std::fabs( quality - 2.884068 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio for random tetrahedron ", quality );
    OPENGEODE_EXCEPTION(
        std::fabs( quality2 - 0.341354 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio 2 for random tetrahedron ", quality2 );
    OPENGEODE_EXCEPTION(
        std::fabs( quality3 - 4.618802 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio 3 for random tetrahedron ", quality3 );
    OPENGEODE_EXCEPTION(
        std::fabs( quality4 - 0.346732 ) < geode::GLOBAL_EPSILON,
        "[Test] Wrong aspect ratio 4 for random tetrahedron ", quality4 );
}

void test()
{
    geode::Logger::set_level( geode::Logger::LEVEL::debug );
    test_perfect_tetrahedron();
    test_regular_tetrahedron();
    test_sliver_tetrahedron();
    test_other_tetrahedron();
}

OPENGEODE_TEST( "quality" )