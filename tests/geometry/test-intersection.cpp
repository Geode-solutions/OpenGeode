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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/basic_objects/circle.h>
#include <geode/geometry/basic_objects/cylinder.h>
#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/sphere.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/intersection.h>
#include <geode/geometry/point.h>

#include <geode/tests/common.h>

/*
 * InfiniteLines in this test are all defined using two points (constructor
 * using a Segment)
 */

void test_line_sphere_intersection()
{
    // Tests for 2D cases
    const geode::Point2D sphere_origin{ { 2., 0. } };
    const geode::Sphere2D sphere{ sphere_origin, 1. };
    const geode::Point2D line1_origin{ { 0., 0.5 } };
    const geode::Point2D line1_dest{ { 1., 0.5 } };
    const geode::Segment2D segment1{ line1_origin, line1_dest };
    const geode::InfiniteLine2D line1{ segment1 };
    const geode::Point2D line2_origin{ { 0., 1. } };
    const geode::Point2D line2_dest{ { 1., 1. } };
    const geode::Segment2D segment2{ line2_origin, line2_dest };
    const geode::InfiniteLine2D line2{ segment2 };
    const geode::Point2D line3_origin{ { 0., 2. } };
    const geode::Point2D line3_dest{ { 1., 2. } };
    const geode::Segment2D segment3{ line3_origin, line3_dest };
    const geode::InfiniteLine2D line3{ segment3 };

    const auto intersection_points1 =
        geode::line_sphere_intersection( line1, sphere );
    OPENGEODE_EXCEPTION(
        intersection_points1 && intersection_points1.result.value().size() == 2,
        "[Test] Wrong result for line_sphere_intersection "
        "with query line1" );
    OPENGEODE_EXCEPTION(
        std::fabs( intersection_points1.result.value().front().value( 0 ) - 2 )
                    - std::sqrt( 3 ) / 2
                < geode::GLOBAL_EPSILON
            && std::fabs(
                   intersection_points1.result.value().back().value( 0 ) - 2 )
                       - std::sqrt( 3 ) / 2
                   < geode::GLOBAL_EPSILON
            && intersection_points1.result.value().front().value( 1 ) == 0.5
            && intersection_points1.result.value().back().value( 1 ),
        "[Test] Wrong result for line_sphere_intersection "
        "with query line 1 (coords)" );

    const auto intersection_points2 =
        geode::line_sphere_intersection( line2, sphere );
    OPENGEODE_EXCEPTION(
        intersection_points2 && intersection_points2.result.value().size() == 1,
        "[Test] Wrong result for line_sphere_intersection "
        "with query line 2" );
    const geode::Point2D result{ { 2.0, 1.0 } };
    OPENGEODE_EXCEPTION( intersection_points2.result.value().front() == result,
        "[Test] Wrong result for line_sphere_intersection "
        "with query line 2 (coords)" );

    const auto intersection_points3 =
        geode::line_sphere_intersection( line3, sphere );
    OPENGEODE_EXCEPTION( !intersection_points3,
        "[Test] Wrong result for line_sphere_intersection "
        "with query line 3" );

    // Tests for 3D cases
    // TODO
}

