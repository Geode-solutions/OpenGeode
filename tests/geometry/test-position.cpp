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

#include <cmath>
#include <limits>
#include <random>
#include <string>
#include <tuple>

#include <absl/algorithm/container.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/polygon.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/is_point_inside.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/position.hpp>

#include <geode/tests/common.hpp>

void test_point_side_to_segment()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Segment2D segment2D{ a, b };

    const auto side_a = geode::point_side_to_segment( a, segment2D );
    geode::OpenGeodeGeometryException::test( side_a == geode::SIDE::zero,
        "Wrong result for point_side_to_segment with query point "
        "a" );

    const auto side_b = geode::point_side_to_segment( b, segment2D );
    geode::OpenGeodeGeometryException::test( side_b == geode::SIDE::zero,
        "Wrong result for point_side_to_segment with query point "
        "b" );

    const geode::Point2D q1{ { 0.5, 0 } };
    const auto side_q1 = geode::point_side_to_segment( q1, segment2D );
    geode::OpenGeodeGeometryException::test( side_q1 == geode::SIDE::zero,
        "Wrong result for point_side_to_segment with query point "
        "q1" );

    const geode::Point2D q2{ { 10.0, 0 } };
    const auto side_q2 = geode::point_side_to_segment( q2, segment2D );
    geode::OpenGeodeGeometryException::test( side_q2 == geode::SIDE::zero,
        "Wrong result for point_side_to_segment with query point "
        "q2" );

    const geode::Point2D q3{ { 0.3, 2.0 } };
    const auto side_q3 = geode::point_side_to_segment( q3, segment2D );
    geode::OpenGeodeGeometryException::test( side_q3 == geode::SIDE::positive,
        "Wrong result for point_side_to_segment with query point "
        "q3" );

    const geode::Point2D q4{ { 12.0, -6.0 } };
    const auto side_q4 = geode::point_side_to_segment( q4, segment2D );
    geode::OpenGeodeGeometryException::test( side_q4 == geode::SIDE::negative,
        "Wrong result for point_side_to_segment with query point "
        "q4" );

    const geode::Point2D q5{ { 0.0, -1e-10 } };
    const auto side_q5 = geode::point_side_to_segment( q5, segment2D );
    geode::OpenGeodeGeometryException::test( side_q5 == geode::SIDE::negative,
        "Wrong result for point_side_to_segment with query point "
        "q5" );
}

void test_point_side_to_plane()
{
    const geode::Point3D origin{ { 0.0, 0.0, 2.0 } };
    const geode::Vector3D normal{ { 0.0, 0.0, 4.0 } };
    const geode::Plane plane{ normal, origin };

    const geode::Point3D q1{ { 0.5, 1.0, 2.0 } };
    const auto side_q1 = geode::point_side_to_plane( q1, plane );
    geode::OpenGeodeGeometryException::test( side_q1 == geode::SIDE::zero,
        "Wrong result for point_side_to_plane with query point "
        "q1" );

    const geode::Point3D q2{ { 0.5, 1.0, 3.0 } };
    const auto side_q2 = geode::point_side_to_plane( q2, plane );
    geode::OpenGeodeGeometryException::test( side_q2 == geode::SIDE::positive,
        "Wrong result for point_side_to_plane with query point "
        "q2" );

    const geode::Point3D q3{ { 0.5, 1.0, 1.0 } };
    const auto side_q3 = geode::point_side_to_plane( q3, plane );
    geode::OpenGeodeGeometryException::test( side_q3 == geode::SIDE::negative,
        "Wrong result for point_side_to_plane with query point "
        "q3" );

    const geode::Point3D q4{ { 0.5, 1.0, 2.0 + 1e-10 } };
    const auto side_q4 = geode::point_side_to_plane( q4, plane );
    geode::OpenGeodeGeometryException::test( side_q4 == geode::SIDE::positive,
        "Wrong result for point_side_to_plane with query point "
        "q4" );

    const geode::Point3D q5{ { 0.5, 1.0, 2.0 - 1e-10 } };
    const auto side_q5 = geode::point_side_to_plane( q5, plane );
    geode::OpenGeodeGeometryException::test( side_q5 == geode::SIDE::negative,
        "Wrong result for point_side_to_plane with query point "
        "q5" );
}

