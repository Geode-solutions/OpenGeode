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

#include <geode/basic/logger.h>

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

#include <geode/tests/common.h>

void test_bbox()
{
    geode::BoundingBox2D box;
    box.add_point( { { -1, -1 } } );
    box.add_point( { { 1, 1 } } );

    geode::BoundingBox2D box2;
    box2.add_point( { { -2, -2 } } );
    box2.add_point( { { -1, -1 } } );

    OPENGEODE_EXCEPTION( box2.min() == geode::Point2D( { -2, -2 } ),
        "[Test] Error in BoundingBox initialization" );
    OPENGEODE_EXCEPTION( box2.max() == geode::Point2D( { -1, -1 } ),
        "[Test] Error in BoundingBox initialization" );

    box2.add_box( box );
    OPENGEODE_EXCEPTION( box2.min() == geode::Point2D( { -2, -2 } ),
        "[Test] Error in BoundingBox union computation" );
    OPENGEODE_EXCEPTION( box2.max() == geode::Point2D( { 1, 1 } ),
        "[Test] Error in BoundingBox union computation" );
    OPENGEODE_EXCEPTION( box.intersects( box2 ), "[Test] BBox should overlap" );

    geode::BoundingBox2D box3;
    box3.add_point( { { 2, 2 } } );
    box3.add_point( { { 3, 3 } } );
    OPENGEODE_EXCEPTION(
        !box.intersects( box3 ), "[Test] BBox should not overlap" );
    OPENGEODE_EXCEPTION( box3.contains( { { 2.5, 2.5 } } ),
        "[Test] BBox should contain this point" );
    OPENGEODE_EXCEPTION( !box3.contains( { { 10, 0 } } ),
        "[Test] BBox should not contain this point" );
    OPENGEODE_EXCEPTION( !box3.contains( { { 0, 0 } } ),
        "[Test] BBox should not contain that point" );

    geode::BoundingBox2D box_negative;
    box_negative.add_point( { { -2, -2 } } );
    box_negative.add_point( { { -1.5, -1.5 } } );
    OPENGEODE_EXCEPTION( box_negative.min() == geode::Point2D( { -2, -2 } ),
        "[Test] Error in BoundingBox initialization and point additions" );
    OPENGEODE_EXCEPTION( box_negative.max() == geode::Point2D( { -1.5, -1.5 } ),
        "[Test] Error in BoundingBox initialization and point additions" );
    OPENGEODE_EXCEPTION( !box.intersects( box_negative ),
        "[Test] BBox should not overlap box_negative" );

    const geode::BoundingBox2D copy_box = box2;
    OPENGEODE_EXCEPTION( copy_box.min() == geode::Point2D( { -2, -2 } )
                             && copy_box.max() == geode::Point2D( { 1, 1 } ),
        "[Test] Copied BBox has wrong extension" );

    const auto box_neg2 = std::move( box_negative );
    OPENGEODE_EXCEPTION( box_neg2.min() == geode::Point2D( { -2, -2 } ),
        "[Test] Error in BoundingBox move" );
    OPENGEODE_EXCEPTION( box_neg2.max() == geode::Point2D( { -1.5, -1.5 } ),
        "[Test] Error in BoundingBox move" );

    const geode::BoundingBox2D box_neg3( std::move( box_neg2 ) );
    OPENGEODE_EXCEPTION( box_neg3.min() == geode::Point2D( { -2, -2 } ),
        "[Test] Error in BoundingBox move constructor" );
    OPENGEODE_EXCEPTION( box_neg3.max() == geode::Point2D( { -1.5, -1.5 } ),
        "[Test] Error in BoundingBox move constructor" );
}

void test_ray_intersections()
{
    geode::BoundingBox3D bbox;
    bbox.add_point( { { -1, -1, -1 } } );
    bbox.add_point( { { 1, 1, 1 } } );

    const geode::OwnerRay3D ray_inside{ { { 0, 0, 1 } }, { { 0, 0, 0 } } };
    OPENGEODE_EXCEPTION(
        bbox.intersects( ray_inside ), "[Test] Wrong result with ray_inside" );

    const geode::OwnerRay3D ray_up{ { { 0, 0, 1 } }, { { 0, 0, 2 } } };
    OPENGEODE_EXCEPTION(
        !bbox.intersects( ray_up ), "[Test] Wrong result with ray_up" );

    const geode::OwnerRay3D ray_down{ { { 0, 0, 1 } }, { { 0, 0, -2 } } };
    OPENGEODE_EXCEPTION(
        bbox.intersects( ray_down ), "[Test] Wrong result with ray_down" );
}

