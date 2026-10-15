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

#include <geode/geometry/barycentric_coordinates.hpp>
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
#include <geode/geometry/projection.hpp>

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

template < geode::index_t dimension >
void check_correctness(
    const typename geode::CorrectnessInfo<
        geode::Point< dimension > >::Correctness& correctness,
    bool expected_flag,
    const geode::Point< dimension >& expected_point,
    std::string_view name )
{
    geode::OpenGeodeGeometryException::test( correctness.first == expected_flag,
        "[Test] Wrong ", name, " correctness flag" );
    geode::OpenGeodeGeometryException::test(
        geode::point_point_distance( correctness.second, expected_point )
            < 1e-12
                  * ( 1
                      + geode::Vector< dimension >{ expected_point }.length() ),
        "[Test] Wrong ", name, " correctness point ",
        correctness.second.string(), " vs ", expected_point.string() );
}

void test_random_intersection_correctness()
{
    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ -10, 10 };
    const auto random_point3d = [&generator, &distribution] {
        return geode::Point3D{ { distribution( generator ),
            distribution( generator ), distribution( generator ) } };
    };
    const auto random_point2d = [&generator, &distribution] {
        return geode::Point2D{ { distribution( generator ),
            distribution( generator ) } };
    };
    for( const auto unused : geode::Range{ 20000 } )
    {
        geode_unused( unused );
        const auto t0 = random_point3d();
        const auto t1 = random_point3d();
        const auto t2 = random_point3d();
        const geode::Triangle3D triangle{ t0, t1, t2 };
        const auto s0 = random_point3d();
        const auto s1 = random_point3d();
        const geode::Segment3D segment{ s0, s1 };
        const auto segment_result =
            geode::segment_triangle_intersection( segment, triangle );
        if( segment_result.has_intersection() )
        {
            const auto& result = segment_result.result.value();
            check_correctness< 3 >( segment_result.correctness->first,
                geode::point_segment_distance( result, segment )
                    <= geode::GLOBAL_EPSILON,
                geode::point_segment_projection( result, segment ),
                "segment_triangle segment" );
            const auto [distance, closest] =
                geode::point_triangle_distance( result, triangle );
            check_correctness< 3 >( segment_result.correctness->second,
                distance <= geode::GLOBAL_EPSILON, closest,
                "segment_triangle triangle" );
        }

        const geode::InfiniteLine3D line{ segment };
        const auto line_result =
            geode::line_triangle_intersection( line, triangle );
        if( line_result.has_intersection() )
        {
            const auto& result = line_result.result.value();
            check_correctness< 3 >( line_result.correctness->first,
                geode::point_line_distance( result, line )
                    <= geode::GLOBAL_EPSILON,
                geode::point_line_projection( result, line ),
                "line_triangle line" );
            const auto lambdas = geode::safe_triangle_barycentric_coordinates(
                result, triangle );
            const auto& vertices = triangle.vertices();
            const auto point = vertices[0].get() * lambdas[0]
                               + vertices[1].get() * lambdas[1]
                               + vertices[2].get() * lambdas[2];
            check_correctness< 3 >( line_result.correctness->second,
                result.inexact_equal( point ), point,
                "line_triangle triangle" );
        }

        const auto plane_origin = random_point3d();
        const auto n0 = random_point3d();
        const auto n1 = random_point3d();
        const geode::Plane plane{ geode::Vector3D{ n0, n1 }.normalize(),
            plane_origin };
        const auto plane_result = geode::line_plane_intersection( line, plane );
        if( plane_result.has_intersection() )
        {
            const auto& result = plane_result.result.value();
            check_correctness< 3 >( plane_result.correctness->first,
                geode::point_line_distance( result, line )
                    <= geode::GLOBAL_EPSILON,
                geode::point_line_projection( result, line ),
                "line_plane line" );
            const auto [distance, projection] =
                geode::point_plane_distance( result, plane );
            check_correctness< 3 >( plane_result.correctness->second,
                distance <= geode::GLOBAL_EPSILON, projection,
                "line_plane plane" );
        }

        const auto a0 = random_point2d();
        const auto a1 = random_point2d();
        const auto b0 = random_point2d();
        const auto b1 = random_point2d();
        const geode::InfiniteLine2D line0{ geode::Segment2D{ a0, a1 } };
        const geode::InfiniteLine2D line1{ geode::Segment2D{ b0, b1 } };
        const auto lines_result = geode::line_line_intersection( line0, line1 );
        if( lines_result.has_intersection() )
        {
            const auto& result = lines_result.result.value();
            check_correctness< 2 >( lines_result.correctness->first,
                geode::point_line_distance( result, line0 )
                    <= geode::GLOBAL_EPSILON,
                geode::point_line_projection( result, line0 ),
                "line_line first" );
            check_correctness< 2 >( lines_result.correctness->second,
                geode::point_line_distance( result, line1 )
                    <= geode::GLOBAL_EPSILON,
                geode::point_line_projection( result, line1 ),
                "line_line second" );
        }
    }
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