void test_point_segment_position_2d()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Segment2D segment2D{ a, b };

    const auto position_a = geode::point_segment_position( a, segment2D );
    geode::OpenGeodeGeometryException::test(
        position_a == geode::POSITION::vertex0,
        "Wrong result for point_segment_position_2d with query point "
        "a" );

    const auto position_b = geode::point_segment_position( b, segment2D );
    geode::OpenGeodeGeometryException::test(
        position_b == geode::POSITION::vertex1,
        "Wrong result for point_segment_position_2d with query point "
        "b" );

    const geode::Point2D q1{ { 0.5, 0 } };
    const auto position_q1 = geode::point_segment_position( q1, segment2D );
    geode::OpenGeodeGeometryException::test(
        position_q1 == geode::POSITION::inside,
        "Wrong result for point_segment_position_2d with query point "
        "q1" );

    const geode::Point2D q2{ { 10.0, 0 } };
    const auto position_q2 = geode::point_segment_position( q2, segment2D );
    geode::OpenGeodeGeometryException::test(
        position_q2 == geode::POSITION::outside,
        "Wrong result for point_segment_position_2d with query point "
        "q2" );

    const geode::Point2D q3{ { 0.3, 2.0 } };
    const auto position_q3 = geode::point_segment_position( q3, segment2D );
    geode::OpenGeodeGeometryException::test(
        position_q3 == geode::POSITION::outside,
        "Wrong result for point_segment_position_2d with query point "
        "q3" );

    const geode::Point2D q4{ { 12.0, -6.0 } };
    const auto position_q4 = geode::point_segment_position( q4, segment2D );
    geode::OpenGeodeGeometryException::test(
        position_q4 == geode::POSITION::outside,
        "Wrong result for point_segment_position_2d with query point "
        "q4" );

    const geode::Point2D q5{ { 0.5, -1e-10 } };
    const auto position_q5 = geode::point_segment_position( q5, segment2D );
    geode::OpenGeodeGeometryException::test(
        position_q5 == geode::POSITION::outside,
        "Wrong result for point_segment_position_2d with query point "
        "q5" );
}

void test_point_segment_position_3d()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Segment3D segment3D{ a, b };

    const auto position_a = geode::point_segment_position( a, segment3D );
    geode::OpenGeodeGeometryException::test(
        position_a == geode::POSITION::vertex0,
        "Wrong result for point_segment_position_3d with query point "
        "a" );

    const auto position_b = geode::point_segment_position( b, segment3D );
    geode::OpenGeodeGeometryException::test(
        position_b == geode::POSITION::vertex1,
        "Wrong result for point_segment_position_3d with query point "
        "b" );

    const geode::Point3D q1{ { 0.5, 0, 0 } };
    const auto position_q1 = geode::point_segment_position( q1, segment3D );
    geode::OpenGeodeGeometryException::test(
        position_q1 == geode::POSITION::inside,
        "Wrong result for point_segment_position_3d with query point "
        "q1" );

    const geode::Point3D q2{ { 10.0, 0, 0 } };
    const auto position_q2 = geode::point_segment_position( q2, segment3D );
    geode::OpenGeodeGeometryException::test(
        position_q2 == geode::POSITION::outside,
        "Wrong result for point_segment_position_3d with query point "
        "q2" );

    const geode::Point3D q3{ { 0.3, 2.0, 0 } };
    const auto position_q3 = geode::point_segment_position( q3, segment3D );
    geode::OpenGeodeGeometryException::test(
        position_q3 == geode::POSITION::outside,
        "Wrong result for point_segment_position_3d with query point "
        "q3" );

    const geode::Point3D q4{ { 12.0, -6.0, 0 } };
    const auto position_q4 = geode::point_segment_position( q4, segment3D );
    geode::OpenGeodeGeometryException::test(
        position_q4 == geode::POSITION::outside,
        "Wrong result for point_segment_position_3d with query point "
        "q4" );

    const geode::Point3D q5{ { 0.5, -1e-10, 0 } };
    const auto position_q5 = geode::point_segment_position( q5, segment3D );
    geode::OpenGeodeGeometryException::test(
        position_q5 == geode::POSITION::outside,
        "Wrong result for point_segment_position_3d with query point "
        "q5" );
}

void test_point_segment_position()
{
    test_point_segment_position_2d();
    test_point_segment_position_3d();
}