void test_segment_sphere_intersection()
{
    // Tests for 2D cases
    const geode::Point2D origin{ { 0.0, 0.0 } };
    const geode::Point2D a{ { 2.0, 0.0 } };
    const geode::Point2D b{ { 1.5, 0.0 } };
    const geode::Point2D c{ { 2.5, 0.0 } };
    const geode::Point2D d{ { 3.0, 0.0 } };
    const geode::Point2D e{ { 4.0, 0.0 } };
    const geode::Point2D f{ { 5.0, 0.0 } };
    const geode::Point2D g{ { 4.0, 4.0 } };
    const geode::Sphere2D sphere{ a, 1. };
    const geode::Segment2D segment1{ b, c }; // Included in sphere
    const geode::Segment2D segment2{ a, d }; // Touch the sphere from inside
    const geode::Segment2D segment3{ d, g }; // Touch the sphere from outside
    const geode::Segment2D segment4{ origin, e }; // Intersect twice the sphere
    const geode::Segment2D segment5{ e, f }; // Do not intersect the sphere

    const auto intersection_points1 =
        geode::segment_sphere_intersection( segment1, sphere );
    OPENGEODE_EXCEPTION( !intersection_points1,
        "[Test] Wrong result for line_sphere_intersection "
        "with query segment 1" );

    const auto intersection_points2 =
        geode::segment_sphere_intersection( segment2, sphere );
    OPENGEODE_EXCEPTION(
        intersection_points2 && intersection_points2.result.value().size() == 1,
        "[Test] Wrong result for line_sphere_intersection "
        "with query segment 2" );

    const auto intersection_points3 =
        geode::segment_sphere_intersection( segment3, sphere );
    OPENGEODE_EXCEPTION(
        intersection_points3 && intersection_points3.result.value().size() == 1,
        "[Test] Wrong result for line_sphere_intersection "
        "with query segment 3" );

    const auto intersection_points4 =
        geode::segment_sphere_intersection( segment4, sphere );
    OPENGEODE_EXCEPTION(
        intersection_points4 && intersection_points4.result.value().size() == 2,
        "[Test] Wrong result for line_sphere_intersection "
        "with query segment 4" );

    const auto intersection_points5 =
        geode::segment_sphere_intersection( segment5, sphere );
    OPENGEODE_EXCEPTION( !intersection_points5,
        "[Test] Wrong result for line_sphere_intersection "
        "with query segment 5" );

    // Tests for 3D cases
    // TODO
}

void test_line_line_intersection()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };
    const geode::Point2D d{ { 2.0, 1.0 } };
    const geode::InfiniteLine2D line1{ geode::Segment2D{ a, b } };
    const geode::InfiniteLine2D line2{ geode::Segment2D{
        a, b } }; // same than line1
    const geode::InfiniteLine2D line3{ geode::Segment2D{
        b, c } }; // perpendicular to line1
    const geode::InfiniteLine2D line4{ geode::Segment2D{
        c, d } }; // parallel to line1
    const geode::InfiniteLine2D line5{ geode::Segment2D{ a, c } }; // other

    const auto intersection_point =
        geode::line_line_intersection( line1, line2 );
    OPENGEODE_EXCEPTION( !intersection_point,
        "[Test] Wrong result for line_line_intersection "
        "with query lines 1 and 2" );

    const auto intersection_point1 =
        geode::line_line_intersection( line1, line3 );
    OPENGEODE_EXCEPTION(
        intersection_point1 && intersection_point1.result.value() == b,
        "[Test] Wrong result for line_line_intersection with query lines 1 "
        "and 3" );

    const auto intersection_point2 =
        geode::line_line_intersection( line1, line4 );
    OPENGEODE_EXCEPTION( !intersection_point2,
        "[Test] Wrong result for line_line_intersection "
        "with query lines 1 and 4" );

    const auto intersection_point3 =
        geode::line_line_intersection( line1, line5 );
    OPENGEODE_EXCEPTION(
        intersection_point3 && intersection_point3.result.value() == a,
        "[Test] Wrong result for line_line_intersection with query lines 1 "
        "and 5" );
}

