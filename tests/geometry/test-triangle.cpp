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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>

#include <geode/tests/common.hpp>

void test_set_point()
{
    geode::OwnerTriangle3D triangle{ geode::Point3D{ { 0, 0, 0 } },
        geode::Point3D{ { 1, 0, 0 } }, geode::Point3D{ { 0, 1, 0 } } };
    const auto normal = triangle.normal();
    geode::OpenGeodeGeometryException::test(
        normal && normal->inexact_equal( geode::Vector3D{ { 0, 0, 1 } } ),
        "Wrong normal before set_point" );
    auto copy = triangle;
    triangle.set_point( 2, geode::Point3D{ { 0, 0, 1 } } );
    const auto new_normal = triangle.normal();
    geode::OpenGeodeGeometryException::test(
        new_normal
            && new_normal->inexact_equal( geode::Vector3D{ { 0, -1, 0 } } ),
        "Wrong normal after set_point" );
    copy.set_point( 2, geode::Point3D{ { 0, 0, 1 } } );
    const auto copy_normal = copy.normal();
    geode::OpenGeodeGeometryException::test(
        copy_normal
            && copy_normal->inexact_equal( geode::Vector3D{ { 0, -1, 0 } } ),
        "Wrong normal after set_point on copy" );
}

void test_2d_triangle_without_cache()
{
    static_assert(
        sizeof( geode::Triangle2D ) == 3 * sizeof( geode::RefPoint2D ),
        "Triangle2D should only store its vertices" );
    static_assert(
        sizeof( geode::OwnerTriangle2D ) == 3 * sizeof( geode::Point2D ),
        "OwnerTriangle2D should only store its vertices" );
    geode::OwnerTriangle2D triangle{ geode::Point2D{ { 0, 0 } },
        geode::Point2D{ { 1, 0 } }, geode::Point2D{ { 0, 1 } } };
    auto copy = triangle;
    copy.set_point( 2, geode::Point2D{ { 2, 2 } } );
    geode::OpenGeodeGeometryException::test(
        copy.vertices()[2] == geode::Point2D{ { 2, 2 } }
            && triangle.vertices()[2] == geode::Point2D{ { 0, 1 } },
        "[Test] Wrong 2D triangle set_point" );
    geode::OpenGeodeGeometryException::test(
        copy.barycenter().inexact_equal( geode::Point2D{ { 1, 2. / 3 } } ),
        "[Test] Wrong 2D triangle barycenter" );

    const geode::OwnerTriangle3D triangle3d{ geode::Point3D{ { 0, 0, 0 } },
        geode::Point3D{ { 1, 0, 0 } }, geode::Point3D{ { 0, 1, 0 } } };
    const auto normal = triangle3d.normal();
    auto copy3d = triangle3d;
    geode::OpenGeodeGeometryException::test(
        copy3d.normal() == normal, "[Test] Copied triangle normal" );
    copy3d.set_point( 2, geode::Point3D{ { 0, 0, 1 } } );
    geode::OpenGeodeGeometryException::test(
        copy3d.normal()->inexact_equal( geode::Vector3D{ { 0, -1, 0 } } ),
        "[Test] Normal should be updated after set_point" );
}

void test()
{
    test_set_point();
    test_2d_triangle_without_cache();
    const geode::Point3D a{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D b_ref{ { 0.5, 0.1, 0.0 } };
    const geode::Point3D b{ { 0.6, 0.3 * geode::GLOBAL_EPSILON, 0.0 } };
    const geode::Point3D c{ { 1.0, 0.0, 0.0 } };

    const geode::Vector3D answer{ { 0, 0, -1 } };

    const geode::Triangle3D triangle_ref{ a, b_ref, c };
    const auto normal_ref = triangle_ref.normal();
    geode::OpenGeodeGeometryException::test( normal_ref.has_value(),
        "Normal of reference triangle should be calculated" );
    geode::OpenGeodeGeometryException::test(
        normal_ref.value() == answer, "Wrong normal of reference triangle" );
    const auto pivot_ref = triangle_ref.pivot();
    geode::OpenGeodeGeometryException::test( pivot_ref.has_value(),
        "Pivot of reference triangle should be calculated" );
    geode::OpenGeodeGeometryException::test(
        pivot_ref.value() == 0, "Wrong pivot of reference triangle" );

    const geode::Triangle3D triangle2{ a, b, c };
    const auto normal2 = triangle2.normal();
    geode::OpenGeodeGeometryException::test(
        !normal2, "Normal of triangle2 should not be calculated" );
    const auto pivot2 = triangle2.pivot();
    geode::OpenGeodeGeometryException::test(
        !pivot2, "Pivot of triangle2 should not be calculated" );
}

OPENGEODE_TEST( "distance" )