void test_line_intersections()
{
    geode::BoundingBox3D bbox;
    bbox.add_point( { { -1, -1, -1 } } );
    bbox.add_point( { { 1, 1, 1 } } );

    const geode::OwnerInfiniteLine3D line_inside{ { { 0, 0, 1 } },
        { { 0, 0, 0 } } };
    OPENGEODE_EXCEPTION( bbox.intersects( line_inside ),
        "[Test] Wrong result with line_inside" );

    const geode::OwnerInfiniteLine3D line_up{ { { 0, 0, 1 } },
        { { 0, 0, 2 } } };
    OPENGEODE_EXCEPTION(
        bbox.intersects( line_up ), "[Test] Wrong result with line_up" );

    const geode::OwnerInfiniteLine3D line_down{ { { 0, 0, 1 } },
        { { 0, 0, -2 } } };
    OPENGEODE_EXCEPTION(
        bbox.intersects( line_down ), "[Test] Wrong result with line_down" );
}

void test_triangle2d_intersections()
{
    geode::BoundingBox2D bbox;
    bbox.add_point( { { -1, -1 } } );
    bbox.add_point( { { 1, 1 } } );

    const geode::Point2D O{ { 0, 0 } };
    const geode::Point2D a{ { 0.5, 0.5 } };
    const geode::Point2D b{ { 0.5, -0.5 } };

    const geode::Triangle2D inside{ O, a, b };
    OPENGEODE_EXCEPTION(
        bbox.intersects( inside ), "[Test] Wrong result with inside" );

    const geode::Point2D c{ { 2, 0 } };
    const geode::Triangle2D one_edge_crossing{ O, a, c };
    OPENGEODE_EXCEPTION( bbox.intersects( one_edge_crossing ),
        "[Test] Wrong result with one_edge_crossing" );

    const geode::Point2D c2{ { 2, 2 } };
    const geode::Triangle2D one_edge_crossing2{ O, a, c2 };
    OPENGEODE_EXCEPTION( bbox.intersects( one_edge_crossing2 ),
        "[Test] Wrong result with one_edge_crossing2" );

    const geode::Point2D d{ { 0, 2 } };
    const geode::Triangle2D two_edge_crossing{ O, d, c };
    OPENGEODE_EXCEPTION( bbox.intersects( two_edge_crossing ),
        "[Test] Wrong result with two_edge_crossing" );

    const geode::Point2D e{ { -2, -2 } };
    const geode::Triangle2D no_edge_crossing{ c, d, e };
    OPENGEODE_EXCEPTION( bbox.intersects( no_edge_crossing ),
        "[Test] Wrong result with no_edge_crossing" );

    const geode::Point2D e2{ { 0, -2 } };
    const geode::Triangle2D no_edge_crossing2{ c, d, e2 };
    OPENGEODE_EXCEPTION( bbox.intersects( no_edge_crossing2 ),
        "[Test] Wrong result with no_edge_crossing2" );

    const geode::Point2D f{ { 5, 0 } };
    const geode::Point2D g{ { 0, 5 } };
    const geode::Point2D h{ { 5, 5 } };
    const geode::Triangle2D no_crossing{ f, g, h };
    OPENGEODE_EXCEPTION( !bbox.intersects( no_crossing ),
        "[Test] Wrong result with no_crossing" );

    const geode::Point2D i{ { -5, -5 } };
    const geode::Point2D j{ { 5, -5 } };
    const geode::Point2D k{ { 0, 5 } };
    const geode::Triangle2D outside{ i, j, k };
    OPENGEODE_EXCEPTION(
        bbox.intersects( outside ), "[Test] Wrong result with outside" );
}

void test_triangle3d_intersections()
{
    geode::BoundingBox3D bbox;
    bbox.add_point( { { -1, -1, -1 } } );
    bbox.add_point( { { 1, 1, 1 } } );

    const geode::Point3D O{ { 0, 0, 0 } };
    const geode::Point3D a{ { 0.5, 0.5, 0.5 } };
    const geode::Point3D b{ { 0.5, 0.5, -0.5 } };

    const geode::Triangle3D inside{ O, a, b };
    OPENGEODE_EXCEPTION(
        bbox.intersects( inside ), "[Test] Wrong result with inside" );

    const geode::Point3D c{ { 2, 0, 0 } };
    const geode::Triangle3D one_edge_crossing{ O, a, c };
    OPENGEODE_EXCEPTION( bbox.intersects( one_edge_crossing ),
        "[Test] Wrong result with one_edge_crossing" );

    const geode::Point3D c2{ { 2, 2, 0 } };
    const geode::Triangle3D one_edge_crossing2{ O, a, c2 };
    OPENGEODE_EXCEPTION( bbox.intersects( one_edge_crossing2 ),
        "[Test] Wrong result with one_edge_crossing2" );

    const geode::Point3D d{ { 0, 0, 2 } };
    const geode::Triangle3D two_edge_crossing{ O, d, c };
    OPENGEODE_EXCEPTION( bbox.intersects( two_edge_crossing ),
        "[Test] Wrong result with two_edge_crossing" );

    const geode::Point3D e{ { -2, -2, 0 } };
    const geode::Triangle3D no_edge_crossing{ c, d, e };
    OPENGEODE_EXCEPTION( bbox.intersects( no_edge_crossing ),
        "[Test] Wrong result with no_edge_crossing" );

    const geode::Point3D e2{ { 0, -2, 0 } };
    const geode::Triangle3D no_edge_crossing2{ c, d, e2 };
    OPENGEODE_EXCEPTION( bbox.intersects( no_edge_crossing2 ),
        "[Test] Wrong result with no_edge_crossing2" );

    const geode::Point3D f{ { 5, 0, 0 } };
    const geode::Point3D g{ { 0, 5, 0 } };
    const geode::Point3D h{ { 0, 0, 5 } };
    const geode::Triangle3D no_crossing{ f, g, h };
    OPENGEODE_EXCEPTION( !bbox.intersects( no_crossing ),
        "[Test] Wrong result with no_crossing" );

    const geode::Point3D g2{ { 5, 5, 0 } };
    const geode::Point3D h2{ { 5, 5, 5 } };
    const geode::Triangle3D no_crossing2{ f, g, h };
    OPENGEODE_EXCEPTION( !bbox.intersects( no_crossing2 ),
        "[Test] Wrong result with no_crossing2" );
}