void test_degenerated_segment_sphere_intersection( double length )
{
    const geode::Point3D origin{ { 0, 0, 0 } };
    const geode::Sphere3D sphere{ origin, 1 };
    const geode::Point3D on_sphere{ { 1, 0, 0 } };
    const geode::Point3D on_sphere2{ { 1 + length, 0, 0 } };
    const auto sphere_result = geode::segment_sphere_intersection(
        geode::Segment3D{ on_sphere, on_sphere2 }, sphere );
    geode::OpenGeodeGeometryException::test(
        sphere_result && sphere_result.result->size() == 1,
        "Wrong degenerated segment_sphere_intersection" );
    const geode::Point3D far_sphere{ { 5, 0, 0 } };
    const geode::Point3D far_sphere2{ { 5 + length, 0, 0 } };
    geode::OpenGeodeGeometryException::test(
        !geode::segment_sphere_intersection(
            geode::Segment3D{ far_sphere, far_sphere2 }, sphere ),
        "Wrong degenerated segment_sphere_intersection (far)" );
}

void test_degenerated_segment_plane_intersection( double length )
{
    const geode::Point3D origin{ { 0, 0, 0 } };
    const geode::Plane plane{ geode::Vector3D{ { 0, 0, 1 } }, origin };
    const geode::Point3D on_plane{ { 0.3, 0.2, 0 } };
    const geode::Point3D on_plane2{ { 0.3, 0.2, length } };
    const auto plane_result = geode::segment_plane_intersection(
        geode::Segment3D{ on_plane, on_plane2 }, plane );
    geode::OpenGeodeGeometryException::test(
        plane_result
            && plane_result.result->inexact_equal(
                geode::Point3D{ { 0.3, 0.2, 0 } } ),
        "Wrong degenerated segment_plane_intersection" );
    const geode::Point3D off_plane{ { 0.3, 0.2, 1 } };
    const geode::Point3D off_plane2{ { 0.3, 0.2, 1 + length } };
    geode::OpenGeodeGeometryException::test(
        !geode::segment_plane_intersection(
            geode::Segment3D{ off_plane, off_plane2 }, plane ),
        "Wrong degenerated segment_plane_intersection (far)" );
}

