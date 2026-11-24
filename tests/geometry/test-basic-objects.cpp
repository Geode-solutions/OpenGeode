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

#include <algorithm>
#include <string_view>
#include <vector>

#include <absl/strings/str_cat.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>

#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/cylinder.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/polygon.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/sign.hpp>

#include <geode/tests/common.hpp>

void test_segment()
{
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Segment2D segment{ a, b };

    const geode::Point2D barycenter{ { 0, 0 } };
    geode::OpenGeodeGeometryException::test(
        segment.barycenter() == barycenter, "Wrong barycenter result" );

    geode::Segment2D segment2{ segment };
    geode::OpenGeodeGeometryException::test(
        segment2.vertices()[0].get() == a && segment2.vertices()[1].get() == b,
        "Wrong result for segment2" );

    geode::Segment2D segment3{ std::move( segment ) };
    geode::OpenGeodeGeometryException::test(
        segment3.vertices()[0].get() == a && segment3.vertices()[1].get() == b,
        "Wrong result for segment3" );

    geode::Segment2D segment4{ b, a };
    segment4 = segment2;
    geode::OpenGeodeGeometryException::test(
        segment4.vertices()[0].get() == a && segment4.vertices()[1].get() == b,
        "Wrong result for segment4" );

    geode::Segment2D segment5{ b, a };
    segment5 = std::move( segment2 );
    geode::OpenGeodeGeometryException::test(
        segment5.vertices()[0].get() == a && segment5.vertices()[1].get() == b,
        "Wrong result for segment5" );

    geode::OwnerSegment2D segment6{ a, b };
    geode::OpenGeodeGeometryException::test(
        segment6.vertices()[0] == a && segment6.vertices()[1] == b,
        "Wrong result for segment6" );

    geode::OwnerSegment2D segment7{ segment6 };
    geode::OpenGeodeGeometryException::test(
        segment7.vertices()[0] == a && segment7.vertices()[1] == b,
        "Wrong result for segment7" );
}

void test_line()
{
    const auto direction = geode::Vector2D{ { 1.0, 5.0 } }.normalize();
    const geode::Point2D origin{ { -1.0, -5.0 } };
    const geode::InfiniteLine2D line{ direction, origin };

    geode::InfiniteLine2D line2{ line };
    geode::OpenGeodeGeometryException::test(
        line2.direction() == direction && line2.origin() == origin,
        "Wrong result for line2" );

    geode::InfiniteLine2D line3{ std::move( line ) };
    geode::OpenGeodeGeometryException::test(
        line3.direction() == direction && line3.origin() == origin,
        "Wrong result for line3" );

    const auto direction2 = geode::Vector2D{ { -1.0, -5.0 } }.normalize();
    const geode::Point2D origin2{ { 1.0, 5.0 } };
    geode::InfiniteLine2D line4{ direction2, origin2 };
    line4 = line2;
    geode::OpenGeodeGeometryException::test(
        line4.direction() == direction && line4.origin() == origin,
        "Wrong result for line4" );

    geode::InfiniteLine2D line5{ direction2, origin2 };
    line5 = std::move( line2 );
    geode::OpenGeodeGeometryException::test(
        line5.direction() == direction && line5.origin() == origin,
        "Wrong result for line5" );
}

void test_plane()
{
    const auto normal = geode::Vector3D{ { 1.0, 5.0, 42 } }.normalize();
    const geode::Point3D origin{ { -1.0, -5.0, 42 } };
    const geode::Plane plane{ normal, origin };

    geode::Plane plane2{ plane };
    geode::OpenGeodeGeometryException::test(
        plane2.normal() == normal && plane2.origin() == origin,
        "Wrong result for plane2" );

    geode::Plane plane3{ std::move( plane ) };
    geode::OpenGeodeGeometryException::test(
        plane3.normal() == normal && plane3.origin() == origin,
        "Wrong result for plane3" );

    const auto normal2 = geode::Vector3D{ { -1.0, -5.0, 42 } }.normalize();
    const geode::Point3D origin2{ { 1.0, 5.0, 42 } };
    geode::Plane plane4{ normal2, origin2 };
    plane4 = plane2;
    geode::OpenGeodeGeometryException::test(
        plane4.normal() == normal && plane4.origin() == origin,
        "Wrong result for plane4" );

    geode::Plane plane5{ normal2, origin2 };
    plane5 = std::move( plane2 );
    geode::OpenGeodeGeometryException::test(
        plane5.normal() == normal && plane5.origin() == origin,
        "Wrong result for plane5" );
}