void test_point_triangle_degenerated_position_2d()
{
    const geode::Point2D a{ { 1.0, 1.0 } };
    const geode::Point2D b{ { 0.5, 0.5 } };
    const geode::Point2D c{ { 1.2, 1.2 } };
    const geode::Triangle2D triangle2D{ a, b, c };

    const geode::Point2D q1{ { 1.5, 1.5 } };
    const auto position_q1 = geode::point_triangle_position( q1, triangle2D );
    geode::OpenGeodeGeometryException::test(
        position_q1 == geode::POSITION::outside,
        "Wrong result for test_point_triangle_degenerated_position_2d "
        "with query point q1" );

    const auto position_c = geode::point_triangle_position( c, triangle2D );
    geode::OpenGeodeGeometryException::test(
        position_c == geode::POSITION::parallel,
        "Wrong result for test_point_triangle_degenerated_position_2d "
        "with query point c" );

    const geode::Point2D q2{ { 0.75, 0.75 } };
    const auto position_q2 = geode::point_triangle_position( q2, triangle2D );
    geode::OpenGeodeGeometryException::test(
        position_q2 == geode::POSITION::parallel,
        "Wrong result for test_point_triangle_degenerated_position_2d "
        "with query point q2" );
}

void test_point_triangle_position_2d()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };
    const geode::Triangle2D triangle2D{ a, b, c };

    const auto position_a = geode::point_triangle_position( a, triangle2D );
    geode::OpenGeodeGeometryException::test(
        position_a == geode::POSITION::vertex0,
        "Wrong result for point_triangle_position_2d with query point "
        "a" );

    const auto position_b = geode::point_triangle_position( b, triangle2D );
    geode::OpenGeodeGeometryException::test(
        position_b == geode::POSITION::vertex1,
        "Wrong result for point_triangle_position_2d with query point "
        "b" );

    const geode::Point2D q1{ { 0.5, 0 } };
    const auto position_q1 = geode::point_triangle_position( q1, triangle2D );
    geode::OpenGeodeGeometryException::test(
        position_q1 == geode::POSITION::edge0,
        "Wrong result for point_triangle_position_2d with query point "
        "q1" );

    const geode::Point2D q2{ { 0.0, 1.0 } };
    const auto position_q2 = geode::point_triangle_position( q2, triangle2D );
    geode::OpenGeodeGeometryException::test(
        position_q2 == geode::POSITION::outside,
        "Wrong result for point_triangle_position_2d with query point "
        "q2" );

    const geode::Point2D q3{ { 2.0, 1.0 } };
    const auto position_q3 = geode::point_triangle_position( q3, triangle2D );
    geode::OpenGeodeGeometryException::test(
        position_q3 == geode::POSITION::outside,
        "Wrong result for point_triangle_position_2d with query point "
        "q3" );

    const geode::Point2D q4{ { 0.5, 1e-10 } };
    const auto position_q4 = geode::point_triangle_position( q4, triangle2D );
    geode::OpenGeodeGeometryException::test(
        position_q4 == geode::POSITION::inside,
        "Wrong result for point_triangle_position_2d with query point "
        "q4" );

    const geode::Point2D q5{ { 0.5, -1e-10 } };
    const auto position_q5 = geode::point_triangle_position( q5, triangle2D );
    geode::OpenGeodeGeometryException::test(
        position_q5 == geode::POSITION::outside,
        "Wrong result for point_triangle_position_2d with query point "
        "q5" );
}

