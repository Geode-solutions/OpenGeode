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
#include <geode/geometry/point.hpp>

#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/ellipse.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/projection.hpp>

#include <geode/tests/common.hpp>

void test_point_segment_distance_2d()
{
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Segment2D segment2D{ a, b };

    double distance;

    distance = geode::point_segment_distance( a, segment2D );
    OPENGEODE_EXCEPTION(
        distance == 0 && geode::point_segment_projection( a, segment2D ) == a,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "a" );

    distance = geode::point_segment_distance( b, segment2D );
    OPENGEODE_EXCEPTION(
        distance == 0 && geode::point_segment_projection( b, segment2D ) == b,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "b" );

    const geode::Point2D q1{ { 0.0, 0.0 } };
    distance = geode::point_segment_distance( q1, segment2D );
    OPENGEODE_EXCEPTION(
        distance == 0 && geode::point_segment_projection( q1, segment2D ) == q1,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "q1" );

    const geode::Point2D q2{ { 10.0, 10.0 } };
    distance = geode::point_segment_distance( q2, segment2D );
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 106 )
            && geode::point_segment_projection( q2, segment2D ) == a,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "q2" );

    const geode::Point2D q3{ { 5.0, -1.0 } };
    distance = geode::point_segment_distance( q3, segment2D );
    const geode::Point2D result_q3{ { 0.0, 0.0 } };
    OPENGEODE_EXCEPTION(
        std::fabs( std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_segment_projection( q3, segment2D ) == result_q3,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "q3" );

    const geode::Point2D q4{ { 5.5, 1.5 } };
    distance = geode::point_segment_distance( q4, segment2D );
    const geode::Point2D result_q4{ { 0.5, 2.5 } };
    OPENGEODE_EXCEPTION(
        std::fabs( std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_segment_projection( q4, segment2D ) == result_q4,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "q4" );
}

void test_point_segment_distance_3d()
{
    const geode::Point3D a{ { 1.0, 5.0, 2.0 } };
    const geode::Point3D b{ { -1.0, -5.0, -2.0 } };
    const geode::Segment3D segment3D{ a, b };

    double distance;

    distance = geode::point_segment_distance( a, segment3D );
    OPENGEODE_EXCEPTION( distance < geode::GLOBAL_EPSILON
                             && geode::point_segment_projection( a, segment3D )
                                 .inexact_equal( a ),
        "[Test] Wrong result for point_segment_distance with query Point3D "
        "a" );

    distance = geode::point_segment_distance( b, segment3D );
    OPENGEODE_EXCEPTION( distance < geode::GLOBAL_EPSILON
                             && geode::point_segment_projection( b, segment3D )
                                 .inexact_equal( b ),
        "[Test] Wrong result for point_segment_distance with query Point3D "
        "b" );

    const geode::Point3D q1{ { 0.0, 0.0, 0.0 } };
    distance = geode::point_segment_distance( q1, segment3D );
    OPENGEODE_EXCEPTION( distance < geode::GLOBAL_EPSILON
                             && geode::point_segment_projection( q1, segment3D )
                                 .inexact_equal( q1 ),
        "[Test] Wrong result for point_segment_distance with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 10.0, 10.0, 10.0 } };
    distance = geode::point_segment_distance( q2, segment3D );
    OPENGEODE_EXCEPTION( distance == std::sqrt( 170 )
                             && geode::point_segment_projection( q2, segment3D )
                                 .inexact_equal( a ),
        "[Test] Wrong result for point_segment_distance with query Point3D "
        "q2" );

    const geode::Point3D q3{ { 5.0, -1.0, 0.0 } };
    distance = geode::point_segment_distance( q3, segment3D );
    const geode::Point3D result_q3{ { 0.0, 0.0, 0.0 } };
    OPENGEODE_EXCEPTION( distance == std::sqrt( 26 )
                             && geode::point_segment_projection( q3, segment3D )
                                 .inexact_equal( result_q3 ),
        "[Test] Wrong result for point_segment_distance with query Point3D "
        "q3" );

    const geode::Point3D q4{ { 5.5, 1.5, 0.25 } };
    distance = geode::point_segment_distance( q4, segment3D );
    const geode::Point3D result_q4{ { 0.45, 2.25, 0.9 } };
    OPENGEODE_EXCEPTION(
        std::fabs(
            std::sqrt( 5.05 * 5.05 + 0.75 * 0.75 + 0.65 * 0.65 ) - distance )
                < geode::GLOBAL_EPSILON
            && geode::point_segment_projection( q4, segment3D )
                .inexact_equal( result_q4 ),
        "[Test] Wrong result for point_segment_distance with query Point3D "
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
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == a,
        "[Test] Wrong result for point_triangle_distance with query point "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( b, triangle2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == b,
        "[Test] Wrong result for point_triangle_distance with query point "
        "b" );

    const geode::Point2D q1{ { 0.5, 0.5 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q1, triangle2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == q1,
        "[Test] Wrong result for point_triangle_distance with query point "
        "q1" );

    const geode::Point2D q2{ { 0.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q2, triangle2D );
    const geode::Point2D result_q2{ { 0.5, 0.5 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 2 ) / 2. && closest_point == result_q2,
        "[Test] Wrong result for point_triangle_distance with query point "
        "q2" );

    const geode::Point2D q3{ { 2.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q3, triangle2D );
    OPENGEODE_EXCEPTION( distance == 1 && closest_point == c,
        "[Test] Wrong result for point_triangle_distance with query point "
        "q3" );

    const geode::Point2D q4{ { 0.8, 0.2 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q4, triangle2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == q4,
        "[Test] Wrong result for point_triangle_distance with query point "
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
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == a,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( b, triangle3D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == b,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "b" );

    const geode::Point3D q1{ { 0.5, 0.5, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q1, triangle3D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == q1,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 0.0, 1.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q2, triangle3D );
    const geode::Point3D result_q2{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 2 ) / 2. && closest_point == result_q2,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q2" );

    const geode::Point3D q3{ { 2.0, 1.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q3, triangle3D );
    OPENGEODE_EXCEPTION( distance == 1 && closest_point == c,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q3" );

    const geode::Point3D q4{ { 0.5, 0.5, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q4, triangle3D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == q4,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q4" );

    const geode::Point3D q5{ { 2.0, -2.0, -1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q5, triangle3D );
    OPENGEODE_EXCEPTION( distance == std::sqrt( 6 ) && closest_point == b,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q5" );

    const geode::Point3D q6{ { -1.0, -0.5, -1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q6, triangle3D );
    OPENGEODE_EXCEPTION( distance == std::sqrt( 2.25 ) && closest_point == a,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q6" );

    const geode::Point3D q7{ { 2.0, 3.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q7, triangle3D );
    OPENGEODE_EXCEPTION( distance == std::sqrt( 6 ) && closest_point == c,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q7" );

    const geode::Point3D q8{ { 0.5, -3.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q8, triangle3D );
    const geode::Point3D result_q8{ { 0.5, 0.0, 0.0 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 10 ) && closest_point == result_q8,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q8" );

    const geode::Point3D q9{ { 0.5, -0.2, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q9, triangle3D );
    const geode::Point3D result_q9{ { 0.5, 0.0, 0.0 } };
    OPENGEODE_EXCEPTION( std::fabs( distance - 0.2 ) < geode::GLOBAL_EPSILON
                             && closest_point == result_q9,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q9" );

    const geode::Point3D q10{ { 0.75, 0.25, 10.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q10, triangle3D );
    const geode::Point3D result_q10{ { 0.75, 0.25, 0.0 } };
    OPENGEODE_EXCEPTION( std::fabs( distance - 10 ) < geode::GLOBAL_EPSILON
                             && closest_point == result_q10,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q10" );

    const geode::Point3D q11{ { 2.0, 0.5, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q11, triangle3D );
    const geode::Point3D result_q11{ { 1.0, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION( std::fabs( distance - 1 ) < geode::GLOBAL_EPSILON
                             && closest_point == result_q11,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q11" );

    const geode::Point3D q12{ { -1.0, 2.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q12, triangle3D );
    const geode::Point3D result_q12{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 1.5 * 1.5 * 2 ) && closest_point == result_q12,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q12" );

    const geode::Point3D q13{ { -2.0, 1.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q13, triangle3D );
    OPENGEODE_EXCEPTION(
        std::fabs( distance - std::sqrt( 5 ) ) < geode::GLOBAL_EPSILON
            && closest_point == a,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q13" );

    const geode::Point3D q14{ { 0.5, 2.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q14, triangle3D );
    OPENGEODE_EXCEPTION(
        std::fabs( distance - std::sqrt( 1.25 ) ) < geode::GLOBAL_EPSILON
            && closest_point == c,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
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
    OPENGEODE_EXCEPTION( distance == 0. && closest_point0 == result_t00
                             && closest_point1 == result_t01,
        "[Test] Wrong result for segment_segment_distance 2D with segment_ab "
        "and segment_dc" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance( segment_ab, segment_ab );
    const geode::Point2D result_t10{ { 0.0, 0.0 } };
    const geode::Point2D result_t11{ { 0.0, 0.0 } };
    OPENGEODE_EXCEPTION( distance == 0. && closest_point0 == result_t10
                             && closest_point1 == result_t11,
        "[Test] Wrong result for segment_segment_distance 2D with segment_ab "
        "and segment_ab" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance( segment_dc, segment_gh );
    OPENGEODE_EXCEPTION(
        distance == 1. && closest_point0 == d && closest_point1 == g,
        "[Test] Wrong result for segment_segment_distance 2D with segment_dc "
        "and segment_gh" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance< 2 >( segment_dc, owner_segment_gh );
    OPENGEODE_EXCEPTION(
        distance == 1. && closest_point0 == d && closest_point1 == g,
        "[Test] Wrong result for segment_segment_distance 2D with segment_dc "
        "and owner_segment_gh" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance< 2 >(
            owner_segment_dc, owner_segment_gh );
    OPENGEODE_EXCEPTION(
        distance == 1. && closest_point0 == d && closest_point1 == g,
        "[Test] Wrong result for segment_segment_distance 2D with "
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
    OPENGEODE_EXCEPTION( distance == 1.
                             && closest_point0.inexact_equal( result_t00 )
                             && closest_point1.inexact_equal( result_t01 ),
        "[Test] Wrong result for segment_segment_distance with segment_ab and "
        "segment_cd" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance( segment_ab, segment_ef );
    const geode::Point3D result_t10{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D result_t11{ { 0.0, 0.0, 1.0 } };
    OPENGEODE_EXCEPTION( std::fabs( distance - 1 ) <= geode::GLOBAL_EPSILON
                             && closest_point0.inexact_equal( result_t10 )
                             && closest_point1.inexact_equal( result_t11 ),
        "[Test] Wrong result for segment_segment_distance with segment_ab and "
        "segment_ef" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_segment_distance( segment_cd, segment_gh );
    OPENGEODE_EXCEPTION(
        distance == 1. && closest_point0 == d && closest_point1 == g,
        "[Test] Wrong result for segment_segment_distance with segment_cd and "
        "segment_gh" );
}

void test_segment_segment_distance()
{
    test_segment_segment_distance_2d();
    test_segment_segment_distance_3d();
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
    OPENGEODE_EXCEPTION( distance == 1.
                             && closest_point0.inexact_equal( result_t00 )
                             && closest_point1.inexact_equal( result_t01 ),
        "[Test] Wrong result for segment_line_distance with segment_ab and "
        "line_cd" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_line_distance( segment_ab, line_ef );
    const geode::Point3D result_t10{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D result_t11{ { 0.0, 0.0, 1.0 } };
    OPENGEODE_EXCEPTION( std::fabs( distance - 1 ) <= geode::GLOBAL_EPSILON
                             && closest_point0.inexact_equal( result_t10 )
                             && closest_point1.inexact_equal( result_t11 ),
        "[Test] Wrong result for segment_line_distance with segment_ab and "
        "line_ef" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_line_distance( segment_cd, line_gh );
    const geode::Point3D result_t21{ { 0.5, 2.5, 1.0 } };
    OPENGEODE_EXCEPTION( distance == sqrt( 2 ) / 2. && closest_point0 == c
                             && closest_point1.inexact_equal( result_t21 ),
        "[Test] Wrong result for segment_line_distance with segment_cd and "
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
    OPENGEODE_EXCEPTION( distance == 1.0 && closest_point == answer,
        "[Test] Wrong result for point_plane_distance with query point "
        "q1" );
    std::tie( distance, closest_point ) =
        geode::point_plane_signed_distance( q1, plane );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION( distance == 1.0 && closest_point == answer,
        "[Test] Wrong result for point_plane_signed_distance with query point "
        "q1" );

    const geode::Point3D q2{ { 0.5, 0.5, -1 } };
    std::tie( distance, closest_point ) =
        geode::point_plane_distance( q2, plane );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION( distance == 1.0 && closest_point == answer,
        "[Test] Wrong result for point_plane_distance with query point "
        "q2" );
    std::tie( distance, closest_point ) =
        geode::point_plane_signed_distance( q2, plane );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION( distance == -1.0 && closest_point == answer,
        "[Test] Wrong result for point_plane_signed_distance with query point "
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
    OPENGEODE_EXCEPTION(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( a, line2D ).inexact_equal( a ),
        "[Test] Wrong result for point_line_distance with query Point2D "
        "a" );

    distance = geode::point_line_distance( b, line2D );
    OPENGEODE_EXCEPTION(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( b, line2D ).inexact_equal( b ),
        "[Test] Wrong result for point_line_distance with query Point2D "
        "b" );

    const geode::Point2D q1{ { 0.0, 0.0 } };
    distance = geode::point_line_distance( q1, line2D );
    OPENGEODE_EXCEPTION(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q1, line2D ).inexact_equal( q1 ),
        "[Test] Wrong result for point_line_distance with query Point2D "
        "q1" );

    const geode::Point2D q2{ { 5.0, -1.0 } };
    distance = geode::point_line_distance( q2, line2D );
    const geode::Point2D result_q2{ { 0.0, 0.0 } };
    OPENGEODE_EXCEPTION(
        std::fabs( std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q2, line2D )
                .inexact_equal( result_q2 ),
        "[Test] Wrong result for point_line_distance with query Point2D "
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
    OPENGEODE_EXCEPTION(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( a, line2D ).inexact_equal( a ),
        "[Test] Wrong result for point_line_signed_distance with query Point2D "
        "a" );

    distance = geode::point_line_signed_distance( b, line2D );
    OPENGEODE_EXCEPTION(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( b, line2D ).inexact_equal( b ),
        "[Test] Wrong result for point_line_signed_distance with query Point2D "
        "b" );

    const geode::Point2D q1{ { 0.0, 0.0 } };
    distance = geode::point_line_signed_distance( q1, line2D );
    OPENGEODE_EXCEPTION(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q1, line2D ).inexact_equal( q1 ),
        "[Test] Wrong result for point_line_signed_distance with query Point2D "
        "q1" );

    const geode::Point2D q2{ { 2.0, 10.0 } };
    distance = geode::point_line_signed_distance( q2, line2D );
    OPENGEODE_EXCEPTION(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q2, line2D ).inexact_equal( q2 ),
        "[Test] Wrong result for point_line_signed_distance with query Point2D "
        "q2" );

    const geode::Point2D q3{ { 5.0, -1.0 } };
    distance = geode::point_line_signed_distance( q3, line2D );
    const geode::Point2D result_q3{ { 0.0, 0.0 } };
    OPENGEODE_EXCEPTION(
        std::fabs( std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q3, line2D )
                .inexact_equal( result_q3 ),
        "[Test] Wrong result for point_line_signed_distance with query Point2D "
        "q3" );

    const geode::Point2D q4{ { -5.0, 1.0 } };
    distance = geode::point_line_signed_distance( q4, line2D );
    const geode::Point2D result_q4{ { 0.0, 0.0 } };
    OPENGEODE_EXCEPTION(
        std::fabs( -std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q4, line2D )
                .inexact_equal( result_q3 ),
        "[Test] Wrong result for point_line_signed_distance with query Point2D "
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
    OPENGEODE_EXCEPTION(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( a, line3D ).inexact_equal( a ),
        "[Test] Wrong result for point_line_distance with query Point3D "
        "a" );

    distance = geode::point_line_distance( b, line3D );
    OPENGEODE_EXCEPTION(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( b, line3D ).inexact_equal( b ),
        "[Test] Wrong result for point_line_distance with query Point3D "
        "b" );

    const geode::Point3D q1{ { 0.0, 0.0, 0.0 } };
    distance = geode::point_line_distance( q1, line3D );
    OPENGEODE_EXCEPTION(
        distance < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q1, line3D ).inexact_equal( q1 ),
        "[Test] Wrong result for point_line_distance with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 5.0, -1.0, 0.0 } };
    distance = geode::point_line_distance( q2, line3D );
    const geode::Point3D result_q2{ { 0.0, 0.0, 0.0 } };
    OPENGEODE_EXCEPTION(
        std::fabs( std::sqrt( 26 ) - distance ) < geode::GLOBAL_EPSILON
            && geode::point_line_projection( q2, line3D )
                .inexact_equal( result_q2 ),
        "[Test] Wrong result for point_line_distance with query Point3D "
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
        OPENGEODE_EXCEPTION( distance == 0 && closest_point == v0,
            "[Test] Wrong result for point_tetrahedron_distance with query "
            "Point3D v0" );

        const auto& v1 = tetra.vertices()[1];
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( v1, tetra );
        OPENGEODE_EXCEPTION( distance == 0 && closest_point == v1,
            "[Test] Wrong result for point_tetrahedron_distance with query "
            "Point3D v1" );

        const auto& v2 = tetra.vertices()[2];
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( v2, tetra );
        OPENGEODE_EXCEPTION( distance == 0 && closest_point == v2,
            "[Test] Wrong result for point_tetrahedron_distance with query "
            "Point3D v2" );

        const auto& v3 = tetra.vertices()[3];
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( v3, tetra );
        OPENGEODE_EXCEPTION( distance == 0 && closest_point == v3,
            "[Test] Wrong result for point_tetrahedron_distance with query "
            "Point3D v3" );

        const geode::Point3D q1{ { 0.25, 0.25, 0.25 } };
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( q1, tetra );
        OPENGEODE_EXCEPTION( distance == 0 && closest_point == q1,
            "[Test] Wrong result for point_tetrahedron_distance with query "
            "Point3D q1" );

        const geode::Point3D q2{ { 0.25, 0.25, 0.0 } };
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( q2, tetra );
        OPENGEODE_EXCEPTION( distance == 0 && closest_point == q2,
            "[Test] Wrong result for point_tetrahedron_distance with query "
            "Point3D q2" );

        const geode::Point3D q3{ { 2.5, 2.5, 0.0 } };
        std::tie( distance, closest_point ) =
            geode::point_tetrahedron_distance( q3, tetra );
        answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
        OPENGEODE_EXCEPTION(
            distance == std::sqrt( 8 ) && closest_point.inexact_equal( answer ),
            "[Test] Wrong result for point_tetrahedron_distance with query "
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
    OPENGEODE_EXCEPTION( distance == 0 && closest_point.inexact_equal( q1 ),
        "[Test] Wrong result for point_sphere_distance with query Point3D "
        "q1" );

    std::tie( distance, closest_point ) =
        geode::point_sphere_signed_distance( q1, sphere );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point.inexact_equal( q1 ),
        "[Test] Wrong result for point_sphere_signed_distance with query "
        "Point3D q1" );

    std::tie( distance, closest_point ) =
        geode::point_ball_distance( q1, sphere );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point.inexact_equal( q1 ),
        "[Test] Wrong result for point_ball_distance with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 0.0, 3.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_sphere_distance( q2, sphere );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 1 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_sphere_distance with query Point3D "
        "q2" );

    std::tie( distance, closest_point ) =
        geode::point_sphere_signed_distance( q2, sphere );
    OPENGEODE_EXCEPTION( distance == 1 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_sphere_signed_distance with query "
        "Point3D q2" );

    std::tie( distance, closest_point ) =
        geode::point_ball_distance( q2, sphere );
    OPENGEODE_EXCEPTION( distance == 1 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_ball_distance with query Point3D "
        "q2" );

    const geode::Point3D q3{ { 0.0, 1.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_sphere_distance( q3, sphere );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 1 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_sphere_distance with query Point3D "
        "q3" );

    std::tie( distance, closest_point ) =
        geode::point_sphere_signed_distance( q3, sphere );
    OPENGEODE_EXCEPTION(
        distance == -1 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_sphere_signed_distance with query "
        "Point3D q3" );

    std::tie( distance, closest_point ) =
        geode::point_ball_distance( q3, sphere );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point.inexact_equal( q3 ),
        "[Test] Wrong result for point_ball_distance with query Point3D "
        "q3" );

    std::tie( distance, closest_point ) =
        geode::point_sphere_distance( a, sphere );
    answer = geode::Point3D{ { 2.0, 0.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 2 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_sphere_distance with query Point3D "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_sphere_signed_distance( a, sphere );
    OPENGEODE_EXCEPTION(
        distance == -2 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_sphere_signed_distance with query "
        "Point3D a" );

    std::tie( distance, closest_point ) =
        geode::point_ball_distance( a, sphere );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point.inexact_equal( a ),
        "[Test] Wrong result for point_ball_distance with query "
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
    OPENGEODE_EXCEPTION(
        distance == sqrt( 8 ) && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_circle_distance with query Point3D "
        "q1 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_circle_distance( q1, circle_z );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point.inexact_equal( q1 ),
        "[Test] Wrong result for point_circle_distance with query Point3D "
        "q1 and circle_z" );

    std::tie( distance, closest_point ) =
        geode::point_circle_signed_distance( q1, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    OPENGEODE_EXCEPTION(
        distance == -sqrt( 8 ) && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_circle_signed_distance with query "
        "Point3D "
        "q1 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_circle_signed_distance( q1, circle_z );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point.inexact_equal( q1 ),
        "[Test] Wrong result for point_circle_signed_distance with query "
        "Point3D "
        "q1 and circle_z" );

    std::tie( distance, closest_point ) =
        geode::point_disk_distance( q1, circle_x );
    answer = geode::Point3D{ { 0.0, 0.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 2 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_disk_distance with query Point3D "
        "q1 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_disk_distance( q1, circle_z );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point.inexact_equal( q1 ),
        "[Test] Wrong result for point_disk_distance with query Point3D "
        "q1 and circle_z" );

    const geode::Point3D q2{ { 0.0, 3.0, 1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_circle_distance( q2, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 1 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_circle_distance with query Point3D "
        "q2 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_circle_signed_distance( q2, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 1 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_circle_signed_distance with query "
        "Point3D "
        "q2 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_disk_distance( q2, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 1 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_disk_distance with query Point3D "
        "q2 and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_circle_distance( a, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 2 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_circle_distance with query Point3D "
        "a and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_circle_signed_distance( a, circle_x );
    answer = geode::Point3D{ { 0.0, 2.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 2 && closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_circle_signed_distance with query "
        "Point3D "
        "a and circle_x" );

    std::tie( distance, closest_point ) =
        geode::point_disk_distance( a, circle_x );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point.inexact_equal( a ),
        "[Test] Wrong result for point_disk_distance with query Point3D "
        "a and circle_x" );
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
    OPENGEODE_EXCEPTION( distance == 1.
                             && closest_point0.inexact_equal( result_t00 )
                             && closest_point1.inexact_equal( result_t01 ),
        "[Test] Wrong result for line_triangle_distance with line_ab" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::line_triangle_distance( line_ef, cdg );
    const geode::Point3D result_t10{ { 1.0, 1.0, 1.0 } };
    const geode::Point3D result_t11{ { 1.0, 1.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 0.
                             && closest_point0.inexact_equal( result_t10 )
                             && closest_point1.inexact_equal( result_t11 ),
        "[Test] Wrong result for line_triangle_distance with line_ef" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::line_triangle_distance( line_bz, cdg );
    OPENGEODE_EXCEPTION( distance == 0. && closest_point0.inexact_equal( f )
                             && closest_point1.inexact_equal( f ),
        "[Test] Wrong result for line_triangle_distance with line_bz" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::line_triangle_distance( line_hx, cdg );
    const geode::Point3D result_t30{ { 3.0, 5.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 2.
                             && closest_point0.inexact_equal( result_t30 )
                             && closest_point1.inexact_equal( g ),
        "[Test] Wrong result for line_triangle_distance with line_hx" );
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

    double distance;
    geode::Point3D closest_point0;
    geode::Point3D closest_point1;

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_triangle_distance( seg_ab, cdg );
    const geode::Point3D result_t00{ { 1.0, 1.0, 0.0 } };
    const geode::Point3D result_t01{ { 1.0, 1.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 1.
                             && closest_point0.inexact_equal( result_t00 )
                             && closest_point1.inexact_equal( result_t01 ),
        "[Test] Wrong result for segment_triangle_distance with seg_ab" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_triangle_distance( seg_ef, cdg );
    const geode::Point3D result_t10{ { 1.0, 1.0, 1.0 } };
    const geode::Point3D result_t11{ { 1.0, 1.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 0.
                             && closest_point0.inexact_equal( result_t10 )
                             && closest_point1.inexact_equal( result_t11 ),
        "[Test] Wrong result for segment_triangle_distance with seg_ef" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_triangle_distance( seg_bz0, cdg );
    OPENGEODE_EXCEPTION( distance == 0. && closest_point0.inexact_equal( f )
                             && closest_point1.inexact_equal( f ),
        "[Test] Wrong result for segment_triangle_distance with seg_bz0" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_triangle_distance( seg_bz1, cdg );
    OPENGEODE_EXCEPTION( distance == 1. && closest_point0.inexact_equal( b )
                             && closest_point1.inexact_equal( f ),
        "[Test] Wrong result for segment_triangle_distance with seg_bz1" );

    std::tie( distance, closest_point0, closest_point1 ) =
        geode::segment_triangle_distance( seg_hx, cdg );
    const geode::Point3D result_t30{ { 3.0, 5.0, 1.0 } };
    OPENGEODE_EXCEPTION( distance == 2.
                             && closest_point0.inexact_equal( result_t30 )
                             && closest_point1.inexact_equal( g ),
        "[Test] Wrong result for segment_triangle_distance with seg_hx" );
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
    OPENGEODE_EXCEPTION(
        on_boundary_distance == 0. && on_boundary_closest_point == on_boundary,
        "[Test] Wrong result for point_ellipse_distance_2d with on_boundary "
        "point" );
    const auto [outside1_distance, outside1_closest_point] =
        geode::point_ellipse_distance( outside1, ellipse );
    const geode::Point2D result_outside1{ { 3, 0 } };
    OPENGEODE_EXCEPTION(
        outside1_distance == 1
            && outside1_closest_point.inexact_equal( result_outside1 ),
        "[Test] Wrong result for point_ellipse_distance_2d with outside1 "
        "point" );
    const auto [outside2_distance, outside2_closest_point] =
        geode::point_ellipse_distance( outside2, ellipse );
    const geode::Point2D result_outside2{ { 0, 2 } };
    OPENGEODE_EXCEPTION(
        outside2_distance == 3
            && outside2_closest_point.inexact_equal( result_outside2 ),
        "[Test] Wrong result for point_ellipse_distance_2d with outside2 "
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
    OPENGEODE_EXCEPTION(
        std::fabs( std::sqrt( 2 ) - distance ) < geode::GLOBAL_EPSILON
            && closest_point.inexact_equal( result ),
        "[Test] Wrong result for point_ellipse_distance_2d_not_aligned" );
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
    OPENGEODE_EXCEPTION(
        on_boundary_distance == 0. && on_boundary_closest_point == on_boundary,
        "[Test] Wrong result for point_ellipse_distance_3d with "
        "on_boundary "
        "point" );
    const auto [outside1_distance, outside1_closest_point] =
        geode::point_ellipse_distance( outside1, ellipse );
    const geode::Point3D result_outside1{ { 3, 0, 0 } };
    OPENGEODE_EXCEPTION(
        outside1_distance == 1
            && outside1_closest_point.inexact_equal( result_outside1 ),
        "[Test] Wrong result for point_ellipse_distance_3d with outside1 "
        "point" );
    const auto [outside2_distance, outside2_closest_point] =
        geode::point_ellipse_distance( outside2, ellipse );
    const geode::Point3D result_outside2{ { 0, 2, 0 } };
    OPENGEODE_EXCEPTION(
        outside2_distance == 3
            && outside2_closest_point.inexact_equal( result_outside2 ),
        "[Test] Wrong result for point_ellipse_distance_3d with outside2 "
        "point" );
    const auto [outside3_distance, outside3_closest_point] =
        geode::point_ellipse_distance( outside3, ellipse );
    const geode::Point3D result_outside3{ { 0, 0, 1 } };
    OPENGEODE_EXCEPTION(
        outside3_distance == 2
            && outside3_closest_point.inexact_equal( result_outside3 ),
        "[Test] Wrong result for point_ellipse_distance_3d with outside3 "
        "point" );
    const auto [inside_distance, inside_closest_point] =
        geode::point_ellipse_distance( center, ellipse );
    const geode::Point3D result_inside{ { 0, 0, 1 } };
    OPENGEODE_EXCEPTION(
        inside_distance == 1
            && inside_closest_point.inexact_equal( result_inside ),
        "[Test] Wrong result for point_ellipse_distance_3d with inside "
        "point" );
}

void test_point_ellipse_distance()
{
    test_point_ellipse_distance_2d();
    test_point_ellipse_distance_3d();
    test_point_ellipse_distance_2d_not_aligned();
}

void test()
{
    // test_point_segment_distance();
    // test_segment_segment_distance();
    // test_segment_line_distance();
    // test_point_line_distance();
    // test_point_triangle_distance();
    // test_point_tetrahedron_distance();
    // test_point_plane_distance();
    // test_point_sphere_distance();
    // test_point_circle_distance();
    // test_line_triangle_distance();
    // test_segment_triangle_distance();
    test_point_ellipse_distance();
}

OPENGEODE_TEST( "distance" )