void test_segment_segment_intersection()
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

    const auto intersection_point =
        geode::segment_segment_intersection( segment_cf, segment_de );
    const geode::Point2D result_cf_de{ { 2.5, 0.5 } };
    OPENGEODE_EXCEPTION(
        intersection_point && intersection_point.result.value() == result_cf_de,
        "[Test] Wrong result for segment_segment_intersection "
        "with query segment_cf and segment_de" );

    const auto intersection_point1 =
        geode::segment_segment_intersection( segment_bc, segment_af );
    OPENGEODE_EXCEPTION(
        intersection_point1 && intersection_point1.result.value() == b,
        "[Test] Wrong result for segment_segment_intersection with query "
        "segment_bc and segment_af" );

    const auto intersection_point2 =
        geode::segment_segment_intersection( segment_cd, segment_df );
    OPENGEODE_EXCEPTION(
        intersection_point2 && intersection_point2.result.value() == d,
        "[Test] Wrong result for segment_segment_intersection "
        "with query segment_cd and segment_df" );

    const auto intersection_point3 =
        geode::segment_segment_intersection( segment_ab, segment_cd );
    OPENGEODE_EXCEPTION( !intersection_point3,
        "[Test] Wrong result for segment_segment_intersection with query "
        "segment_ab and segment_cd" );

    const auto intersection_point4 =
        geode::segment_segment_intersection( segment_ab, segment_ab2 );
    OPENGEODE_EXCEPTION( !intersection_point4,
        "[Test] Wrong result for segment_segment_intersection with query "
        "segment_ab and segment_ab2" );

    const auto intersection_point5 =
        geode::segment_segment_intersection( segment_ab, segment_ef );
    OPENGEODE_EXCEPTION( !intersection_point5,
        "[Test] Wrong result for segment_segment_intersection with query "
        "segment_ab and segment_ef" );

    const auto intersection_point6 =
        geode::segment_segment_intersection( segment_bc, segment_df );
    OPENGEODE_EXCEPTION( !intersection_point6,
        "[Test] Wrong result for segment_segment_intersection with query "
        "segment_bc and segment_df" );
}

void test_segment_line_intersection()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 2.0, 1.0 } };
    const geode::Point2D d{ { 3.0, 1.0 } };
    const geode::Point2D e{ { 2.0, 0.0 } };
    const geode::Point2D e_epsilon{ { 2.0 - 1e-10, 0.0 } };
    const geode::Point2D f{ { 3.0, 0.0 } };

    const geode::Segment2D segment_ab{ a, b };
    const geode::Segment2D segment_cd{ c, d };
    const geode::Segment2D segment_de{ d, e };
    const geode::Segment2D segment_ef{ e, f };

    const geode::InfiniteLine2D line_cf{ geode::Segment2D{ c, f } };
    const geode::InfiniteLine2D line_df{ geode::Segment2D{ d, f } };
    const geode::InfiniteLine2D line_ef{ geode::Segment2D{ e, f } };
    const geode::InfiniteLine2D line_ce{ geode::Segment2D{ c, e } };

    const auto intersection_point =
        geode::segment_line_intersection( segment_de, line_cf );
    const geode::Point2D result_de_cf{ { 2.5, 0.5 } };
    OPENGEODE_EXCEPTION(
        intersection_point && intersection_point.result.value() == result_de_cf,
        "[Test] Wrong result for segment_line_intersection "
        "with query segment_de and line_cf" );

    const auto intersection_point1 =
        geode::segment_line_intersection( segment_ef, line_df );
    OPENGEODE_EXCEPTION(
        intersection_point1 && intersection_point1.result.value() == f,
        "[Test] Wrong result for segment_line_intersection "
        "with query segment_ef and line_df" );

    const auto intersection_point2 =
        geode::segment_line_intersection( segment_cd, line_ef );
    OPENGEODE_EXCEPTION( !intersection_point2,
        "[Test] Wrong result for segment_line_intersection "
        "with query segment_cd and line_ef" );

    const auto intersection_point3 =
        geode::segment_line_intersection( segment_ab, line_ef );
    OPENGEODE_EXCEPTION( !intersection_point3,
        "[Test] Wrong result for segment_line_intersection "
        "with query segment_ab and line_ef" );

    const auto intersection_point4 =
        geode::segment_line_intersection( segment_ab, line_ce );
    OPENGEODE_EXCEPTION( !intersection_point4,
        "[Test] Wrong result for segment_line_intersection "
        "with query segment_ab and line_ce" );
}