void test_degenerated_segment_cylinder_intersection( double length )
{
    const geode::Point3D origin{ { 0, 0, 0 } };
    const geode::Point3D x_axis{ { 1, 0, 0 } };
    const geode::Cylinder cylinder{ geode::Segment3D{ origin, x_axis }, 0.2 };
    const geode::Point3D on_cylinder{ { 0.5, 0.2, 0 } };
    const geode::Point3D on_cylinder2{ { 0.5, 0.2 + length, 0 } };
    const auto cylinder_result = geode::segment_cylinder_intersection(
        geode::Segment3D{ on_cylinder, on_cylinder2 }, cylinder );
    geode::OpenGeodeGeometryException::test(
        cylinder_result && cylinder_result.result->size() == 1,
        "Wrong degenerated segment_cylinder_intersection" );
    // The segment itself is closer than GLOBAL_EPSILON to the cylinder: only
    // an exact comparison checks that the reported point is on the cylinder
    const auto closest_point = std::get< 1 >( geode::point_cylinder_distance(
        geode::Segment3D{ on_cylinder, on_cylinder2 }.barycenter(),
        cylinder ) );
    geode::OpenGeodeGeometryException::test(
        cylinder_result.correctness->second.second.front() == closest_point,
        "Wrong degenerated segment_cylinder_intersection correctness: "
        "point should be the closest point on the cylinder" );
    const geode::Point3D in_cylinder{ { 0.5, 0, 0 } };
    const geode::Point3D in_cylinder2{ { 0.5, length, 0 } };
    geode::OpenGeodeGeometryException::test(
        !geode::segment_cylinder_intersection(
            geode::Segment3D{ in_cylinder, in_cylinder2 }, cylinder ),
        "Wrong degenerated segment_cylinder_intersection (inside)" );
}

void test_degenerated_segment_ellipse_intersection( double length )
{
    const geode::Frame2D frame{ { geode::Vector2D{ { 3.0, 0.0 } },
        geode::Vector2D{ { 0.0, 2.0 } } } };
    const geode::Point2D origin2d{ { 0, 0 } };
    const geode::Ellipse2D ellipse{ origin2d, frame };
    const geode::Point2D on_ellipse{ { 3, 0 } };
    const geode::Point2D on_ellipse2{ { 3, length } };
    const auto ellipse_result = geode::segment_ellipse_intersection(
        geode::Segment2D{ on_ellipse, on_ellipse2 }, ellipse );
    geode::OpenGeodeGeometryException::test(
        ellipse_result && ellipse_result.result->size() == 1,
        "Wrong degenerated segment_ellipse_intersection" );
    const geode::Point2D in_ellipse{ { 1, 0 } };
    const geode::Point2D in_ellipse2{ { 1, length } };
    geode::OpenGeodeGeometryException::test(
        !geode::segment_ellipse_intersection(
            geode::Segment2D{ in_ellipse, in_ellipse2 }, ellipse ),
        "Wrong degenerated segment_ellipse_intersection (inside)" );
}

void test_degenerated_segment_segment_intersection( double length )
{
    const geode::Point2D origin2d{ { 0, 0 } };
    const geode::Point2D x_axis2d{ { 1, 0 } };
    const geode::Segment2D segment{ origin2d, x_axis2d };
    const geode::Point2D on_segment{ { 0.5, 0 } };
    const geode::Point2D on_segment2{ { 0.5, length } };
    const geode::Segment2D small_on{ on_segment, on_segment2 };
    const geode::Point2D off_segment{ { 0.5, 1 } };
    const geode::Point2D off_segment2{ { 0.5, 1 + length } };
    const geode::Segment2D small_off{ off_segment, off_segment2 };
    const auto segment_result0 =
        geode::segment_segment_intersection( small_on, segment );
    const auto segment_result1 =
        geode::segment_segment_intersection( segment, small_on );
    geode::OpenGeodeGeometryException::test(
        segment_result0 && segment_result1
            && segment_result0.result->inexact_equal(
                segment_result1.result.value() ),
        "Wrong degenerated segment_segment_intersection" );
    geode::OpenGeodeGeometryException::test(
        !geode::segment_segment_intersection( small_off, segment )
            && !geode::segment_segment_intersection( segment, small_off ),
        "Wrong degenerated segment_segment_intersection (far)" );
}