void test_triangle()
{
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Point2D c{ { 1.0, -5.0 } };
    const geode::Triangle2D triangle{ a, b, c };

    const geode::Point2D barycenter{ { 1.0 / 3., -5.0 / 3. } };
    geode::OpenGeodeGeometryException::test(
        triangle.barycenter() == barycenter, "Wrong barycenter result" );

    geode::Triangle2D triangle2{ triangle };
    geode::OpenGeodeGeometryException::test(
        triangle2.vertices()[0].get() == a && triangle2.vertices()[1].get() == b
            && triangle2.vertices()[2].get() == c,
        "Wrong result for triangle2" );

    geode::Triangle2D triangle3{ std::move( triangle ) };
    geode::OpenGeodeGeometryException::test(
        triangle3.vertices()[0].get() == a && triangle3.vertices()[1].get() == b
            && triangle3.vertices()[2].get() == c,
        "Wrong result for triangle3" );

    geode::Triangle2D triangle4{ c, b, a };
    triangle4 = triangle2;
    geode::OpenGeodeGeometryException::test(
        triangle4.vertices()[0].get() == a && triangle4.vertices()[1].get() == b
            && triangle4.vertices()[2].get() == c,
        "Wrong result for triangle4" );

    geode::Triangle2D triangle5{ c, b, a };
    triangle5 = std::move( triangle2 );
    geode::OpenGeodeGeometryException::test(
        triangle5.vertices()[0].get() == a && triangle5.vertices()[1].get() == b
            && triangle2.vertices()[2].get() == c,
        "Wrong result for triangle5" );
}

void test_tetrahedron()
{
    const geode::Point3D a{ { 1.0, 5.0 } };
    const geode::Point3D b{ { -1.0, -5.0 } };
    const geode::Point3D c{ { 1.0, -5.0 } };
    const geode::Point3D d{ { -1.0, 5.0 } };
    const geode::Tetrahedron tetra{ a, b, c, d };

    const geode::Point3D barycenter{ { 0, 0 } };
    geode::OpenGeodeGeometryException::test(
        tetra.barycenter() == barycenter, "Wrong barycenter result" );

    geode::Tetrahedron tetra2{ tetra };
    geode::OpenGeodeGeometryException::test(
        tetra2.vertices()[0].get() == a && tetra2.vertices()[1].get() == b
            && tetra2.vertices()[2].get() == c
            && tetra2.vertices()[3].get() == d,
        "Wrong result for tetra2" );

    geode::Tetrahedron tetra3{ std::move( tetra ) };
    geode::OpenGeodeGeometryException::test(
        tetra3.vertices()[0].get() == a && tetra3.vertices()[1].get() == b
            && tetra3.vertices()[2].get() == c
            && tetra2.vertices()[3].get() == d,
        "Wrong result for tetra3" );

    geode::Tetrahedron tetra4{ d, c, b, a };
    tetra4 = tetra2;
    geode::OpenGeodeGeometryException::test(
        tetra4.vertices()[0].get() == a && tetra4.vertices()[1].get() == b
            && tetra4.vertices()[2].get() == c
            && tetra2.vertices()[3].get() == d,
        "Wrong result for tetra4" );

    geode::Tetrahedron tetra5{ d, c, b, a };
    tetra5 = std::move( tetra2 );
    geode::OpenGeodeGeometryException::test(
        tetra5.vertices()[0].get() == a && tetra5.vertices()[1].get() == b
            && tetra2.vertices()[2].get() == c
            && tetra2.vertices()[3].get() == d,
        "Wrong result for tetra5" );
}

void test_sphere()
{
    const geode::Point2D origin{ { -1.0, -5.0 } };
    const double radius{ 3 };
    const geode::Sphere2D sphere{ origin, radius };

    geode::Sphere2D sphere2{ sphere };
    geode::OpenGeodeGeometryException::test(
        sphere2.radius() == radius && sphere2.origin() == origin,
        "Wrong result for sphere2" );

    geode::Sphere2D sphere3{ std::move( sphere ) };
    geode::OpenGeodeGeometryException::test(
        sphere3.radius() == radius && sphere3.origin() == origin,
        "Wrong result for sphere3" );

    const geode::Point2D origin2{ { 1.0, 5.0 } };
    const double radius2{ 30 };
    geode::Sphere2D sphere4{ origin2, radius2 };
    sphere4 = sphere2;
    geode::OpenGeodeGeometryException::test(
        sphere4.radius() == radius && sphere4.origin() == origin,
        "Wrong result for sphere4" );

    geode::Sphere2D sphere5{ origin2, radius2 };
    sphere5 = std::move( sphere2 );
    geode::OpenGeodeGeometryException::test(
        sphere5.radius() == radius && sphere5.origin() == origin,
        "Wrong result for sphere5" );
}