void test_segment_plane_intersection()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 0.0 } };

    const geode::Triangle3D triangle{ a, b, c };
    const auto plane = triangle.plane().value();

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

    const geode::Segment3D segment_de{ a, b };
    const geode::Segment3D segment_da{ d, a };
    const geode::Segment3D segment_fg{ f, g };
    const geode::Segment3D segment_hi{ h, i };
    const geode::Segment3D segment_hj{ h, j };
    const geode::Segment3D segment_ki{ k, i };
    const geode::Segment3D segment_li{ l, i };
    const geode::Segment3D segment_kb{ k, b };
    const geode::Segment3D segment_mn{ m, n };

    const auto intersect =
        geode::segment_plane_intersection( segment_de, plane );
    OPENGEODE_EXCEPTION( !intersect,
        "[Test] Wrong result for segment_plane_intersection with "
        "query segment_de" );

    const auto intersect1 =
        geode::segment_plane_intersection( segment_da, plane );
    OPENGEODE_EXCEPTION( !intersect1,
        "[Test] Wrong result for segment_plane_intersection with "
        "query segment_da" );

    const auto intersect2 =
        geode::segment_plane_intersection( segment_fg, plane );
    OPENGEODE_EXCEPTION( !intersect2,
        "[Test] Wrong result for segment_plane_intersection with "
        "query segment_fg" );

    const auto intersect3 =
        geode::segment_plane_intersection( segment_hi, plane );
    OPENGEODE_EXCEPTION( intersect3.result.value() == a,
        "[Test] Wrong result for segment_plane_intersection with "
        "query segment_hi" );

    const auto intersect4 =
        geode::segment_plane_intersection( segment_hj, plane );
    geode::Point3D answer4{ { 0.5, 0.5, 0.0 } };
    double distance_to_answer =
        geode::point_point_distance( intersect4.result.value(), answer4 );
    OPENGEODE_EXCEPTION( distance_to_answer < geode::GLOBAL_EPSILON,
        "[Test] Wrong result for segment_plane_intersection with "
        "query segment_hj" );

    const auto intersect5 =
        geode::segment_plane_intersection( segment_ki, plane );
    geode::Point3D answer{ { 0.5, 0.25, 0.0 } };
    distance_to_answer =
        geode::point_point_distance( intersect5.result.value(), answer );
    OPENGEODE_EXCEPTION( distance_to_answer < geode::GLOBAL_EPSILON,
        "[Test] Wrong result for segment_plane_intersection with "
        "query segment_ki" );

    const auto intersect6 =
        geode::segment_plane_intersection( segment_li, plane );
    geode::Point3D answer6{ { -4.5, 0.0, 0.0 } };
    distance_to_answer =
        geode::point_point_distance( intersect6.result.value(), answer6 );
    OPENGEODE_EXCEPTION( distance_to_answer < geode::GLOBAL_EPSILON,
        "[Test] Wrong result for segment_plane_intersection with "
        "query segment_li" );

    const auto intersect7 =
        geode::segment_plane_intersection( segment_kb, plane );
    OPENGEODE_EXCEPTION( intersect7.result.value() == b,
        "[Test] Wrong result for segment_plane_intersection with "
        "query segment_kb" );

    const auto intersect8 =
        geode::segment_plane_intersection( segment_mn, plane );
    OPENGEODE_EXCEPTION( !intersect8,
        "[Test] Wrong result for segment_plane_intersection with "
        "query segment_mn" );
}