void test_point_triangle_position_3d()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 0.0 } };
    const geode::Point3D d{ { 2.0, 0.0, 0.0 } };
    const geode::Triangle3D triangle3D{ a, b, c };
    const geode::Triangle3D triangle3D_degen{ a, b, d };

    const geode::Point3D qout{ { 0.75, -0.25, 0.0 } };
    const auto position_qout =
        geode::point_triangle_position( qout, triangle3D );
    geode::OpenGeodeGeometryException::test(
        position_qout == geode::POSITION::outside,
        "Wrong result for point_triangle_position_3d with query point "
        "qout" );

    const geode::Point3D q0{ { 0.75, 0.25, 0.0 } };
    const auto position_q0 = geode::point_triangle_position( q0, triangle3D );
    geode::OpenGeodeGeometryException::test(
        position_q0 == geode::POSITION::inside,
        "Wrong result for point_triangle_position_3d with query point "
        "q0" );

    const auto position_a = geode::point_triangle_position( a, triangle3D );
    geode::OpenGeodeGeometryException::test(
        position_a == geode::POSITION::vertex0,
        "Wrong result for point_triangle_position_3d with query point "
        "a" );

    const auto position_b = geode::point_triangle_position( b, triangle3D );
    geode::OpenGeodeGeometryException::test(
        position_b == geode::POSITION::vertex1,
        "Wrong result for point_triangle_position_3d with query point "
        "b" );

    const geode::Point3D q1{ { 0.5, 0.0, 0.0 } };
    const auto position_q1 = geode::point_triangle_position( q1, triangle3D );
    geode::OpenGeodeGeometryException::test(
        position_q1 == geode::POSITION::edge0,
        "Wrong result for point_triangle_position_3d with query point "
        "q1" );

    const geode::Point3D q2{ { 0.0, 1.0, 0.0 } };
    const auto position_q2 = geode::point_triangle_position( q2, triangle3D );
    geode::OpenGeodeGeometryException::test(
        position_q2 == geode::POSITION::outside,
        "Wrong result for point_triangle_position_3d with query point "
        "q2" );

    const geode::Point3D q3{ { 2.0, 1.0, 0.0 } };
    const auto position_q3 = geode::point_triangle_position( q3, triangle3D );
    geode::OpenGeodeGeometryException::test(
        position_q3 == geode::POSITION::outside,
        "Wrong result for point_triangle_position_3d with query point "
        "q3" );

    const geode::Point3D q4{ { 0.5, 1e-10, 0.0 } };
    const auto position_q4 = geode::point_triangle_position( q4, triangle3D );
    geode::OpenGeodeGeometryException::test(
        position_q4 == geode::POSITION::inside,
        "Wrong result for point_triangle_position_3d with query point "
        "q4" );

    const geode::Point3D q5{ { 0.5, -1e-10, 0.0 } };
    const auto position_q5 = geode::point_triangle_position( q5, triangle3D );
    geode::OpenGeodeGeometryException::test(
        position_q5 == geode::POSITION::outside,
        "Wrong result for point_triangle_position_3d with query point "
        "q5" );

    const geode::Point3D q6{ { 0.5, 1e-10, 1e-10 } };
    const auto position_q6 = geode::point_triangle_position( q6, triangle3D );
    geode::OpenGeodeGeometryException::test(
        position_q6 == geode::POSITION::outside,
        "Wrong result for point_triangle_position_3d with query point "
        "q6" );

    const geode::Point3D qdeg{ { 0.75, 0.0, 0.0 } };
    const auto position_inside_degen =
        geode::point_triangle_position( qdeg, triangle3D_degen );

    geode::OpenGeodeGeometryException::test(
        position_inside_degen == geode::POSITION::parallel,
        "Wrong result for point_triangle_position_3d with query point "
        "qdeg (degenerate case)" );
}

void test_point_triangle_position_3d_in_plane_of_projection_vector()
{
    // The triangle plane contains the (1, 1, 1) direction
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 1.0, 1.0 } };
    const geode::Point3D c{ { 0.0, 0.0, 1.0 } };
    const geode::Triangle3D triangle{ a, b, c };

    const geode::Point3D q_inside{ { 0.2, 0.2, 0.5 } };
    geode::OpenGeodeGeometryException::test(
        geode::point_triangle_position( q_inside, triangle )
            == geode::POSITION::inside,
        "Wrong result for point_triangle_position_3d with point inside a "
        "triangle containing the (1,1,1) direction" );

    const geode::Point3D q_edge{ { 0.5, 0.5, 0.5 } };
    geode::OpenGeodeGeometryException::test(
        geode::point_triangle_position( q_edge, triangle )
            == geode::POSITION::edge0,
        "Wrong result for point_triangle_position_3d with point on edge of "
        "a triangle containing the (1,1,1) direction" );

    geode::OpenGeodeGeometryException::test(
        geode::point_triangle_position( b, triangle )
            == geode::POSITION::vertex1,
        "Wrong result for point_triangle_position_3d with vertex of a "
        "triangle containing the (1,1,1) direction" );

    const geode::Point3D q_outside{ { 1e-7, 1e-7, -1e-7 } };
    geode::OpenGeodeGeometryException::test(
        geode::point_triangle_position( q_outside, triangle )
            == geode::POSITION::outside,
        "Wrong result for point_triangle_position_3d with point outside a "
        "triangle containing the (1,1,1) direction" );
}

void test_point_triangle_position()
{
    test_point_triangle_position_2d();
    test_point_triangle_degenerated_position_2d();
    test_point_triangle_position_3d();
    test_point_triangle_position_3d_in_plane_of_projection_vector();
}

