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

#include <string_view>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/cylinder.hpp>
#include <geode/geometry/basic_objects/ellipse.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/frame.hpp>
#include <geode/geometry/intersection.hpp>
#include <geode/geometry/point.hpp>

#include <geode/tests/common.hpp>

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
    geode::OpenGeodeGeometryException::test(
        intersection_points1 && intersection_points1.result.value().size() == 2,
        "Wrong result for line_sphere_intersection "
        "with query line1" );
    geode::OpenGeodeGeometryException::test(
        std::fabs( intersection_points1.result.value().front().value( 0 ) - 2 )
                    - std::sqrt( 3 ) / 2
                < geode::GLOBAL_EPSILON
            && std::fabs(
                   intersection_points1.result.value().back().value( 0 ) - 2 )
                       - std::sqrt( 3 ) / 2
                   < geode::GLOBAL_EPSILON
            && intersection_points1.result.value().front().value( 1 ) == 0.5
            && intersection_points1.result.value().back().value( 1 ),
        "Wrong result for line_sphere_intersection "
        "with query line 1 (coords)" );

    const auto intersection_points2 =
        geode::line_sphere_intersection( line2, sphere );
    geode::OpenGeodeGeometryException::test(
        intersection_points2 && intersection_points2.result.value().size() == 1,
        "Wrong result for line_sphere_intersection "
        "with query line 2" );
    const geode::Point2D result{ { 2.0, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        intersection_points2.result.value().front() == result,
        "Wrong result for line_sphere_intersection "
        "with query line 2 (coords)" );

    const auto intersection_points3 =
        geode::line_sphere_intersection( line3, sphere );
    geode::OpenGeodeGeometryException::test( !intersection_points3,
        "Wrong result for line_sphere_intersection "
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
    geode::OpenGeodeGeometryException::test( !intersection_points1,
        "Wrong result for line_sphere_intersection "
        "with query segment 1" );

    const auto intersection_points2 =
        geode::segment_sphere_intersection( segment2, sphere );
    geode::OpenGeodeGeometryException::test(
        intersection_points2 && intersection_points2.result.value().size() == 1,
        "Wrong result for line_sphere_intersection "
        "with query segment 2" );

    const auto intersection_points3 =
        geode::segment_sphere_intersection( segment3, sphere );
    geode::OpenGeodeGeometryException::test(
        intersection_points3 && intersection_points3.result.value().size() == 1,
        "Wrong result for line_sphere_intersection "
        "with query segment 3" );

    const auto intersection_points4 =
        geode::segment_sphere_intersection( segment4, sphere );
    geode::OpenGeodeGeometryException::test(
        intersection_points4 && intersection_points4.result.value().size() == 2,
        "Wrong result for line_sphere_intersection "
        "with query segment 4" );

    const auto intersection_points5 =
        geode::segment_sphere_intersection( segment5, sphere );
    geode::OpenGeodeGeometryException::test( !intersection_points5,
        "Wrong result for line_sphere_intersection "
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
    geode::OpenGeodeGeometryException::test( !intersection_point,
        "Wrong result for line_line_intersection "
        "with query lines 1 and 2" );

    const auto intersection_point1 =
        geode::line_line_intersection( line1, line3 );
    geode::OpenGeodeGeometryException::test(
        intersection_point1 && intersection_point1.result.value() == b,
        "Wrong result for line_line_intersection with query lines 1 "
        "and 3" );

    const auto intersection_point2 =
        geode::line_line_intersection( line1, line4 );
    geode::OpenGeodeGeometryException::test( !intersection_point2,
        "Wrong result for line_line_intersection "
        "with query lines 1 and 4" );

    geode::OpenGeodeGeometryException::test(
        intersection_point.type == geode::INTERSECTION_TYPE::parallel,
        "Wrong type for line_line_intersection with query lines 1 and 2" );
    geode::OpenGeodeGeometryException::test(
        intersection_point2.type == geode::INTERSECTION_TYPE::parallel,
        "Wrong type for line_line_intersection with query lines 1 and 4" );

    const auto intersection_point3 =
        geode::line_line_intersection( line1, line5 );
    geode::OpenGeodeGeometryException::test(
        intersection_point3 && intersection_point3.result.value() == a,
        "Wrong result for line_line_intersection with query lines 1 "
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
    const geode::Segment2D segment_bf{ b, f };
    const geode::Segment2D segment_cd{ c, d };
    const geode::Segment2D segment_cf{ c, f };
    const geode::Segment2D segment_de{ d, e };
    const geode::Segment2D segment_df{ d, f };
    const geode::Segment2D segment_ef{ e, f };

    const auto intersection_point =
        geode::segment_segment_intersection( segment_cf, segment_de );
    const geode::Point2D result_cf_de{ { 2.5, 0.5 } };
    geode::OpenGeodeGeometryException::test(
        intersection_point && intersection_point.result.value() == result_cf_de,
        "Wrong result for segment_segment_intersection "
        "with query segment_cf and segment_de" );

    const auto intersection_point1 =
        geode::segment_segment_intersection( segment_bc, segment_af );
    geode::OpenGeodeGeometryException::test(
        intersection_point1 && intersection_point1.result.value() == b,
        "Wrong result for segment_segment_intersection with query "
        "segment_bc and segment_af" );

    const auto intersection_point2 =
        geode::segment_segment_intersection( segment_cd, segment_df );
    geode::OpenGeodeGeometryException::test(
        intersection_point2 && intersection_point2.result.value() == d,
        "Wrong result for segment_segment_intersection "
        "with query segment_cd and segment_df" );

    const auto intersection_point3 =
        geode::segment_segment_intersection( segment_ab, segment_cd );
    geode::OpenGeodeGeometryException::test( !intersection_point3,
        "Wrong result for segment_segment_intersection with query "
        "segment_ab and segment_cd" );

    const auto intersection_point4 =
        geode::segment_segment_intersection( segment_ab, segment_ab2 );
    geode::OpenGeodeGeometryException::test( !intersection_point4,
        "Wrong result for segment_segment_intersection with query "
        "segment_ab and segment_ab2" );

    const auto intersection_point5 =
        geode::segment_segment_intersection( segment_ab, segment_ef );
    geode::OpenGeodeGeometryException::test( !intersection_point5,
        "Wrong result for segment_segment_intersection with query "
        "segment_ab and segment_ef" );

    geode::OpenGeodeGeometryException::test(
        intersection_point3.type == geode::INTERSECTION_TYPE::parallel,
        "Wrong type for segment_segment_intersection with query "
        "segment_ab and segment_cd" );
    const geode::Segment2D segment_ae{ a, e };
    const auto collinear_overlap =
        geode::segment_segment_intersection( segment_ae, segment_bf );
    geode::OpenGeodeGeometryException::test(
        collinear_overlap.type == geode::INTERSECTION_TYPE::parallel,
        "Wrong type for segment_segment_intersection with collinear "
        "overlapping segments" );

    const auto intersection_point6 =
        geode::segment_segment_intersection( segment_bc, segment_df );
    geode::OpenGeodeGeometryException::test( !intersection_point6,
        "Wrong result for segment_segment_intersection with query "
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
    geode::OpenGeodeGeometryException::test(
        intersection_point && intersection_point.result.value() == result_de_cf,
        "Wrong result for segment_line_intersection "
        "with query segment_de and line_cf" );

    const auto intersection_point1 =
        geode::segment_line_intersection( segment_ef, line_df );
    geode::OpenGeodeGeometryException::test(
        intersection_point1 && intersection_point1.result.value() == f,
        "Wrong result for segment_line_intersection "
        "with query segment_ef and line_df" );

    const auto intersection_point2 =
        geode::segment_line_intersection( segment_cd, line_ef );
    geode::OpenGeodeGeometryException::test( !intersection_point2,
        "Wrong result for segment_line_intersection "
        "with query segment_cd and line_ef" );

    const auto intersection_point3 =
        geode::segment_line_intersection( segment_ab, line_ef );
    geode::OpenGeodeGeometryException::test( !intersection_point3,
        "Wrong result for segment_line_intersection "
        "with query segment_ab and line_ef" );

    const auto intersection_point4 =
        geode::segment_line_intersection( segment_ab, line_ce );
    geode::OpenGeodeGeometryException::test( !intersection_point4,
        "Wrong result for segment_line_intersection "
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
    geode::OpenGeodeGeometryException::test( !intersect,
        "Wrong result for segment_plane_intersection with "
        "query segment_de" );

    const auto intersect1 =
        geode::segment_plane_intersection( segment_da, plane );
    geode::OpenGeodeGeometryException::test( !intersect1,
        "Wrong result for segment_plane_intersection with "
        "query segment_da" );

    const auto intersect2 =
        geode::segment_plane_intersection( segment_fg, plane );
    geode::OpenGeodeGeometryException::test( !intersect2,
        "Wrong result for segment_plane_intersection with "
        "query segment_fg" );

    const auto intersect3 =
        geode::segment_plane_intersection( segment_hi, plane );
    geode::OpenGeodeGeometryException::test( intersect3.result.value() == a,
        "Wrong result for segment_plane_intersection with "
        "query segment_hi" );

    const auto intersect4 =
        geode::segment_plane_intersection( segment_hj, plane );
    geode::Point3D answer4{ { 0.5, 0.5, 0.0 } };
    double distance_to_answer =
        geode::point_point_distance( intersect4.result.value(), answer4 );
    geode::OpenGeodeGeometryException::test(
        distance_to_answer < geode::GLOBAL_EPSILON,
        "Wrong result for segment_plane_intersection with "
        "query segment_hj" );

    const auto intersect5 =
        geode::segment_plane_intersection( segment_ki, plane );
    geode::Point3D answer{ { 0.5, 0.25, 0.0 } };
    distance_to_answer =
        geode::point_point_distance( intersect5.result.value(), answer );
    geode::OpenGeodeGeometryException::test(
        distance_to_answer < geode::GLOBAL_EPSILON,
        "Wrong result for segment_plane_intersection with "
        "query segment_ki" );

    const auto intersect6 =
        geode::segment_plane_intersection( segment_li, plane );
    geode::Point3D answer6{ { -4.5, 0.0, 0.0 } };
    distance_to_answer =
        geode::point_point_distance( intersect6.result.value(), answer6 );
    geode::OpenGeodeGeometryException::test(
        distance_to_answer < geode::GLOBAL_EPSILON,
        "Wrong result for segment_plane_intersection with "
        "query segment_li" );

    const auto intersect7 =
        geode::segment_plane_intersection( segment_kb, plane );
    geode::OpenGeodeGeometryException::test( intersect7.result.value() == b,
        "Wrong result for segment_plane_intersection with "
        "query segment_kb" );

    const auto intersect8 =
        geode::segment_plane_intersection( segment_mn, plane );
    geode::OpenGeodeGeometryException::test( !intersect8,
        "Wrong result for segment_plane_intersection with "
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
    geode::OpenGeodeGeometryException::test( !intersection,
        "Wrong result for segment_triangle_intersection with "
        "query segment_de" );

    const auto intersection1 =
        geode::segment_triangle_intersection( segment_da, triangle );
    geode::OpenGeodeGeometryException::test( !intersection1,
        "Wrong result for segment_triangle_intersection with "
        "query segment_da" );

    const auto intersection2 =
        geode::segment_triangle_intersection( segment_fg, triangle );
    geode::OpenGeodeGeometryException::test( !intersection2,
        "Wrong result for segment_triangle_intersection with "
        "query segment_fg" );

    const auto intersection3 =
        geode::segment_triangle_intersection( segment_hi, triangle );
    geode::OpenGeodeGeometryException::test( intersection3.result.value() == a,
        "Wrong result for segment_triangle_intersection with "
        "query segment_hi" );

    const auto intersection4 =
        geode::segment_triangle_intersection( segment_hj, triangle );
    geode::Point3D answer{ { 0.5, 0.5, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        intersection4.result.value() == answer,
        "Wrong result for segment_triangle_intersection with "
        "query segment_hj" );

    const auto intersection5 =
        geode::segment_triangle_intersection( segment_ki, triangle );
    geode::Point3D answer5{ { 0.5, 0.25, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        intersection5.result.value() == answer5,
        "Wrong result for segment_triangle_intersection with "
        "query segment_ki" );

    const auto intersection6 =
        geode::segment_triangle_intersection( segment_li, triangle );
    geode::OpenGeodeGeometryException::test( !intersection6,
        "Wrong result for segment_triangle_intersection with "
        "query segment_li" );

    const auto intersection7 =
        geode::segment_triangle_intersection( segment_kb, triangle );
    geode::OpenGeodeGeometryException::test( intersection7.result.value() == b,
        "Wrong result for segment_triangle_intersection with "
        "query segment_kb" );

    const auto intersection8 =
        geode::segment_triangle_intersection( segment_mn, triangle );
    geode::OpenGeodeGeometryException::test( !intersection8,
        "Wrong result for segment_triangle_intersection with "
        "query segment_mn" );
}

void test_segment_triangle_intersection_touching_endpoint()
{
    const geode::Point3D v0{ { 0.1, 0.2, 0.3 } };
    const geode::Point3D v1{ { 1.7, 0.4, 0.9 } };
    const geode::Point3D v2{ { 0.3, 1.9, 1.1 } };
    const geode::Triangle3D triangle{ v0, v1, v2 };
    const std::array< std::array< double, 3 >, 8 > barycentrics{ {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 },
        { 0.5, 0.25, 0.25 },
        { 0.25, 0.5, 0.25 },
        { 0.25, 0.25, 0.5 },
        { 0.125, 0.375, 0.5 },
        { 0.5, 0.5, 0 },
    } };
    const std::array< geode::Vector3D, 4 > offsets{ {
        geode::Vector3D{ { 0.3, -0.2, 1.0 } },
        geode::Vector3D{ { -0.7, 0.4, -0.5 } },
        geode::Vector3D{ { 1.3, 2.1, 0.1 } },
        geode::Vector3D{ { -0.05, -0.6, 0.9 } },
    } };
    for( const auto& lambdas : barycentrics )
    {
        const auto endpoint =
            v0 * lambdas[0] + v1 * lambdas[1] + v2 * lambdas[2];
        for( const auto& offset : offsets )
        {
            const auto other = endpoint + offset;
            for( const auto reversed : { false, true } )
            {
                const auto segment = reversed
                                         ? geode::Segment3D{ other, endpoint }
                                         : geode::Segment3D{ endpoint, other };
                const auto result =
                    geode::segment_triangle_intersection( segment, triangle );
                geode::OpenGeodeGeometryException::test(
                    result.result.has_value()
                        && geode::point_point_distance(
                               result.result.value(), endpoint )
                               < geode::GLOBAL_EPSILON,
                    "Wrong result for segment_triangle_intersection with "
                    "segment touching the triangle at ",
                    endpoint.string(), " from ", other.string() );
            }
            // Segment crossing the triangle close to its first vertex
            for( const auto ratio : { 1e-9, 1e-6, 1e-3 } )
            {
                const auto start = endpoint - offset * ratio;
                const auto result = geode::segment_triangle_intersection(
                    geode::Segment3D{ start, other }, triangle );
                geode::OpenGeodeGeometryException::test(
                    result.result.has_value()
                        && geode::point_point_distance(
                               result.result.value(), endpoint )
                               < geode::GLOBAL_EPSILON,
                    "Wrong result for segment_triangle_intersection with "
                    "segment crossing the triangle at ",
                    endpoint.string(), " from ", start.string() );
            }
        }
    }
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
    geode::OpenGeodeGeometryException::test( result_ab,
        "Wrong result for line_cylinder_intersection with "
        "query line_ab" );
    geode::OpenGeodeGeometryException::test( result_ab.result->size() == 2,
        "Wrong result size for line_cylinder_intersection with "
        "query line_ab" );
    geode::OpenGeodeGeometryException::test( result_ab.result->front() == a,
        "Wrong result values for line_cylinder_intersection with "
        "query line_ab" );
    geode::OpenGeodeGeometryException::test( result_ab.result->back() == b,
        "Wrong result values for line_cylinder_intersection with "
        "query line_ab" );

    const geode::Point3D c{ { 0.5, 0.0, 0.0 } };
    const geode::Point3D d{ { 0.5, 1.0, 0.0 } };
    const geode::Segment3D segment_cd{ c, d };
    const geode::InfiniteLine3D line_cd{ segment_cd };
    const auto result_cd =
        geode::line_cylinder_intersection( line_cd, cylinder );
    geode::OpenGeodeGeometryException::test( result_cd,
        "Wrong result for line_cylinder_intersection with "
        "query line_cd" );
    geode::OpenGeodeGeometryException::test( result_cd.result->size() == 2,
        "Wrong result size for line_cylinder_intersection with "
        "query line_cd" );
    const geode::Point3D answer_cd0{ { 0.5, -0.2, 0.0 } };
    const geode::Point3D answer_cd1{ { 0.5, 0.2, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        result_cd.result->front() == answer_cd0,
        "Wrong result values for line_cylinder_intersection with "
        "query line_cd" );
    geode::OpenGeodeGeometryException::test(
        result_cd.result->back() == answer_cd1,
        "Wrong result values for line_cylinder_intersection with "
        "query line_cd" );

    const geode::Point3D e{ { 0.2, 0.1, 0.0 } };
    const geode::Point3D f{ { 0.4, 0.2, 0.0 } };
    const geode::Segment3D segment_ef{ e, f };
    const geode::InfiniteLine3D line_ef{ segment_ef };
    const auto result_ef =
        geode::line_cylinder_intersection( line_ef, cylinder );
    geode::OpenGeodeGeometryException::test( result_ef,
        "Wrong result for line_cylinder_intersection with "
        "query line_ef" );
    geode::OpenGeodeGeometryException::test( result_ef.result->size() == 2,
        "Wrong result size for line_cylinder_intersection with "
        "query line_ef" );
    const geode::Point3D answer_ef1{ { 0.4, 0.2, 0.0 } };
    geode::OpenGeodeGeometryException::test( result_ef.result->front() == a,
        "Wrong result values for line_cylinder_intersection with "
        "query line_ef" );
    geode::OpenGeodeGeometryException::test(
        result_ef.result->back() == answer_ef1,
        "Wrong result values for line_cylinder_intersection with "
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
    geode::OpenGeodeGeometryException::test( result_ab,
        "Wrong result for segment_cylinder_intersection with "
        "query segment_ab" );
    geode::OpenGeodeGeometryException::test( result_ab.result->size() == 2,
        "Wrong result size for segment_cylinder_intersection with "
        "query segment_ab" );
    geode::OpenGeodeGeometryException::test( result_ab.result->front() == a,
        "Wrong result values for segment_cylinder_intersection with "
        "query segment_ab" );
    geode::OpenGeodeGeometryException::test( result_ab.result->back() == b,
        "Wrong result values for segment_cylinder_intersection with "
        "query segment_ab" );

    const geode::Point3D c{ { 0.5, 0.0, 0.0 } };
    const geode::Point3D d{ { 0.5, 1.0, 0.0 } };
    const geode::Segment3D segment_cd{ c, d };
    const auto result_cd =
        geode::segment_cylinder_intersection( segment_cd, cylinder );
    geode::OpenGeodeGeometryException::test( result_cd,
        "Wrong result for segment_cylinder_intersection with "
        "query segment_cd" );
    geode::OpenGeodeGeometryException::test( result_cd.result->size() == 1,
        "Wrong result size for segment_cylinder_intersection with "
        "query segment_cd" );
    const geode::Point3D answer_cd{ { 0.5, 0.2, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        result_cd.result->front() == answer_cd,
        "Wrong result values for segment_cylinder_intersection with "
        "query segment_cd" );

    const geode::Point3D e{ { 0.2, 0.1, 0.0 } };
    const geode::Point3D f{ { 0.3, 0.15, 0.0 } };
    const geode::Segment3D segment_ef{ e, f };
    const auto result_ef =
        geode::segment_cylinder_intersection( segment_ef, cylinder );
    geode::OpenGeodeGeometryException::test( !result_ef,
        "Wrong result for segment_cylinder_intersection with "
        "query segment_ef" );
}

void test_plane_plane_intersection()
{
    const geode::Vector3D Z{ { 0, 0, 1 } };
    const geode::Point3D O{ { 0, 0, 0 } };
    const geode::Plane planeZO{ Z, O };
    const auto OO_result = geode::plane_plane_intersection( planeZO, planeZO );
    geode::OpenGeodeGeometryException::test(
        OO_result.type == geode::INTERSECTION_TYPE::parallel,
        "Wrong intersection between planeZO, planeZO" );

    const geode::Point3D o{ { 0, 0, 1 } };
    const geode::Plane planeZo{ Z, o };
    const auto Oo_result = geode::plane_plane_intersection( planeZO, planeZo );
    geode::OpenGeodeGeometryException::test(
        !Oo_result, "Wrong intersection between planeZO, planeZo" );

    const geode::Vector3D XY{ { 1, 1, 0 } };
    const geode::Plane planeXY0{ XY, o };
    const auto XYO_result =
        geode::plane_plane_intersection( planeZO, planeXY0 );
    geode::OpenGeodeGeometryException::test(
        XYO_result, "Wrong intersection between planeZO, planeXY0" );
    geode::OpenGeodeGeometryException::test(
        XYO_result.result->origin().inexact_equal( O ),
        "Wrong intersection between planeZO, planeXY0" );
    const auto answer = geode::Vector3D{ { -1, 1, 0 } }.normalize();
    geode::OpenGeodeGeometryException::test(
        XYO_result.result->direction().inexact_equal( answer ),
        "Wrong intersection between planeZO, planeXY0" );
}

void test_plane_circle_intersection()
{
    const geode::Vector3D Z{ { 0, 0, 1 } };
    const geode::Point3D O{ { 0, 0, 0 } };
    const geode::Plane planeZO{ Z, O };
    const geode::Circle circleZO{ planeZO, 42 };
    const auto OO_result =
        geode::plane_circle_intersection( planeZO, circleZO );
    geode::OpenGeodeGeometryException::test(
        OO_result.type == geode::INTERSECTION_TYPE::parallel,
        "Wrong intersection between planeZO, circleZO" );

    const geode::Point3D o{ { 0, 0, 1 } };
    const geode::Plane planeZo{ Z, o };
    const geode::Circle circleZo{ planeZo, 42 };
    const auto Oo_result =
        geode::plane_circle_intersection( planeZO, circleZo );
    geode::OpenGeodeGeometryException::test(
        !Oo_result, "Wrong intersection between planeZO, circleZo" );

    const geode::Vector3D XY{ { 1, 1, 0 } };
    const geode::Plane planeXY0{ XY, o };
    const geode::Circle circleYZ42{ planeXY0, 42 };
    const auto XY42_result =
        geode::plane_circle_intersection( planeZO, circleYZ42 );
    geode::OpenGeodeGeometryException::test(
        XY42_result, "Wrong intersection between planeZO, circleYZ42" );
    geode::OpenGeodeGeometryException::test( XY42_result.result->size() == 2,
        "Wrong intersection between planeZO, circleYZ42" );
    const double value42{ 29.690065678607 };
    const geode::Point3D answer420{ { value42, -value42, 0 } };
    geode::OpenGeodeGeometryException::test(
        XY42_result.result->at( 0 ).inexact_equal( answer420 ),
        "Wrong intersection between planeZO, circleYZ42" );
    const geode::Point3D answer421{ { -value42, value42, 0 } };
    geode::OpenGeodeGeometryException::test(
        XY42_result.result->at( 1 ).inexact_equal( answer421 ),
        "Wrong intersection between planeZO, circleYZ42" );

    const geode::Circle circleYZ1{ planeXY0, 1 };
    const auto XY1_result =
        geode::plane_circle_intersection( planeZO, circleYZ1 );
    geode::OpenGeodeGeometryException::test(
        XY1_result, "Wrong intersection between planeZO, circleYZ1" );
    geode::OpenGeodeGeometryException::test( XY1_result.result->size() == 1,
        "Wrong intersection between planeZO, circleYZ1" );
    geode::OpenGeodeGeometryException::test(
        XY1_result.result->at( 0 ).inexact_equal( O ),
        "Wrong intersection between planeZO, circleYZ1" );

    const geode::Circle circleYZ0{ planeXY0, 0.1 };
    const auto XY0_result =
        geode::plane_circle_intersection( planeZO, circleYZ0 );
    geode::OpenGeodeGeometryException::test(
        !XY0_result, "Wrong intersection between planeZO, circleYZ0" );
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
    geode::OpenGeodeGeometryException::test(
        Oabc_result.type == geode::INTERSECTION_TYPE::parallel,
        "Wrong intersection between triangleABC, circleZO" );

    const geode::Point3D o{ { 0, 0, 1 } };
    const geode::Plane planeZo{ Z, o };
    const geode::Circle circleZo{ planeZo, 42 };
    const auto oabc_result =
        geode::triangle_circle_intersection( triangleABC, circleZo );
    geode::OpenGeodeGeometryException::test(
        !oabc_result, "Wrong intersection between triangleABC, circleZo" );

    const geode::Vector3D XY{ { 1, 1, 0 } };
    const geode::Plane planeXY0{ XY, o };
    const geode::Circle circleYZ11{ planeXY0, 1.1 };
    const auto XY11_result =
        geode::triangle_circle_intersection( triangleABC, circleYZ11 );
    geode::OpenGeodeGeometryException::test(
        XY11_result, "Wrong intersection between triangleABC, circleYZ11" );
    geode::OpenGeodeGeometryException::test( XY11_result.result->size() == 2,
        "Wrong intersection between triangleABC, circleYZ11" );
    const double value11{ 0.324037034920393 };
    const geode::Point3D answer110{ { -value11, value11, 0 } };
    geode::OpenGeodeGeometryException::test(
        XY11_result.result->at( 0 ).inexact_equal( answer110 ),
        "Wrong intersection between triangleABC, circleYZ11" );
    const geode::Point3D answer111{ { value11, -value11, 0 } };
    geode::OpenGeodeGeometryException::test(
        XY11_result.result->at( 1 ).inexact_equal( answer111 ),
        "Wrong intersection between triangleABC, circleYZ11" );

    const geode::Circle circleYZ1{ planeXY0, 1 };
    const auto XY1_result =
        geode::triangle_circle_intersection( triangleABC, circleYZ1 );
    geode::OpenGeodeGeometryException::test(
        XY1_result, "Wrong intersection between triangleABC, circleYZ1" );
    geode::OpenGeodeGeometryException::test( XY1_result.result->size() == 1,
        "Wrong intersection between triangleABC, circleYZ1" );
    geode::OpenGeodeGeometryException::test(
        XY1_result.result->at( 0 ).inexact_equal( O ),
        "Wrong intersection between triangleABC, circleYZ1" );

    const geode::Circle circleYZ0{ planeXY0, 0.1 };
    const auto XY0_result =
        geode::plane_circle_intersection( planeZO, circleYZ0 );
    geode::OpenGeodeGeometryException::test(
        !XY0_result, "Wrong intersection between planeZO, circleYZ0" );
}

void test_line_ellipse_intersection_2D()
{
    const geode::Point2D center{ { 0.0, 0.0 } };
    const geode::Vector2D first_axis{ { 3.0, 0.0 } };
    const geode::Vector2D second_axis{ { 0.0, 2.0 } };
    const geode::Frame2D frame{ { first_axis, second_axis } };
    const geode::Ellipse2D ellipse{ center, frame };
    const geode::Point2D parallel_origin{ { 0., 3 } };
    const geode::Point2D parallel_dest{ { 1, 3 } };
    const geode::Segment2D parallel_segment{ parallel_origin, parallel_dest };
    const geode::InfiniteLine2D parallel{ parallel_segment };
    const auto parallel_result =
        geode::line_ellipse_intersection( parallel, ellipse );
    geode::OpenGeodeGeometryException::test(
        !parallel_result, "Wrong intersection between line and ellipse" );
    const geode::Point2D intersecting_once_origin{ { 3.0, 2.0 } };
    const geode::Point2D intersecting_once_dest{ { 3.0, 0.0 } };
    const geode::Segment2D intersecting_once_segment{ intersecting_once_origin,
        intersecting_once_dest };
    const geode::InfiniteLine2D intersecting_once{ intersecting_once_segment };
    const auto intersecting_once_result =
        geode::line_ellipse_intersection( intersecting_once, ellipse );
    geode::OpenGeodeGeometryException::test(
        intersecting_once_result.result->size() == 1,
        "Wrong intersection between line and ellipse" );
    geode::OpenGeodeGeometryException::test(
        intersecting_once_result.result->at( 0 ) == intersecting_once_dest,
        "Wrong intersection between line and ellipse" );
    const geode::Segment2D intersecting_twice_segment(
        center, geode::Point2D{ { 3, 0 } } );
    const geode::InfiniteLine2D intersecting_twice_line{
        intersecting_twice_segment
    };
    const auto intersecting_twice_result =
        geode::line_ellipse_intersection( intersecting_twice_line, ellipse );
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->size() == 2,
        "Wrong intersection between line and ellipse" );
    const geode::Point2D result1{ { -3, 0 } };
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->at( 0 ) == result1,
        "Wrong intersection between line and ellipse" );
    const geode::Point2D result2{ { 3, 0 } };
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->at( 1 ) == result2,
        "Wrong intersection between line and ellipse" );
}

void test_line_ellipse_intersection_3D()
{
    const geode::Point3D center{ { 0.0, 0.0 } };
    const geode::Vector3D first_axis{ { 3.0, 0.0, 0.0 } };
    const geode::Vector3D second_axis{ { 0.0, 2.0, 0.0 } };
    const geode::Vector3D third_axis{ { 0.0, 0.0, 1.0 } };
    const geode::Frame3D frame{ { first_axis, second_axis, third_axis } };
    const geode::Ellipse3D ellipse{ center, frame };

    const geode::Point3D parallel_origin{ { 0., 3, 1 } };
    const geode::Point3D parallel_dest{ { 1, 3, 1 } };
    const geode::Segment3D parallel_segment{ parallel_origin, parallel_dest };
    const geode::InfiniteLine3D parallel{ parallel_segment };
    const auto parallel_result =
        geode::line_ellipse_intersection( parallel, ellipse );
    geode::OpenGeodeGeometryException::test(
        !parallel_result, "Wrong intersection between line and ellipse" );
    const geode::Point3D intersecting_once_origin{ { 3.0, 2.0, 0.0 } };
    const geode::Point3D intersecting_once_dest{ { 3.0, 0.0, 0.0 } };
    const geode::Segment3D intersecting_once_segment{ intersecting_once_origin,
        intersecting_once_dest };
    const geode::InfiniteLine3D intersecting_once{ intersecting_once_segment };
    const auto intersecting_once_result =
        geode::line_ellipse_intersection( intersecting_once, ellipse );
    geode::OpenGeodeGeometryException::test(
        intersecting_once_result.result->size() == 1,
        "Wrong intersection between line and ellipse" );
    geode::OpenGeodeGeometryException::test(
        intersecting_once_result.result->at( 0 ) == intersecting_once_dest,
        "Wrong intersection between line and ellipse" );
    const geode::Segment3D intersecting_twice_segment(
        geode::Point3D{ { 0, 0, 1 } }, geode::Point3D{ { 3, 0, 0 } } );
    const geode::InfiniteLine3D intersecting_twice_line{
        intersecting_twice_segment
    };
    const auto intersecting_twice_result =
        geode::line_ellipse_intersection( intersecting_twice_line, ellipse );
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->size() == 2,
        "Wrong intersection between line and ellipse" );
    const geode::Point3D result1{ { 0, 0, 1 } };
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->at( 0 ).inexact_equal( result1 ),
        "Wrong intersection between line and ellipse" );
    const geode::Point3D result2{ { 3, 0, 0 } };
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->at( 1 ).inexact_equal( result2 ),
        "Wrong intersection between line and ellipse" );
}

void test_line_ellipse_intersection()
{
    test_line_ellipse_intersection_2D();
    test_line_ellipse_intersection_3D();
}

void test_segment_ellipse_intersection_2D()
{
    const geode::Point2D center{ { 0.0, 0.0 } };
    const geode::Vector2D first_axis{ { 3.0, 0.0 } };
    const geode::Vector2D second_axis{ { 0.0, 2.0 } };
    const geode::Frame2D frame{ { first_axis, second_axis } };
    const geode::Ellipse2D ellipse{ center, frame };
    const geode::Point2D parallel_origin{ { 0., 3 } };
    const geode::Point2D parallel_dest{ { 1, 3 } };
    const geode::Segment2D parallel_segment{ parallel_origin, parallel_dest };
    const auto parallel_result =
        geode::segment_ellipse_intersection( parallel_segment, ellipse );
    geode::OpenGeodeGeometryException::test(
        !parallel_result, "Wrong intersection between segment and ellipse" );
    const geode::Point2D intersecting_once_origin{ { 3.0, 2.0 } };
    const geode::Point2D intersecting_once_dest{ { 3.0, -2.0 } };
    const geode::Segment2D intersecting_once_segment{ intersecting_once_origin,
        intersecting_once_dest };
    const auto intersecting_once_result = geode::segment_ellipse_intersection(
        intersecting_once_segment, ellipse );
    geode::OpenGeodeGeometryException::test(
        intersecting_once_result.result->size() == 1,
        "Wrong intersection between segment and ellipse" );
    geode::Point2D result{ { 3.0, 0.0 } };
    geode::OpenGeodeGeometryException::test(
        intersecting_once_result.result->at( 0 ) == result,
        "Wrong intersection between segment and ellipse" );
    const geode::Point2D intersecting_twice_origin{ { -4.0, 0 } };
    const geode::Point2D intersecting_twice_dest{ { 4.0, 0 } };
    const geode::Segment2D intersecting_twice_segment{
        intersecting_twice_origin, intersecting_twice_dest
    };
    const auto intersecting_twice_result = geode::segment_ellipse_intersection(
        intersecting_twice_segment, ellipse );
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->size() == 2,
        "Wrong intersection between segment and ellipse" );
    const geode::Point2D result1{ { -3, 0 } };
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->at( 0 ).inexact_equal( result1 ),
        "Wrong intersection between segment and ellipse" );
    const geode::Point2D result2{ { 3, 0 } };
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->at( 1 ).inexact_equal( result2 ),
        "Wrong intersection between segment and ellipse" );
    const geode::Point2D not_intersecting_origin{ { 1.0, 0.0 } };
    const geode::Point2D not_intersecting_dest{ { 2.0, 0.0 } };
    const geode::Segment2D not_intersecting_segment{ not_intersecting_origin,
        not_intersecting_dest };
    const auto not_intersecting_result = geode::segment_ellipse_intersection(
        not_intersecting_segment, ellipse );
    geode::OpenGeodeGeometryException::test( !not_intersecting_result,
        "Wrong intersection between segment and ellipse" );
}

void test_segment_ellipse_intersection_3D()
{
    const geode::Point3D center{ { 0.0, 0.0, 0.0 } };
    const geode::Vector3D first_axis{ { 3.0, 0.0, 0.0 } };
    const geode::Vector3D second_axis{ { 0.0, 2.0, 0.0 } };
    const geode::Vector3D third_axis{ { 0.0, 0.0, 1.0 } };
    const geode::Frame3D frame{ { first_axis, second_axis, third_axis } };
    const geode::Ellipse3D ellipse{ center, frame };

    const geode::Point3D parallel_origin{ { 0., 3, 1 } };
    const geode::Point3D parallel_dest{ { 1, 3, 1 } };
    const geode::Segment3D parallel_segment{ parallel_origin, parallel_dest };
    const auto parallel_result =
        geode::segment_ellipse_intersection( parallel_segment, ellipse );
    geode::OpenGeodeGeometryException::test(
        !parallel_result, "Wrong intersection between segment and ellipse" );
    const geode::Point3D intersecting_once_origin{ { 3.0, 2.0, 0 } };
    const geode::Point3D intersecting_once_dest{ { 3.0, -2.0, 0 } };
    const geode::Segment3D intersecting_once_segment{ intersecting_once_origin,
        intersecting_once_dest };
    const auto intersecting_once_result = geode::segment_ellipse_intersection(
        intersecting_once_segment, ellipse );
    geode::OpenGeodeGeometryException::test(
        intersecting_once_result.result->size() == 1,
        "Wrong intersection between segment and ellipse" );
    geode::Point3D result{ { 3.0, 0.0, 0 } };
    geode::OpenGeodeGeometryException::test(
        intersecting_once_result.result->at( 0 ) == result,
        "Wrong intersection between segment and ellipse" );
    const geode::Point3D intersecting_twice_origin{ { -4.0, 0, 0 } };
    const geode::Point3D intersecting_twice_dest{ { 4.0, 0, 0 } };
    const geode::Segment3D intersecting_twice_segment{
        intersecting_twice_origin, intersecting_twice_dest
    };
    const auto intersecting_twice_result = geode::segment_ellipse_intersection(
        intersecting_twice_segment, ellipse );
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->size() == 2,
        "Wrong intersection between segment and ellipse" );
    const geode::Point3D result1{ { -3, 0 } };
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->at( 0 ).inexact_equal( result1 ),
        "Wrong intersection between segment and ellipse" );
    const geode::Point3D result2{ { 3, 0 } };
    geode::OpenGeodeGeometryException::test(
        intersecting_twice_result.result->at( 1 ).inexact_equal( result2 ),
        "Wrong intersection between segment and ellipse" );
    const geode::Point3D not_intersecting_origin{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D not_intersecting_dest{ { 2.0, 0.0, 0.5 } };
    const geode::Segment3D not_intersecting_segment{ not_intersecting_origin,
        not_intersecting_dest };
    const auto not_intersecting_result = geode::segment_ellipse_intersection(
        not_intersecting_segment, ellipse );
    geode::OpenGeodeGeometryException::test( !not_intersecting_result,
        "Wrong intersection between segment and ellipse" );
}

