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

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/information.h>
#include <geode/geometry/intersection_detection.h>
#include <geode/geometry/point.h>

#include <geode/tests/common.h>

void test_segment_segment_intersection_detection()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 2.0, 1.0 } };
    const geode::Point2D d{ { 3.0, 1.0 } };
    const geode::Point2D e{ { 2.0, 0.0 } };
    const geode::Point2D f{ { 3.0, 0.0 } };

    const geode::Segment2D segment_ab{ a, b };
    const geode::Segment2D segment_ab2{ a, b };
    const geode::Segment2D segment_af{ a, f };
    const geode::Segment2D segment_bc{ b, c };
    const geode::Segment2D segment_cd{ c, d };
    const geode::Segment2D segment_cf{ c, f };
    const geode::Segment2D segment_de{ d, e };
    const geode::Segment2D segment_df{ d, f };
    const geode::Segment2D segment_ef{ e, f };
    const geode::Segment2D segment_bf{ b, f };
    const geode::Segment2D segment_ae{ a, e };

    geode::SegmentSegmentIntersection result, answer;

    result =
        geode::segment_segment_intersection_detection( segment_cf, segment_de );
    answer.first = geode::Position::inside;
    answer.second = geode::Position::inside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_segment_intersection_detection "
        "with query segment_cf and segment_de" );

    result =
        geode::segment_segment_intersection_detection( segment_bc, segment_af );
    answer.first = geode::Position::vertex0;
    answer.second = geode::Position::inside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_segment_intersection_detection with "
        "query segment_bc and segment_af" );

    result =
        geode::segment_segment_intersection_detection( segment_cd, segment_df );
    answer.first = geode::Position::vertex1;
    answer.second = geode::Position::vertex0;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_segment_intersection_detection "
        "with query segment_cd and segment_df" );

    result =
        geode::segment_segment_intersection_detection( segment_ab, segment_cd );
    answer.first = geode::Position::outside;
    answer.second = geode::Position::outside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_segment_intersection_detection with "
        "query segment_ab and segment_cd" );

    result = geode::segment_segment_intersection_detection(
        segment_ab, segment_ab2 );
    answer.first = geode::Position::parallel;
    answer.second = geode::Position::parallel;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_segment_intersection_detection with "
        "query segment_ab and segment_ab2" );
    result = geode::colinear_segment_segment_intersection_detection(
        segment_ab, segment_ab2 );
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for "
        "colinear_segment_segment_intersection_detection with "
        "query segment_ab and segment_ab2" );

    result =
        geode::segment_segment_intersection_detection( segment_ab, segment_ef );
    answer.first = geode::Position::parallel;
    answer.second = geode::Position::parallel;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_segment_intersection_detection with "
        "query segment_ab and segment_ef" );
    result = geode::colinear_segment_segment_intersection_detection(
        segment_ab, segment_ef );
    answer.first = geode::Position::outside;
    answer.second = geode::Position::outside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for "
        "colinear_segment_segment_intersection_detection with "
        "query segment_ab and segment_ef" );

    result =
        geode::segment_segment_intersection_detection( segment_ae, segment_bf );
    answer.first = geode::Position::parallel;
    answer.second = geode::Position::parallel;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_segment_intersection_detection with "
        "query segment_ae and segment_bf" );
    result = geode::colinear_segment_segment_intersection_detection(
        segment_ae, segment_bf );
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for "
        "colinear_segment_segment_intersection_detection with "
        "query segment_ae and segment_bf" );

    result =
        geode::segment_segment_intersection_detection( segment_ae, segment_ef );
    answer.first = geode::Position::parallel;
    answer.second = geode::Position::parallel;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_segment_intersection_detection with "
        "query segment_ae and segment_ef" );
    result = geode::colinear_segment_segment_intersection_detection(
        segment_ae, segment_ef );
    answer.first = geode::Position::vertex1;
    answer.second = geode::Position::vertex0;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for "
        "colinear_segment_segment_intersection_detection with "
        "query segment_ae and segment_ef" );

    result =
        geode::segment_segment_intersection_detection( segment_bc, segment_df );
    answer.first = geode::Position::outside;
    answer.second = geode::Position::outside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_segment_intersection_detection with "
        "query segment_bc and segment_df" );
}

