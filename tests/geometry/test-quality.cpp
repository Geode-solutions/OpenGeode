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

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

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
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality - 1 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio for perfect tetrahedron ", quality );
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality2 - 1 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio 2 for perfect tetrahedron ", quality2 );
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality3 - 1 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio 3 for perfect tetrahedron ", quality3 );
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality4 - 1 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio 4 for perfect tetrahedron ", quality4 );
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
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality - 1.366026 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio for regular tetrahedron ", quality );
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality2 - 0.7698 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio 2 for regular tetrahedron ", quality2 );
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality3 - 2 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio 3 for regular tetrahedron ", quality3 );
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality4 - 0.732051 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio 4 for regular tetrahedron ", quality4 );
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
    geode::OpenGeodeGeometryException::test(
        quality == std::numeric_limits< double >::max(),
        "Wrong aspect ratio for sliver tetrahedron ", quality );
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality2 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio 2 for sliver tetrahedron ", quality2 );
    geode::OpenGeodeGeometryException::test(
        quality3 == std::numeric_limits< double >::max(),
        "Wrong aspect ratio 3 for sliver tetrahedron ", quality3 );
    geode::OpenGeodeGeometryException::test( quality4 == 0,
        "Wrong aspect ratio 4 for sliver tetrahedron ", quality4 );
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

    geode::OpenGeodeGeometryException::test(
        std::fabs( quality - 2.884068 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio for random tetrahedron ", quality );
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality2 - 0.341354 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio 2 for random tetrahedron ", quality2 );
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality3 - 4.618802 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio 3 for random tetrahedron ", quality3 );
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality4 - 0.346732 ) < geode::GLOBAL_EPSILON,
        "Wrong aspect ratio 4 for random tetrahedron ", quality4 );
}

// Previous angle-based implementation
template < geode::index_t dimension >
double reference_triangle_angle_based_quality(
    const geode::Triangle< dimension >& triangle )
{
    const auto& vertices = triangle.vertices();
    std::array< double, 3 > sinus;
    for( const auto v : geode::LRange{ 3 } )
    {
        const auto& point = vertices[v].get();
        const geode::Vector< dimension > prev{ point,
            vertices[( v + 2 ) % 3].get() };
        const geode::Vector< dimension > next{ point,
            vertices[( v + 1 ) % 3].get() };
        const auto dot = std::clamp(
            prev.dot( next ) / ( prev.length() * next.length() ), -1.0, 1.0 );
        sinus[v] = std::sin( std::acos( dot ) );
    }
    return 4 * sinus[0] * sinus[1] * sinus[2]
           / ( sinus[0] + sinus[1] + sinus[2] );
}

template < geode::index_t dimension >
void check_triangle_quality( const geode::Triangle< dimension >& triangle )
{
    const auto quality = geode::triangle_angle_based_quality( triangle );
    const auto reference = reference_triangle_angle_based_quality( triangle );
    // Quality is in [0, 1]
    geode::OpenGeodeGeometryException::test(
        std::fabs( quality - reference ) <= 1e-12,
        "[Test] Wrong triangle quality ", quality, " vs ", reference );
}

void test_triangle_angle_based_quality()
{
    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ -10, 10 };
    for( const auto unused : geode::Range{ 10000 } )
    {
        geode_unused( unused );
        const geode::Point3D p0{ { distribution( generator ),
            distribution( generator ), distribution( generator ) } };
        const geode::Point3D p1{ { distribution( generator ),
            distribution( generator ), distribution( generator ) } };
        const geode::Point3D p2{ { distribution( generator ),
            distribution( generator ), distribution( generator ) } };
        check_triangle_quality< 3 >( { p0, p1, p2 } );
        const geode::Point2D q0{ { p0.value( 0 ), p0.value( 1 ) } };
        const geode::Point2D q1{ { p1.value( 0 ), p1.value( 1 ) } };
        const geode::Point2D q2{ { p2.value( 0 ), p2.value( 1 ) } };
        check_triangle_quality< 2 >( { q0, q1, q2 } );
    }
    const geode::Point2D a{ { 0, 0 } };
    const geode::Point2D b{ { 1, 0 } };
    const geode::Point2D c{ { 0.5, std::sqrt( 3. ) / 2 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs(
            geode::triangle_angle_based_quality( geode::Triangle2D{ a, b, c } )
            - 1 )
            < 1e-12,
        "[Test] Equilateral triangle quality should be 1" );
    const geode::Point2D d{ { 2, 0 } };
    geode::OpenGeodeGeometryException::test(
        geode::triangle_angle_based_quality( geode::Triangle2D{ a, b, d } )
            == 0,
        "[Test] Flat triangle quality should be 0" );
    geode::OpenGeodeGeometryException::test(
        geode::triangle_angle_based_quality( geode::Triangle2D{ a, a, d } )
            == 0,
        "[Test] Degenerated triangle quality should be 0" );
    geode::OpenGeodeGeometryException::test(
        geode::triangle_angle_based_quality( geode::Triangle2D{ a, a, a } )
            == 0,
        "[Test] Point triangle quality should be 0" );
}

void test()
{
    geode::Logger::set_level( geode::Logger::LEVEL::debug );
    test_perfect_tetrahedron();
    test_regular_tetrahedron();
    test_sliver_tetrahedron();
    test_other_tetrahedron();
    test_triangle_angle_based_quality();
}

OPENGEODE_TEST( "quality" )