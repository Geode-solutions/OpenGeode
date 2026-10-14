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

#include <random>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/frame.hpp>
#include <geode/geometry/point.hpp>

#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/cylinder.hpp>
#include <geode/geometry/basic_objects/ellipse.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/position.hpp>
#include <geode/geometry/projection.hpp>

#include <geode/tests/common.hpp>

#include <random>

void test_point_segment_distance_2d()
{
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Segment2D segment2D{ a, b };

    double distance;

    distance = geode::point_segment_distance( a, segment2D );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && geode::point_segment_projection( a, segment2D ) == a,
        "Wrong result for point_segment_distance with query Point2D "
        "a" );

    distance = geode::point_segment_distance( b, segment2D );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && geode::point_segment_projection( b, segment2D ) == b,
        "Wrong result for point_segment_distance with query Point2D "
        "b" );

    const geode::Point2D q1{ { 0.0, 0.0 } };
    distance = geode::point_segment_distance( q1, segment2D );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && geode::point_segment_projection( q1, segment2D ) == q1,
        "Wrong result for point_segment_distance with query Point2D "
        "q1" );

    const geode::Point2D q2{ { 10.0, 10.0 } };
    distance = geode::point_segment_distance( q2, segment2D );
    geode::OpenGeodeGeometryException::test(
        distance == std::sqrt( 106 )
            && geode::point_segment_projection( q2, segment2D ) == a,
        "Wrong result for point_segment_distance with query Point2D "
        "q2" );

    const geode::Point2D q3{ { 5.0, -1.0 } };
    distance = geode::point_segment_distance( q3, segment2D );
    const geode::Point2D result_q3{ { 0.0, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_segment_projection( q3, segment2D ) == result_q3,
        "Wrong result for point_segment_distance with query Point2D "
        "q3" );

    const geode::Point2D q4{ { 5.5, 1.5 } };
    distance = geode::point_segment_distance( q4, segment2D );
    const geode::Point2D result_q4{ { 0.5, 2.5 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_segment_projection( q4, segment2D ) == result_q4,
        "Wrong result for point_segment_distance with query Point2D "
        "q4" );
}

void test_point_segment_distance_3d()
{
    const geode::Point3D a{ { 1.0, 5.0, 2.0 } };
    const geode::Point3D b{ { -1.0, -5.0, -2.0 } };
    const geode::Segment3D segment3D{ a, b };

    double distance;

    distance = geode::point_segment_distance( a, segment3D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_segment_projection( a, segment3D )
                .inexact_equal( a ),
        "Wrong result for point_segment_distance with query Point3D "
        "a" );

    distance = geode::point_segment_distance( b, segment3D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_segment_projection( b, segment3D )
                .inexact_equal( b ),
        "Wrong result for point_segment_distance with query Point3D "
        "b" );

    const geode::Point3D q1{ { 0.0, 0.0, 0.0 } };
    distance = geode::point_segment_distance( q1, segment3D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_segment_projection( q1, segment3D )
                .inexact_equal( q1 ),
        "Wrong result for point_segment_distance with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 10.0, 10.0, 10.0 } };
    distance = geode::point_segment_distance( q2, segment3D );
    geode::OpenGeodeGeometryException::test(
        distance == std::sqrt( 170 )
            && geode::point_segment_projection( q2, segment3D )
                .inexact_equal( a ),
        "Wrong result for point_segment_distance with query Point3D "
        "q2" );

    const geode::Point3D q3{ { 5.0, -1.0, 0.0 } };
    distance = geode::point_segment_distance( q3, segment3D );
    const geode::Point3D result_q3{ { 0.0, 0.0, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == std::sqrt( 26 )
            && geode::point_segment_projection( q3, segment3D )
                .inexact_equal( result_q3 ),
        "Wrong result for point_segment_distance with query Point3D "
        "q3" );

    const geode::Point3D q4{ { 5.5, 1.5, 0.25 } };
    distance = geode::point_segment_distance( q4, segment3D );
    const geode::Point3D result_q4{ { 0.45, 2.25, 0.9 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs(
            std::sqrt( 5.05 * 5.05 + 0.75 * 0.75 + 0.65 * 0.65 ) - distance )
                < geode::GLOBAL_EPSILON
            && geode::point_segment_projection( q4, segment3D )
                .inexact_equal( result_q4 ),
        "Wrong result for point_segment_distance with query Point3D "
        "q4" );
}

void test_point_segment_distance()
{
    test_point_segment_distance_2d();
    test_point_segment_distance_3d();
}

void test_point_triangle_distance_2d()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };
    const geode::Triangle2D triangle2D{ a, b, c };

    double distance;
    geode::Point2D closest_point;

    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( a, triangle2D );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point == a,
        "Wrong result for point_triangle_distance with query point "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( b, triangle2D );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point == b,
        "Wrong result for point_triangle_distance with query point "
        "b" );

    const geode::Point2D q1{ { 0.5, 0.5 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q1, triangle2D );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point == q1,
        "Wrong result for point_triangle_distance with query point "
        "q1" );

    const geode::Point2D q2{ { 0.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q2, triangle2D );
    const geode::Point2D result_q2{ { 0.5, 0.5 } };
    geode::OpenGeodeGeometryException::test(
        distance == std::sqrt( 2 ) / 2. && closest_point == result_q2,
        "Wrong result for point_triangle_distance with query point "
        "q2" );

    const geode::Point2D q3{ { 2.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q3, triangle2D );
    geode::OpenGeodeGeometryException::test(
        distance == 1 && closest_point == c,
        "Wrong result for point_triangle_distance with query point "
        "q3" );

    const geode::Point2D q4{ { 0.8, 0.2 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q4, triangle2D );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point == q4,
        "Wrong result for point_triangle_distance with query point "
        "q4" );
}

void test_point_triangle_distance_3d()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 0.0 } };
    const geode::Triangle3D triangle3D{ a, b, c };

    double distance;
    geode::Point3D closest_point;

    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( a, triangle3D );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point == a,
        "Wrong result for point_triangle_distance with query Point3D "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( b, triangle3D );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point == b,
        "Wrong result for point_triangle_distance with query Point3D "
        "b" );

    const geode::Point3D q1{ { 0.5, 0.5, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q1, triangle3D );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point == q1,
        "Wrong result for point_triangle_distance with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 0.0, 1.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q2, triangle3D );
    const geode::Point3D result_q2{ { 0.5, 0.5, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == std::sqrt( 2 ) / 2. && closest_point == result_q2,
        "Wrong result for point_triangle_distance with query Point3D "
        "q2" );

    const geode::Point3D q3{ { 2.0, 1.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q3, triangle3D );
    geode::OpenGeodeGeometryException::test(
        distance == 1 && closest_point == c,
        "Wrong result for point_triangle_distance with query Point3D "
        "q3" );

    const geode::Point3D q4{ { 0.5, 0.5, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q4, triangle3D );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point == q4,
        "Wrong result for point_triangle_distance with query Point3D "
        "q4" );

    const geode::Point3D q5{ { 2.0, -2.0, -1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q5, triangle3D );
    geode::OpenGeodeGeometryException::test(
        distance == std::sqrt( 6 ) && closest_point == b,
        "Wrong result for point_triangle_distance with query Point3D "
        "q5" );

    const geode::Point3D q6{ { -1.0, -0.5, -1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q6, triangle3D );
    geode::OpenGeodeGeometryException::test(
        distance == std::sqrt( 2.25 ) && closest_point == a,
        "Wrong result for point_triangle_distance with query Point3D "
        "q6" );

    const geode::Point3D q7{ { 2.0, 3.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q7, triangle3D );
    geode::OpenGeodeGeometryException::test(
        distance == std::sqrt( 6 ) && closest_point == c,
        "Wrong result for point_triangle_distance with query Point3D "
        "q7" );

    const geode::Point3D q8{ { 0.5, -3.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q8, triangle3D );
    const geode::Point3D result_q8{ { 0.5, 0.0, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == std::sqrt( 10 ) && closest_point == result_q8,
        "Wrong result for point_triangle_distance with query Point3D "
        "q8" );

    const geode::Point3D q9{ { 0.5, -0.2, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q9, triangle3D );
    const geode::Point3D result_q9{ { 0.5, 0.0, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance - 0.2 ) < geode::GLOBAL_EPSILON
            && closest_point == result_q9,
        "Wrong result for point_triangle_distance with query Point3D "
        "q9" );

    const geode::Point3D q10{ { 0.75, 0.25, 10.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q10, triangle3D );
    const geode::Point3D result_q10{ { 0.75, 0.25, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance - 10 ) < geode::GLOBAL_EPSILON
            && closest_point == result_q10,
        "Wrong result for point_triangle_distance with query Point3D "
        "q10" );

    const geode::Point3D q11{ { 2.0, 0.5, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q11, triangle3D );
    const geode::Point3D result_q11{ { 1.0, 0.5, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance - 1 ) < geode::GLOBAL_EPSILON
            && closest_point == result_q11,
        "Wrong result for point_triangle_distance with query Point3D "
        "q11" );

    const geode::Point3D q12{ { -1.0, 2.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q12, triangle3D );
    const geode::Point3D result_q12{ { 0.5, 0.5, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == std::sqrt( 1.5 * 1.5 * 2 ) && closest_point == result_q12,
        "Wrong result for point_triangle_distance with query Point3D "
        "q12" );

    const geode::Point3D q13{ { -2.0, 1.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q13, triangle3D );
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance - std::sqrt( 5 ) ) < geode::GLOBAL_EPSILON
            && closest_point == a,
        "Wrong result for point_triangle_distance with query Point3D "
        "q13" );

    const geode::Point3D q14{ { 0.5, 2.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q14, triangle3D );
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance - std::sqrt( 1.25 ) ) < geode::GLOBAL_EPSILON
            && closest_point == c,
        "Wrong result for point_triangle_distance with query Point3D "
        "q13" );
}

void test_segment_segment_distance_2d()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 2.0, 2.0 } };
    const geode::Point2D c{ { 0.0, 2.0 } };
    const geode::Point2D d{ { 2.0, 0.0 } };
    const geode::Point2D g{ { 3.0, 0.0 } };
    const geode::Point2D h{ { 5.0, -2.0 } };
    const geode::Segment2D segment_ab{ a, b };
    const geode::Segment2D segment_dc{ d, c };
    const geode::Segment2D segment_gh{ g, h };
    const geode::OwnerSegment2D owner_segment_ab{ a, b };
    const geode::OwnerSegment2D owner_segment_dc{ d, c };
    const geode::OwnerSegment2D owner_segment_gh{ g, h };

    double distance;
    geode::Point2D closest_point0;
    geode::Point2D closest_point1;

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance( segment_ab, segment_dc );
    const geode::Point2D result_t00{ { 1.0, 1.0 } };
    const geode::Point2D result_t01{ { 1.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 0. && closest_point0 == result_t00
            && closest_point1 == result_t01,
        "Wrong result for segment_segment_distance 2D with segment_ab "
        "and segment_dc" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance( segment_ab, segment_ab );
    const geode::Point2D result_t10{ { 0.0, 0.0 } };
    const geode::Point2D result_t11{ { 0.0, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 0. && closest_point0 == result_t10
            && closest_point1 == result_t11,
        "Wrong result for segment_segment_distance 2D with segment_ab "
        "and segment_ab" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance( segment_dc, segment_gh );
    geode::OpenGeodeGeometryException::test(
        distance == 1. && closest_point0 == d && closest_point1 == g,
        "Wrong result for segment_segment_distance 2D with segment_dc "
        "and segment_gh" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance< 2 >( segment_dc, owner_segment_gh );
    geode::OpenGeodeGeometryException::test(
        distance == 1. && closest_point0 == d && closest_point1 == g,
        "Wrong result for segment_segment_distance 2D with segment_dc "
        "and owner_segment_gh" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance< 2 >(
            owner_segment_dc, owner_segment_gh );
    geode::OpenGeodeGeometryException::test(
        distance == 1. && closest_point0 == d && closest_point1 == g,
        "Wrong result for segment_segment_distance 2D with "
        "owner_segment_dc and owner_segment_gh" );
}

void test_segment_segment_distance_3d()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 2.0, 2.0, 0.0 } };
    const geode::Point3D c{ { 0.0, 2.0, 1.0 } };
    const geode::Point3D d{ { 2.0, 0.0, 1.0 } };
    const geode::Point3D e{ { 0.0, 0.0, 1.0 } };
    const geode::Point3D f{ { 2.0, 2.0, 1.0 } };
    const geode::Point3D g{ { 3.0, 0.0, 1.0 } };
    const geode::Point3D h{ { 5.0, -2.0, 1.0 } };
    const geode::Segment3D segment_ab{ a, b };
    const geode::Segment3D segment_cd{ c, d };
    const geode::Segment3D segment_ef{ e, f };
    const geode::Segment3D segment_gh{ g, h };

    double distance;
    geode::Point3D closest_point0;
    geode::Point3D closest_point1;

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance( segment_ab, segment_cd );
    const geode::Point3D result_t00{ { 1.0, 1.0, 0.0 } };
    const geode::Point3D result_t01{ { 1.0, 1.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1. && closest_point0.inexact_equal( result_t00 )
            && closest_point1.inexact_equal( result_t01 ),
        "Wrong result for segment_segment_distance with segment_ab and "
        "segment_cd" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance( segment_ab, segment_ef );
    const geode::Point3D result_t10{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D result_t11{ { 0.0, 0.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance - 1 ) <= geode::GLOBAL_EPSILON
            && closest_point0.inexact_equal( result_t10 )
            && closest_point1.inexact_equal( result_t11 ),
        "Wrong result for segment_segment_distance with segment_ab and "
        "segment_ef" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance( segment_cd, segment_gh );
    geode::OpenGeodeGeometryException::test(
        distance == 1. && closest_point0 == d && closest_point1 == g,
        "Wrong result for segment_segment_distance with segment_cd and "
        "segment_gh" );
}

void test_segment_segment_distance_near_parallel_fixed()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 10.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 2.0, 1.0, 0.0 } };
    const geode::Point3D d{ { 5.0, 1.0, 1e-5 } };
    const geode::Segment3D segment_ab{ a, b };
    const geode::Segment3D segment_cd{ c, d };
    const geode::Point3D result0{ { 2.0, 0.0, 0.0 } };
    const auto [distance, closest0, closest1] =
        geode::segment_segment_distance( segment_ab, segment_cd );
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance - 1 ) < geode::GLOBAL_EPSILON
            && closest0.inexact_equal( result0 ) && closest1.inexact_equal( c ),
        "Wrong result for segment_segment_distance with near parallel "
        "segments: ",
        distance, " ", closest0.string(), " ", closest1.string() );
    const auto [swapped_distance, swapped_closest0, swapped_closest1] =
        geode::segment_segment_distance( segment_cd, segment_ab );
    geode::OpenGeodeGeometryException::test(
        std::fabs( swapped_distance - 1 ) < geode::GLOBAL_EPSILON
            && swapped_closest0.inexact_equal( c )
            && swapped_closest1.inexact_equal( result0 ),
        "Wrong result for segment_segment_distance with swapped near "
        "parallel segments" );

    const geode::Point3D e{ { 1.0, 1.0, 1.0 } };
    const geode::Point3D f{ { 1.0 + 5e-7, 1.0, 1.0 } };
    const geode::Point3D g{ { 1.0, 1.0 + 1e-7, 1.0 } };
    const geode::Point3D h{ { 1.0 + 5e-7, 1.0 + 1.1e-7, 1.0 } };
    const auto [tiny_distance, tiny_closest0, tiny_closest1] =
        geode::segment_segment_distance(
            geode::Segment3D{ e, f }, geode::Segment3D{ g, h } );
    geode::OpenGeodeGeometryException::test(
        std::fabs( tiny_distance - 1e-7 ) < geode::GLOBAL_EPSILON,
        "Wrong result for segment_segment_distance with tiny near parallel "
        "segments: ",
        tiny_distance );

    // Deviation below GLOBAL_EPSILON: segments are considered parallel
    const geode::Point3D i{ { 5.0, 1.0, 3e-8 } };
    const auto [parallel_distance, parallel_closest0, parallel_closest1] =
        geode::segment_segment_distance( segment_ab, geode::Segment3D{ c, i } );
    geode::OpenGeodeGeometryException::test(
        std::fabs( parallel_distance - 1 ) < geode::GLOBAL_EPSILON
            && parallel_closest0.inexact_equal( result0 )
            && parallel_closest1.inexact_equal( c ),
        "Wrong result for segment_segment_distance with parallel segments "
        "below GLOBAL_EPSILON: ",
        parallel_distance, " ", parallel_closest0.string(), " ",
        parallel_closest1.string() );
}

void test_segment_segment_distance_near_parallel_random()
{
    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > coordinate{ -5, 5 };
    std::uniform_real_distribution< double > unit{ -1, 1 };
    std::uniform_real_distribution< double > exponent{ -12, -3 };
    const auto random_vector = [&]( double scale ) {
        return geode::Vector3D{
            { unit( generator ), unit( generator ), unit( generator ) }
        } * scale;
    };
    for( const auto unused : geode::Range{ 2000 } )
    {
        geode_unused( unused );
        const geode::Point3D p0{ { coordinate( generator ),
            coordinate( generator ), coordinate( generator ) } };
        const auto direction = random_vector( 3 );
        const geode::Point3D p1 = p0 + direction;
        const auto angle = std::pow( 10., exponent( generator ) );
        const auto q0 =
            p0 + direction * unit( generator ) + random_vector( 0.5 );
        const auto q1 = q0
                        + ( direction + random_vector( angle ) )
                              * ( 0.5 + std::fabs( unit( generator ) ) );
        const geode::Segment3D segment0{ p0, p1 };
        const geode::Segment3D segment1{ q0, q1 };
        const auto [distance, closest0, closest1] =
            geode::segment_segment_distance( segment0, segment1 );
        geode::OpenGeodeGeometryException::test(
            std::fabs(
                geode::point_point_distance( closest0, closest1 ) - distance )
                < geode::GLOBAL_EPSILON,
            "Wrong result for segment_segment_distance: distance does not "
            "match closest points" );
        geode::OpenGeodeGeometryException::test(
            geode::point_segment_distance( closest0, segment0 )
                    < geode::GLOBAL_EPSILON
                && geode::point_segment_distance( closest1, segment1 )
                       < geode::GLOBAL_EPSILON,
            "Wrong result for segment_segment_distance: closest points not "
            "on segments" );
        auto brute_force = std::numeric_limits< double >::max();
        constexpr geode::index_t nb_samples{ 1000 };
        for( const auto i : geode::Range{ nb_samples + 1 } )
        {
            const auto ratio = static_cast< double >( i ) / nb_samples;
            brute_force = std::min(
                brute_force, geode::point_segment_distance(
                                 p0 + ( p1 - p0 ) * ratio, segment1 ) );
            brute_force = std::min(
                brute_force, geode::point_segment_distance(
                                 q0 + ( q1 - q0 ) * ratio, segment0 ) );
        }
        geode::OpenGeodeGeometryException::test(
            distance <= brute_force + geode::GLOBAL_EPSILON,
            "Wrong result for segment_segment_distance with near parallel "
            "segments: ",
            distance, " | brute force: ", brute_force );
    }
}

void test_segment_segment_distance()
{
    test_segment_segment_distance_2d();
    test_segment_segment_distance_3d();
    test_segment_segment_distance_near_parallel_fixed();
    test_segment_segment_distance_near_parallel_random();
}

void test_segment_line_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 2.0, 2.0, 0.0 } };
    const geode::Point3D c{ { 0.0, 2.0, 1.0 } };
    const geode::Point3D d{ { 2.0, 0.0, 1.0 } };
    const geode::Point3D e{ { 0.0, 0.0, 1.0 } };
    const geode::Point3D f{ { 2.0, 2.0, 1.0 } };
    const geode::Point3D g{ { 3.0, 0.0, 1.0 } };
    const geode::Point3D h{ { 5.0, -2.0, 1.0 } };
    const geode::Segment3D segment_ab{ a, b };
    const geode::Segment3D segment_cd{ c, d };
    const geode::Segment3D segment_ef{ e, f };
    const geode::Segment3D segment_gh{ g, h };
    const geode::InfiniteLine3D line_cd{ segment_cd };
    const geode::InfiniteLine3D line_ef{ segment_ef };
    const geode::InfiniteLine3D line_gh{ segment_gh };

    double distance;
    geode::Point3D closest_point0;
    geode::Point3D closest_point1;

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_line_distance( segment_ab, line_cd );
    const geode::Point3D result_t00{ { 1.0, 1.0, 0.0 } };
    const geode::Point3D result_t01{ { 1.0, 1.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1. && closest_point0.inexact_equal( result_t00 )
            && closest_point1.inexact_equal( result_t01 ),
        "Wrong result for segment_line_distance with segment_ab and "
        "line_cd" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_line_distance( segment_ab, line_ef );
    const geode::Point3D result_t10{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D result_t11{ { 0.0, 0.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance - 1 ) <= geode::GLOBAL_EPSILON
            && closest_point0.inexact_equal( result_t10 )
            && closest_point1.inexact_equal( result_t11 ),
        "Wrong result for segment_line_distance with segment_ab and "
        "line_ef" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_line_distance( segment_cd, line_gh );
    const geode::Point3D result_t21{ { 0.5, 2.5, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == sqrt( 2 ) / 2. && closest_point0 == c
            && closest_point1.inexact_equal( result_t21 ),
        "Wrong result for segment_line_distance with segment_cd and "
        "line_gh" );
}

void test_point_triangle_distance()
{
    test_point_triangle_distance_2d();
    test_point_triangle_distance_3d();
}

void test_point_plane_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 0.0 } };
    const geode::Triangle3D triangle{ a, b, c };
    const auto plane = triangle.plane().value();

    double distance;
    geode::Point3D closest_point, answer;

    const geode::Point3D q1{ { 0.5, 0.5, 1 } };
    std::tie( distance, closest_point ) =
        geode::point_plane_distance( q1, plane );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1.0 && closest_point == answer,
        "Wrong result for point_plane_distance with query point "
        "q1" );
    std::tie( distance, closest_point ) =
        geode::point_plane_signed_distance( q1, plane );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1.0 && closest_point == answer,
        "Wrong result for point_plane_signed_distance with query point "
        "q1" );

    const geode::Point3D q2{ { 0.5, 0.5, -1 } };
    std::tie( distance, closest_point ) =
        geode::point_plane_distance( q2, plane );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1.0 && closest_point == answer,
        "Wrong result for point_plane_distance with query point "
        "q2" );
    std::tie( distance, closest_point ) =
        geode::point_plane_signed_distance( q2, plane );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == -1.0 && closest_point == answer,
        "Wrong result for point_plane_signed_distance with query point "
        "q2" );
}

void test_point_line_distance_2d()
{
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Segment2D segment2D{ a, b };
    const geode::InfiniteLine2D line2D{ segment2D };

    double distance;

    distance = geode::point_line_distance( a, line2D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( a, line2D ).inexact_equal( a ),
        "Wrong result for point_line_distance with query Point2D "
        "a" );

    distance = geode::point_line_distance( b, line2D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( b, line2D ).inexact_equal( b ),
        "Wrong result for point_line_distance with query Point2D "
        "b" );

    const geode::Point2D q1{ { 0.0, 0.0 } };
    distance = geode::point_line_distance( q1, line2D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q1, line2D ).inexact_equal( q1 ),
        "Wrong result for point_line_distance with query Point2D "
        "q1" );

    const geode::Point2D q2{ { 5.0, -1.0 } };
    distance = geode::point_line_distance( q2, line2D );
    const geode::Point2D result_q2{ { 0.0, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q2, line2D )
                .inexact_equal( result_q2 ),
        "Wrong result for point_line_distance with query Point2D "
        "q2" );
}

void test_point_line_signed_distance_2d()
{
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Segment2D segment2D{ a, b };
    const geode::InfiniteLine2D line2D{ segment2D };

    double distance;

    distance = geode::point_line_signed_distance( a, line2D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( a, line2D ).inexact_equal( a ),
        "Wrong result for point_line_signed_distance with query Point2D "
        "a" );

    distance = geode::point_line_signed_distance( b, line2D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( b, line2D ).inexact_equal( b ),
        "Wrong result for point_line_signed_distance with query Point2D "
        "b" );

    const geode::Point2D q1{ { 0.0, 0.0 } };
    distance = geode::point_line_signed_distance( q1, line2D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q1, line2D ).inexact_equal( q1 ),
        "Wrong result for point_line_signed_distance with query Point2D "
        "q1" );

    const geode::Point2D q2{ { 2.0, 10.0 } };
    distance = geode::point_line_signed_distance( q2, line2D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q2, line2D ).inexact_equal( q2 ),
        "Wrong result for point_line_signed_distance with query Point2D "
        "q2" );

    const geode::Point2D q3{ { 5.0, -1.0 } };
    distance = geode::point_line_signed_distance( q3, line2D );
    const geode::Point2D result_q3{ { 0.0, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q3, line2D )
                .inexact_equal( result_q3 ),
        "Wrong result for point_line_signed_distance with query Point2D "
        "q3" );

    const geode::Point2D q4{ { -5.0, 1.0 } };
    distance = geode::point_line_signed_distance( q4, line2D );
    const geode::Point2D result_q4{ { 0.0, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( -std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q4, line2D )
                .inexact_equal( result_q3 ),
        "Wrong result for point_line_signed_distance with query Point2D "
        "q4" );
}

void test_point_line_distance_3d()
{
    const geode::Point3D a{ { 1.0, 5.0, 2.0 } };
    const geode::Point3D b{ { -1.0, -5.0, -2.0 } };
    const geode::Segment3D segment3D{ a, b };
    const geode::InfiniteLine3D line3D{ segment3D };

    double distance;

    distance = geode::point_line_distance( a, line3D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( a, line3D ).inexact_equal( a ),
        "Wrong result for point_line_distance with query Point3D "
        "a" );

    distance = geode::point_line_distance( b, line3D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( b, line3D ).inexact_equal( b ),
        "Wrong result for point_line_distance with query Point3D "
        "b" );

    const geode::Point3D q1{ { 0.0, 0.0, 0.0 } };
    distance = geode::point_line_distance( q1, line3D );
    geode::OpenGeodeGeometryException::test(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q1, line3D ).inexact_equal( q1 ),
        "Wrong result for point_line_distance with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 5.0, -1.0, 0.0 } };
    distance = geode::point_line_distance( q2, line3D );
    const geode::Point3D result_q2{ { 0.0, 0.0, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q2, line3D )
                .inexact_equal( result_q2 ),
        "Wrong result for point_line_distance with query Point3D "
        "q2" );
}

void test_point_line_distance()
{
    test_point_line_distance_2d();
    test_point_line_signed_distance_2d();
    test_point_line_distance_3d();
}

void test_point_tetrahedron_distance()
{
    const auto check_tetra_distance = []( const geode::Tetrahedron& tetra ) {
        double distance;
        geode::Point3D closest_point, answer;

        const auto& v0 = tetra.vertices()[0];
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( v0, tetra );
        geode::OpenGeodeGeometryException::test(
            distance == 0 && closest_point == v0,
            "Wrong result for point_tetrahedron_distance with query "
            "Point3D v0" );

        const auto& v1 = tetra.vertices()[1];
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( v1, tetra );
        geode::OpenGeodeGeometryException::test(
            distance == 0 && closest_point == v1,
            "Wrong result for point_tetrahedron_distance with query "
            "Point3D v1" );

        const auto& v2 = tetra.vertices()[2];
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( v2, tetra );
        geode::OpenGeodeGeometryException::test(
            distance == 0 && closest_point == v2,
            "Wrong result for point_tetrahedron_distance with query "
            "Point3D v2" );

        const auto& v3 = tetra.vertices()[3];
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( v3, tetra );
        geode::OpenGeodeGeometryException::test(
            distance == 0 && closest_point == v3,
            "Wrong result for point_tetrahedron_distance with query "
            "Point3D v3" );

        const geode::Point3D q1{ { 0.25, 0.25, 0.25 } };
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( q1, tetra );
        geode::OpenGeodeGeometryException::test(
            distance == 0 && closest_point == q1,
            "Wrong result for point_tetrahedron_distance with query "
            "Point3D q1" );

        const geode::Point3D q2{ { 0.25, 0.25, 0.0 } };
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( q2, tetra );
        geode::OpenGeodeGeometryException::test(
            distance == 0 && closest_point == q2,
            "Wrong result for point_tetrahedron_distance with query "
            "Point3D q2" );

        const geode::Point3D q3{ { 2.5, 2.5, 0.0 } };
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( q3, tetra );
        answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
        geode::OpenGeodeGeometryException::test(
            distance == std::sqrt( 8 ) && closest_point.inexact_equal( answer ),
            "Wrong result for point_tetrahedron_distance with query "
            "Point3D q3" );
    };

    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 0.0, 1.0, 0.0 } };
    const geode::Point3D d{ { 0.0, 0.0, 1.0 } };

    const geode::Tetrahedron tetra{ a, b, c, d };
    check_tetra_distance( tetra );
    const geode::Tetrahedron negative_tetra{ b, a, c, d };
    check_tetra_distance( negative_tetra );
}

