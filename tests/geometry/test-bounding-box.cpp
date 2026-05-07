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

#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>

#include <geode/tests/common.hpp>

void test_bbox()
{
    geode::BoundingBox2D box;
    box.add_point( geode::Point2D{ { -1, -1 } } );
    box.add_point( geode::Point2D{ { 1, 1 } } );

    geode::BoundingBox2D box2;
    box2.add_point( geode::Point2D{ { -2, -2 } } );
    box2.add_point( geode::Point2D{ { -1, -1 } } );

    geode::OpenGeodeGeometryException::test(
        box2.min() == geode::Point2D( { -2, -2 } ),
        "Error in BoundingBox initialization" );
    geode::OpenGeodeGeometryException::test(
        box2.max() == geode::Point2D( { -1, -1 } ),
        "Error in BoundingBox initialization" );

    box2.add_box( box );
    geode::OpenGeodeGeometryException::test(
        box2.min() == geode::Point2D( { -2, -2 } ),
        "Error in BoundingBox union computation" );
    geode::OpenGeodeGeometryException::test(
        box2.max() == geode::Point2D( { 1, 1 } ),
        "Error in BoundingBox union computation" );
    geode::OpenGeodeGeometryException::test(
        box.intersects( box2 ), "BBox should overlap" );

    geode::BoundingBox2D box3;
    box3.add_point( geode::Point2D{ { 2, 2 } } );
    box3.add_point( geode::Point2D{ { 3, 3 } } );
    geode::OpenGeodeGeometryException::test(
        !box.intersects( box3 ), "BBox should not overlap" );
    geode::OpenGeodeGeometryException::test(
        box3.contains( geode::Point2D{ { 2.5, 2.5 } } ),
        "BBox should contain this point" );
    geode::OpenGeodeGeometryException::test(
        !box3.contains( geode::Point2D{ { 10, 0 } } ),
        "BBox should not contain this point" );
    geode::OpenGeodeGeometryException::test(
        !box3.contains( geode::Point2D{ { 0, 0 } } ),
        "BBox should not contain that point" );

    geode::BoundingBox2D box_negative;
    box_negative.add_point( geode::Point2D{ { -2, -2 } } );
    box_negative.add_point( geode::Point2D{ { -1.5, -1.5 } } );
    geode::OpenGeodeGeometryException::test(
        box_negative.min() == geode::Point2D( { -2, -2 } ),
        "Error in BoundingBox initialization and point additions" );
    geode::OpenGeodeGeometryException::test(
        box_negative.max() == geode::Point2D( { -1.5, -1.5 } ),
        "Error in BoundingBox initialization and point additions" );
    geode::OpenGeodeGeometryException::test( !box.intersects( box_negative ),
        "BBox should not overlap box_negative" );

    const geode::BoundingBox2D copy_box = box2;
    geode::OpenGeodeGeometryException::test(
        copy_box.min() == geode::Point2D( { -2, -2 } )
            && copy_box.max() == geode::Point2D( { 1, 1 } ),
        "Copied BBox has wrong extension" );

    const auto box_neg2 = std::move( box_negative );
    geode::OpenGeodeGeometryException::test(
        box_neg2.min() == geode::Point2D( { -2, -2 } ),
        "Error in BoundingBox move" );
    geode::OpenGeodeGeometryException::test(
        box_neg2.max() == geode::Point2D( { -1.5, -1.5 } ),
        "Error in BoundingBox move" );

    const geode::BoundingBox2D box_neg3( std::move( box_neg2 ) );
    geode::OpenGeodeGeometryException::test(
        box_neg3.min() == geode::Point2D( { -2, -2 } ),
        "Error in BoundingBox move constructor" );
    geode::OpenGeodeGeometryException::test(
        box_neg3.max() == geode::Point2D( { -1.5, -1.5 } ),
        "Error in BoundingBox move constructor" );
}

void test_ray_intersections()
{
    geode::BoundingBox3D bbox;
    bbox.add_point( geode::Point3D{ { -1, -1, -1 } } );
    bbox.add_point( geode::Point3D{ { 1, 1, 1 } } );

    const geode::OwnerRay3D ray_inside{ geode::Vector3D{ { 0, 0, 1 } },
        geode::Point3D{ { 0, 0, 0 } } };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( ray_inside ), "Wrong result with ray_inside" );

    const geode::OwnerRay3D ray_up{ geode::Vector3D{ { 0, 0, 1 } },
        geode::Point3D{ { 0, 0, 2 } } };
    geode::OpenGeodeGeometryException::test(
        !bbox.intersects( ray_up ), "Wrong result with ray_up" );

    const geode::OwnerRay3D ray_down{ geode::Vector3D{ { 0, 0, 1 } },
        geode::Point3D{ { 0, 0, -2 } } };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( ray_down ), "Wrong result with ray_down" );
}