void test_segment_triangle_intersection()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 0.0 } };

    const geode::Triangle3D triangle{ a, b, c };

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

    const geode::Segment3D segment_de{ a, b };
    const geode::Segment3D segment_da{ d, a };
    const geode::Segment3D segment_fg{ f, g };
    const geode::Segment3D segment_hi{ h, i };
    const geode::Segment3D segment_hj{ h, j };
    const geode::Segment3D segment_ki{ k, i };
    const geode::Segment3D segment_li{ l, i };
    const geode::Segment3D segment_kb{ k, b };
    const geode::Segment3D segment_mn{ m, n };

    const auto intersection =
        geode::segment_triangle_intersection( segment_de, triangle );
    OPENGEODE_EXCEPTION( !intersection,
        "[Test] Wrong result for segment_triangle_intersection with "
        "query segment_de" );

    const auto intersection1 =
        geode::segment_triangle_intersection( segment_da, triangle );
    OPENGEODE_EXCEPTION( !intersection1,
        "[Test] Wrong result for segment_triangle_intersection with "
        "query segment_da" );

    const auto intersection2 =
        geode::segment_triangle_intersection( segment_fg, triangle );
    OPENGEODE_EXCEPTION( !intersection2,
        "[Test] Wrong result for segment_triangle_intersection with "
        "query segment_fg" );

    const auto intersection3 =
        geode::segment_triangle_intersection( segment_hi, triangle );
    OPENGEODE_EXCEPTION( intersection3.result.value() == a,
        "[Test] Wrong result for segment_triangle_intersection with "
        "query segment_hi" );

    const auto intersection4 =
        geode::segment_triangle_intersection( segment_hj, triangle );
    geode::Point3D answer{ { 0.5, 0.5, 0.0 } };
    OPENGEODE_EXCEPTION( intersection4.result.value() == answer,
        "[Test] Wrong result for segment_triangle_intersection with "
        "query segment_hj" );

    const auto intersection5 =
        geode::segment_triangle_intersection( segment_ki, triangle );
    geode::Point3D answer5{ { 0.5, 0.25, 0.0 } };
    OPENGEODE_EXCEPTION( intersection5.result.value() == answer5,
        "[Test] Wrong result for segment_triangle_intersection with "
        "query segment_ki" );

    const auto intersection6 =
        geode::segment_triangle_intersection( segment_li, triangle );
    OPENGEODE_EXCEPTION( !intersection6,
        "[Test] Wrong result for segment_triangle_intersection with "
        "query segment_li" );

    const auto intersection7 =
        geode::segment_triangle_intersection( segment_kb, triangle );
    OPENGEODE_EXCEPTION( intersection7.result.value() == b,
        "[Test] Wrong result for segment_triangle_intersection with "
        "query segment_kb" );

    const auto intersection8 =
        geode::segment_triangle_intersection( segment_mn, triangle );
    OPENGEODE_EXCEPTION( !intersection8,
        "[Test] Wrong result for segment_triangle_intersection with "
        "query segment_mn" );
}

void test_line_cylinder_intersection()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Segment3D segment_ab{ a, b };
    const geode::Cylinder cylinder{ segment_ab, 0.2 };

    const geode::InfiniteLine3D line_ab{ segment_ab };
    const auto result_ab =
        geode::line_cylinder_intersection( line_ab, cylinder );
    OPENGEODE_EXCEPTION( result_ab,
        "[Test] Wrong result for line_cylinder_intersection with "
        "query line_ab" );
    OPENGEODE_EXCEPTION( result_ab.result->size() == 2,
        "[Test] Wrong result size for line_cylinder_intersection with "
        "query line_ab" );
    OPENGEODE_EXCEPTION( result_ab.result->front() == a,
        "[Test] Wrong result values for line_cylinder_intersection with "
        "query line_ab" );
    OPENGEODE_EXCEPTION( result_ab.result->back() == b,
        "[Test] Wrong result values for line_cylinder_intersection with "
        "query line_ab" );

    const geode::Point3D c{ { 0.5, 0.0, 0.0 } };
    const geode::Point3D d{ { 0.5, 1.0, 0.0 } };
    const geode::Segment3D segment_cd{ c, d };
    const geode::InfiniteLine3D line_cd{ segment_cd };
    const auto result_cd =
        geode::line_cylinder_intersection( line_cd, cylinder );
    OPENGEODE_EXCEPTION( result_cd,
        "[Test] Wrong result for line_cylinder_intersection with "
        "query line_cd" );
    OPENGEODE_EXCEPTION( result_cd.result->size() == 2,
        "[Test] Wrong result size for line_cylinder_intersection with "
        "query line_cd" );
    const geode::Point3D answer_cd0{ { 0.5, -0.2, 0.0 } };
    const geode::Point3D answer_cd1{ { 0.5, 0.2, 0.0 } };
    OPENGEODE_EXCEPTION( result_cd.result->front() == answer_cd0,
        "[Test] Wrong result values for line_cylinder_intersection with "
        "query line_cd" );
    OPENGEODE_EXCEPTION( result_cd.result->back() == answer_cd1,
        "[Test] Wrong result values for line_cylinder_intersection with "
        "query line_cd" );

    const geode::Point3D e{ { 0.2, 0.1, 0.0 } };
    const geode::Point3D f{ { 0.4, 0.2, 0.0 } };
    const geode::Segment3D segment_ef{ e, f };
    const geode::InfiniteLine3D line_ef{ segment_ef };
    const auto result_ef =
        geode::line_cylinder_intersection( line_ef, cylinder );
    OPENGEODE_EXCEPTION( result_ef,
        "[Test] Wrong result for line_cylinder_intersection with "
        "query line_ef" );
    OPENGEODE_EXCEPTION( result_ef.result->size() == 2,
        "[Test] Wrong result size for line_cylinder_intersection with "
        "query line_ef" );
    const geode::Point3D answer_ef1{ { 0.4, 0.2, 0.0 } };
    OPENGEODE_EXCEPTION( result_ef.result->front() == a,
        "[Test] Wrong result values for line_cylinder_intersection with "
        "query line_ef" );
    OPENGEODE_EXCEPTION( result_ef.result->back() == answer_ef1,
        "[Test] Wrong result values for line_cylinder_intersection with "
        "query line_ef" );
}