void test_point_sphere_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 1.0 } };
    const geode::Sphere3D sphere{ a, 2 };

    double distance;
    geode::Point3D closest_point, answer;

    const geode::Point3D q1{ { 2.0, 0.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_sphere_distance( q1, sphere );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point.inexact_equal( q1 ),
        "Wrong result for point_sphere_distance with query Point3D "
        "q1" );

    std::tie( distance, closest_point ) =
        geode::point_sphere_signed_distance( q1, sphere );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point.inexact_equal( q1 ),
        "Wrong result for point_sphere_signed_distance with query "
        "Point3D q1" );

    std::tie( distance, closest_point ) =
        geode::point_ball_distance( q1, sphere );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point.inexact_equal( q1 ),
        "Wrong result for point_ball_distance with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 0.0, 3.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_sphere_distance( q2, sphere );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1 && closest_point.inexact_equal( answer ),
        "Wrong result for point_sphere_distance with query Point3D "
        "q2" );

    std::tie( distance, closest_point ) =
        geode::point_sphere_signed_distance( q2, sphere );
    geode::OpenGeodeGeometryException::test(
        distance == 1 && closest_point.inexact_equal( answer ),
        "Wrong result for point_sphere_signed_distance with query "
        "Point3D q2" );

    std::tie( distance, closest_point ) =
        geode::point_ball_distance( q2, sphere );
    geode::OpenGeodeGeometryException::test(
        distance == 1 && closest_point.inexact_equal( answer ),
        "Wrong result for point_ball_distance with query Point3D "
        "q2" );

    const geode::Point3D q3{ { 0.0, 1.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_sphere_distance( q3, sphere );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1 && closest_point.inexact_equal( answer ),
        "Wrong result for point_sphere_distance with query Point3D "
        "q3" );

    std::tie( distance, closest_point ) =
        geode::point_sphere_signed_distance( q3, sphere );
    geode::OpenGeodeGeometryException::test(
        distance == -1 && closest_point.inexact_equal( answer ),
        "Wrong result for point_sphere_signed_distance with query "
        "Point3D q3" );

    std::tie( distance, closest_point ) =
        geode::point_ball_distance( q3, sphere );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point.inexact_equal( q3 ),
        "Wrong result for point_ball_distance with query Point3D "
        "q3" );

    std::tie( distance, closest_point ) =
        geode::point_sphere_distance( a, sphere );
    answer = geode::Point3D{ { 2.0, 0.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 2 && closest_point.inexact_equal( answer ),
        "Wrong result for point_sphere_distance with query Point3D "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_sphere_signed_distance( a, sphere );
    geode::OpenGeodeGeometryException::test(
        distance == -2 && closest_point.inexact_equal( answer ),
        "Wrong result for point_sphere_signed_distance with query "
        "Point3D a" );

    std::tie( distance, closest_point ) =
        geode::point_ball_distance( a, sphere );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point.inexact_equal( a ),
        "Wrong result for point_ball_distance with query "
        "Point3D a" );
}

