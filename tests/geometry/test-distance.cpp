/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/geometry/basic_objects.h>
#include <geode/geometry/distance.h>

#include <geode/tests/common.h>

void test_point_segment_distance_2d()
{
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Segment2D segment2D{ a, b };

    double distance;
    geode::Point2D closest_point;

    std::tie( distance, closest_point ) =
        geode::point_segment_distance( a, segment2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == a,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_segment_distance( b, segment2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == b,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "b" );

    const geode::Point2D q1{ { 0.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q1, segment2D );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == q1,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "q1" );

    const geode::Point2D q2{ { 10.0, 10.0 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q2, segment2D );
    OPENGEODE_EXCEPTION( distance == std::sqrt( 106 ) && closest_point == a,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "q2" );

    const geode::Point2D q3{ { 5.0, -1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q3, segment2D );
    const geode::Point2D result_q3{ { 0.0, 0.0 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 26 ) && closest_point == result_q3,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "q3" );

    const geode::Point2D q4{ { 5.5, 1.5 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q4, segment2D );
    const geode::Point2D result_q4{ { 0.5, 2.5 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 26 ) && closest_point == result_q4,
        "[Test] Wrong result for point_segment_distance with query Point2D "
        "q4" );
}

void test_point_segment_distance_3d()
{
    const geode::Point3D a{ { 1.0, 5.0, 2.0 } };
    const geode::Point3D b{ { -1.0, -5.0, -2.0 } };
    const geode::Segment3D segment3D{ a, b };

    double distance;
    geode::Point3D closest_point;

    std::tie( distance, closest_point ) =
        geode::point_segment_distance( a, segment3D );
    OPENGEODE_EXCEPTION(
        distance < geode::global_epsilon
            && closest_point.inexact_equal( a, geode::global_epsilon ),
        "[Test] Wrong result for point_segment_distance with query Point3D "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_segment_distance( b, segment3D );
    OPENGEODE_EXCEPTION(
        distance < geode::global_epsilon
            && closest_point.inexact_equal( b, geode::global_epsilon ),
        "[Test] Wrong result for point_segment_distance with query Point3D "
        "b" );

    const geode::Point3D q1{ { 0.0, 0.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q1, segment3D );
    OPENGEODE_EXCEPTION(
        distance < geode::global_epsilon
            && closest_point.inexact_equal( q1, geode::global_epsilon ),
        "[Test] Wrong result for point_segment_distance with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 10.0, 10.0, 10.0 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q2, segment3D );
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 170 )
            && closest_point.inexact_equal( a, geode::global_epsilon ),
        "[Test] Wrong result for point_segment_distance with query Point3D "
        "q2" );

    const geode::Point3D q3{ { 5.0, -1.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q3, segment3D );
    const geode::Point3D result_q3{ { 0.0, 0.0, 0.0 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 26 )
            && closest_point.inexact_equal( result_q3, geode::global_epsilon ),
        "[Test] Wrong result for point_segment_distance with query Point3D "
        "q3" );

    const geode::Point3D q4{ { 5.5, 1.5, 0.25 } };
    std::tie( distance, closest_point ) =
        geode::point_segment_distance( q4, segment3D );
    const geode::Point3D result_q4{ { 0.45, 2.25, 0.9 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 5.05 * 5.05 + 0.75 * 0.75 + 0.65 * 0.65 )
            && closest_point.inexact_equal( result_q4, geode::global_epsilon ),
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

    const geode::Point2D q4{ { 0.5, 0.5 } };
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
    OPENGEODE_EXCEPTION( std::fabs( distance - 0.2 ) < geode::global_epsilon
                             && closest_point == result_q9,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q9" );

    const geode::Point3D q10{ { 0.75, 0.25, 10.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q10, triangle3D );
    SDEBUG( closest_point );
    DEBUG( distance );
    const geode::Point3D result_q10{ { 0.75, 0.25, 0.0 } };
    OPENGEODE_EXCEPTION( std::fabs( distance - 10 ) < geode::global_epsilon
                             && closest_point == result_q10,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q10" );

    const geode::Point3D q11{ { 2.0, 0.5, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q11, triangle3D );
    SDEBUG( closest_point );
    DEBUG( distance );
    const geode::Point3D result_q11{ { 1.0, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION( std::fabs( distance - 1 ) < geode::global_epsilon
                             && closest_point == result_q11,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q11" );

    DEBUG( "======" );
    const geode::Point3D q12{ { -1.0, 2.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q12, triangle3D );
    SDEBUG( closest_point );
    DEBUG( distance );
    DEBUG( std::sqrt( 1.5 * 1.5 * 2 ) );
    const geode::Point3D result_q12{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 1.5 * 1.5 * 2 ) && closest_point == result_q12,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q12" );

    DEBUG( "======" );
    const geode::Point3D q13{ { -2.0, 1.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q13, triangle3D );
    SDEBUG( closest_point );
    DEBUG( distance );
    // const geode::Point3D result_q12{ { 1.0, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION(
        std::fabs( distance - std::sqrt( 5 ) ) < geode::global_epsilon
            && closest_point == a,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q13" );

    DEBUG( "=====" );
    const geode::Point3D q14{ { 0.5, 2.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_triangle_distance( q14, triangle3D );
    SDEBUG( closest_point );
    DEBUG( distance );
    // const geode::Point3D result_q12{ { 1.0, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION(
        std::fabs( distance - std::sqrt( 1.25 ) ) < geode::global_epsilon
            && closest_point == c,
        "[Test] Wrong result for point_triangle_distance with query Point3D "
        "q13" );

    // exit( 1 );
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
    const auto plane = triangle.plane();

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
    geode::Point2D closest_point;

    std::tie( distance, closest_point ) =
        geode::point_line_distance( a, line2D );
    OPENGEODE_EXCEPTION(
        distance < geode::global_epsilon
            && closest_point.inexact_equal( a, geode::global_epsilon ),
        "[Test] Wrong result for point_line_distance with query Point2D "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_line_distance( b, line2D );
    OPENGEODE_EXCEPTION(
        distance < geode::global_epsilon
            && closest_point.inexact_equal( b, geode::global_epsilon ),
        "[Test] Wrong result for point_line_distance with query Point2D "
        "b" );

    const geode::Point2D q1{ { 0.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_line_distance( q1, line2D );
    OPENGEODE_EXCEPTION(
        distance < geode::global_epsilon
            && closest_point.inexact_equal( q1, geode::global_epsilon ),
        "[Test] Wrong result for point_line_distance with query Point2D "
        "q1" );

    const geode::Point2D q2{ { 5.0, -1.0 } };
    std::tie( distance, closest_point ) =
        geode::point_line_distance( q2, line2D );
    const geode::Point2D result_q2{ { 0.0, 0.0 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 26 )
            && closest_point.inexact_equal( result_q2, geode::global_epsilon ),
        "[Test] Wrong result for point_line_distance with query Point2D "
        "q2" );
}

void test_point_line_distance_3d()
{
    const geode::Point3D a{ { 1.0, 5.0, 2.0 } };
    const geode::Point3D b{ { -1.0, -5.0, -2.0 } };
    const geode::Segment3D segment3D{ a, b };
    const geode::InfiniteLine3D line3D{ segment3D };

    double distance;
    geode::Point3D closest_point;

    std::tie( distance, closest_point ) =
        geode::point_line_distance( a, line3D );
    OPENGEODE_EXCEPTION(
        distance < geode::global_epsilon
            && closest_point.inexact_equal( a, geode::global_epsilon ),
        "[Test] Wrong result for point_line_distance with query Point3D "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_line_distance( b, line3D );
    OPENGEODE_EXCEPTION(
        distance < geode::global_epsilon
            && closest_point.inexact_equal( b, geode::global_epsilon ),
        "[Test] Wrong result for point_line_distance with query Point3D "
        "b" );

    const geode::Point3D q1{ { 0.0, 0.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_line_distance( q1, line3D );
    OPENGEODE_EXCEPTION(
        distance < geode::global_epsilon
            && closest_point.inexact_equal( q1, geode::global_epsilon ),
        "[Test] Wrong result for point_line_distance with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 5.0, -1.0, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_line_distance( q2, line3D );
    const geode::Point3D result_q2{ { 0.0, 0.0, 0.0 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 26 )
            && closest_point.inexact_equal( result_q2, geode::global_epsilon ),
        "[Test] Wrong result for point_line_distance with query Point3D "
        "q2" );
}

void test_point_line_distance()
{
    test_point_line_distance_2d();
    test_point_line_distance_3d();
}

void test_point_tetra_distance()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 0.0, 1.0, 0.0 } };
    const geode::Point3D d{ { 0.0, 0.0, 1.0 } };
    const geode::Tetra tetra{ a, b, c, d };

    double distance;
    geode::Point3D closest_point, answer;

    std::tie( distance, closest_point ) =
        geode::point_tetra_distance( a, tetra );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == a,
        "[Test] Wrong result for point_tetra_distance with query Point3D "
        "a" );

    std::tie( distance, closest_point ) =
        geode::point_tetra_distance( b, tetra );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == b,
        "[Test] Wrong result for point_tetra_distance with query Point3D "
        "b" );

    std::tie( distance, closest_point ) =
        geode::point_tetra_distance( c, tetra );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == c,
        "[Test] Wrong result for point_tetra_distance with query Point3D "
        "c" );

    std::tie( distance, closest_point ) =
        geode::point_tetra_distance( d, tetra );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == d,
        "[Test] Wrong result for point_tetra_distance with query Point3D "
        "d" );

    const geode::Point3D q1{ { 0.25, 0.25, 0.25 } };
    std::tie( distance, closest_point ) =
        geode::point_tetra_distance( q1, tetra );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == q1,
        "[Test] Wrong result for point_tetra_distance with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 0.25, 0.25, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_tetra_distance( q2, tetra );
    OPENGEODE_EXCEPTION( distance == 0 && closest_point == q2,
        "[Test] Wrong result for point_tetra_distance with query Point3D "
        "q2" );

    const geode::Point3D q3{ { 2.5, 2.5, 0.0 } };
    std::tie( distance, closest_point ) =
        geode::point_tetra_distance( q3, tetra );
    answer = geode::Point3D{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION(
        distance == std::sqrt( 8 )
            && closest_point.inexact_equal( answer, geode::global_epsilon ),
        "[Test] Wrong result for point_tetra_distance with query Point3D "
        "q3" );
}

void test()
{
    test_point_segment_distance();
    test_point_line_distance();
    test_point_triangle_distance();
    test_point_tetra_distance();
    test_point_plane_distance();
}

OPENGEODE_TEST( "distance" )