void test_segment_cylinder_intersection()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Segment3D segment_ab{ a, b };
    const geode::Cylinder cylinder{ segment_ab, 0.2 };

    const auto result_ab =
        geode::segment_cylinder_intersection( segment_ab, cylinder );
    OPENGEODE_EXCEPTION( result_ab,
        "[Test] Wrong result for segment_cylinder_intersection with "
        "query segment_ab" );
    OPENGEODE_EXCEPTION( result_ab.result->size() == 2,
        "[Test] Wrong result size for segment_cylinder_intersection with "
        "query segment_ab" );
    OPENGEODE_EXCEPTION( result_ab.result->front() == a,
        "[Test] Wrong result values for segment_cylinder_intersection with "
        "query segment_ab" );
    OPENGEODE_EXCEPTION( result_ab.result->back() == b,
        "[Test] Wrong result values for segment_cylinder_intersection with "
        "query segment_ab" );

    const geode::Point3D c{ { 0.5, 0.0, 0.0 } };
    const geode::Point3D d{ { 0.5, 1.0, 0.0 } };
    const geode::Segment3D segment_cd{ c, d };
    const auto result_cd =
        geode::segment_cylinder_intersection( segment_cd, cylinder );
    OPENGEODE_EXCEPTION( result_cd,
        "[Test] Wrong result for segment_cylinder_intersection with "
        "query segment_cd" );
    OPENGEODE_EXCEPTION( result_cd.result->size() == 1,
        "[Test] Wrong result size for segment_cylinder_intersection with "
        "query segment_cd" );
    const geode::Point3D answer_cd{ { 0.5, 0.2, 0.0 } };
    OPENGEODE_EXCEPTION( result_cd.result->front() == answer_cd,
        "[Test] Wrong result values for segment_cylinder_intersection with "
        "query segment_cd" );

    const geode::Point3D e{ { 0.2, 0.1, 0.0 } };
    const geode::Point3D f{ { 0.3, 0.15, 0.0 } };
    const geode::Segment3D segment_ef{ e, f };
    const auto result_ef =
        geode::segment_cylinder_intersection( segment_ef, cylinder );
    OPENGEODE_EXCEPTION( !result_ef,
        "[Test] Wrong result for segment_cylinder_intersection with "
        "query segment_ef" );
}