void test_point_circle_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 1.0 } };
    const geode::Vector3D normal_x{ { -1.0, 0.0, 0.0 } };
    const geode::Vector3D normal_z{ { 0.0, 0.0, 1.0 } };
    const geode::Plane plane_x{ normal_x, a };
    const geode::Plane plane_z{ normal_z, a };
    const geode::Circle circle_x{ plane_x, 2 };
    const geode::Circle circle_z{ plane_z, 2 };

    double distance;
    geode::Point3D closest_point, answer;

    const geode::Point3D q1{ { 2.0, 0.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_circle_distance( q1, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == sqrt( 8 ) && closest_point.inexact_equal( answer ),
        "Wrong result for point_circle_distance with query Point3D "
        "q1 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_circle_distance( q1, circle_z );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point.inexact_equal( q1 ),
        "Wrong result for point_circle_distance with query Point3D "
        "q1 and circle_z" );

    std::tie( distance, closest_point ) =
        geode::point_circle_signed_distance( q1, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == -sqrt( 8 ) && closest_point.inexact_equal( answer ),
        "Wrong result for point_circle_signed_distance with query "
        "Point3D "
        "q1 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_circle_signed_distance( q1, circle_z );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point.inexact_equal( q1 ),
        "Wrong result for point_circle_signed_distance with query "
        "Point3D "
        "q1 and circle_z" );

    std::tie( distance, closest_point ) =
        geode::point_disk_distance( q1, circle_x );
    answer = geode::Point3D{ { 0.0, 0.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 2 && closest_point.inexact_equal( answer ),
        "Wrong result for point_disk_distance with query Point3D "
        "q1 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_disk_distance( q1, circle_z );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point.inexact_equal( q1 ),
        "Wrong result for point_disk_distance with query Point3D "
        "q1 and circle_z" );

    const geode::Point3D q2{ { 0.0, 3.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_circle_distance( q2, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1 && closest_point.inexact_equal( answer ),
        "Wrong result for point_circle_distance with query Point3D "
        "q2 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_circle_signed_distance( q2, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1 && closest_point.inexact_equal( answer ),
        "Wrong result for point_circle_signed_distance with query "
        "Point3D "
        "q2 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_disk_distance( q2, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1 && closest_point.inexact_equal( answer ),
        "Wrong result for point_disk_distance with query Point3D "
        "q2 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_circle_distance( a, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 2 && closest_point.inexact_equal( answer ),
        "Wrong result for point_circle_distance with query Point3D "
        "a and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_circle_signed_distance( a, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 2 && closest_point.inexact_equal( answer ),
        "Wrong result for point_circle_signed_distance with query "
        "Point3D "
        "a and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_disk_distance( a, circle_x );
    geode::OpenGeodeGeometryException::test(
        distance == 0 && closest_point.inexact_equal( a ),
        "Wrong result for point_disk_distance with query Point3D "
        "a and circle_x" );
}

void test_point_circle_signed_distance_far_from_origin()
{
    const geode::Point3D center{ { 0.0, 0.0, 10.0 } };
    const geode::Vector3D normal{ { 0.0, 0.0, 1.0 } };
    const geode::Plane plane{ normal, center };
    const geode::Circle circle{ plane, 1 };

    const auto [distance_below, closest_below] =
        geode::point_circle_signed_distance(
            geode::Point3D{ { 1.0, 0.0, 9.0 } }, circle );
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance_below + 1 ) < geode::GLOBAL_EPSILON,
        "Wrong result for point_circle_signed_distance below the circle "
        "plane: ",
        distance_below );

    const auto [distance_above, closest_above] =
        geode::point_circle_signed_distance(
            geode::Point3D{ { 1.0, 0.0, 11.0 } }, circle );
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance_above - 1 ) < geode::GLOBAL_EPSILON,
        "Wrong result for point_circle_signed_distance above the circle "
        "plane: ",
        distance_above );
}

void test_line_triangle_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 2.0, 2.0, 0.0 } };
    const geode::Point3D c{ { 0.0, 2.0, 1.0 } };
    const geode::Point3D d{ { 2.0, 0.0, 1.0 } };
    const geode::Point3D e{ { 0.0, 0.0, 1.0 } };
    const geode::Point3D f{ { 2.0, 2.0, 1.0 } };
    const geode::Point3D g{ { 3.0, 3.0, 1.0 } };
    const geode::Point3D h{ { 5.0, 5.0, 1.0 } };
    const geode::Triangle3D cdg{ c, d, g };
    const geode::Segment3D seg_ab{ a, b };
    const geode::Segment3D seg_ef{ e, f };
    const geode::InfiniteLine3D line_ab{ seg_ab };
    const geode::InfiniteLine3D line_ef{ seg_ef };
    const geode::InfiniteLine3D line_bz{ geode::Vector3D{ { 0, 0, 1 } }, b };
    const geode::InfiniteLine3D line_hx{ geode::Vector3D{ { 1, 0, 0 } }, h };

    double distance;
    geode::Point3D closest_point0;
    geode::Point3D closest_point1;

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::line_triangle_distance( line_ab, cdg );
    const geode::Point3D result_t00{ { 1.0, 1.0, 0.0 } };
    const geode::Point3D result_t01{ { 1.0, 1.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1. && closest_point0.inexact_equal( result_t00 )
            && closest_point1.inexact_equal( result_t01 ),
        "Wrong result for line_triangle_distance with line_ab" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::line_triangle_distance( line_ef, cdg );
    const geode::Point3D result_t10{ { 1.0, 1.0, 1.0 } };
    const geode::Point3D result_t11{ { 1.0, 1.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 0. && closest_point0.inexact_equal( result_t10 )
            && closest_point1.inexact_equal( result_t11 ),
        "Wrong result for line_triangle_distance with line_ef" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::line_triangle_distance( line_bz, cdg );
    geode::OpenGeodeGeometryException::test(
        distance == 0. && closest_point0.inexact_equal( f )
            && closest_point1.inexact_equal( f ),
        "Wrong result for line_triangle_distance with line_bz" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::line_triangle_distance( line_hx, cdg );
    const geode::Point3D result_t30{ { 3.0, 5.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 2. && closest_point0.inexact_equal( result_t30 )
            && closest_point1.inexact_equal( g ),
        "Wrong result for line_triangle_distance with line_hx" );
}

void test_segment_triangle_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 2.0, 2.0, 0.0 } };
    const geode::Point3D bplus{ { 2.0, 2.0, 2.0 } };
    const geode::Point3D bminus{ { 2.0, 2.0, -2.0 } };
    const geode::Point3D c{ { 0.0, 2.0, 1.0 } };
    const geode::Point3D d{ { 2.0, 0.0, 1.0 } };
    const geode::Point3D e{ { 0.0, 0.0, 1.0 } };
    const geode::Point3D f{ { 2.0, 2.0, 1.0 } };
    const geode::Point3D g{ { 3.0, 3.0, 1.0 } };
    const geode::Point3D h{ { 5.0, 5.0, 1.0 } };
    const geode::Point3D h2{ { 0.0, 5.0, 1.0 } };
    const geode::Triangle3D cdg{ c, d, g };
    const geode::Segment3D seg_ab{ a, b };
    const geode::Segment3D seg_ef{ e, f };
    const geode::Segment3D seg_bz0{ bplus, b };
    const geode::Segment3D seg_bz1{ bminus, b };
    const geode::Segment3D seg_hx{ h2, h };
    const geode::Point3D t0{ { 481.283958699181, 450.641409484767,
        16.0218597413699 } };
    const geode::Point3D t1{ { 436.66619673526, 410.956383185562,
        -9.33593054802634 } };
    const geode::Point3D t2{ { 437.005650057499, 490.326435783972,
        -9.33593054802634 } };
    const geode::Point3D s0{ { 478.259567983804, 448.24094898319,
        14.302289193253 } };
    const geode::Point3D s1{ { 478.345179768413, 450.641409484767,
        14.3452740537089 } };

    double distance;
    geode::Point3D closest_point0;
    geode::Point3D closest_point1;

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_triangle_distance( seg_ab, cdg );
    const geode::Point3D result_t00{ { 1.0, 1.0, 0.0 } };
    const geode::Point3D result_t01{ { 1.0, 1.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 1. && closest_point0.inexact_equal( result_t00 )
            && closest_point1.inexact_equal( result_t01 ),
        "Wrong result for segment_triangle_distance with seg_ab" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_triangle_distance( seg_ef, cdg );
    const geode::Point3D result_t10{ { 1.0, 1.0, 1.0 } };
    const geode::Point3D result_t11{ { 1.0, 1.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 0. && closest_point0.inexact_equal( result_t10 )
            && closest_point1.inexact_equal( result_t11 ),
        "Wrong result for segment_triangle_distance with seg_ef" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_triangle_distance( seg_bz0, cdg );
    geode::OpenGeodeGeometryException::test(
        distance == 0. && closest_point0.inexact_equal( f )
            && closest_point1.inexact_equal( f ),
        "Wrong result for segment_triangle_distance with seg_bz0" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_triangle_distance( seg_bz1, cdg );
    geode::OpenGeodeGeometryException::test(
        distance == 1. && closest_point0.inexact_equal( b )
            && closest_point1.inexact_equal( f ),
        "Wrong result for segment_triangle_distance with seg_bz1" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_triangle_distance( seg_hx, cdg );
    const geode::Point3D result_t30{ { 3.0, 5.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        distance == 2. && closest_point0.inexact_equal( result_t30 )
            && closest_point1.inexact_equal( g ),
        "Wrong result for segment_triangle_distance with seg_hx" );

    const geode::Triangle3D real_triangle{ t0, t1, t2 };
    const geode::Segment3D real_segment{ s0, s1 };
    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_triangle_distance( real_segment, real_triangle );
    geode::OpenGeodeGeometryException::test(
        distance <= geode::GLOBAL_EPSILON && closest_point0.inexact_equal( s0 )
            && closest_point1.inexact_equal( s0 ),
        "Wrong result for segment_triangle_distance with real_segment" );
}

void test_point_ellipse_distance_2d()
{
    const geode::Point2D center{ { 0.0, 0.0 } };
    const geode::Vector2D first_axis{ { 3.0, 0.0 } };
    const geode::Vector2D second_axis{ { 0.0, 2.0 } };
    const geode::Frame2D frame{ { first_axis, second_axis } };
    const geode::Ellipse2D ellipse{ center, frame };
    const geode::Point2D on_boundary{ { 3.0, 0.0 } };
    const geode::Point2D outside1{ { 4.0, 0 } };
    const geode::Point2D outside2{ { 0, 5.0 } };
    const auto [on_boundary_distance, on_boundary_closest_point] =
        geode::point_ellipse_distance( on_boundary, ellipse );
    geode::OpenGeodeGeometryException::test(
        on_boundary_distance == 0. && on_boundary_closest_point == on_boundary,
        "Wrong result for point_ellipse_distance_2d with on_boundary "
        "point" );
    const auto [outside1_distance, outside1_closest_point] =
        geode::point_ellipse_distance( outside1, ellipse );
    const geode::Point2D result_outside1{ { 3, 0 } };
    geode::OpenGeodeGeometryException::test(
        outside1_distance == 1
            && outside1_closest_point.inexact_equal( result_outside1 ),
        "Wrong result for point_ellipse_distance_2d with outside1 "
        "point" );
    const auto [outside2_distance, outside2_closest_point] =
        geode::point_ellipse_distance( outside2, ellipse );
    const geode::Point2D result_outside2{ { 0, 2 } };
    geode::OpenGeodeGeometryException::test(
        outside2_distance == 3
            && outside2_closest_point.inexact_equal( result_outside2 ),
        "Wrong result for point_ellipse_distance_2d with outside2 "
        "point" );
}

void test_point_ellipse_distance_2d_not_aligned()
{
    const geode::Point2D center{ { 0.0, 0.0 } };
    const geode::Vector2D first_axis{ { 1.0, 1.0 } };
    const geode::Vector2D second_axis{ { -2.0, 2.0 } };
    const geode::Frame2D frame{ { first_axis, second_axis } };
    const geode::Ellipse2D ellipse{ center, frame };
    const geode::Point2D point{ { 2.0, 2.0 } };
    const auto [distance, closest_point] =
        geode::point_ellipse_distance( point, ellipse );
    const geode::Point2D result{ { 1.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        std::fabs( std::sqrt( 2 ) - distance ) < geode::GLOBAL_EPSILON
            && closest_point.inexact_equal( result ),
        "Wrong result for point_ellipse_distance_2d_not_aligned" );
}

void test_point_ellipse_distance_3d()
{
    const geode::Point3D center{ { 0.0, 0.0, 0.0 } };
    const geode::Vector3D first_axis{ { 3.0, 0.0, 0.0 } };
    const geode::Vector3D second_axis{ { 0.0, 2.0, 0.0 } };
    const geode::Vector3D third_axis{ { 0.0, 0.0, 1.0 } };
    const geode::Frame3D frame{ { first_axis, second_axis, third_axis } };
    const geode::Ellipse3D ellipse{ center, frame };
    const geode::Point3D on_boundary{ { 3.0, 0.0, 0.0 } };
    const geode::Point3D outside1{ { 4.0, 0, 0 } };
    const geode::Point3D outside2{ { 0, 5.0, 0 } };
    const geode::Point3D outside3{ { 0, 0, 3 } };
    const auto [on_boundary_distance, on_boundary_closest_point] =
        geode::point_ellipse_distance( on_boundary, ellipse );
    geode::OpenGeodeGeometryException::test(
        on_boundary_distance == 0. && on_boundary_closest_point == on_boundary,
        "Wrong result for point_ellipse_distance_3d with "
        "on_boundary "
        "point" );
    const auto [outside1_distance, outside1_closest_point] =
        geode::point_ellipse_distance( outside1, ellipse );
    const geode::Point3D result_outside1{ { 3, 0, 0 } };
    geode::OpenGeodeGeometryException::test(
        outside1_distance == 1
            && outside1_closest_point.inexact_equal( result_outside1 ),
        "Wrong result for point_ellipse_distance_3d with outside1 "
        "point" );
    const auto [outside2_distance, outside2_closest_point] =
        geode::point_ellipse_distance( outside2, ellipse );
    const geode::Point3D result_outside2{ { 0, 2, 0 } };
    geode::OpenGeodeGeometryException::test(
        outside2_distance == 3
            && outside2_closest_point.inexact_equal( result_outside2 ),
        "Wrong result for point_ellipse_distance_3d with outside2 "
        "point" );
    const auto [outside3_distance, outside3_closest_point] =
        geode::point_ellipse_distance( outside3, ellipse );
    const geode::Point3D result_outside3{ { 0, 0, 1 } };
    geode::OpenGeodeGeometryException::test(
        outside3_distance == 2
            && outside3_closest_point.inexact_equal( result_outside3 ),
        "Wrong result for point_ellipse_distance_3d with outside3 "
        "point" );
    const auto [inside_distance, inside_closest_point] =
        geode::point_ellipse_distance( center, ellipse );
    const geode::Point3D result_inside{ { 0, 0, 1 } };
    geode::OpenGeodeGeometryException::test(
        inside_distance == 1
            && inside_closest_point.inexact_equal( result_inside ),
        "Wrong result for point_ellipse_distance_3d with inside "
        "point" );
}

template < geode::index_t dimension >
double ellipse_level( const geode::Ellipse< dimension >& ellipse,
    const geode::Point< dimension >& point )
{
    const geode::Vector< dimension > center_to_point{ ellipse.center(), point };
    double level{ 0 };
    for( const auto d : geode::LRange{ dimension } )
    {
        const auto& axis = ellipse.axes().direction( d );
        const auto coordinate = center_to_point.dot( axis ) / axis.length2();
        level += coordinate * coordinate;
    }
    return level;
}

double brute_force_ellipse_distance(
    const geode::Ellipse2D& ellipse, const geode::Point2D& point )
{
    const auto& axes = ellipse.axes();
    auto min_distance = std::numeric_limits< double >::max();
    constexpr geode::index_t nb_samples{ 4000 };
    for( const auto s : geode::Range{ nb_samples } )
    {
        const auto angle = 2. * M_PI * s / nb_samples;
        const auto sample = ellipse.center()
                            + axes.direction( 0 ) * std::cos( angle )
                            + axes.direction( 1 ) * std::sin( angle );
        min_distance = std::min(
            min_distance, geode::point_point_distance( point, sample ) );
    }
    return min_distance;
}

double brute_force_ellipse_distance(
    const geode::Ellipse3D& ellipse, const geode::Point3D& point )
{
    const auto& axes = ellipse.axes();
    auto min_distance = std::numeric_limits< double >::max();
    constexpr geode::index_t nb_samples{ 100 };
    for( const auto s0 : geode::Range{ nb_samples + 1 } )
    {
        const auto theta = M_PI * s0 / nb_samples;
        for( const auto s1 : geode::Range{ 2 * nb_samples } )
        {
            const auto phi = M_PI * s1 / nb_samples;
            const auto sample =
                ellipse.center()
                + axes.direction( 0 ) * ( std::sin( theta ) * std::cos( phi ) )
                + axes.direction( 1 ) * ( std::sin( theta ) * std::sin( phi ) )
                + axes.direction( 2 ) * std::cos( theta );
            min_distance = std::min(
                min_distance, geode::point_point_distance( point, sample ) );
        }
    }
    return min_distance;
}

template < geode::index_t dimension >
void check_point_ellipse_distance( const geode::Ellipse< dimension >& ellipse,
    const geode::Point< dimension >& point )
{
    const auto [distance, closest_point] =
        geode::point_ellipse_distance( point, ellipse );
    geode::OpenGeodeGeometryException::test(
        std::fabs( ellipse_level( ellipse, closest_point ) - 1 )
            < geode::GLOBAL_EPSILON,
        "Wrong result for point_ellipse_distance with query ", point.string(),
        ": closest point ", closest_point.string(), " not on ellipse" );
    geode::OpenGeodeGeometryException::test(
        std::fabs(
            geode::point_point_distance( point, closest_point ) - distance )
            < geode::GLOBAL_EPSILON,
        "Wrong result for point_ellipse_distance with query ", point.string(),
        ": distance does not match closest point" );
    const auto brute_force = brute_force_ellipse_distance( ellipse, point );
    geode::OpenGeodeGeometryException::test(
        distance <= brute_force + geode::GLOBAL_EPSILON,
        "Wrong result for point_ellipse_distance with query ", point.string(),
        ": ", distance, " | brute force: ", brute_force );
}

void test_point_ellipse_distance_random()
{
    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ -4, 4 };
    const geode::Point2D origin2d{ { 0, 0 } };
    const geode::Point2D center2d{ { 0.5, -0.25 } };
    const geode::Frame2D sorted_frame2d{ { geode::Vector2D{ { 2, 0 } },
        geode::Vector2D{ { 0, 1 } } } };
    const geode::Frame2D unsorted_frame2d{ { geode::Vector2D{ { 1, 0 } },
        geode::Vector2D{ { 0, 2 } } } };
    const geode::Frame2D rotated_frame2d{ { geode::Vector2D{ { 0.6, 0.8 } },
        geode::Vector2D{ { -2.4, 1.8 } } } };
    const geode::Ellipse2D sorted2d{ center2d, sorted_frame2d };
    const geode::Ellipse2D unsorted2d{ center2d, unsorted_frame2d };
    const geode::Ellipse2D rotated2d{ center2d, rotated_frame2d };
    check_point_ellipse_distance( geode::Ellipse2D{ origin2d, sorted_frame2d },
        geode::Point2D{ { -3, -0.5 } } );
    for( const auto unused : geode::Range{ 200 } )
    {
        geode_unused( unused );
        const geode::Point2D query{ { distribution( generator ),
            distribution( generator ) } };
        check_point_ellipse_distance( sorted2d, query );
        check_point_ellipse_distance( unsorted2d, query );
        check_point_ellipse_distance( rotated2d, query );
    }
    const geode::Point3D center3d{ { 0.5, -0.25, 1 } };
    const geode::Frame3D frame3d{ { geode::Vector3D{ { 0, 0, 1 } },
        geode::Vector3D{ { 0, 3, 0 } }, geode::Vector3D{ { 2, 0, 0 } } } };
    const geode::Ellipse3D ellipse3d{ center3d, frame3d };
    for( const auto unused : geode::Range{ 50 } )
    {
        geode_unused( unused );
        const geode::Point3D query{ { distribution( generator ),
            distribution( generator ), distribution( generator ) } };
        check_point_ellipse_distance( ellipse3d, query );
    }
}

void test_point_ellipse_distance()
{
    test_point_ellipse_distance_random();
    test_point_ellipse_distance_2d();
    test_point_ellipse_distance_3d();
    test_point_ellipse_distance_2d_not_aligned();
}

void test_triangle_triangle_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 1.0, 0.0 } };
    const geode::Point3D c{ { -1.0, 1.0, 0.0 } };
    const geode::Point3D d{ { 0.0, 2.0, 0.0 } };
    const geode::Triangle3D triangle0{ a, b, d };
    const geode::Triangle3D triangle1{ c, b, d };
    const auto result0 =
        geode::triangle_triangle_distance( triangle0, triangle1 );
    geode::OpenGeodeGeometryException::test(
        std::get< 0 >( result0 ) < geode::GLOBAL_EPSILON,
        "The two triangles should have a distance smaller than "
        "epsilon" );
    const auto result1 =
        geode::triangle_triangle_distance_between_non_conformal_parts(
            triangle0, triangle1 );
    geode::OpenGeodeGeometryException::test(
        std::get< 0 >( result1.value() ) < geode::GLOBAL_EPSILON,
        "The two triangles should have a distance smaller than "
        "epsilon (non conformal parts only)" );
}

void test_degenerated_segment_triangle_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 0.0, 1.0, 0.0 } };
    const geode::Triangle3D triangle{ a, b, c };
    for( const auto length :
        { 0.4 * geode::GLOBAL_EPSILON, 0.1 * geode::GLOBAL_EPSILON } )
    {
        const geode::Point3D p0{ { 0.2, 0.2, 1.0 } };
        const geode::Point3D p1{ { 0.2, 0.2 + length, 1.0 } };
        const auto [distance, closest_on_segment, closest_on_triangle] =
            geode::segment_triangle_distance(
                geode::Segment3D{ p0, p1 }, triangle );
        geode::OpenGeodeGeometryException::test(
            std::fabs( distance - 1. ) <= geode::GLOBAL_EPSILON
                && closest_on_triangle.inexact_equal(
                    geode::Point3D{ { 0.2, 0.2, 0.0 } } ),
            "Wrong result for segment_triangle_distance with degenerated "
            "segment" );
    }
}

void test_degenerated_triangle_triangle_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 0.0, 0.0, 1.0 } };
    const geode::Point3D d{ { 1.0, 0.0, 1.0 } };
    const geode::Point3D e{ { 0.0, 1.0, 1.0 } };
    const geode::Triangle3D degenerated{ a, b, b };
    const geode::Triangle3D triangle{ c, d, e };
    const auto result0 =
        geode::triangle_triangle_distance( degenerated, triangle );
    geode::OpenGeodeGeometryException::test(
        std::fabs( std::get< 0 >( result0 ) - 1. ) <= geode::GLOBAL_EPSILON,
        "Wrong triangle_triangle_distance with degenerated triangle" );
    const auto result1 =
        geode::triangle_triangle_distance( triangle, degenerated );
    geode::OpenGeodeGeometryException::test(
        std::fabs( std::get< 0 >( result1 ) - 1. ) <= geode::GLOBAL_EPSILON,
        "Wrong triangle_triangle_distance with degenerated triangle "
        "(reversed)" );
}