void test_line_intersections()
{
    geode::BoundingBox3D bbox;
    bbox.add_point( geode::Point3D{ { -1, -1, -1 } } );
    bbox.add_point( geode::Point3D{ { 1, 1, 1 } } );

    const geode::OwnerInfiniteLine3D line_inside{
        geode::Vector3D{ { 0, 0, 1 } }, geode::Point3D{ { 0, 0, 0 } }
    };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( line_inside ), "Wrong result with line_inside" );

    const geode::OwnerInfiniteLine3D line_up{ geode::Vector3D{ { 0, 0, 1 } },
        geode::Point3D{ { 0, 0, 2 } } };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( line_up ), "Wrong result with line_up" );

    const geode::OwnerInfiniteLine3D line_down{ geode::Vector3D{ { 0, 0, 1 } },
        geode::Point3D{ { 0, 0, -2 } } };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( line_down ), "Wrong result with line_down" );
}

void test_triangle2d_intersections()
{
    geode::BoundingBox2D bbox;
    bbox.add_point( geode::Point2D{ { -1, -1 } } );
    bbox.add_point( geode::Point2D{ { 1, 1 } } );

    const geode::Point2D O{ { 0, 0 } };
    const geode::Point2D a{ { 0.5, 0.5 } };
    const geode::Point2D b{ { 0.5, -0.5 } };

    const geode::Triangle2D inside{ O, a, b };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( inside ), "Wrong result with inside" );

    const geode::Point2D c{ { 2, 0 } };
    const geode::Triangle2D one_edge_crossing{ O, a, c };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( one_edge_crossing ),
        "Wrong result with one_edge_crossing" );

    const geode::Point2D c2{ { 2, 2 } };
    const geode::Triangle2D one_edge_crossing2{ O, a, c2 };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( one_edge_crossing2 ),
        "Wrong result with one_edge_crossing2" );

    const geode::Point2D d{ { 0, 2 } };
    const geode::Triangle2D two_edge_crossing{ O, d, c };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( two_edge_crossing ),
        "Wrong result with two_edge_crossing" );

    const geode::Point2D e{ { -2, -2 } };
    const geode::Triangle2D no_edge_crossing{ c, d, e };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( no_edge_crossing ),
        "Wrong result with no_edge_crossing" );

    const geode::Point2D e2{ { 0, -2 } };
    const geode::Triangle2D no_edge_crossing2{ c, d, e2 };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( no_edge_crossing2 ),
        "Wrong result with no_edge_crossing2" );

    const geode::Point2D f{ { 5, 0 } };
    const geode::Point2D g{ { 0, 5 } };
    const geode::Point2D h{ { 5, 5 } };
    const geode::Triangle2D no_crossing{ f, g, h };
    geode::OpenGeodeGeometryException::test(
        !bbox.intersects( no_crossing ), "Wrong result with no_crossing" );

    const geode::Point2D i{ { -5, -5 } };
    const geode::Point2D j{ { 5, -5 } };
    const geode::Point2D k{ { 0, 5 } };
    const geode::Triangle2D outside{ i, j, k };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( outside ), "Wrong result with outside" );
}

void test_triangle3d_intersections()
{
    geode::BoundingBox3D bbox;
    bbox.add_point( geode::Point3D{ { -1, -1, -1 } } );
    bbox.add_point( geode::Point3D{ { 1, 1, 1 } } );

    const geode::Point3D O{ { 0, 0, 0 } };
    const geode::Point3D a{ { 0.5, 0.5, 0.5 } };
    const geode::Point3D b{ { 0.5, 0.5, -0.5 } };

    const geode::Triangle3D inside{ O, a, b };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( inside ), "Wrong result with inside" );

    const geode::Point3D c{ { 2, 0, 0 } };
    const geode::Triangle3D one_edge_crossing{ O, a, c };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( one_edge_crossing ),
        "Wrong result with one_edge_crossing" );

    const geode::Point3D c2{ { 2, 2, 0 } };
    const geode::Triangle3D one_edge_crossing2{ O, a, c2 };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( one_edge_crossing2 ),
        "Wrong result with one_edge_crossing2" );

    const geode::Point3D d{ { 0, 0, 2 } };
    const geode::Triangle3D two_edge_crossing{ O, d, c };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( two_edge_crossing ),
        "Wrong result with two_edge_crossing" );

    const geode::Point3D e{ { -2, -2, 0 } };
    const geode::Triangle3D no_edge_crossing{ c, d, e };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( no_edge_crossing ),
        "Wrong result with no_edge_crossing" );

    const geode::Point3D e2{ { 0, -2, 0 } };
    const geode::Triangle3D no_edge_crossing2{ c, d, e2 };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( no_edge_crossing2 ),
        "Wrong result with no_edge_crossing2" );

    const geode::Point3D f{ { 5, 0, 0 } };
    const geode::Point3D g{ { 0, 5, 0 } };
    const geode::Point3D h{ { 0, 0, 5 } };
    const geode::Triangle3D no_crossing{ f, g, h };
    geode::OpenGeodeGeometryException::test(
        !bbox.intersects( no_crossing ), "Wrong result with no_crossing" );

    const geode::Point3D g2{ { 5, 5, 0 } };
    const geode::Point3D h2{ { 5, 5, 5 } };
    const geode::Triangle3D no_crossing2{ f, g, h };
    geode::OpenGeodeGeometryException::test(
        !bbox.intersects( no_crossing2 ), "Wrong result with no_crossing2" );
}