void test_plane_plane_intersection()
{
    const geode::Vector3D Z{ { 0, 0, 1 } };
    const geode::Point3D O{ { 0, 0, 0 } };
    const geode::Plane planeZO{ Z, O };
    const auto OO_result = geode::plane_plane_intersection( planeZO, planeZO );
    OPENGEODE_EXCEPTION( OO_result.type == geode::IntersectionType::PARALLEL,
        "[Test] Wrong intersection between planeZO, planeZO" );

    const geode::Point3D o{ { 0, 0, 1 } };
    const geode::Plane planeZo{ Z, o };
    const auto Oo_result = geode::plane_plane_intersection( planeZO, planeZo );
    OPENGEODE_EXCEPTION(
        !Oo_result, "[Test] Wrong intersection between planeZO, planeZo" );

    const geode::Vector3D XY{ { 1, 1, 0 } };
    const geode::Plane planeXY0{ XY, o };
    const auto XYO_result =
        geode::plane_plane_intersection( planeZO, planeXY0 );
    OPENGEODE_EXCEPTION(
        XYO_result, "[Test] Wrong intersection between planeZO, planeXY0" );
    OPENGEODE_EXCEPTION( XYO_result.result->origin().inexact_equal( O ),
        "[Test] Wrong intersection between planeZO, planeXY0" );
    const auto answer = geode::Vector3D{ { -1, 1, 0 } }.normalize();
    OPENGEODE_EXCEPTION( XYO_result.result->direction().inexact_equal( answer ),
        "[Test] Wrong intersection between planeZO, planeXY0" );
}

void test_plane_circle_intersection()
{
    const geode::Vector3D Z{ { 0, 0, 1 } };
    const geode::Point3D O{ { 0, 0, 0 } };
    const geode::Plane planeZO{ Z, O };
    const geode::Circle circleZO{ planeZO, 42 };
    const auto OO_result =
        geode::plane_circle_intersection( planeZO, circleZO );
    OPENGEODE_EXCEPTION( OO_result.type == geode::IntersectionType::PARALLEL,
        "[Test] Wrong intersection between planeZO, circleZO" );

    const geode::Point3D o{ { 0, 0, 1 } };
    const geode::Plane planeZo{ Z, o };
    const geode::Circle circleZo{ planeZo, 42 };
    const auto Oo_result =
        geode::plane_circle_intersection( planeZO, circleZo );
    OPENGEODE_EXCEPTION(
        !Oo_result, "[Test] Wrong intersection between planeZO, circleZo" );

    const geode::Vector3D XY{ { 1, 1, 0 } };
    const geode::Plane planeXY0{ XY, o };
    const geode::Circle circleYZ42{ planeXY0, 42 };
    const auto XY42_result =
        geode::plane_circle_intersection( planeZO, circleYZ42 );
    OPENGEODE_EXCEPTION(
        XY42_result, "[Test] Wrong intersection between planeZO, circleYZ42" );
    OPENGEODE_EXCEPTION( XY42_result.result->size() == 2,
        "[Test] Wrong intersection between planeZO, circleYZ42" );
    const double value42{ 29.690065678607 };
    const geode::Point3D answer420{ { value42, -value42, 0 } };
    OPENGEODE_EXCEPTION( XY42_result.result->at( 0 ).inexact_equal( answer420 ),
        "[Test] Wrong intersection between planeZO, circleYZ42" );
    const geode::Point3D answer421{ { -value42, value42, 0 } };
    OPENGEODE_EXCEPTION( XY42_result.result->at( 1 ).inexact_equal( answer421 ),
        "[Test] Wrong intersection between planeZO, circleYZ42" );

    const geode::Circle circleYZ1{ planeXY0, 1 };
    const auto XY1_result =
        geode::plane_circle_intersection( planeZO, circleYZ1 );
    OPENGEODE_EXCEPTION(
        XY1_result, "[Test] Wrong intersection between planeZO, circleYZ1" );
    OPENGEODE_EXCEPTION( XY1_result.result->size() == 1,
        "[Test] Wrong intersection between planeZO, circleYZ1" );
    OPENGEODE_EXCEPTION( XY1_result.result->at( 0 ).inexact_equal( O ),
        "[Test] Wrong intersection between planeZO, circleYZ1" );

    const geode::Circle circleYZ0{ planeXY0, 0.1 };
    const auto XY0_result =
        geode::plane_circle_intersection( planeZO, circleYZ0 );
    OPENGEODE_EXCEPTION(
        !XY0_result, "[Test] Wrong intersection between planeZO, circleYZ0" );
}