void test_random_triangle_triangle_distance()
{
    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ -1., 1. };
    const auto random_point = [&generator, &distribution] {
        return geode::Point3D{ { distribution( generator ),
            distribution( generator ), distribution( generator ) } };
    };
    for( const auto trial : geode::Range{ 500 } )
    {
        geode_unused( trial );
        const auto a = random_point();
        const auto b = random_point();
        const auto c = random_point();
        const geode::Vector3D shift{ { 0, 0, 1. } };
        const auto d = random_point() + shift;
        const auto e = random_point() + shift;
        const auto f = random_point() + shift;
        const geode::Triangle3D triangle0{ a, b, c };
        const geode::Triangle3D triangle1{ d, e, f };
        double reference{ std::numeric_limits< double >::max() };
        for( const auto& [base, other] :
            { std::make_pair( &triangle0, &triangle1 ),
                std::make_pair( &triangle1, &triangle0 ) } )
        {
            const auto& vertices = base->vertices();
            for( const auto v0 : geode::LRange{ 3 } )
            {
                for( const auto v1 : geode::LRange{ 3 } )
                {
                    if( v0 == v1 )
                    {
                        continue;
                    }
                    reference = std::min( reference,
                        std::get< 0 >( geode::segment_triangle_distance(
                            { vertices[v0], vertices[v1] }, *other ) ) );
                }
            }
        }
        const auto [distance, point0, point1] =
            geode::triangle_triangle_distance( triangle0, triangle1 );
        geode::OpenGeodeGeometryException::test(
            std::fabs( distance - reference ) <= geode::GLOBAL_EPSILON,
            "Wrong triangle_triangle_distance on random triangles: ", distance,
            " vs ", reference );
        geode::OpenGeodeGeometryException::test(
            std::fabs(
                geode::point_point_distance( point0, point1 ) - distance )
                <= geode::GLOBAL_EPSILON,
            "Wrong triangle_triangle_distance closest points" );
    }
}

