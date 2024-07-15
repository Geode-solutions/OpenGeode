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

#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/information.h>
#include <geode/geometry/point.h>
#include <geode/geometry/position.h>

#include <geode/tests/common.h>

void test_point_side_to_segment()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Segment2D segment2D{ a, b };

    const auto side_a = geode::point_side_to_segment( a, segment2D );
    OPENGEODE_EXCEPTION( side_a == geode::SIDE::zero,
        "[Test] Wrong result for point_side_to_segment with query point "
        "a" );

    const auto side_b = geode::point_side_to_segment( b, segment2D );
    OPENGEODE_EXCEPTION( side_b == geode::SIDE::zero,
        "[Test] Wrong result for point_side_to_segment with query point "
        "b" );

    const geode::Point2D q1{ { 0.5, 0 } };
    const auto side_q1 = geode::point_side_to_segment( q1, segment2D );
    OPENGEODE_EXCEPTION( side_q1 == geode::SIDE::zero,
        "[Test] Wrong result for point_side_to_segment with query point "
        "q1" );

    const geode::Point2D q2{ { 10.0, 0 } };
    const auto side_q2 = geode::point_side_to_segment( q2, segment2D );
    OPENGEODE_EXCEPTION( side_q2 == geode::SIDE::zero,
        "[Test] Wrong result for point_side_to_segment with query point "
        "q2" );

    const geode::Point2D q3{ { 0.3, 2.0 } };
    const auto side_q3 = geode::point_side_to_segment( q3, segment2D );
    OPENGEODE_EXCEPTION( side_q3 == geode::SIDE::positive,
        "[Test] Wrong result for point_side_to_segment with query point "
        "q3" );

    const geode::Point2D q4{ { 12.0, -6.0 } };
    const auto side_q4 = geode::point_side_to_segment( q4, segment2D );
    OPENGEODE_EXCEPTION( side_q4 == geode::SIDE::negative,
        "[Test] Wrong result for point_side_to_segment with query point "
        "q4" );

    const geode::Point2D q5{ { 0.0, -1e-10 } };
    const auto side_q5 = geode::point_side_to_segment( q5, segment2D );
    OPENGEODE_EXCEPTION( side_q5 == geode::SIDE::negative,
        "[Test] Wrong result for point_side_to_segment with query point "
        "q5" );
}

void test_point_side_to_plane()
{
    const geode::Point3D origin{ { 0.0, 0.0, 2.0 } };
    const geode::Vector3D normal{ { 0.0, 0.0, 4.0 } };
    const geode::Plane plane{ normal, origin };

    const geode::Point3D q1{ { 0.5, 1.0, 2.0 } };
    const auto side_q1 = geode::point_side_to_plane( q1, plane );
    OPENGEODE_EXCEPTION( side_q1 == geode::SIDE::zero,
        "[Test] Wrong result for point_side_to_plane with query point "
        "q1" );

    const geode::Point3D q2{ { 0.5, 1.0, 3.0 } };
    const auto side_q2 = geode::point_side_to_plane( q2, plane );
    OPENGEODE_EXCEPTION( side_q2 == geode::SIDE::positive,
        "[Test] Wrong result for point_side_to_plane with query point "
        "q2" );

    const geode::Point3D q3{ { 0.5, 1.0, 1.0 } };
    const auto side_q3 = geode::point_side_to_plane( q3, plane );
    OPENGEODE_EXCEPTION( side_q3 == geode::SIDE::negative,
        "[Test] Wrong result for point_side_to_plane with query point "
        "q3" );

    const geode::Point3D q4{ { 0.5, 1.0, 2.0 + 1e-10 } };
    const auto side_q4 = geode::point_side_to_plane( q4, plane );
    OPENGEODE_EXCEPTION( side_q4 == geode::SIDE::positive,
        "[Test] Wrong result for point_side_to_plane with query point "
        "q4" );

    const geode::Point3D q5{ { 0.5, 1.0, 2.0 - 1e-10 } };
    const auto side_q5 = geode::point_side_to_plane( q5, plane );
    OPENGEODE_EXCEPTION( side_q5 == geode::SIDE::negative,
        "[Test] Wrong result for point_side_to_plane with query point "
        "q5" );
}