void test_segment_line_intersection_detection()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 2.0, 1.0 } };
    const geode::Point2D d{ { 3.0, 1.0 } };
    const geode::Point2D e{ { 2.0, 0.0 } };
    const geode::Point2D f{ { 3.0, 0.0 } };

    const geode::Segment2D segment_ab{ a, b };
    const geode::Segment2D segment_ab2{ a, b };
    const geode::Segment2D segment_af{ a, f };
    const geode::Segment2D segment_bc{ b, c };
    const geode::Segment2D segment_cd{ c, d };
    const geode::Segment2D segment_cf{ c, f };
    const geode::Segment2D segment_de{ d, e };
    const geode::Segment2D segment_df{ d, f };
    const geode::Segment2D segment_ef{ e, f };

    geode::Position result;

    result = geode::segment_line_intersection_detection(
        segment_cf, geode::InfiniteLine2D{ segment_de } );
    OPENGEODE_EXCEPTION( result == geode::Position::inside,
        "[Test] Wrong result for segment_line_intersection_detection "
        "with query segment_cf and segment_de" );

    result = geode::segment_line_intersection_detection(
        segment_bc, geode::InfiniteLine2D{ segment_af } );
    OPENGEODE_EXCEPTION( result == geode::Position::vertex0,
        "[Test] Wrong result for segment_line_intersection_detection with "
        "query segment_bc and segment_af" );

    result = geode::segment_line_intersection_detection(
        segment_cd, geode::InfiniteLine2D{ segment_df } );
    OPENGEODE_EXCEPTION( result == geode::Position::vertex1,
        "[Test] Wrong result for segment_line_intersection_detection "
        "with query segment_cd and segment_df" );

    result = geode::segment_line_intersection_detection(
        segment_ab, geode::InfiniteLine2D{ segment_cd } );
    OPENGEODE_EXCEPTION( result == geode::Position::outside,
        "[Test] Wrong result for segment_line_intersection_detection with "
        "query segment_ab and segment_cd" );

    result = geode::segment_line_intersection_detection(
        segment_ab, geode::InfiniteLine2D{ segment_ab2 } );
    OPENGEODE_EXCEPTION( result == geode::Position::parallel,
        "[Test] Wrong result for segment_line_intersection_detection with "
        "query segment_ab and segment_ab2" );

    result = geode::segment_line_intersection_detection(
        segment_ab, geode::InfiniteLine2D{ segment_ef } );
    OPENGEODE_EXCEPTION( result == geode::Position::parallel,
        "[Test] Wrong result for segment_line_intersection_detection with "
        "query segment_ab and segment_ef" );

    result = geode::segment_line_intersection_detection(
        segment_bc, geode::InfiniteLine2D{ segment_df } );
    OPENGEODE_EXCEPTION( result == geode::Position::outside,
        "[Test] Wrong result for segment_line_intersection_detection with "
        "query segment_bc and segment_df" );
}

void test_segment_triangle_intersection_detection()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 0.0 } };
    const geode::Point3D d{ { -1.0, -1.0, 0.0 } };
    const geode::Point3D e{ { 2.0, 2.0, 0.0 } };
    const geode::Point3D f{ { -1.0, -1.0, 3.0 } };
    const geode::Point3D g{ { 2.0, 2.0, 3.0 } };
    const geode::Point3D h{ { -1.0, -1.0, -1.0 } };
    const geode::Point3D i{ { 1.0, 1.0, 1.0 } };
    const geode::Point3D j{ { 2.0, 2.0, 1.0 } };
    const geode::Point3D k{ { 0.0, -0.5, -1.0 } };
    const geode::Point3D l{ { -10.0, -1.0, -1.0 } };
    const geode::Point3D m{ { 0.5, 0.25, 1.0 } };
    const geode::Point3D n{ { 0.5, 0.25, 3.0 } };
    const geode::Point3D o{ { 0.75, 0.25, 0.0 } };
    const geode::Point3D p{ { 0.75, -0.25, 0.0 } };

    const geode::Triangle3D triangle{ a, b, c };

    const geode::Segment3D segment_ab{ a, b };
    const geode::Segment3D segment_da{ d, a };
    const geode::Segment3D segment_fg{ f, g };
    const geode::Segment3D segment_hi{ h, i };
    const geode::Segment3D segment_hj{ h, j };
    const geode::Segment3D segment_ki{ k, i };
    const geode::Segment3D segment_li{ l, i };
    const geode::Segment3D segment_kb{ k, b };
    const geode::Segment3D segment_mn{ m, n };
    const geode::Segment3D segment_op{ o, p };

    geode::SegmentTriangleIntersection result, answer;

    result =
        geode::segment_triangle_intersection_detection( segment_ab, triangle );
    answer = { geode::Position::parallel, geode::Position::parallel };
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_triangle_intersection_detection with "
        "query segment_ab" );

    result =
        geode::segment_triangle_intersection_detection( segment_da, triangle );
    answer = { geode::Position::parallel, geode::Position::parallel };
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_triangle_intersection_detection with "
        "query segment_da" );

    result =
        geode::segment_triangle_intersection_detection( segment_fg, triangle );
    answer = { geode::Position::outside, geode::Position::outside };
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_triangle_intersection_detection with "
        "query segment_fg" );

    result =
        geode::segment_triangle_intersection_detection( segment_hi, triangle );
    answer = { geode::Position::inside, geode::Position::vertex0 };
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_triangle_intersection_detection with "
        "query segment_hi" );

    result =
        geode::segment_triangle_intersection_detection( segment_hj, triangle );
    answer = { geode::Position::inside, geode::Position::edge2 };
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_triangle_intersection_detection with "
        "query segment_hj" );

    result =
        geode::segment_triangle_intersection_detection( segment_ki, triangle );
    answer = { geode::Position::inside, geode::Position::inside };
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_triangle_intersection_detection with "
        "query segment_ki" );

    result =
        geode::segment_triangle_intersection_detection( segment_li, triangle );
    answer = { geode::Position::outside, geode::Position::outside };
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_triangle_intersection_detection with "
        "query segment_li" );

    result =
        geode::segment_triangle_intersection_detection( segment_kb, triangle );
    answer = { geode::Position::vertex1, geode::Position::vertex1 };
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_triangle_intersection_detection with "
        "query segment_kb" );

    result =
        geode::segment_triangle_intersection_detection( segment_mn, triangle );
    answer = { geode::Position::outside, geode::Position::outside };
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_triangle_intersection_detection with "
        "query segment_mn" );

    result =
        geode::segment_triangle_intersection_detection( segment_op, triangle );
    answer = { geode::Position::parallel, geode::Position::parallel };
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_triangle_intersection_detection with "
        "query segment_op" );
}