void test_nan_point_triangle_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Triangle3D degenerated{ a, b, b };
    const auto nan = std::numeric_limits< double >::quiet_NaN();
    const geode::Point3D point{ { nan, nan, nan } };
    const auto [distance, closest] =
        geode::point_triangle_distance( point, degenerated );
    geode::OpenGeodeGeometryException::test( std::isnan( distance ),
        "Wrong point_triangle_distance with NaN point" );
    const auto [signed_distance, signed_closest] =
        geode::point_triangle_signed_distance( point, degenerated );
    geode::OpenGeodeGeometryException::test( std::isnan( signed_distance ),
        "Wrong point_triangle_signed_distance with NaN point" );
}

void test_degenerated_point_triangle_distance_2d()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 2.0, 0.0 } };
    const geode::Triangle2D flat{ a, b, c };
    const geode::Point2D q{ { 5.0, 0.0 } };
    const auto [distance, closest] = geode::point_triangle_distance( q, flat );
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance - 3. ) <= geode::GLOBAL_EPSILON
            && closest.inexact_equal( c ),
        "Wrong point_triangle_distance 2D with flat triangle: ", distance );
    const geode::Point2D on{ { 1.5, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        std::get< 0 >( geode::point_triangle_distance( on, flat ) )
            <= geode::GLOBAL_EPSILON,
        "Wrong point_triangle_distance 2D with flat triangle (on)" );

    const geode::Point2D p{ { 1.0, 1.0 } };
    const geode::Triangle2D point_triangle{ p, p, p };
    const geode::Point2D r{ { 7.0, 3.0 } };
    const auto [distance2, closest2] =
        geode::point_triangle_distance( r, point_triangle );
    geode::OpenGeodeGeometryException::test(
        std::fabs( distance2 - std::sqrt( 40. ) ) <= geode::GLOBAL_EPSILON
            && closest2.inexact_equal( p ),
        "Wrong point_triangle_distance 2D with point triangle: ", distance2 );
}