void test_degenerated_segment_line_intersection( double length )
{
    // InfiniteLine2D and Segment2D store references: keep the points alive
    const geode::Point2D origin{ { 0, 0 } };
    const geode::InfiniteLine2D line{ geode::Vector2D{ { 1, 0 } }, origin };
    const geode::Point2D on0{ { 0.5, 0 } };
    const geode::Point2D on1{ { 0.5, length } };
    const geode::Segment2D small_on{ on0, on1 };
    geode::OpenGeodeGeometryException::test(
        geode::segment_line_intersection( small_on, line ),
        "Wrong degenerated segment_line_intersection" );
    const geode::Point2D off0{ { 0.5, 1 } };
    const geode::Point2D off1{ { 0.5, 1 + length } };
    const geode::Segment2D small_off{ off0, off1 };
    geode::OpenGeodeGeometryException::test(
        !geode::segment_line_intersection( small_off, line ),
        "Wrong degenerated segment_line_intersection (far)" );
}

void test_degenerated_segment_intersections()
{
    for( const auto length :
        { 0.4 * geode::GLOBAL_EPSILON, 0.1 * geode::GLOBAL_EPSILON } )
    {
        test_degenerated_segment_sphere_intersection( length );
        test_degenerated_segment_plane_intersection( length );
        test_degenerated_segment_cylinder_intersection( length );
        test_degenerated_segment_ellipse_intersection( length );
        test_degenerated_segment_segment_intersection( length );
        test_degenerated_segment_line_intersection( length );
    }
}

void test_line_cylinder_perpendicular()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Cylinder cylinder{ geode::Segment3D{ a, b }, 0.2 };
    const auto check_correctness = []( const auto& result ) {
        return result.correctness->first.second.size() == result.result->size()
               && result.correctness->second.second.size()
                      == result.result->size();
    };

    for( const auto offset :
        { 0., 0.1 * geode::GLOBAL_EPSILON, -0.1 * geode::GLOBAL_EPSILON } )
    {
        const geode::Point3D c{ { 0.5, 0.2 + offset, -1.0 } };
        const geode::Point3D d{ { 0.5, 0.2 + offset, 1.0 } };
        const geode::InfiniteLine3D tangent{ geode::Segment3D{ c, d } };
        const auto tangent_result =
            geode::line_cylinder_intersection( tangent, cylinder );
        geode::OpenGeodeGeometryException::test(
            tangent_result && tangent_result.result->size() == 1
                && tangent_result.result->front().inexact_equal(
                    geode::Point3D{ { 0.5, 0.2, 0.0 } } )
                && check_correctness( tangent_result ),
            "Wrong result for line_cylinder_intersection with perpendicular "
            "tangent line" );
    }

    const geode::Point3D e{ { 0.5, 0.1, -1.0 } };
    const geode::Point3D f{ { 0.5, 0.1, 1.0 } };
    const geode::InfiniteLine3D crossing{ geode::Segment3D{ e, f } };
    const auto crossing_result =
        geode::line_cylinder_intersection( crossing, cylinder );
    const auto z = std::sqrt( 0.2 * 0.2 - 0.1 * 0.1 );
    geode::OpenGeodeGeometryException::test(
        crossing_result && crossing_result.result->size() == 2
            && crossing_result.result->front().inexact_equal(
                geode::Point3D{ { 0.5, 0.1, -z } } )
            && crossing_result.result->back().inexact_equal(
                geode::Point3D{ { 0.5, 0.1, z } } )
            && check_correctness( crossing_result ),
        "Wrong result for line_cylinder_intersection with perpendicular "
        "crossing line" );

    const geode::Point3D g{ { 0.5, 0.3, -1.0 } };
    const geode::Point3D h{ { 0.5, 0.3, 1.0 } };
    geode::OpenGeodeGeometryException::test(
        !geode::line_cylinder_intersection(
            geode::InfiniteLine3D{ geode::Segment3D{ g, h } }, cylinder ),
        "Wrong result for line_cylinder_intersection with perpendicular "
        "outside line" );

    const geode::Point3D i{ { 0.0, -1.0, 0.0 } };
    const geode::Point3D j{ { 1.0, 1.0, 0.0 } };
    const auto oblique_result = geode::line_cylinder_intersection(
        geode::InfiniteLine3D{ geode::Segment3D{ i, j } }, cylinder );
    geode::OpenGeodeGeometryException::test(
        oblique_result && oblique_result.result->size() == 2
            && check_correctness( oblique_result ),
        "Wrong result for line_cylinder_intersection with oblique line" );
}