void test_segment_ellipse_intersection()
{
    test_segment_ellipse_intersection_2D();
    test_segment_ellipse_intersection_3D();
}

void test_small_sphere_intersection()
{
    const geode::Point2D origin{ { 0.0, 0.0 } };
    const geode::Sphere2D small{ origin, 1e-4 };
    const geode::InfiniteLine2D far_line{ geode::Vector2D{ { 1, 0 } },
        geode::Point2D{ { 0, 5e-4 } } };
    const auto far = geode::line_sphere_intersection( far_line, small );
    geode::OpenGeodeGeometryException::test(
        far.type == geode::INTERSECTION_TYPE::none,
        "Wrong result for line_sphere_intersection with line far from a small "
        "sphere" );

    const geode::InfiniteLine2D center_line{ geode::Vector2D{ { 1, 0 } },
        geode::Point2D{ { -1, 0 } } };
    const auto crossing = geode::line_sphere_intersection( center_line, small );
    geode::OpenGeodeGeometryException::test(
        crossing.has_intersection() && crossing.result->size() == 2,
        "Wrong result for line_sphere_intersection with line crossing a small "
        "sphere" );
    for( const auto& point : crossing.result.value() )
    {
        geode::OpenGeodeGeometryException::test(
            std::get< 0 >( geode::point_sphere_distance( point, small ) )
                < geode::GLOBAL_EPSILON,
            "Wrong point for line_sphere_intersection with line crossing a "
            "small sphere" );
    }

    const geode::Sphere2D unit{ origin, 1 };
    const geode::Point2D on_sphere{ { 1.0, 0.0 } };
    const geode::Point2D close{ { 1.0 + 1e-7, 0.0 } };
    const auto tiny = geode::segment_sphere_intersection(
        geode::Segment2D{ on_sphere, close }, unit );
    geode::OpenGeodeGeometryException::test(
        tiny.has_intersection() && tiny.result->size() == 1,
        "Wrong result for segment_sphere_intersection with tiny segment" );

    // Line passing close to the center of a sphere of radius GLOBAL_EPSILON
    const geode::Point2D center{ { -0x1.2f27bf7ab2068p+0,
        -0x1.19ed33959c664p+2 } };
    const geode::Sphere2D tiny_sphere{ center, 0x1.39cbe52173991p-20 };
    const geode::Point2D line0{ { -0x1.2f27aa494ab6cp+0,
        -0x1.19ed32bc45ep+2 } };
    const geode::Point2D line1{ { 0x1.10adc34e01184p+2, 0x1.9d613ed64784p-3 } };
    const auto tangent = geode::line_sphere_intersection(
        geode::InfiniteLine2D{ geode::Segment2D{ line0, line1 } },
        tiny_sphere );
    geode::OpenGeodeGeometryException::test( tangent.has_intersection(),
        "Wrong result for line_sphere_intersection with sphere of radius "
        "GLOBAL_EPSILON" );

    // Segment starting 1.02 * GLOBAL_EPSILON away from a small sphere: its
    // line touches the sphere within GLOBAL_EPSILON of the segment
    const geode::Point3D small_center{ { -0x1.e059c1d05becdp+1,
        -0x1.abea685a05f8p-2, 0x1.23b2c3df35a8p+1 } };
    const geode::Sphere3D small_sphere{ small_center, 0x1.8f84f79ed9dbdp-20 };
    const geode::Point3D start{ { -0x1.e059b62766559p+1, -0x1.abe9ea07260bfp-2,
        0x1.23b2bc340e903p+1 } };
    const geode::Point3D end{ { -0x1.4ccf4e11f9ea4p+2, -0x1.a7318e3f7335p+0,
        -0x1.b4f7162f931f4p+2 } };
    const auto separated = geode::segment_sphere_intersection(
        geode::Segment3D{ start, end }, small_sphere );
    geode::OpenGeodeGeometryException::test(
        separated.type == geode::INTERSECTION_TYPE::none,
        "Wrong result for segment_sphere_intersection with segment just "
        "farther than GLOBAL_EPSILON from a small sphere" );
}