void test_line_triangle_intersection_detection()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 0.0 } };
    const geode::Point3D d{ { -1.0, -1.0, 0.0 } };
    const geode::Point3D e{ { 2.0, 2.0, 0.0 } };
    const geode::Point3D f{ { -1.0, -1.0, 3.0 } };
    const geode::Point3D g{ { 2.0, 2.0, 3.0 } };
    const geode::Point3D h{ { -1.0, -1.0, -1.0 } };
    const geode::Point3D i{ { 1.0, 1.0, 1.0 } };
    const geode::Point3D j{ { 2.0, 2.0, 1.0 } };
    const geode::Point3D k{ { 0.0, -0.5, -1.0 } };
    const geode::Point3D l{ { -10.0, -1.0, -1.0 } };
    const geode::Point3D m{ { 0.5, 0.25, 1.0 } };
    const geode::Point3D n{ { 0.5, 0.25, 3.0 } };
    const geode::Point3D o{ { 0.75, 0.25, 0.0 } };
    const geode::Point3D p{ { 0.75, -0.25, 0.0 } };

    const geode::Triangle3D triangle{ a, b, c };

    const geode::InfiniteLine3D line_ab{ geode::Segment3D{ a, b } };
    const geode::InfiniteLine3D line_da{ geode::Segment3D{ d, a } };
    const geode::InfiniteLine3D line_fg{ geode::Segment3D{ f, g } };
    const geode::InfiniteLine3D line_hi{ geode::Segment3D{ h, i } };
    const geode::InfiniteLine3D line_hj{ geode::Segment3D{ h, j } };
    const geode::InfiniteLine3D line_ki{ geode::Segment3D{ k, i } };
    const geode::InfiniteLine3D line_li{ geode::Segment3D{ l, i } };
    const geode::InfiniteLine3D line_kb{ geode::Segment3D{ k, b } };
    const geode::InfiniteLine3D line_mn{ geode::Segment3D{ m, n } };
    const geode::InfiniteLine3D line_op{ geode::Segment3D{ o, p } };

    geode::Position result, answer;

    result = geode::line_triangle_intersection_detection( line_ab, triangle );
    answer = geode::Position::parallel;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for line_triangle_intersection_detection with "
        "query line_ab" );

    result = geode::line_triangle_intersection_detection( line_da, triangle );
    answer = geode::Position::parallel;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for line_triangle_intersection_detection with "
        "query line_da" );

    result = geode::line_triangle_intersection_detection( line_fg, triangle );
    answer = geode::Position::outside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for line_triangle_intersection_detection with "
        "query line_fg" );

    result = geode::line_triangle_intersection_detection( line_hi, triangle );
    answer = geode::Position::vertex0;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for line_triangle_intersection_detection with "
        "query line_hi" );

    result = geode::line_triangle_intersection_detection( line_hj, triangle );
    answer = geode::Position::edge2;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for line_triangle_intersection_detection with "
        "query line_hj" );

    result = geode::line_triangle_intersection_detection( line_ki, triangle );
    answer = geode::Position::inside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for line_triangle_intersection_detection with "
        "query line_ki" );

    result = geode::line_triangle_intersection_detection( line_li, triangle );
    answer = geode::Position::outside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for line_triangle_intersection_detection with "
        "query line_li" );

    result = geode::line_triangle_intersection_detection( line_kb, triangle );
    answer = geode::Position::vertex1;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for line_triangle_intersection_detection with "
        "query line_kb" );

    result = geode::line_triangle_intersection_detection( line_mn, triangle );
    answer = geode::Position::inside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for line_triangle_intersection_detection with "
        "query line_mn" );

    result = geode::line_triangle_intersection_detection( line_op, triangle );
    answer = geode::Position::parallel;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for line_triangle_intersection_detection with "
        "query line_op" );
}

