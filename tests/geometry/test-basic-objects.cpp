/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geometry/point.h>

#include <geode/geometry/basic_objects/circle.h>
#include <geode/geometry/basic_objects/cylinder.h>
#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/sphere.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>

#include <geode/tests/common.h>

void test_segment()
{
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Segment2D segment{ a, b };

    const geode::Point2D barycenter{ { 0, 0 } };
    OPENGEODE_EXCEPTION(
        segment.barycenter() == barycenter, "[Test] Wrong barycenter result" );

    geode::Segment2D segment2{ segment };
    OPENGEODE_EXCEPTION(
        segment2.vertices()[0].get() == a && segment2.vertices()[1].get() == b,
        "[Test] Wrong result for segment2" );

    geode::Segment2D segment3{ std::move( segment ) };
    OPENGEODE_EXCEPTION(
        segment3.vertices()[0].get() == a && segment3.vertices()[1].get() == b,
        "[Test] Wrong result for segment3" );

    geode::Segment2D segment4{ b, a };
    segment4 = segment2;
    OPENGEODE_EXCEPTION(
        segment4.vertices()[0].get() == a && segment4.vertices()[1].get() == b,
        "[Test] Wrong result for segment4" );

    geode::Segment2D segment5{ b, a };
    segment5 = std::move( segment2 );
    OPENGEODE_EXCEPTION(
        segment5.vertices()[0].get() == a && segment5.vertices()[1].get() == b,
        "[Test] Wrong result for segment5" );

    geode::OwnerSegment2D segment6{ a, b };
    OPENGEODE_EXCEPTION(
        segment6.vertices()[0] == a && segment6.vertices()[1] == b,
        "[Test] Wrong result for segment6" );

    geode::OwnerSegment2D segment7{ segment6 };
    OPENGEODE_EXCEPTION(
        segment7.vertices()[0] == a && segment7.vertices()[1] == b,
        "[Test] Wrong result for segment7" );
}

void test_line()
{
    const auto direction = geode::Vector2D{ { 1.0, 5.0 } }.normalize();
    const geode::Point2D origin{ { -1.0, -5.0 } };
    const geode::InfiniteLine2D line{ direction, origin };

    geode::InfiniteLine2D line2{ line };
    OPENGEODE_EXCEPTION(
        line2.direction() == direction && line2.origin() == origin,
        "[Test] Wrong result for line2" );

    geode::InfiniteLine2D line3{ std::move( line ) };
    OPENGEODE_EXCEPTION(
        line3.direction() == direction && line3.origin() == origin,
        "[Test] Wrong result for line3" );

    const auto direction2 = geode::Vector2D{ { -1.0, -5.0 } }.normalize();
    const geode::Point2D origin2{ { 1.0, 5.0 } };
    geode::InfiniteLine2D line4{ direction2, origin2 };
    line4 = line2;
    OPENGEODE_EXCEPTION(
        line4.direction() == direction && line4.origin() == origin,
        "[Test] Wrong result for line4" );

    geode::InfiniteLine2D line5{ direction2, origin2 };
    line5 = std::move( line2 );
    OPENGEODE_EXCEPTION(
        line5.direction() == direction && line5.origin() == origin,
        "[Test] Wrong result for line5" );
}

void test_plane()
{
    const auto normal = geode::Vector3D{ { 1.0, 5.0, 42 } }.normalize();
    const geode::Point3D origin{ { -1.0, -5.0, 42 } };
    const geode::Plane plane{ normal, origin };

    geode::Plane plane2{ plane };
    OPENGEODE_EXCEPTION( plane2.normal() == normal && plane2.origin() == origin,
        "[Test] Wrong result for plane2" );

    geode::Plane plane3{ std::move( plane ) };
    OPENGEODE_EXCEPTION( plane3.normal() == normal && plane3.origin() == origin,
        "[Test] Wrong result for plane3" );

    const auto normal2 = geode::Vector3D{ { -1.0, -5.0, 42 } }.normalize();
    const geode::Point3D origin2{ { 1.0, 5.0, 42 } };
    geode::Plane plane4{ normal2, origin2 };
    plane4 = plane2;
    OPENGEODE_EXCEPTION( plane4.normal() == normal && plane4.origin() == origin,
        "[Test] Wrong result for plane4" );

    geode::Plane plane5{ normal2, origin2 };
    plane5 = std::move( plane2 );
    OPENGEODE_EXCEPTION( plane5.normal() == normal && plane5.origin() == origin,
        "[Test] Wrong result for plane5" );
}

void test_triangle()
{
    const geode::Point2D a{ { 1.0, 5.0 } };
    const geode::Point2D b{ { -1.0, -5.0 } };
    const geode::Point2D c{ { 1.0, -5.0 } };
    const geode::Triangle2D triangle{ a, b, c };

    const geode::Point2D barycenter{ { 1.0 / 3., -5.0 / 3. } };
    OPENGEODE_EXCEPTION(
        triangle.barycenter() == barycenter, "[Test] Wrong barycenter result" );

    geode::Triangle2D triangle2{ triangle };
    OPENGEODE_EXCEPTION( triangle2.vertices()[0].get() == a
                             && triangle2.vertices()[1].get() == b
                             && triangle2.vertices()[2].get() == c,
        "[Test] Wrong result for triangle2" );

    geode::Triangle2D triangle3{ std::move( triangle ) };
    OPENGEODE_EXCEPTION( triangle3.vertices()[0].get() == a
                             && triangle3.vertices()[1].get() == b
                             && triangle3.vertices()[2].get() == c,
        "[Test] Wrong result for triangle3" );

    geode::Triangle2D triangle4{ c, b, a };
    triangle4 = triangle2;
    OPENGEODE_EXCEPTION( triangle4.vertices()[0].get() == a
                             && triangle4.vertices()[1].get() == b
                             && triangle4.vertices()[2].get() == c,
        "[Test] Wrong result for triangle4" );

    geode::Triangle2D triangle5{ c, b, a };
    triangle5 = std::move( triangle2 );
    OPENGEODE_EXCEPTION( triangle5.vertices()[0].get() == a
                             && triangle5.vertices()[1].get() == b
                             && triangle2.vertices()[2].get() == c,
        "[Test] Wrong result for triangle5" );
}