void test_line_cylinder_tolerance()
{
    // Oblique line tangent to the wall, within GLOBAL_EPSILON
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Cylinder cylinder{ geode::Segment3D{ a, b }, 0.2 };
    for( const auto offset :
        { 0., 0.1 * geode::GLOBAL_EPSILON, -0.1 * geode::GLOBAL_EPSILON } )
    {
        const geode::Point3D c{ { 0.0, 0.2 + offset, -0.5 } };
        const geode::Point3D d{ { 1.0, 0.2 + offset, 0.5 } };
        const auto result = geode::line_cylinder_intersection(
            geode::InfiniteLine3D{ geode::Segment3D{ c, d } }, cylinder );
        geode::OpenGeodeGeometryException::test(
            result && result.result->size() == 1
                && result.result->front().inexact_equal(
                    geode::Point3D{ { 0.5, 0.2, 0.0 } } ),
            "Wrong result for line_cylinder_intersection with oblique "
            "tangent line (offset ",
            offset, ")" );
    }

    // Line entering through the rim of an end disk and leaving through the
    // wall: the rim point must be reported once
    const geode::Point3D bottom{ { 0.0, 0.0, -1.0 } };
    const geode::Point3D top{ { 0.0, 0.0, 1.0 } };
    const geode::Cylinder z_cylinder{ geode::Segment3D{ bottom, top }, 1.0 };
    const geode::Point3D rim{ { 1.0, 0.0, 1.0 } };
    const geode::Point3D wall{ { -1.0, 0.0, 0.0 } };
    const auto rim_result = geode::line_cylinder_intersection(
        geode::InfiniteLine3D{ geode::Segment3D{ rim, wall } }, z_cylinder );
    geode::OpenGeodeGeometryException::test(
        rim_result && rim_result.result->size() == 2
            && rim_result.result->at( 0 ).inexact_equal( rim )
            && rim_result.result->at( 1 ).inexact_equal( wall ),
        "Wrong result for line_cylinder_intersection through the rim" );
}

template < typename Result >
bool has_consistent_correctness( const Result& result )
{
    return result.has_intersection()
           && result.correctness->first.second.size() == result.result->size()
           && result.correctness->second.second.size() == result.result->size();
}