void test_tetra_intersections()
{
    geode::BoundingBox3D bbox;
    bbox.add_point( { { -1, -1, -1 } } );
    bbox.add_point( { { 1, 1, 1 } } );

    const geode::Point3D a{ { 0.5, 0.5, 0.5 } };
    const geode::Point3D b{ { 0.5, 0.5, -0.5 } };
    const geode::Point3D c{ { -0.5, 0.5, 0.5 } };
    const geode::Point3D d{ { 0.5, -0.5, -0.5 } };

    const geode::Tetrahedron inside{ a, b, c, d };
    OPENGEODE_EXCEPTION(
        bbox.intersects( inside ), "[Test] Wrong result with inside" );

    const geode::Point3D a2{ { -5, -5, -5 } };
    const geode::Point3D b2{ { 5, -5, -5 } };
    const geode::Point3D c2{ { 0, 5, -5 } };
    const geode::Point3D d2{ { 0, 0, 5 } };

    const geode::Tetrahedron outside{ a2, b2, c2, d2 };
    OPENGEODE_EXCEPTION(
        bbox.intersects( outside ), "[Test] Wrong result with outside" );
}

void test_segment_intersections()
{
    geode::BoundingBox2D bbox;
    bbox.add_point( { { -1, -1 } } );
    bbox.add_point( { { 1, 1 } } );

    const geode::Point2D O{ { 0, 0 } };
    const geode::Point2D a{ { 0.5, 0.5 } };

    const geode::Segment2D inside{ O, a };
    OPENGEODE_EXCEPTION(
        bbox.intersects( inside ), "[Test] Wrong result with inside" );

    const geode::Point2D b{ { 5, 5 } };
    const geode::Segment2D crossing_once{ O, b };
    OPENGEODE_EXCEPTION( bbox.intersects( crossing_once ),
        "[Test] Wrong result with crossing_once" );

    const geode::Point2D c{ { -5, -5 } };
    const geode::Segment2D crossing_twice{ b, c };
    OPENGEODE_EXCEPTION( bbox.intersects( crossing_twice ),
        "[Test] Wrong result with crossing_twice" );

    const geode::Point2D d{ { 1.5, 0 } };
    const geode::Point2D e{ { 0, 1.5 } };
    const geode::Segment2D crossing_sideway{ d, e };
    OPENGEODE_EXCEPTION( bbox.intersects( crossing_twice ),
        "[Test] Wrong result with crossing_twice" );

    const geode::Point2D f{ { 5, 0 } };
    const geode::Point2D g{ { 0, 5 } };
    const geode::Segment2D no_crossing{ f, g };
    OPENGEODE_EXCEPTION( !bbox.intersects( no_crossing ),
        "[Test] Wrong result with no_crossing" );

    const geode::Point2D h{ { 5, 5 } };
    const geode::Segment2D parallel{ f, h };
    OPENGEODE_EXCEPTION(
        !bbox.intersects( parallel ), "[Test] Wrong result with parallel" );

    const geode::Point2D i{ { 10, 0 } };
    const geode::Segment2D parallel2{ f, i };
    OPENGEODE_EXCEPTION(
        !bbox.intersects( parallel2 ), "[Test] Wrong result with parallel2" );
}

void test()
{
    test_bbox();
    test_ray_intersections();
    test_line_intersections();
    test_segment_intersections();
    test_tetra_intersections();
    test_triangle2d_intersections();
    test_triangle3d_intersections();
}

OPENGEODE_TEST( "bounding-box" )