void test_tetrahedron()
{
    const geode::Point3D a{ { 1.0, 5.0 } };
    const geode::Point3D b{ { -1.0, -5.0 } };
    const geode::Point3D c{ { 1.0, -5.0 } };
    const geode::Point3D d{ { -1.0, 5.0 } };
    const geode::Tetrahedron tetra{ a, b, c, d };

    const geode::Point3D barycenter{ { 0, 0 } };
    OPENGEODE_EXCEPTION(
        tetra.barycenter() == barycenter, "[Test] Wrong barycenter result" );

    geode::Tetrahedron tetra2{ tetra };
    OPENGEODE_EXCEPTION( tetra2.vertices()[0].get() == a
                             && tetra2.vertices()[1].get() == b
                             && tetra2.vertices()[2].get() == c
                             && tetra2.vertices()[3].get() == d,
        "[Test] Wrong result for tetra2" );

    geode::Tetrahedron tetra3{ std::move( tetra ) };
    OPENGEODE_EXCEPTION( tetra3.vertices()[0].get() == a
                             && tetra3.vertices()[1].get() == b
                             && tetra3.vertices()[2].get() == c
                             && tetra2.vertices()[3].get() == d,
        "[Test] Wrong result for tetra3" );

    geode::Tetrahedron tetra4{ d, c, b, a };
    tetra4 = tetra2;
    OPENGEODE_EXCEPTION( tetra4.vertices()[0].get() == a
                             && tetra4.vertices()[1].get() == b
                             && tetra4.vertices()[2].get() == c
                             && tetra2.vertices()[3].get() == d,
        "[Test] Wrong result for tetra4" );

    geode::Tetrahedron tetra5{ d, c, b, a };
    tetra5 = std::move( tetra2 );
    OPENGEODE_EXCEPTION( tetra5.vertices()[0].get() == a
                             && tetra5.vertices()[1].get() == b
                             && tetra2.vertices()[2].get() == c
                             && tetra2.vertices()[3].get() == d,
        "[Test] Wrong result for tetra5" );
}

void test_sphere()
{
    const geode::Point2D origin{ { -1.0, -5.0 } };
    const double radius{ 3 };
    const geode::Sphere2D sphere{ origin, radius };

    geode::Sphere2D sphere2{ sphere };
    OPENGEODE_EXCEPTION(
        sphere2.radius() == radius && sphere2.origin() == origin,
        "[Test] Wrong result for sphere2" );

    geode::Sphere2D sphere3{ std::move( sphere ) };
    OPENGEODE_EXCEPTION(
        sphere3.radius() == radius && sphere3.origin() == origin,
        "[Test] Wrong result for sphere3" );

    const geode::Point2D origin2{ { 1.0, 5.0 } };
    const double radius2{ 30 };
    geode::Sphere2D sphere4{ origin2, radius2 };
    sphere4 = sphere2;
    OPENGEODE_EXCEPTION(
        sphere4.radius() == radius && sphere4.origin() == origin,
        "[Test] Wrong result for sphere4" );

    geode::Sphere2D sphere5{ origin2, radius2 };
    sphere5 = std::move( sphere2 );
    OPENGEODE_EXCEPTION(
        sphere5.radius() == radius && sphere5.origin() == origin,
        "[Test] Wrong result for sphere5" );
}

void test_circle()
{
    const auto normal = geode::Vector3D{ { 1.0, 5.0, 42 } }.normalize();
    const geode::Point3D origin{ { -1.0, -5.0, 42 } };
    const geode::Plane plane{ normal, origin };
    const double radius{ 3 };
    const geode::Circle circle{ plane, radius };

    geode::Circle circle2{ circle };
    OPENGEODE_EXCEPTION( circle2.radius() == radius
                             && circle2.plane().normal() == plane.normal()
                             && circle2.plane().origin() == plane.origin(),
        "[Test] Wrong result for circle2" );

    geode::Circle circle3{ std::move( circle ) };
    OPENGEODE_EXCEPTION( circle3.radius() == radius
                             && circle3.plane().normal() == plane.normal()
                             && circle3.plane().origin() == plane.origin(),
        "[Test] Wrong result for circle3" );

    const auto normal2 = geode::Vector3D{ { -1.0, -5.0, 42 } }.normalize();
    const geode::Point3D origin2{ { 1.0, 5.0, 42 } };
    const geode::Plane plane2{ normal2, origin2 };
    const double radius2{ 30 };
    geode::Circle circle4{ plane2, radius2 };
    circle4 = circle2;
    OPENGEODE_EXCEPTION( circle4.radius() == radius
                             && circle4.plane().normal() == plane.normal()
                             && circle4.plane().origin() == plane.origin(),
        "[Test] Wrong result for circle4" );

    geode::Circle circle5{ plane2, radius2 };
    circle5 = std::move( circle2 );
    OPENGEODE_EXCEPTION( circle5.radius() == radius
                             && circle5.plane().normal() == plane.normal()
                             && circle5.plane().origin() == plane.origin(),
        "[Test] Wrong result for circle5" );
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
}

OPENGEODE_TEST( "basic-object" )