void test_segment_plane_intersection_detection()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 0.0 } };
    const geode::Point3D d{ { -1.0, -1.0, 0.0 } };
    const geode::Point3D e{ { 2.0, 2.0, 0.0 } };
    const geode::Point3D f{ { -1.0, -1.0, 3.0 } };
    const geode::Point3D g{ { 2.0, 2.0, 3.0 } };
    const geode::Point3D h{ { -1.0, -1.0, -1.0 } };
    const geode::Point3D i{ { 1.0, 1.0, 1.0 } };
    const geode::Point3D j{ { 2.0, 2.0, 1.0 } };
    const geode::Point3D k{ { 0.0, -0.5, -1.0 } };
    const geode::Point3D l{ { -10.0, -1.0, -1.0 } };
    const geode::Point3D m{ { 0.5, 0.25, 1.0 } };
    const geode::Point3D n{ { 0.5, 0.25, 3.0 } };
    const geode::Point3D o{ { 0.75, 0.25, 0.0 } };
    const geode::Point3D p{ { 0.75, -0.25, 0.0 } };

    const geode::Plane plane{ { { 0.0, 0.0, 1.0 } }, a };

    const geode::Segment3D segment_ab{ a, b };
    const geode::Segment3D segment_da{ d, a };
    const geode::Segment3D segment_fg{ f, g };
    const geode::Segment3D segment_hi{ h, i };
    const geode::Segment3D segment_hj{ h, j };
    const geode::Segment3D segment_ki{ k, i };
    const geode::Segment3D segment_li{ l, i };
    const geode::Segment3D segment_kb{ k, b };
    const geode::Segment3D segment_mn{ m, n };
    const geode::Segment3D segment_op{ o, p };

    geode::Position result, answer;

    result = geode::segment_plane_intersection_detection( segment_ab, plane );
    answer = geode::Position::parallel;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_plane_intersection_detection with "
        "query segment_ab" );

    result = geode::segment_plane_intersection_detection( segment_da, plane );
    answer = geode::Position::parallel;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_plane_intersection_detection with "
        "query segment_da" );

    result = geode::segment_plane_intersection_detection( segment_fg, plane );
    answer = geode::Position::outside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_plane_intersection_detection with "
        "query segment_fg" );

    result = geode::segment_plane_intersection_detection( segment_hi, plane );
    answer = geode::Position::inside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_plane_intersection_detection with "
        "query segment_hi" );

    result = geode::segment_plane_intersection_detection( segment_hj, plane );
    answer = geode::Position::inside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_plane_intersection_detection with "
        "query segment_hj" );

    result = geode::segment_plane_intersection_detection( segment_ki, plane );
    answer = geode::Position::inside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_plane_intersection_detection with "
        "query segment_ki" );

    result = geode::segment_plane_intersection_detection( segment_li, plane );
    answer = geode::Position::inside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_plane_intersection_detection with "
        "query segment_li" );

    result = geode::segment_plane_intersection_detection( segment_kb, plane );
    answer = geode::Position::vertex1;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_plane_intersection_detection with "
        "query segment_kb" );

    result = geode::segment_plane_intersection_detection( segment_mn, plane );
    answer = geode::Position::outside;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_plane_intersection_detection with "
        "query segment_mn" );

    result = geode::segment_plane_intersection_detection( segment_op, plane );
    answer = geode::Position::parallel;
    OPENGEODE_EXCEPTION( result == answer,
        "[Test] Wrong result for segment_plane_intersection_detection with "
        "query segment_op" );
}

void test()
{
    test_segment_segment_intersection_detection();
    test_segment_line_intersection_detection();
    test_segment_triangle_intersection_detection();
    test_line_triangle_intersection_detection();
    test_segment_plane_intersection_detection();

    geode::Logger::info( "TEST SUCCESS" );
}

OPENGEODE_TEST( "intersection-detection" )