long double reference_distance( const geode::Point3D& point,
    const geode::Point3D& origin,
    const geode::Vector3D& direction,
    bool clamp )
{
    std::array< long double, 3 > diff;
    long double dot{ 0 };
    long double length2{ 0 };
    for( const auto d : geode::LRange{ 3 } )
    {
        diff[d] = static_cast< long double >( point.value( d ) )
                  - static_cast< long double >( origin.value( d ) );
        const auto dir = static_cast< long double >( direction.value( d ) );
        dot += diff[d] * dir;
        length2 += dir * dir;
    }
    auto t = dot / length2;
    if( clamp )
    {
        t = std::clamp( t, 0.0L, 1.0L );
    }
    long double distance2{ 0 };
    for( const auto d : geode::LRange{ 3 } )
    {
        const auto perp =
            diff[d] - t * static_cast< long double >( direction.value( d ) );
        distance2 += perp * perp;
    }
    return std::sqrt( distance2 );
}

void test_point_segment_line_distance_accuracy()
{
    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ -1., 1. };
    std::uniform_real_distribution< double > far_distribution{ -1e4, 1e4 };
    std::uniform_real_distribution< double > height_distribution{ 0, 1e-3 };
    const auto random_vector = [&generator, &distribution] {
        return geode::Vector3D{ { distribution( generator ),
            distribution( generator ), distribution( generator ) } };
    };
    for( const auto trial : geode::Range{ 1000 } )
    {
        geode_unused( trial );
        const geode::Point3D v0{ random_vector() };
        const auto edge = random_vector();
        const geode::Point3D v1 = v0 + edge;
        const geode::Segment3D segment{ v0, v1 };
        const geode::InfiniteLine3D line{ segment };
        const auto perpendicular = edge.cross( random_vector() ).normalize()
                                   * height_distribution( generator );

        const geode::Point3D far_point =
            v0 + edge * far_distribution( generator ) + perpendicular;
        const auto line_distance =
            geode::point_line_distance( far_point, line );
        const auto line_projection =
            geode::point_line_projection( far_point, line );
        const auto line_reference = reference_distance(
            far_point, line.origin(), line.direction(), false );
        geode::OpenGeodeGeometryException::test(
            std::fabs( geode::point_point_distance( far_point, line_projection )
                       - line_distance )
                <= geode::GLOBAL_EPSILON,
            "Inconsistent point_line_distance and point_line_projection" );
        geode::OpenGeodeGeometryException::test(
            std::fabs( line_distance - line_reference ) < geode::GLOBAL_EPSILON,
            "Inaccurate point_line_distance: ", line_distance, " vs ",
            static_cast< double >( line_reference ) );

        const geode::Point3D point =
            v0 + edge * ( 2 * distribution( generator ) + 0.5 ) + perpendicular;
        const auto segment_distance =
            geode::point_segment_distance( point, segment );
        const auto segment_projection =
            geode::point_segment_projection( point, segment );
        const auto segment_reference =
            reference_distance( point, v0, edge, true );
        geode::OpenGeodeGeometryException::test(
            std::fabs( geode::point_point_distance( point, segment_projection )
                       - segment_distance )
                <= geode::GLOBAL_EPSILON,
            "Inconsistent point_segment_distance and "
            "point_segment_projection" );
        geode::OpenGeodeGeometryException::test(
            std::fabs( segment_distance - segment_reference )
                < geode::GLOBAL_EPSILON,
            "Inaccurate point_segment_distance: ", segment_distance, " vs ",
            static_cast< double >( segment_reference ) );
    }

    const geode::Point3D a{ { 1, 1, 1 } };
    const geode::Point3D b{ { 1, 1, 1 + 0.5 * geode::GLOBAL_EPSILON } };
    const geode::Segment3D degenerated{ a, b };
    const geode::Point3D q{ { 2, 1, 1 } };
    geode::OpenGeodeGeometryException::test(
        geode::point_segment_distance( q, degenerated )
            == geode::point_point_distance(
                q, geode::point_segment_projection( q, degenerated ) ),
        "Inconsistent distance and projection for degenerated segment" );
}