void test_circle()
{
    const auto normal = geode::Vector3D{ { 1.0, 5.0, 42 } }.normalize();
    const geode::Point3D origin{ { -1.0, -5.0, 42 } };
    const geode::Plane plane{ normal, origin };
    const double radius{ 3 };
    const geode::Circle circle{ plane, radius };

    geode::Circle circle2{ circle };
    geode::OpenGeodeGeometryException::test(
        circle2.radius() == radius && circle2.plane().normal() == plane.normal()
            && circle2.plane().origin() == plane.origin(),
        "Wrong result for circle2" );

    geode::Circle circle3{ std::move( circle ) };
    geode::OpenGeodeGeometryException::test(
        circle3.radius() == radius && circle3.plane().normal() == plane.normal()
            && circle3.plane().origin() == plane.origin(),
        "Wrong result for circle3" );

    const auto normal2 = geode::Vector3D{ { -1.0, -5.0, 42 } }.normalize();
    const geode::Point3D origin2{ { 1.0, 5.0, 42 } };
    const geode::Plane plane2{ normal2, origin2 };
    const double radius2{ 30 };
    geode::Circle circle4{ plane2, radius2 };
    circle4 = circle2;
    geode::OpenGeodeGeometryException::test(
        circle4.radius() == radius && circle4.plane().normal() == plane.normal()
            && circle4.plane().origin() == plane.origin(),
        "Wrong result for circle4" );

    geode::Circle circle5{ plane2, radius2 };
    circle5 = std::move( circle2 );
    geode::OpenGeodeGeometryException::test(
        circle5.radius() == radius && circle5.plane().normal() == plane.normal()
            && circle5.plane().origin() == plane.origin(),
        "Wrong result for circle5" );
}

void check_polygon_normal( const geode::OwnerPolygon3D& polygon,
    const geode::Vector3D& expected_normal )
{
    const auto normal = polygon.normal();
    geode::OpenGeodeGeometryException::test(
        normal && normal->inexact_equal( expected_normal ),
        "Wrong polygon normal" );
    const auto triangles = polygon.triangulate();
    geode::OpenGeodeGeometryException::test(
        triangles.size() == polygon.nb_vertices() - 2,
        "Wrong number of triangles in polygon triangulation" );
    const auto& vertices = polygon.vertices();
    for( const auto& triangle : triangles )
    {
        const auto triangle_normal = geode::Triangle3D{ vertices[triangle[0]],
            vertices[triangle[1]], vertices[triangle[2]] }
                                         .normal();
        geode::OpenGeodeGeometryException::test(
            triangle_normal && triangle_normal->dot( expected_normal ) > 0,
            "Wrong triangle orientation in polygon triangulation" );
    }
}

void test_polygon()
{
    const geode::Point3D a{ { 0, 0, 0 } };
    const geode::Point3D b{ { 1, 1, 0 } };
    const geode::Point3D c{ { 2, 0, 0 } };
    const geode::Point3D d{ { 1, 3, 0 } };
    check_polygon_normal( geode::OwnerPolygon3D{ { a, b, c, d } },
        geode::Vector3D{ { 0, 0, 1 } } );
    check_polygon_normal( geode::OwnerPolygon3D{ { d, c, b, a } },
        geode::Vector3D{ { 0, 0, -1 } } );

    // Same polygon in the plane of normal (1,1,1) through (1,2,3), with
    // (1,-1,0) and (1,1,-2) as local axes
    const geode::Point3D a_tilted{ { 1, 2, 3 } };
    const geode::Point3D b_tilted{ { 3, 2, 1 } };
    const geode::Point3D c_tilted{ { 3, 0, 3 } };
    const geode::Point3D d_tilted{ { 5, 4, -3 } };
    const auto tilted_normal = geode::Vector3D{ { 1, 1, 1 } }.normalize();
    check_polygon_normal(
        geode::OwnerPolygon3D{ { a_tilted, b_tilted, c_tilted, d_tilted } },
        tilted_normal );
    check_polygon_normal(
        geode::OwnerPolygon3D{ { d_tilted, c_tilted, b_tilted, a_tilted } },
        tilted_normal * -1 );
}