void test_point_tetrahedron_position()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 0.0, 1.0, 0.0 } };
    const geode::Point3D d{ { 0.0, 0.0, 1.0 } };
    const geode::Tetrahedron tetra{ a, b, c, d };

    const auto position_a = geode::point_tetrahedron_position( a, tetra );
    geode::OpenGeodeGeometryException::test(
        position_a == geode::POSITION::vertex0,
        "Wrong result for point_tetrahedron_position with query point "
        "a" );

    const auto position_b = geode::point_tetrahedron_position( b, tetra );
    geode::OpenGeodeGeometryException::test(
        position_b == geode::POSITION::vertex1,
        "Wrong result for point_tetrahedron_position with query point "
        "b" );

    const auto position_c = geode::point_tetrahedron_position( c, tetra );
    geode::OpenGeodeGeometryException::test(
        position_c == geode::POSITION::vertex2,
        "Wrong result for point_tetrahedron_position with query point "
        "c" );

    const auto position_d = geode::point_tetrahedron_position( d, tetra );
    geode::OpenGeodeGeometryException::test(
        position_d == geode::POSITION::vertex3,
        "Wrong result for point_tetrahedron_position with query point "
        "d" );

    const geode::Point3D q1{ { 0.5, 1e-10, 1e-10 } };
    const auto position_q1 = geode::point_tetrahedron_position( q1, tetra );
    geode::OpenGeodeGeometryException::test(
        position_q1 == geode::POSITION::inside,
        "Wrong result for point_tetrahedron_position with query point "
        "q1" );

    const geode::Point3D q2{ { 0.5, 1e-10, -1e-10 } };
    const auto position_q2 = geode::point_tetrahedron_position( q2, tetra );
    geode::OpenGeodeGeometryException::test(
        position_q2 == geode::POSITION::outside,
        "Wrong result for point_tetrahedron_position with query point "
        "q2" );

    const geode::Point3D q3{ { 0.5, 1e-10, 0.0 } };
    const auto position_q3 = geode::point_tetrahedron_position( q3, tetra );
    geode::OpenGeodeGeometryException::test(
        position_q3 == geode::POSITION::facet3,
        "Wrong result for point_tetrahedron_position with query point "
        "q3" );

    const geode::Point3D q4{ { 0.5, 0.0, 0.0 } };
    const auto position_q4 = geode::point_tetrahedron_position( q4, tetra );
    geode::OpenGeodeGeometryException::test(
        position_q4 == geode::POSITION::edge01,
        "Wrong result for point_tetrahedron_position with query point "
        "q4" );

    const geode::Point3D q5{ { 0.0, 0.5, 0.0 } };
    const auto position_q5 = geode::point_tetrahedron_position( q5, tetra );
    geode::OpenGeodeGeometryException::test(
        position_q5 == geode::POSITION::edge02,
        "Wrong result for point_tetrahedron_position with query point "
        "q5" );

    const geode::Point3D q6{ { 0.0, 0.0, 0.5 } };
    const auto position_q6 = geode::point_tetrahedron_position( q6, tetra );
    geode::OpenGeodeGeometryException::test(
        position_q6 == geode::POSITION::edge03,
        "Wrong result for point_tetrahedron_position with query point "
        "q6" );

    const geode::Point3D q7{ { 0.5, 0.5, 0.0 } };
    const auto position_q7 = geode::point_tetrahedron_position( q7, tetra );
    geode::OpenGeodeGeometryException::test(
        position_q7 == geode::POSITION::edge12,
        "Wrong result for point_tetrahedron_position with query point "
        "q7" );

    const geode::Point3D q8{ { 0.5, 0.0, 0.5 } };
    const auto position_q8 = geode::point_tetrahedron_position( q8, tetra );
    geode::OpenGeodeGeometryException::test(
        position_q8 == geode::POSITION::edge13,
        "Wrong result for point_tetrahedron_position with query point "
        "q8" );

    const geode::Point3D q9{ { 0.0, 0.5, 0.5 } };
    const auto position_q9 = geode::point_tetrahedron_position( q9, tetra );
    geode::OpenGeodeGeometryException::test(
        position_q9 == geode::POSITION::edge23,
        "Wrong result for point_tetrahedron_position with query point "
        "q9" );
}

void test_nan_point_position()
{
    const auto nan = std::numeric_limits< double >::quiet_NaN();
    const geode::Point2D a2{ { 0, 0 } };
    const geode::Point2D b2{ { 1, 0 } };
    const geode::Point2D c2{ { 0, 1 } };
    const geode::Point2D nan2{ { nan, nan } };
    geode::OpenGeodeGeometryException::test(
        geode::point_triangle_position( nan2, geode::Triangle2D{ a2, b2, c2 } )
            == geode::POSITION::outside,
        "[Test] NaN point should be outside the 2D triangle" );
    const geode::Point3D a{ { 0, 0, 0 } };
    const geode::Point3D b{ { 1, 0, 0 } };
    const geode::Point3D c{ { 0, 1, 0 } };
    const geode::Point3D d{ { 0, 0, 1 } };
    const geode::Point3D nan3{ { nan, nan, nan } };
    geode::OpenGeodeGeometryException::test(
        geode::point_triangle_position( nan3, geode::Triangle3D{ a, b, c } )
            == geode::POSITION::outside,
        "[Test] NaN point should be outside the 3D triangle" );
    geode::OpenGeodeGeometryException::test(
        geode::point_tetrahedron_position(
            nan3, geode::Tetrahedron{ a, b, c, d } )
            == geode::POSITION::outside,
        "[Test] NaN point should be outside the tetrahedron" );
}