void test_triangle_intersection_near_parallel()
{
    // Line nearly parallel to the triangle plane, 3 away from the triangle
    const geode::Point3D t0{ { 0x1.03bffd2202968p+2, 0x1.6fd08c18b0728p+0,
        -0x1.177a24c186d1bp+2 } };
    const geode::Point3D t1{ { -0x1.a54aef4753099p+1, -0x1.262ea62ca4626p+0,
        -0x1.f00e403e9344p+0 } };
    const geode::Point3D t2{ { -0x1.a3f71ec8e0ec2p+0, 0x1.2cc398f6a1d96p+1,
        -0x1.1a511f5e5d612p+2 } };
    const geode::Point3D l0{ { -0x1.6ce04f0f9e572p+2, -0x1.0d39029896037p+2,
        0x1.4632a52ff3331p-2 } };
    const geode::Point3D l1{ { -0x1.c80fd9962edd4p+2, 0x1.0b14dfd807139p+1,
        -0x1.d989511eb823ep+1 } };
    const auto far = geode::line_triangle_intersection(
        geode::InfiniteLine3D{ geode::Segment3D{ l0, l1 } },
        geode::Triangle3D{ t0, t1, t2 } );
    geode::OpenGeodeGeometryException::test( !far.has_intersection(),
        "Wrong result for line_triangle_intersection with nearly parallel "
        "line far from the triangle" );

    // Segment nearly parallel to the triangle plane, 0.8 away from it
    const geode::Point3D u0{ { -0x1.197524bb976d8p+0, -0x1.091bcd8fced32p+2,
        0x1.972f348d9a6cp-1 } };
    const geode::Point3D u1{ { -0x1.8a8dbb5920948p+0, 0x1.c2c08a610a734p+1,
        -0x1.974660605015ap+1 } };
    const geode::Point3D u2{ { -0x1.199d82f8d93cbp+1, -0x1.9d1d2fa6a19aep+1,
        0x1.46c5cbcd7fc3p-1 } };
    const geode::Point3D s0{ { -0x1.c5dc4c87e12fbp-1, 0x1.29b68453935d4p+2,
        -0x1.fe4a9aa583717p+1 } };
    const geode::Point3D s1{ { -0x1.d05a434b20318p+1, 0x1.bd4a334a649d7p+1,
        -0x1.435f6a4e39e6ap+1 } };
    const auto separated = geode::segment_triangle_intersection(
        geode::Segment3D{ s0, s1 }, geode::Triangle3D{ u0, u1, u2 } );
    geode::OpenGeodeGeometryException::test( !separated.has_intersection(),
        "Wrong result for segment_triangle_intersection with nearly parallel "
        "segment far from the triangle" );

    // Segment within GLOBAL_EPSILON of the triangle plane, crossing the
    // triangle: coplanar
    const geode::Point3D v0{ { -0x1.34e94ffeb0b77p+2, 0x1.7f8acdaf49f1p-1,
        -0x1.701bf0d23d0dep+1 } };
    const geode::Point3D v1{ { -0x1.75576fd208f7dp+1, -0x1.df8314931b436p+1,
        -0x1.8d5159c2c416cp+1 } };
    const geode::Point3D v2{ { -0x1.2ba7aa65ac70cp+2, 0x1.be0852a8488ecp+0,
        -0x1.f8d8c7d15f43p-1 } };
    const geode::Point3D r0{ { -0x1.18520a425ac3p+2, -0x1.4de1d1066c5a4p-1,
        -0x1.b6f8dd0dbbc99p+1 } };
    const geode::Point3D r1{ { -0x1.e00dc3000ad1dp+1, 0x1.21443b30d1609p-1,
        0x1.6b4680dfa430dp-2 } };
    const geode::Triangle3D crossed{ v0, v1, v2 };
    const geode::Segment3D crossing_segment{ r0, r1 };
    const auto crossing =
        geode::segment_triangle_intersection( crossing_segment, crossed );
    geode::OpenGeodeGeometryException::test(
        crossing.type == geode::INTERSECTION_TYPE::parallel,
        "Wrong result for segment_triangle_intersection with segment in the "
        "triangle plane" );
}