void test_tetra_intersections()
{
    geode::BoundingBox3D bbox;
    bbox.add_point( geode::Point3D{ { -1, -1, -1 } } );
    bbox.add_point( geode::Point3D{ { 1, 1, 1 } } );

    const geode::Point3D a{ { 0.5, 0.5, 0.5 } };
    const geode::Point3D b{ { 0.5, 0.5, -0.5 } };
    const geode::Point3D c{ { -0.5, 0.5, 0.5 } };
    const geode::Point3D d{ { 0.5, -0.5, -0.5 } };

    const geode::Tetrahedron inside{ a, b, c, d };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( inside ), "Wrong result with inside" );

    const geode::Point3D a2{ { -5, -5, -5 } };
    const geode::Point3D b2{ { 5, -5, -5 } };
    const geode::Point3D c2{ { 0, 5, -5 } };
    const geode::Point3D d2{ { 0, 0, 5 } };

    const geode::Tetrahedron outside{ a2, b2, c2, d2 };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( outside ), "Wrong result with outside" );
}

void test_segment_intersections()
{
    geode::BoundingBox2D bbox;
    bbox.add_point( geode::Point2D{ { -1, -1 } } );
    bbox.add_point( geode::Point2D{ { 1, 1 } } );

    const geode::Point2D O{ { 0, 0 } };
    const geode::Point2D a{ { 0.5, 0.5 } };

    const geode::Segment2D inside{ O, a };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( inside ), "Wrong result with inside" );

    const geode::Point2D b{ { 5, 5 } };
    const geode::Segment2D crossing_once{ O, b };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( crossing_once ), "Wrong result with crossing_once" );

    const geode::Point2D c{ { -5, -5 } };
    const geode::Segment2D crossing_twice{ b, c };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( crossing_twice ), "Wrong result with crossing_twice" );

    const geode::Point2D d{ { 1.5, 0 } };
    const geode::Point2D e{ { 0, 1.5 } };
    const geode::Segment2D crossing_sideway{ d, e };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( crossing_twice ), "Wrong result with crossing_twice" );

    const geode::Point2D f{ { 5, 0 } };
    const geode::Point2D g{ { 0, 5 } };
    const geode::Segment2D no_crossing{ f, g };
    geode::OpenGeodeGeometryException::test(
        !bbox.intersects( no_crossing ), "Wrong result with no_crossing" );

    const geode::Point2D h{ { 5, 5 } };
    const geode::Segment2D parallel{ f, h };
    geode::OpenGeodeGeometryException::test(
        !bbox.intersects( parallel ), "Wrong result with parallel" );

    const geode::Point2D i{ { 10, 0 } };
    const geode::Segment2D parallel2{ f, i };
    geode::OpenGeodeGeometryException::test(
        !bbox.intersects( parallel2 ), "Wrong result with parallel2" );

    const geode::Point2D j{ { 5, 5 } };
    const geode::Point2D k{ { 5.5, 5.5 } };
    const geode::Segment2D no_crossing2{ j, k };
    geode::OpenGeodeGeometryException::test(
        !bbox.intersects( no_crossing2 ), "Wrong result with no_crossing2" );

    const geode::Point2D l{ { -0.5, 1.1 } };
    const geode::Point2D m{ { -1.1, -0.5 } };
    const geode::Segment2D crossing2{ l, m };
    geode::OpenGeodeGeometryException::test(
        bbox.intersects( crossing2 ), "Wrong result with crossing2" );
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