void test_random_positions_against_exact_sides()
{
    std::mt19937 generator{ 42 };
    std::uniform_int_distribution< int > distribution{ -20, 20 };
    const auto random_point = [&generator, &distribution] {
        return geode::Point3D{ { static_cast< double >(
                                     distribution( generator ) ),
            static_cast< double >( distribution( generator ) ),
            static_cast< double >( distribution( generator ) ) } };
    };
    for( const auto unused : geode::Range{ 5000 } )
    {
        geode_unused( unused );
        const std::array< geode::Point3D, 4 > vertices{ random_point(),
            random_point(), random_point(), random_point() };
        const geode::Tetrahedron tetra{ vertices[0], vertices[1], vertices[2],
            vertices[3] };
        const auto query = random_point();
        std::array< geode::SIDE, 4 > sides;
        for( const auto f : geode::LRange{ 4 } )
        {
            const auto& facet = geode::Tetrahedron::tetrahedron_facet_vertex[f];
            sides[f] = geode::point_side_to_triangle(
                query, { vertices[facet[0]], vertices[facet[1]],
                           vertices[facet[2]] } );
        }
        const auto position = geode::point_tetrahedron_position( query, tetra );
        const auto all_positive =
            absl::c_all_of( sides, []( geode::SIDE side ) {
                return side == geode::SIDE::positive;
            } );
        const auto any_negative =
            absl::c_any_of( sides, []( geode::SIDE side ) {
                return side == geode::SIDE::negative;
            } );
        if( all_positive )
        {
            geode::OpenGeodeGeometryException::test(
                position == geode::POSITION::inside,
                "[Test] Wrong inside tetrahedron position" );
        }
        else if( any_negative )
        {
            geode::OpenGeodeGeometryException::test(
                position == geode::POSITION::outside,
                "[Test] Wrong outside tetrahedron position" );
        }
        else
        {
            geode::OpenGeodeGeometryException::test(
                position != geode::POSITION::inside
                    && position != geode::POSITION::outside,
                "[Test] Wrong boundary tetrahedron position" );
        }

        const geode::Triangle3D triangle{ vertices[0], vertices[1],
            vertices[2] };
        const auto triangle_position =
            geode::point_triangle_position( query, triangle );
        if( geode::point_side_to_triangle( query, triangle )
            != geode::SIDE::zero )
        {
            geode::OpenGeodeGeometryException::test(
                triangle_position == geode::POSITION::outside,
                "[Test] Point off the triangle plane should be outside" );
        }
        const auto edge_middle = ( vertices[0] + vertices[1] ) / 2.;
        if( vertices[0] != vertices[1]
            && !geode::are_points_aligned(
                vertices[0], vertices[1], vertices[2] ) )
        {
            geode::OpenGeodeGeometryException::test(
                geode::point_triangle_position( edge_middle, triangle )
                    == geode::POSITION::edge0,
                "[Test] Edge middle should be on triangle edge0" );
        }
    }
}

void test_position_to_index_and_string()
{
    const std::array<
        std::tuple< geode::POSITION, geode::local_index_t, std::string >, 21 >
        expected{ { { geode::POSITION::outside, geode::NO_LID, "NO_LID" },
            { geode::POSITION::inside, geode::NO_LID, "NO_LID" },
            { geode::POSITION::vertex0, 0, "vertex0" },
            { geode::POSITION::vertex1, 1, "vertex1" },
            { geode::POSITION::vertex2, 2, "vertex2" },
            { geode::POSITION::vertex3, 3, "vertex3" },
            { geode::POSITION::edge0, 0, "edge0" },
            { geode::POSITION::edge1, 1, "edge1" },
            { geode::POSITION::edge2, 2, "edge2" },
            { geode::POSITION::edge01, 0, "edge01" },
            { geode::POSITION::edge02, 1, "edge02" },
            { geode::POSITION::edge03, 2, "edge03" },
            { geode::POSITION::edge12, 3, "edge12" },
            { geode::POSITION::edge13, 4, "edge13" },
            { geode::POSITION::edge23, 5, "edge23" },
            { geode::POSITION::facet0, 0, "facet0" },
            { geode::POSITION::facet1, 1, "facet1" },
            { geode::POSITION::facet2, 2, "facet2" },
            { geode::POSITION::facet3, 3, "facet3" },
            { geode::POSITION::parallel, geode::NO_LID, "NO_LID" },
            { static_cast< geode::POSITION >( 100 ), geode::NO_LID,
                "NO_LID" } } };
    for( const auto& [position, index, name] : expected )
    {
        geode::OpenGeodeGeometryException::test(
            geode::detail::position_to_index( position ) == index,
            "[Test] Wrong position_to_index for ", name );
        geode::OpenGeodeGeometryException::test(
            geode::detail::position_to_string( position ) == name,
            "[Test] Wrong position_to_string for ", name );
    }
}