void check_crossing_2d(
    const geode::IntersectionResult< geode::Point2D >& result,
    const geode::Segment2D& segment,
    std::string_view message )
{
    geode::OpenGeodeGeometryException::test(
        result.type == geode::INTERSECTION_TYPE::parallel
            || ( result.has_intersection()
                 && geode::point_segment_distance(
                        result.result.value(), segment )
                        < geode::GLOBAL_EPSILON ),
        message );
}

void test_segment_intersection_2d_near_parallel()
{
    // Overlapping collinear segments
    const geode::Point2D a0{ { 0x1.e61fc9a188e38p-1, -0x1.bdfbf5d1ccdap-2 } };
    const geode::Point2D a1{ { 0x1.c49cd43cb87fp-2, 0x1.26f1d4fc386p+0 } };
    const geode::Point2D b0{ { 0x1.a360a0922c338p-2, 0x1.40f0a59bb274ep+0 } };
    const geode::Point2D b1{ { 0x1.058b4a4de5bbep-1, 0x1.dfa46c2b6bb7cp-1 } };
    const auto collinear = geode::segment_segment_intersection(
        geode::Segment2D{ a0, a1 }, geode::Segment2D{ b0, b1 } );
    geode::OpenGeodeGeometryException::test(
        collinear.type == geode::INTERSECTION_TYPE::parallel,
        "Wrong result for segment_segment_intersection with overlapping "
        "collinear segments" );

    // Segments crossing with a small angle
    const geode::Point2D c0{ { 0x1.f1bbfa740ad58p+1, 0x1.606b05c300968p-1 } };
    const geode::Point2D c1{ { -0x1.8bc65165933fp+1, -0x1.9af77575d819p-1 } };
    const geode::Point2D d0{ { 0x1.8ea5460de60cap+2, 0x1.302cc0c7eeb69p+0 } };
    const geode::Point2D d1{ { 0x1.d85dfda85a6bap+1, 0x1.4abec5ea95587p-1 } };
    const geode::Segment2D segment_c{ c0, c1 };
    const geode::Segment2D segment_d{ d0, d1 };
    check_crossing_2d(
        geode::segment_segment_intersection( segment_c, segment_d ), segment_c,
        "Wrong result for segment_segment_intersection with small angle" );
    check_crossing_2d( geode::segment_line_intersection(
                           segment_c, geode::InfiniteLine2D{ segment_d } ),
        segment_c,
        "Wrong result for segment_line_intersection with small angle" );
}