// Triangles without a normal (collinear points) are allowed: earcut may return
// them, and the others must follow the polygon orientation.
void check_triangulation_orientation(
    const geode::OwnerPolygon3D& polygon, std::string_view name )
{
    const auto normal = polygon.normal();
    geode::OpenGeodeGeometryException::test(
        normal.has_value(), name, ": no polygon normal" );
    const auto triangles = polygon.triangulate();
    geode::OpenGeodeGeometryException::test(
        triangles.size() == polygon.nb_vertices() - 2, name,
        ": wrong number of triangles" );
    const auto& vertices = polygon.vertices();
    geode::index_t nb_oriented{ 0 };
    for( const auto& triangle : triangles )
    {
        const auto triangle_normal = geode::Triangle3D{ vertices[triangle[0]],
            vertices[triangle[1]], vertices[triangle[2]] }
                                         .normal();
        if( !triangle_normal )
        {
            continue;
        }
        nb_oriented++;
        geode::OpenGeodeGeometryException::test(
            triangle_normal->dot( normal.value() ) > 0, name,
            ": wrong triangle orientation" );
    }
    geode::OpenGeodeGeometryException::test(
        nb_oriented > 0, name, ": no triangle with a normal" );
}

void check_triangulation_orientation(
    const geode::OwnerPolygon2D& polygon, std::string_view name )
{
    const auto triangles = polygon.triangulate();
    geode::OpenGeodeGeometryException::test(
        triangles.size() == polygon.nb_vertices() - 2, name,
        ": wrong number of triangles" );
    std::vector< geode::RefPoint2D > points;
    for( const auto& point : polygon.vertices() )
    {
        points.emplace_back( point );
    }
    const auto polygon_sign =
        geode::polygon_area_sign( geode::Polygon2D{ std::move( points ) } );
    const auto& vertices = polygon.vertices();
    for( const auto& triangle : triangles )
    {
        const geode::Triangle2D geometry{ vertices[triangle[0]],
            vertices[triangle[1]], vertices[triangle[2]] };
        if( geometry.is_degenerated() )
        {
            continue;
        }
        geode::OpenGeodeGeometryException::test(
            geode::triangle_area_sign( geometry ) == polygon_sign, name,
            ": wrong triangle orientation" );
    }
}

template < geode::index_t dimension >
void check_triangulation_both_windings(
    std::vector< geode::Point< dimension > > points, std::string_view name )
{
    check_triangulation_orientation(
        geode::OwnerPolygon< dimension >{ points }, name );
    std::reverse( points.begin(), points.end() );
    check_triangulation_orientation(
        geode::OwnerPolygon< dimension >{ std::move( points ) },
        absl::StrCat( name, " reversed" ) );
}

std::vector< geode::Point2D > project_points(
    absl::Span< const geode::Point3D > points,
    geode::local_index_t axis0,
    geode::local_index_t axis1 )
{
    std::vector< geode::Point2D > result;
    result.reserve( points.size() );
    for( const auto& point : points )
    {
        result.emplace_back(
            geode::Point2D{ { point.value( axis0 ), point.value( axis1 ) } } );
    }
    return result;
}

void test_polygon_triangulation_orientation()
{
    check_triangulation_both_windings< 2 >(
        { geode::Point2D{ { 0, 0 } }, geode::Point2D{ { 1, 0 } },
            geode::Point2D{ { 1, 1 } }, geode::Point2D{ { 0, 1 } } },
        "Square 2D" );
    check_triangulation_both_windings< 2 >(
        { geode::Point2D{ { 0, 0 } }, geode::Point2D{ { 1, 1 } },
            geode::Point2D{ { 2, 0 } }, geode::Point2D{ { 1, 3 } } },
        "Concave quad 2D" );
}