void test_point_side_to_triangle()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 0.0 } };
    const geode::Triangle3D triangle3D{ a, b, c };

    const geode::Point3D q1{ { 0, 0, 1.0 } };
    const auto side_q1 = geode::point_side_to_triangle( q1, triangle3D );
    geode::OpenGeodeGeometryException::test( side_q1 == geode::SIDE::positive,
        "Wrong result for point_side_to_triangle with query point "
        "q1" );

    const geode::Point3D q2{ { 0, 0, -6.0 } };
    const auto side_q2 = geode::point_side_to_triangle( q2, triangle3D );
    geode::OpenGeodeGeometryException::test( side_q2 == geode::SIDE::negative,
        "Wrong result for point_side_to_triangle with query point "
        "q2" );

    const geode::Point3D q3{ { -2.0, 4.0, 0.0 } };
    const auto side_q3 = geode::point_side_to_triangle( q3, triangle3D );
    geode::OpenGeodeGeometryException::test( side_q3 == geode::SIDE::zero,
        "Wrong result for point_side_to_triangle with query point "
        "q3" );
}

// Smallest double strictly greater than value, i.e. value + 1 ulp
double next_double_above( double value )
{
    return std::nextafter( value, std::numeric_limits< double >::infinity() );
}

// Largest double strictly lower than value, i.e. value - 1 ulp
double next_double_below( double value )
{
    return std::nextafter( value, -std::numeric_limits< double >::infinity() );
}

void test_point_inside_polygon()
{
    const geode::Point2D a{ { 0, 0 } };
    const geode::Point2D b{ { 1, 0 } };
    const geode::Point2D c{ { 1, 1 } };
    const geode::Point2D d{ { 0, 1 } };
    const geode::Polygon2D ccw{ { a, b, c, d } };
    const geode::Polygon2D cw{ { a, d, c, b } };
    const std::array< geode::Point2D, 9 > inside_points{
        geode::Point2D{ { 0.5, 0.5 } }, geode::Point2D{ { 0.5, 0 } },
        geode::Point2D{ { 1, 0.5 } }, geode::Point2D{ { 0.5, 1 } },
        geode::Point2D{ { 0, 0.5 } }, a, b, c, d
    };
    for( const auto& point : inside_points )
    {
        geode::OpenGeodeGeometryException::test(
            geode::is_point_inside_polygon( point, ccw )
                && geode::is_point_inside_polygon( point, cw ),
            "Point ", point.string(), " should be inside the polygon" );
    }
    const std::array< geode::Point2D, 6 > outside_points{
        geode::Point2D{ { 1.5, 0.5 } }, geode::Point2D{ { -0.5, 0.5 } },
        geode::Point2D{ { 0.5, 1.5 } }, geode::Point2D{ { 0.5, -0.5 } },
        geode::Point2D{ { 2, 0 } }, geode::Point2D{ { -1, 1 } }
    };
    for( const auto& point : outside_points )
    {
        geode::OpenGeodeGeometryException::test(
            !geode::is_point_inside_polygon( point, ccw )
                && !geode::is_point_inside_polygon( point, cw ),
            "Point ", point.string(), " should be outside the polygon" );
    }

    // Points inside the bounding box of the diagonal edge but not on it
    const geode::Polygon2D triangle{ { a, b, c } };
    const std::array< geode::Point2D, 2 > off_diagonal_points{
        geode::Point2D{ { 0.2, 0.8 } },
        geode::Point2D{ { 0.5, next_double_above( 0.5 ) } }
    };
    for( const auto& point : off_diagonal_points )
    {
        geode::OpenGeodeGeometryException::test(
            !geode::is_point_inside_polygon( point, triangle ), "Point ",
            point.string(), " should be outside the triangle" );
    }
    const std::array< geode::Point2D, 2 > on_diagonal_points{
        geode::Point2D{ { 0.2, 0.2 } }, geode::Point2D{ { 0.5, 0.5 } }
    };
    for( const auto& point : on_diagonal_points )
    {
        geode::OpenGeodeGeometryException::test(
            geode::is_point_inside_polygon( point, triangle ), "Point ",
            point.string(), " should be on the triangle boundary" );
    }
}