void test_near_parallel_planes_intersection()
{
    // Planes with an angle of 1e-5
    const geode::Point3D origin0{ { 3, 4, 5 } };
    const geode::Vector3D normal0{ { 0, 0, 1 } };
    const geode::Point3D origin1{ { -2, 1, -3 } };
    const geode::Vector3D normal1{ { 0, 1e-5, 1 } };
    const geode::Plane plane0{ normal0, origin0 };
    const geode::Plane plane1{ normal1, origin1 };
    const auto planes = geode::plane_plane_intersection( plane0, plane1 );
    geode::OpenGeodeGeometryException::test( planes.has_intersection(),
        "Wrong result for plane_plane_intersection with nearly parallel "
        "planes" );
    const auto& line = planes.result.value();
    for( const auto t : { 0., 1., -10. } )
    {
        const auto point = line.origin() + line.direction() * t;
        geode::OpenGeodeGeometryException::test(
            std::get< 0 >( geode::point_plane_distance( point, plane0 ) )
                    < geode::GLOBAL_EPSILON
                && std::get< 0 >( geode::point_plane_distance( point, plane1 ) )
                       < geode::GLOBAL_EPSILON,
            "Wrong line for plane_plane_intersection with nearly parallel "
            "planes" );
    }

    // Planes with an angle below GLOBAL_EPSILON: parallel
    const geode::Point3D origin2{ { -0x1.1afe28d3d8774p+0,
        -0x1.8b34a1799de93p+1, -0x1.9f276e61666fcp+0 } };
    const geode::Vector3D normal2{ { -0x1.4f9f1b072e83bp-1,
        0x1.42e543054b722p-1, -0x1.a965a61857dccp-2 } };
    const geode::Point3D origin3{ { -0x1.fa1659e5b6684p+0,
        -0x1.770e0cff20488p+0, 0x1.02ed32a70e25cp+1 } };
    const geode::Vector3D normal3{ { -0x1.4f9f252f05046p-1,
        0x1.42e52d84f60b6p-1, -0x1.a965c753cc90dp-2 } };
    const auto parallel_planes = geode::plane_plane_intersection(
        geode::Plane{ normal2, origin2 }, geode::Plane{ normal3, origin3 } );
    geode::OpenGeodeGeometryException::test(
        parallel_planes.type == geode::INTERSECTION_TYPE::none,
        "Wrong result for plane_plane_intersection with planes parallel up "
        "to GLOBAL_EPSILON" );

    // Circle in the plane (up to GLOBAL_EPSILON)
    const geode::Point3D origin{ { -0x1.94d1db9854a7p+0, 0x1.68e1433c9226ep+1,
        -0x1.eeedf6fe2fed8p-1 } };
    const geode::Vector3D normal{ { -0x1.f86da7c7ff3bp-3, 0x1.2044a2b12fc87p-2,
        -0x1.dad53f776a8e3p-1 } };
    const geode::Point3D center{ { -0x1.2537df0082acdp+0, 0x1.5151b3ba60958p+0,
        -0x1.89ccc55b8a3d2p+0 } };
    const geode::Vector3D circle_normal{ { -0x1.f86da7d8c3964p-3,
        0x1.2044a28e94a6ep-2, -0x1.dad53f7b8e24bp-1 } };
    const geode::Plane plane{ normal, origin };
    const geode::Plane circle_plane{ circle_normal, center };
    const geode::Circle circle{ circle_plane, 0x1.224e1deef52adp-4 };
    const auto coplanar = geode::plane_circle_intersection( plane, circle );
    geode::OpenGeodeGeometryException::test(
        coplanar.type == geode::INTERSECTION_TYPE::parallel,
        "Wrong result for plane_circle_intersection with circle in the "
        "plane" );
}

void test()
{
    test_near_parallel_planes_intersection();
    test_segment_intersection_2d_near_parallel();
    test_triangle_intersection_near_parallel();
    test_small_sphere_intersection();
    test_line_sphere_intersection();
    test_segment_sphere_intersection();
    test_line_line_intersection();
    test_segment_segment_intersection();
    test_segment_line_intersection();
    test_segment_plane_intersection();
    test_segment_triangle_intersection();
    test_segment_triangle_intersection_touching_endpoint();
    test_line_cylinder_intersection();
    test_segment_cylinder_intersection();
    test_plane_plane_intersection();
    test_plane_circle_intersection();
    test_triangle_circle_intersection();
    test_line_ellipse_intersection();
    test_segment_ellipse_intersection();
}

OPENGEODE_TEST( "intersection" )