void test_triangle_circle_intersection()
{
    const geode::Vector3D Z{ { 0, 0, 1 } };
    const geode::Point3D O{ { 0, 0, 0 } };
    const geode::Plane planeZO{ Z, O };
    const geode::Circle circleZO{ planeZO, 42 };
    const geode::Point3D A{ { 1, 1, 0 } };
    const geode::Point3D B{ { 1, -1, 0 } };
    const geode::Point3D C{ { -1, 0, 0 } };
    const geode::Triangle3D triangleABC{ A, B, C };
    const auto Oabc_result =
        geode::triangle_circle_intersection( triangleABC, circleZO );
    OPENGEODE_EXCEPTION( Oabc_result.type == geode::IntersectionType::PARALLEL,
        "[Test] Wrong intersection between triangleABC, circleZO" );

    const geode::Point3D o{ { 0, 0, 1 } };
    const geode::Plane planeZo{ Z, o };
    const geode::Circle circleZo{ planeZo, 42 };
    const auto oabc_result =
        geode::triangle_circle_intersection( triangleABC, circleZo );
    OPENGEODE_EXCEPTION( !oabc_result,
        "[Test] Wrong intersection between triangleABC, circleZo" );

    const geode::Vector3D XY{ { 1, 1, 0 } };
    const geode::Plane planeXY0{ XY, o };
    const geode::Circle circleYZ11{ planeXY0, 1.1 };
    const auto XY11_result =
        geode::triangle_circle_intersection( triangleABC, circleYZ11 );
    OPENGEODE_EXCEPTION( XY11_result,
        "[Test] Wrong intersection between triangleABC, circleYZ11" );
    OPENGEODE_EXCEPTION( XY11_result.result->size() == 2,
        "[Test] Wrong intersection between triangleABC, circleYZ11" );
    const double value11{ 0.324037034920393 };
    const geode::Point3D answer110{ { -value11, value11, 0 } };
    OPENGEODE_EXCEPTION( XY11_result.result->at( 0 ).inexact_equal( answer110 ),
        "[Test] Wrong intersection between triangleABC, circleYZ11" );
    const geode::Point3D answer111{ { value11, -value11, 0 } };
    OPENGEODE_EXCEPTION( XY11_result.result->at( 1 ).inexact_equal( answer111 ),
        "[Test] Wrong intersection between triangleABC, circleYZ11" );

    const geode::Circle circleYZ1{ planeXY0, 1 };
    const auto XY1_result =
        geode::triangle_circle_intersection( triangleABC, circleYZ1 );
    OPENGEODE_EXCEPTION( XY1_result,
        "[Test] Wrong intersection between triangleABC, circleYZ1" );
    OPENGEODE_EXCEPTION( XY1_result.result->size() == 1,
        "[Test] Wrong intersection between triangleABC, circleYZ1" );
    OPENGEODE_EXCEPTION( XY1_result.result->at( 0 ).inexact_equal( O ),
        "[Test] Wrong intersection between triangleABC, circleYZ1" );

    const geode::Circle circleYZ0{ planeXY0, 0.1 };
    const auto XY0_result =
        geode::plane_circle_intersection( planeZO, circleYZ0 );
    OPENGEODE_EXCEPTION(
        !XY0_result, "[Test] Wrong intersection between planeZO, circleYZ0" );
}

void test()
{
    test_line_sphere_intersection();
    test_segment_sphere_intersection();
    test_line_line_intersection();
    test_segment_segment_intersection();
    test_segment_line_intersection();
    test_segment_plane_intersection();
    test_segment_triangle_intersection();
    test_line_cylinder_intersection();
    test_segment_cylinder_intersection();
    test_plane_plane_intersection();
    test_plane_circle_intersection();
    test_triangle_circle_intersection();
}

OPENGEODE_TEST( "intersection" )