void test_point_segment_position_2d()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Segment2D segment2D{ a, b };

    const auto position_a = geode::point_segment_position( a, segment2D );
    OPENGEODE_EXCEPTION( position_a == geode::POSITION::vertex0,
        "[Test] Wrong result for point_segment_position_2d with query point "
        "a" );

    const auto position_b = geode::point_segment_position( b, segment2D );
    OPENGEODE_EXCEPTION( position_b == geode::POSITION::vertex1,
        "[Test] Wrong result for point_segment_position_2d with query point "
        "b" );

    const geode::Point2D q1{ { 0.5, 0 } };
    const auto position_q1 = geode::point_segment_position( q1, segment2D );
    OPENGEODE_EXCEPTION( position_q1 == geode::POSITION::inside,
        "[Test] Wrong result for point_segment_position_2d with query point "
        "q1" );

    const geode::Point2D q2{ { 10.0, 0 } };
    const auto position_q2 = geode::point_segment_position( q2, segment2D );
    OPENGEODE_EXCEPTION( position_q2 == geode::POSITION::outside,
        "[Test] Wrong result for point_segment_position_2d with query point "
        "q2" );

    const geode::Point2D q3{ { 0.3, 2.0 } };
    const auto position_q3 = geode::point_segment_position( q3, segment2D );
    OPENGEODE_EXCEPTION( position_q3 == geode::POSITION::outside,
        "[Test] Wrong result for point_segment_position_2d with query point "
        "q3" );

    const geode::Point2D q4{ { 12.0, -6.0 } };
    const auto position_q4 = geode::point_segment_position( q4, segment2D );
    OPENGEODE_EXCEPTION( position_q4 == geode::POSITION::outside,
        "[Test] Wrong result for point_segment_position_2d with query point "
        "q4" );

    const geode::Point2D q5{ { 0.5, -1e-10 } };
    const auto position_q5 = geode::point_segment_position( q5, segment2D );
    OPENGEODE_EXCEPTION( position_q5 == geode::POSITION::outside,
        "[Test] Wrong result for point_segment_position_2d with query point "
        "q5" );
}

void test_point_segment_position_3d()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Segment3D segment3D{ a, b };

    const auto position_a = geode::point_segment_position( a, segment3D );
    OPENGEODE_EXCEPTION( position_a == geode::POSITION::vertex0,
        "[Test] Wrong result for point_segment_position_3d with query point "
        "a" );

    const auto position_b = geode::point_segment_position( b, segment3D );
    OPENGEODE_EXCEPTION( position_b == geode::POSITION::vertex1,
        "[Test] Wrong result for point_segment_position_3d with query point "
        "b" );

    const geode::Point3D q1{ { 0.5, 0, 0 } };
    const auto position_q1 = geode::point_segment_position( q1, segment3D );
    OPENGEODE_EXCEPTION( position_q1 == geode::POSITION::inside,
        "[Test] Wrong result for point_segment_position_3d with query point "
        "q1" );

    const geode::Point3D q2{ { 10.0, 0, 0 } };
    const auto position_q2 = geode::point_segment_position( q2, segment3D );
    OPENGEODE_EXCEPTION( position_q2 == geode::POSITION::outside,
        "[Test] Wrong result for point_segment_position_3d with query point "
        "q2" );

    const geode::Point3D q3{ { 0.3, 2.0, 0 } };
    const auto position_q3 = geode::point_segment_position( q3, segment3D );
    OPENGEODE_EXCEPTION( position_q3 == geode::POSITION::outside,
        "[Test] Wrong result for point_segment_position_3d with query point "
        "q3" );

    const geode::Point3D q4{ { 12.0, -6.0, 0 } };
    const auto position_q4 = geode::point_segment_position( q4, segment3D );
    OPENGEODE_EXCEPTION( position_q4 == geode::POSITION::outside,
        "[Test] Wrong result for point_segment_position_3d with query point "
        "q4" );

    const geode::Point3D q5{ { 0.5, -1e-10, 0 } };
    const auto position_q5 = geode::point_segment_position( q5, segment3D );
    OPENGEODE_EXCEPTION( position_q5 == geode::POSITION::outside,
        "[Test] Wrong result for point_segment_position_3d with query point "
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
    OPENGEODE_EXCEPTION( position_q1 == geode::POSITION::outside,
        "[Test] Wrong result for test_point_triangle_degenerated_position_2d "
        "with query point q1" );

    const auto position_c = geode::point_triangle_position( c, triangle2D );
    OPENGEODE_EXCEPTION( position_c == geode::POSITION::parallel,
        "[Test] Wrong result for test_point_triangle_degenerated_position_2d "
        "with query point c" );

    const geode::Point2D q2{ { 0.75, 0.75 } };
    const auto position_q2 = geode::point_triangle_position( q2, triangle2D );
    OPENGEODE_EXCEPTION( position_q2 == geode::POSITION::parallel,
        "[Test] Wrong result for test_point_triangle_degenerated_position_2d "
        "with query point q2" );
}