void test_segment_intersections_correctness()
{
    const geode::Point3D origin{ { 0, 0, 0 } };
    const geode::Point3D outside{ { 2, 0, 0 } };
    const geode::Sphere3D sphere{ origin, 1 };
    const auto sphere_result = geode::segment_sphere_intersection(
        geode::Segment3D{ origin, outside }, sphere );
    geode::OpenGeodeGeometryException::test(
        sphere_result.result->size() == 1
            && has_consistent_correctness( sphere_result )
            && sphere_result.correctness->second.second.front().inexact_equal(
                geode::Point3D{ { 1, 0, 0 } } ),
        "Wrong correctness for segment_sphere_intersection" );

    const geode::Point3D x_axis{ { 1, 0, 0 } };
    const geode::Cylinder cylinder{ geode::Segment3D{ origin, x_axis }, 0.2 };
    const geode::Point3D c{ { 0.5, 0.0, 0.0 } };
    const geode::Point3D d{ { 0.5, 1.0, 0.0 } };
    const auto cylinder_result = geode::segment_cylinder_intersection(
        geode::Segment3D{ c, d }, cylinder );
    geode::OpenGeodeGeometryException::test(
        cylinder_result.result->size() == 1
            && has_consistent_correctness( cylinder_result ),
        "Wrong correctness for segment_cylinder_intersection" );

    const geode::Point2D center{ { 0.0, 0.0 } };
    const geode::Point2D far{ { 4.0, 0.0 } };
    const geode::Frame2D frame{ { geode::Vector2D{ { 3.0, 0.0 } },
        geode::Vector2D{ { 0.0, 2.0 } } } };
    const geode::Ellipse2D ellipse{ center, frame };
    const auto ellipse_result = geode::segment_ellipse_intersection(
        geode::Segment2D{ center, far }, ellipse );
    geode::OpenGeodeGeometryException::test(
        ellipse_result.result->size() == 1
            && has_consistent_correctness( ellipse_result )
            && ellipse_result.correctness->second.second.front().inexact_equal(
                geode::Point2D{ { 3, 0 } } ),
        "Wrong correctness for segment_ellipse_intersection" );

    const geode::Plane plane{ geode::Vector3D{ { 0, 0, 1 } }, origin };
    const geode::Plane circle_plane{ geode::Vector3D{ { 1, 0, 0 } }, origin };
    const geode::Circle circle{ circle_plane, 1 };
    const auto circle_result =
        geode::plane_circle_intersection( plane, circle );
    geode::OpenGeodeGeometryException::test(
        circle_result.result->size() == 2
            && has_consistent_correctness( circle_result ),
        "Wrong correctness for plane_circle_intersection" );
}

void test_line_sphere_intersection_robustness()
{
    const geode::Point3D origin{ { 0, 0, 0 } };
    const geode::Sphere3D sphere{ origin, 1 };
    const geode::OwnerInfiniteLine3D far_line{ geode::Vector3D{ { -1, 0, 0 } },
        geode::Point3D{ { 1e8, 0.5, 0 } } };
    const auto far_result = geode::line_sphere_intersection(
        geode::InfiniteLine3D{ far_line }, sphere );
    const auto x = std::sqrt( 0.75 );
    geode::OpenGeodeGeometryException::test(
        far_result && far_result.result->size() == 2
            && far_result.result->front().inexact_equal(
                geode::Point3D{ { x, 0.5, 0 } } )
            && far_result.result->back().inexact_equal(
                geode::Point3D{ { -x, 0.5, 0 } } ),
        "Wrong line_sphere_intersection with far line origin" );

    const geode::Sphere3D small_sphere{ origin, 1e-4 };
    const geode::OwnerInfiniteLine3D close_line{ geode::Vector3D{ { 1, 0, 0 } },
        geode::Point3D{ { 0, 1.4e-4, 0 } } };
    geode::OpenGeodeGeometryException::test(
        !geode::line_sphere_intersection(
            geode::InfiniteLine3D{ close_line }, small_sphere ),
        "Wrong line_sphere_intersection with small sphere" );
    const geode::OwnerInfiniteLine3D tangent_line{
        geode::Vector3D{ { 1, 0, 0 } }, geode::Point3D{ { 0, 1e-4, 0 } }
    };
    const auto tangent_result = geode::line_sphere_intersection(
        geode::InfiniteLine3D{ tangent_line }, small_sphere );
    geode::OpenGeodeGeometryException::test(
        tangent_result && tangent_result.result->size() == 1,
        "Wrong line_sphere_intersection with tangent line" );
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
    test_segment_triangle_intersection_touching_endpoint();
    test_random_intersection_correctness();
    test_line_cylinder_intersection();
    test_segment_cylinder_intersection();
    test_plane_plane_intersection();
    test_plane_circle_intersection();
    test_triangle_circle_intersection();
    test_line_ellipse_intersection();
    test_segment_ellipse_intersection();
    test_degenerated_segment_intersections();
    test_line_cylinder_perpendicular();
    test_line_cylinder_tolerance();
    test_segment_intersections_correctness();
    test_line_sphere_intersection_robustness();
}

OPENGEODE_TEST( "intersection" )