void test_exact_predicates_near_degenerate()
{
    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ -1e3, 1e3 };
    for( const auto trial : geode::Range{ 1000 } )
    {
        geode_unused( trial );
        auto x0 = distribution( generator );
        auto x1 = distribution( generator );
        if( x0 == x1 )
        {
            continue;
        }
        if( x0 > x1 )
        {
            std::swap( x0, x1 );
        }
        const auto x2 = distribution( generator );
        // Points on the line y = x are exactly collinear
        const geode::Point2D p0{ { x0, x0 } };
        const geode::Point2D p1{ { x1, x1 } };
        const geode::Segment2D segment{ p0, p1 };
        geode::OpenGeodeGeometryException::test(
            geode::point_side_to_segment(
                geode::Point2D{ { x2, x2 } }, segment )
                == geode::SIDE::zero,
            "Wrong side for collinear point" );
        const auto side_up = geode::point_side_to_segment(
            geode::Point2D{ { x2, next_double_above( x2 ) } }, segment );
        const auto side_down = geode::point_side_to_segment(
            geode::Point2D{ { x2, next_double_below( x2 ) } }, segment );
        geode::OpenGeodeGeometryException::test(
            side_up != geode::SIDE::zero && side_down != geode::SIDE::zero
                && side_up != side_down,
            "Wrong side for points one ulp away from the line" );

        // Points on the plane x = y are exactly coplanar
        const auto z0 = distribution( generator );
        const auto z1 = distribution( generator );
        const geode::Point3D q0{ { x0, x0, z0 } };
        const geode::Point3D q1{ { x1, x1, z0 } };
        const geode::Point3D q2{ { x2, x2, z1 } };
        const geode::Triangle3D triangle{ q0, q1, q2 };
        const auto x3 = distribution( generator );
        const auto z3 = distribution( generator );
        geode::OpenGeodeGeometryException::test(
            geode::point_side_to_triangle(
                geode::Point3D{ { x3, x3, z3 } }, triangle )
                == geode::SIDE::zero,
            "Wrong side for coplanar point" );
        const auto side_above = geode::point_side_to_triangle(
            geode::Point3D{ { x3, next_double_above( x3 ), z3 } }, triangle );
        const auto side_below = geode::point_side_to_triangle(
            geode::Point3D{ { x3, next_double_below( x3 ), z3 } }, triangle );
        geode::OpenGeodeGeometryException::test(
            side_above != geode::SIDE::zero && side_below != geode::SIDE::zero
                && side_above != side_below,
            "Wrong side for points one ulp away from the plane" );
    }
}

void test_exact_orientation_large_coordinates()
{
    // Products of large coordinates where an FMA contraction in the exact
    // arithmetic would lose the error terms.
    std::mt19937 generator{ 7 };
    std::uniform_real_distribution< double > distribution{ -1e8, 1e8 };
    for( const auto trial : geode::Range{ 1000 } )
    {
        geode_unused( trial );
        const auto x0 = distribution( generator );
        const auto x1 = distribution( generator );
        const auto x2 = distribution( generator );
        const geode::Point2D p0{ { x0, 3 * x0 } };
        const geode::Point2D p1{ { x1, 3 * x1 } };
        const auto y2 = 3 * x2;
        const auto side_up = geode::point_side_to_segment(
            geode::Point2D{ { x2, next_double_above( y2 ) } },
            geode::Segment2D{ p0, p1 } );
        const auto side_down = geode::point_side_to_segment(
            geode::Point2D{ { x2, next_double_below( y2 ) } },
            geode::Segment2D{ p0, p1 } );
        geode::OpenGeodeGeometryException::test(
            x0 == x1
                || ( side_up != side_down && side_up != geode::SIDE::zero
                     && side_down != geode::SIDE::zero )
                || ( geode::point_side_to_segment(
                         geode::Point2D{ { x2, y2 } }, { p0, p1 } )
                     != geode::SIDE::zero ),
            "Wrong side for points one ulp away from a line with large "
            "coordinates" );
    }
}

void test()
{
    test_point_side_to_segment();
    test_point_side_to_plane();
    test_point_side_to_triangle();
    test_point_segment_position();
    test_point_triangle_position();
    test_point_tetrahedron_position();
    test_nan_point_position();
    test_position_to_index_and_string();
    test_random_positions_against_exact_sides();
    test_point_inside_polygon();
    test_exact_predicates_near_degenerate();
    test_exact_orientation_large_coordinates();
}

OPENGEODE_TEST( "position" )