void test_point_triangle_position_2d()
{
    const geode::Point2D a{ { 0.0, 0.0 } };
    const geode::Point2D b{ { 1.0, 0.0 } };
    const geode::Point2D c{ { 1.0, 1.0 } };
    const geode::Triangle2D triangle2D{ a, b, c };

    const auto position_a = geode::point_triangle_position( a, triangle2D );
    OPENGEODE_EXCEPTION( position_a == geode::POSITION::vertex0,
        "[Test] Wrong result for point_triangle_position_2d with query point "
        "a" );

    const auto position_b = geode::point_triangle_position( b, triangle2D );
    OPENGEODE_EXCEPTION( position_b == geode::POSITION::vertex1,
        "[Test] Wrong result for point_triangle_position_2d with query point "
        "b" );

    const geode::Point2D q1{ { 0.5, 0 } };
    const auto position_q1 = geode::point_triangle_position( q1, triangle2D );
    OPENGEODE_EXCEPTION( position_q1 == geode::POSITION::edge0,
        "[Test] Wrong result for point_triangle_position_2d with query point "
        "q1" );

    const geode::Point2D q2{ { 0.0, 1.0 } };
    const auto position_q2 = geode::point_triangle_position( q2, triangle2D );
    OPENGEODE_EXCEPTION( position_q2 == geode::POSITION::outside,
        "[Test] Wrong result for point_triangle_position_2d with query point "
        "q2" );

    const geode::Point2D q3{ { 2.0, 1.0 } };
    const auto position_q3 = geode::point_triangle_position( q3, triangle2D );
    OPENGEODE_EXCEPTION( position_q3 == geode::POSITION::outside,
        "[Test] Wrong result for point_triangle_position_2d with query point "
        "q3" );

    const geode::Point2D q4{ { 0.5, 1e-10 } };
    const auto position_q4 = geode::point_triangle_position( q4, triangle2D );
    OPENGEODE_EXCEPTION( position_q4 == geode::POSITION::inside,
        "[Test] Wrong result for point_triangle_position_2d with query point "
        "q4" );

    const geode::Point2D q5{ { 0.5, -1e-10 } };
    const auto position_q5 = geode::point_triangle_position( q5, triangle2D );
    OPENGEODE_EXCEPTION( position_q5 == geode::POSITION::outside,
        "[Test] Wrong result for point_triangle_position_2d with query point "
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
    OPENGEODE_EXCEPTION( position_qout == geode::POSITION::outside,
        "[Test] Wrong result for point_triangle_position_3d with query point "
        "qout" );

    const geode::Point3D q0{ { 0.75, 0.25, 0.0 } };
    const auto position_q0 = geode::point_triangle_position( q0, triangle3D );
    OPENGEODE_EXCEPTION( position_q0 == geode::POSITION::inside,
        "[Test] Wrong result for point_triangle_position_3d with query point "
        "q0" );

    const auto position_a = geode::point_triangle_position( a, triangle3D );
    OPENGEODE_EXCEPTION( position_a == geode::POSITION::vertex0,
        "[Test] Wrong result for point_triangle_position_3d with query point "
        "a" );

    const auto position_b = geode::point_triangle_position( b, triangle3D );
    OPENGEODE_EXCEPTION( position_b == geode::POSITION::vertex1,
        "[Test] Wrong result for point_triangle_position_3d with query point "
        "b" );

    const geode::Point3D q1{ { 0.5, 0.0, 0.0 } };
    const auto position_q1 = geode::point_triangle_position( q1, triangle3D );
    OPENGEODE_EXCEPTION( position_q1 == geode::POSITION::edge0,
        "[Test] Wrong result for point_triangle_position_3d with query point "
        "q1" );

    const geode::Point3D q2{ { 0.0, 1.0, 0.0 } };
    const auto position_q2 = geode::point_triangle_position( q2, triangle3D );
    OPENGEODE_EXCEPTION( position_q2 == geode::POSITION::outside,
        "[Test] Wrong result for point_triangle_position_3d with query point "
        "q2" );

    const geode::Point3D q3{ { 2.0, 1.0, 0.0 } };
    const auto position_q3 = geode::point_triangle_position( q3, triangle3D );
    OPENGEODE_EXCEPTION( position_q3 == geode::POSITION::outside,
        "[Test] Wrong result for point_triangle_position_3d with query point "
        "q3" );

    const geode::Point3D q4{ { 0.5, 1e-10, 0.0 } };
    const auto position_q4 = geode::point_triangle_position( q4, triangle3D );
    OPENGEODE_EXCEPTION( position_q4 == geode::POSITION::inside,
        "[Test] Wrong result for point_triangle_position_3d with query point "
        "q4" );

    const geode::Point3D q5{ { 0.5, -1e-10, 0.0 } };
    const auto position_q5 = geode::point_triangle_position( q5, triangle3D );
    OPENGEODE_EXCEPTION( position_q5 == geode::POSITION::outside,
        "[Test] Wrong result for point_triangle_position_3d with query point "
        "q5" );

    const geode::Point3D q6{ { 0.5, 1e-10, 1e-10 } };
    const auto position_q6 = geode::point_triangle_position( q6, triangle3D );
    OPENGEODE_EXCEPTION( position_q6 == geode::POSITION::outside,
        "[Test] Wrong result for point_triangle_position_3d with query point "
        "q6" );

    const geode::Point3D qdeg{ { 0.75, 0.0, 0.0 } };
    const auto position_inside_degen =
        geode::point_triangle_position( qdeg, triangle3D_degen );

    OPENGEODE_EXCEPTION( position_inside_degen == geode::POSITION::parallel,
        "[Test] Wrong result for point_triangle_position_3d with query point "
        "qdeg (degenerate case)" );
}

void test_point_triangle_position()
{
    test_point_triangle_position_2d();
    test_point_triangle_degenerated_position_2d();
    test_point_triangle_position_3d();
}

void test_point_tetrahedron_position()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 0.0, 1.0, 0.0 } };
    const geode::Point3D d{ { 0.0, 0.0, 1.0 } };
    const geode::Tetrahedron tetra{ a, b, c, d };

    const auto position_a = geode::point_tetrahedron_position( a, tetra );
    OPENGEODE_EXCEPTION( position_a == geode::POSITION::vertex0,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "a" );

    const auto position_b = geode::point_tetrahedron_position( b, tetra );
    OPENGEODE_EXCEPTION( position_b == geode::POSITION::vertex1,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "b" );

    const auto position_c = geode::point_tetrahedron_position( c, tetra );
    OPENGEODE_EXCEPTION( position_c == geode::POSITION::vertex2,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "c" );

    const auto position_d = geode::point_tetrahedron_position( d, tetra );
    OPENGEODE_EXCEPTION( position_d == geode::POSITION::vertex3,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "d" );

    const geode::Point3D q1{ { 0.5, 1e-10, 1e-10 } };
    const auto position_q1 = geode::point_tetrahedron_position( q1, tetra );
    OPENGEODE_EXCEPTION( position_q1 == geode::POSITION::inside,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "q1" );

    const geode::Point3D q2{ { 0.5, 1e-10, -1e-10 } };
    const auto position_q2 = geode::point_tetrahedron_position( q2, tetra );
    OPENGEODE_EXCEPTION( position_q2 == geode::POSITION::outside,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "q2" );

    const geode::Point3D q3{ { 0.5, 1e-10, 0.0 } };
    const auto position_q3 = geode::point_tetrahedron_position( q3, tetra );
    OPENGEODE_EXCEPTION( position_q3 == geode::POSITION::facet3,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "q3" );

    const geode::Point3D q4{ { 0.5, 0.0, 0.0 } };
    const auto position_q4 = geode::point_tetrahedron_position( q4, tetra );
    OPENGEODE_EXCEPTION( position_q4 == geode::POSITION::edge01,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "q4" );

    const geode::Point3D q5{ { 0.0, 0.5, 0.0 } };
    const auto position_q5 = geode::point_tetrahedron_position( q5, tetra );
    OPENGEODE_EXCEPTION( position_q5 == geode::POSITION::edge02,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "q5" );

    const geode::Point3D q6{ { 0.0, 0.0, 0.5 } };
    const auto position_q6 = geode::point_tetrahedron_position( q6, tetra );
    OPENGEODE_EXCEPTION( position_q6 == geode::POSITION::edge03,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "q6" );

    const geode::Point3D q7{ { 0.5, 0.5, 0.0 } };
    const auto position_q7 = geode::point_tetrahedron_position( q7, tetra );
    OPENGEODE_EXCEPTION( position_q7 == geode::POSITION::edge12,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "q7" );

    const geode::Point3D q8{ { 0.5, 0.0, 0.5 } };
    const auto position_q8 = geode::point_tetrahedron_position( q8, tetra );
    OPENGEODE_EXCEPTION( position_q8 == geode::POSITION::edge13,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "q8" );

    const geode::Point3D q9{ { 0.0, 0.5, 0.5 } };
    const auto position_q9 = geode::point_tetrahedron_position( q9, tetra );
    OPENGEODE_EXCEPTION( position_q9 == geode::POSITION::edge23,
        "[Test] Wrong result for point_tetrahedron_position with query point "
        "q9" );
}