void test_polygon_triangulation_with_collinear_points()
{
    // 4 points, the first 3 collinear: earcut returns 1 triangle without
    // normal out of 2
    const std::vector< geode::Point3D > quad{
        geode::Point3D{ { 0, 7000, -5406.3798828125 } },
        geode::Point3D{ { 433.0127018922193, 7000, -5656.3798828125 } },
        geode::Point3D{ { 866.02540378443916, 7000, -5906.3798828125 } },
        geode::Point3D{ { 866.02540378443882, 7000, -5406.3798828125 } }
    };
    check_triangulation_both_windings< 3 >( quad, "Quad" );
    check_triangulation_both_windings< 2 >(
        project_points( quad, 0, 2 ), "Quad 2D" );

    // Comb with collinear runs along y = -1, y = 0 and y = 1: earcut returns
    // 13 triangles without normal out of 22
    std::vector< geode::Point3D > comb{ geode::Point3D{
                                            { -2, -1, 0.5500000000000056 } },
        geode::Point3D{ { -2, -0.99999999999999989, 0.45000000000000551 } },
        geode::Point3D{ { -2, -1.0000000000000002, 0.35000000000000542 } },
        geode::Point3D{ { -2, 6.3837823915946501e-16, 0.40000000000000557 } },
        geode::Point3D{ { -2, -1.2490009027033011e-16, 0.50000000000000555 } },
        geode::Point3D{ { -2, 1.7763568394002505e-15, 0.60000000000000564 } },
        geode::Point3D{ { -2, 6.5854412903732108e-16, 0.70000000000000562 } },
        geode::Point3D{ { -2, -1.3468048661638304e-15, 0.80000000000000571 } },
        geode::Point3D{ { -2, -1.2111710391207129e-15, 0.90000000000000568 } },
        geode::Point3D{ { -2, 9.0075462384020925e-16, 1.0000000000000056 } },
        geode::Point3D{ { -2, 1.2212453270876722e-15, 1.1000000000000054 } },
        geode::Point3D{ { -2, 0.99999999999999756, 1.150000000000005 } },
        geode::Point3D{ { -2, 0.99999999999999623, 1.250000000000004 } },
        geode::Point3D{ { -2, -8.1785425744099724e-16, 1.3000000000000036 } },
        geode::Point3D{ { -2, -2.195879817686426e-16, 1.4000000000000037 } },
        geode::Point3D{ { -2, -0.99999999999999989, 1.4500000000000037 } },
        geode::Point3D{ { -2, -1.0000000000000004, 1.3500000000000036 } },
        geode::Point3D{ { -2, -0.99999999999999623, 1.250000000000004 } },
        geode::Point3D{ { -2, -0.99999999999999756, 1.150000000000005 } },
        geode::Point3D{ { -2, -1, 1.0500000000000054 } },
        geode::Point3D{ { -2, -0.99999999999999822, 0.9500000000000054 } },
        geode::Point3D{ { -2, -1, 0.85000000000000586 } },
        geode::Point3D{ { -2, -1, 0.75000000000000577 } },
        geode::Point3D{ { -2, -1.0000000000000011, 0.65000000000000568 } } };
    check_triangulation_both_windings< 3 >( comb, "Comb" );
    check_triangulation_both_windings< 2 >(
        project_points( comb, 1, 2 ), "Comb 2D" );
    for( auto& point : comb )
    {
        point.set_value( 0, 2 );
        point.set_value( 1, -point.value( 1 ) );
    }
    check_triangulation_both_windings< 3 >( comb, "Mirrored comb" );
}

void test_polygon_triangulation_without_normal()
{
    // No triangle has a normal: earcut order is kept
    const geode::OwnerPolygon3D polygon{ { geode::Point3D{ { 0, 0, 0 } },
        geode::Point3D{ { 1, 1, 1 } }, geode::Point3D{ { 2, 2, 2 } },
        geode::Point3D{ { 3, 3, 3 } } } };
    geode::OpenGeodeGeometryException::test( !polygon.normal().has_value(),
        "Collinear polygon should have no normal" );
    const auto triangles = polygon.triangulate();
    geode::OpenGeodeGeometryException::test( triangles.size() <= 2,
        "Wrong number of triangles for a collinear polygon" );
}

void test()
{
    test_segment();
    test_line();
    test_plane();
    test_triangle();
    test_tetrahedron();
    test_sphere();
    test_circle();
    test_polygon();
    test_polygon_triangulation_orientation();
    test_polygon_triangulation_with_collinear_points();
    test_polygon_triangulation_without_normal();
}

OPENGEODE_TEST( "basic-object" )
