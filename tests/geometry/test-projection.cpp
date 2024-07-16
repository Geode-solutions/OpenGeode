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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>

#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/projection.hpp>

#include <geode/tests/common.hpp>

void point_segment_projection_2d()
{
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Segment2D segment2D{ a, b };

    geode::Point2D closest_point;

    closest_point = geode::point_segment_projection( a, segment2D );
    OPENGEODE_EXCEPTION( closest_point == a,
        "[Test] Wrong result for point_segment_projection with query Point2D "
        "a" );

    closest_point = geode::point_segment_projection( b, segment2D );
    OPENGEODE_EXCEPTION( closest_point == b,
        "[Test] Wrong result for point_segment_projection with query Point2D "
        "b" );

    const geode::Point2D q1{ { 0.0, 0.0 } };
    closest_point = geode::point_segment_projection( q1, segment2D );
    OPENGEODE_EXCEPTION( closest_point == q1,
        "[Test] Wrong result for point_segment_projection with query Point2D "
        "q1" );

    const geode::Point2D q2{ { 10.0, 10.0 } };
    closest_point = geode::point_segment_projection( q2, segment2D );
    OPENGEODE_EXCEPTION( closest_point == a,
        "[Test] Wrong result for point_segment_projection with query Point2D "
        "q2" );

    const geode::Point2D q3{ { 5.0, -1.0 } };
    closest_point = geode::point_segment_projection( q3, segment2D );
    const geode::Point2D result_q3{ { 0.0, 0.0 } };
    OPENGEODE_EXCEPTION( closest_point == result_q3,
        "[Test] Wrong result for point_segment_projection with query Point2D "
        "q3" );

    const geode::Point2D q4{ { 5.5, 1.5 } };
    closest_point = geode::point_segment_projection( q4, segment2D );
    const geode::Point2D result_q4{ { 0.5, 2.5 } };
    OPENGEODE_EXCEPTION( closest_point == result_q4,
        "[Test] Wrong result for point_segment_projection with query Point2D "
        "q4" );
}

void point_segment_projection_3d()
{
    const geode::Point3D a{ { 1.0, 5.0, 2.0 } };
    const geode::Point3D b{ { -1.0, -5.0, -2.0 } };
    const geode::Segment3D segment3D{ a, b };
    const geode::Segment3D tiny_segment3D{ a, a };

    geode::Point3D closest_point;

    closest_point = geode::point_segment_projection( b, tiny_segment3D );
    OPENGEODE_EXCEPTION( closest_point.inexact_equal( a ),
        "[Test] Wrong result for point_segment_projection with query Point3D "
        "b and tiny_segment" );

    closest_point = geode::point_segment_projection( a, segment3D );
    OPENGEODE_EXCEPTION( closest_point.inexact_equal( a ),
        "[Test] Wrong result for point_segment_projection with query Point3D "
        "a" );

    closest_point = geode::point_segment_projection( b, segment3D );
    OPENGEODE_EXCEPTION( closest_point.inexact_equal( b ),
        "[Test] Wrong result for point_segment_projection with query Point3D "
        "b" );

    const geode::Point3D q1{ { 0.0, 0.0, 0.0 } };
    closest_point = geode::point_segment_projection( q1, segment3D );
    OPENGEODE_EXCEPTION( closest_point.inexact_equal( q1 ),
        "[Test] Wrong result for point_segment_projection with query Point3D "
        "q1" );

    const geode::Point3D q2{ { 10.0, 10.0, 10.0 } };
    closest_point = geode::point_segment_projection( q2, segment3D );
    OPENGEODE_EXCEPTION( closest_point.inexact_equal( a ),
        "[Test] Wrong result for point_segment_projection with query Point3D "
        "q2" );

    const geode::Point3D q3{ { 5.0, -1.0, 0.0 } };
    closest_point = geode::point_segment_projection( q3, segment3D );
    const geode::Point3D result_q3{ { 0.0, 0.0, 0.0 } };
    OPENGEODE_EXCEPTION( closest_point.inexact_equal( result_q3 ),
        "[Test] Wrong result for point_segment_projection with query Point3D "
        "q3" );

    const geode::Point3D q4{ { 5.5, 1.5, 0.25 } };
    closest_point = geode::point_segment_projection( q4, segment3D );
    const geode::Point3D result_q4{ { 0.45, 2.25, 0.9 } };
    OPENGEODE_EXCEPTION( closest_point.inexact_equal( result_q4 ),
        "[Test] Wrong result for point_segment_projection with query Point3D "
        "q4" );
}

void point_triangle_projection_3d()
{
    const geode::Point3D a{ { 1.0, 5.0, 2.0 } };
    const geode::Point3D b{ { -1.0, -5.0, -2.0 } };
    const geode::Point3D c{ { 0.0, 0.0, -2.0 } };
    const geode::Triangle3D triangle{ a, b, c };

    const auto closest_point = geode::point_triangle_projection( a, triangle );
    OPENGEODE_EXCEPTION( closest_point.inexact_equal( a ),
        "[Test] Wrong result for point_triangle_projection with query Point3D "
        "a" );
}

void point_plane_projection()
{
    const geode::Point3D a{ { 1.0, 5.0, 2.0 } };
    const geode::Vector3D normal{ { 0.0, 0.0, 1.0 } };
    const geode::Point3D O{ { 1.0, 1.0, 0.0 } };
    const geode::Plane plane{ normal, O };

    const auto closest_point = geode::point_plane_projection( a, plane );
    const geode::Point3D answer{ { 1.0, 5.0, 0.0 } };
    OPENGEODE_EXCEPTION( closest_point.inexact_equal( answer ),
        "[Test] Wrong result for point_plane_projection with query Point3D a" );
}

void test()
{
    point_segment_projection_2d();
    point_segment_projection_3d();
    point_triangle_projection_3d();
    point_plane_projection();
}

OPENGEODE_TEST( "projection" )