void test_point_side_to_triangle()
{
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D c{ { 1.0, 1.0, 0.0 } };
    const geode::Triangle3D triangle3D{ a, b, c };

    const geode::Point3D q1{ { 0, 0, 1.0 } };
    const auto side_q1 = geode::point_side_to_triangle( q1, triangle3D );
    OPENGEODE_EXCEPTION( side_q1 == geode::SIDE::positive,
        "[Test] Wrong result for point_side_to_triangle with query point "
        "q1" );

    const geode::Point3D q2{ { 0, 0, -6.0 } };
    const auto side_q2 = geode::point_side_to_triangle( q2, triangle3D );
    OPENGEODE_EXCEPTION( side_q2 == geode::SIDE::negative,
        "[Test] Wrong result for point_side_to_triangle with query point "
        "q2" );

    const geode::Point3D q3{ { -2.0, 4.0, 0.0 } };
    const auto side_q3 = geode::point_side_to_triangle( q3, triangle3D );
    OPENGEODE_EXCEPTION( side_q3 == geode::SIDE::zero,
        "[Test] Wrong result for point_side_to_triangle with query point "
        "q3" );
}

void test()
{
    test_point_side_to_segment();
    test_point_side_to_plane();
    test_point_side_to_triangle();
    test_point_segment_position();
    test_point_triangle_position();
    test_point_tetrahedron_position();
}

OPENGEODE_TEST( "position" )