void test_point_cylinder_distance()
{
    // Cylinder along x, from x=0 to x=4, radius 1
    const geode::Point3D axis_start{ { 0, 0, 0 } };
    const geode::Point3D axis_end{ { 4, 0, 0 } };
    const geode::Cylinder cylinder{ geode::Segment3D{ axis_start, axis_end },
        1 };
    const auto check = [&cylinder]( const geode::Point3D& point,
                           double expected_distance,
                           const geode::Point3D& expected_closest,
                           std::string_view label ) {
        const auto [distance, closest] =
            geode::point_cylinder_distance( point, cylinder );
        geode::OpenGeodeGeometryException::test(
            std::fabs( distance - expected_distance ) <= geode::GLOBAL_EPSILON
                && closest.inexact_equal( expected_closest ),
            "Wrong point_cylinder_distance: ", label );
    };
    check( geode::Point3D{ { 2, 3, 0 } }, 2, geode::Point3D{ { 2, 1, 0 } },
        "outside the wall" );
    check( geode::Point3D{ { 6, 0, 0.5 } }, 2, geode::Point3D{ { 4, 0, 0.5 } },
        "outside an end disk" );
    check( geode::Point3D{ { -3, 0, 5 } }, 5, geode::Point3D{ { 0, 0, 1 } },
        "outside the rim" );
    check( geode::Point3D{ { 2, 0, 0.75 } }, 0.25,
        geode::Point3D{ { 2, 0, 1 } }, "inside, close to the wall" );
    check( geode::Point3D{ { 3.75, 0.5, 0 } }, 0.25,
        geode::Point3D{ { 4, 0.5, 0 } }, "inside, close to an end disk" );
    check( geode::Point3D{ { 2, 1, 0 } }, 0, geode::Point3D{ { 2, 1, 0 } },
        "on the wall" );
    const auto [axis_distance, axis_closest] = geode::point_cylinder_distance(
        geode::Point3D{ { 2, 0, 0 } }, cylinder );
    geode::OpenGeodeGeometryException::test(
        std::fabs( axis_distance - 1 ) <= geode::GLOBAL_EPSILON
            && std::fabs( axis_closest.value( 0 ) - 2 ) <= geode::GLOBAL_EPSILON
            && std::get< 0 >(
                   geode::point_cylinder_distance( axis_closest, cylinder ) )
                   <= geode::GLOBAL_EPSILON,
        "Wrong point_cylinder_distance: on the axis" );
    void test_point_skewed_tetrahedron_distance()
    {
        const geode::Point3D a{ { 0, 0, 0 } };
        const geode::Point3D b{ { 4, 0.2, 0.1 } };
        const geode::Point3D c{ { 3.5, 1, 0.3 } };
        const geode::Point3D d{ { 0.5, 0.3, 0.8 } };
        const geode::Tetrahedron tetra{ a, b, c, d };
        const auto& vertices = tetra.vertices();
        std::mt19937 generator{ 42 };
        std::uniform_real_distribution< double > distribution{ -2, 6 };
        for( const auto unused : geode::Range{ 1000 } )
        {
            geode_unused( unused );
            const geode::Point3D query{ { distribution( generator ),
                distribution( generator ), distribution( generator ) } };
            const auto [distance, closest_point] =
                geode::point_tetrahedron_distance( query, tetra );
            if( distance == 0 )
            {
                geode::OpenGeodeGeometryException::test(
                    geode::point_tetrahedron_position( query, tetra )
                        != geode::POSITION::outside,
                    "Wrong null result for point_tetrahedron_distance with "
                    "query ",
                    query.string() );
                continue;
            }
            auto brute_force = std::numeric_limits< double >::max();
            for( const auto& facet :
                geode::Tetrahedron::tetrahedron_facet_vertex )
            {
                brute_force = std::min( brute_force,
                    std::get< 0 >( geode::point_triangle_distance( query,
                        geode::Triangle3D{ vertices[facet[0]],
                            vertices[facet[1]], vertices[facet[2]] } ) ) );
            }
            geode::OpenGeodeGeometryException::test(
                std::fabs( distance - brute_force ) < geode::GLOBAL_EPSILON,
                "Wrong result for point_tetrahedron_distance with query ",
                query.string(), ": ", distance, " | expected: ", brute_force );
            geode::OpenGeodeGeometryException::test(
                std::fabs( geode::point_point_distance( query, closest_point )
                           - distance )
                    < geode::GLOBAL_EPSILON,
                "Wrong closest point for point_tetrahedron_distance with "
                "query ",
                query.string() );
        }
    }

    void test()
    {
        test_point_segment_distance();
        test_segment_segment_distance();
        test_segment_line_distance();
        test_point_line_distance();
        test_point_triangle_distance();
        test_point_tetrahedron_distance();
        test_point_skewed_tetrahedron_distance();
        test_point_plane_distance();
        test_point_sphere_distance();
        test_point_circle_distance();
        test_point_circle_signed_distance_far_from_origin();
        test_line_triangle_distance();
        test_segment_triangle_distance();
        test_triangle_triangle_distance();
        test_point_ellipse_distance();
        test_point_cylinder_distance();
        test_degenerated_segment_triangle_distance();
        test_degenerated_triangle_triangle_distance();
        test_random_triangle_triangle_distance();
        test_nan_point_triangle_distance();
        test_degenerated_point_triangle_distance_2d();
        test_point_segment_line_distance_accuracy();
    }

    OPENGEODE_TEST( "distance" )
