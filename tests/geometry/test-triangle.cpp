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
#include <array>
#include <random>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>

#include <geode/tests/common.hpp>

void test_degenerated_consistency()
{
    const geode::Point3D p0{ { 0.0, 0.0, 0.0 } };
    const geode::Point3D p1{ { 1.0, 0.0, 0.0 } };
    const geode::Point3D p2{ { 3.0, 2.5 * geode::GLOBAL_EPSILON, 0.0 } };
    const geode::Point3D p3{ { 0.0, 0.0, 1.0 } };
    const geode::Triangle3D triangle{ p0, p1, p2 };
    geode::OpenGeodeGeometryException::test( triangle.is_degenerated(),
        "Triangle with minimum height below epsilon should be degenerated" );
    const geode::Tetrahedron tetra{ p0, p1, p2, p3 };
    geode::OpenGeodeGeometryException::test( tetra.is_degenerated(),
        "Tetrahedron with a degenerated facet should be degenerated" );
    const geode::Tetrahedron tetra2{ p3, p0, p1, p2 };
    geode::OpenGeodeGeometryException::test( tetra2.is_degenerated(),
        "Tetrahedron with a degenerated facet should be degenerated" );

    const geode::Point3D q2{ { 3.0, 1e-3, 0.0 } };
    const geode::Triangle3D triangle2{ p0, p1, q2 };
    geode::OpenGeodeGeometryException::test( !triangle2.is_degenerated(),
        "Flat but valid triangle should not be degenerated" );
    const geode::Point3D q3{ { 1.0, 1.0, 0.5 * geode::GLOBAL_EPSILON } };
    const geode::Point3D q4{ { 0.0, 1.0, 0.0 } };
    const geode::Tetrahedron flat_tetra{ p0, p1, q3, q4 };
    geode::OpenGeodeGeometryException::test(
        flat_tetra.is_degenerated(), "Flat tetrahedron should be degenerated" );
    const geode::Tetrahedron tetra3{ p0, p1, q4, p3 };
    geode::OpenGeodeGeometryException::test( !tetra3.is_degenerated(),
        "Valid tetrahedron should not be degenerated" );

    // Sliver: opposite edges 01 and 23 lie in planes z=0 and z=width, so
    // its thickness is width while all its heights are about 2 * width.
    for( const auto width : { 0.5, 0.8, 1., 1.2 } )
    {
        const auto delta = width * geode::GLOBAL_EPSILON;
        const geode::Point3D s0{ { 0.0, 0.0, 0.0 } };
        const geode::Point3D s1{ { 1.0, 1.0, 0.0 } };
        const geode::Point3D s2{ { 1.0, 0.0, delta } };
        const geode::Point3D s3{ { 0.0, 1.0, delta } };
        const geode::Tetrahedron sliver{ s0, s1, s2, s3 };
        geode::OpenGeodeGeometryException::test(
            sliver.is_degenerated() == ( width <= 1. ),
            "Wrong degeneracy for sliver of thickness ", width,
            " * GLOBAL_EPSILON" );
    }

    std::mt19937 generator{ 42 };
    std::uniform_real_distribution< double > distribution{ -1., 1. };
    std::uniform_real_distribution< double > exponent{ -9., -4. };
    for( const auto trial : geode::Range{ 1000 } )
    {
        geode_unused( trial );
        const geode::Point3D a{ { distribution( generator ),
            distribution( generator ), distribution( generator ) } };
        const geode::Point3D b{ { distribution( generator ),
            distribution( generator ), distribution( generator ) } };
        const auto height = std::pow( 10., exponent( generator ) );
        const auto t = distribution( generator ) * 2;
        const geode::Vector3D ab{ a, b };
        const auto offset = ab.cross( geode::Vector3D{ { 0.3, 0.5, 0.7 } } );
        const geode::Point3D c = a + ab * t + offset.normalize() * height;
        const geode::Triangle3D random_triangle{ a, b, c };
        if( !random_triangle.normal() )
        {
            geode::OpenGeodeGeometryException::test(
                random_triangle.is_degenerated(),
                "Triangle without normal should be degenerated: ",
                random_triangle.string() );
        }
    }
}

// Maps local coordinates into a frame which is neither aligned with the axes
// nor at the origin
geode::Point3D to_global( const std::array< double, 3 >& local, double offset )
{
    const geode::Vector3D u{ { 2. / 3., 1. / 3., 2. / 3. } };
    const geode::Vector3D v{ { 1. / 3., 2. / 3., -2. / 3. } };
    const geode::Vector3D w{ { 2. / 3., -2. / 3., -1. / 3. } };
    const geode::Point3D origin{ { offset, -offset, offset } };
    return origin + u * local[0] + v * local[1] + w * local[2];
}

void test_degenerated_triangle_vertex_order()
{
    // Each triangle has a minimum height of factor * GLOBAL_EPSILON
    for( const auto factor : { 0.5, 0.9, 1.1, 2. } )
    {
        const auto height = factor * geode::GLOBAL_EPSILON;
        const std::array< std::array< std::array< double, 3 >, 3 >, 3 >
            configurations{ {
                // Cap: the apex projects inside the longest edge
                { { { 0, 0, 0 }, { 1, 0, 0 }, { 0.5, height, 0 } } },
                // Obtuse: the apex projects outside the opposite edge
                { { { 0, 0, 0 }, { 1, 0, 0 }, { 1.5, 1.5 * height, 0 } } },
                // Needle: one edge is shorter than GLOBAL_EPSILON
                { { { 0, 0, 0 }, { 1, 0, 0 }, { 1, height, 0 } } },
            } };
        for( const auto offset : { 0., 1e3, 1e6 } )
        {
            for( const auto& configuration : configurations )
            {
                std::array< geode::local_index_t, 3 > order{ 0, 1, 2 };
                do
                {
                    const geode::Triangle3D triangle{
                        to_global( configuration[order[0]], offset ),
                        to_global( configuration[order[1]], offset ),
                        to_global( configuration[order[2]], offset )
                    };
                    geode::OpenGeodeGeometryException::test(
                        triangle.is_degenerated() == ( factor < 1 ),
                        "Wrong degeneracy for triangle ", triangle.string(),
                        " of minimum height ", factor, " * GLOBAL_EPSILON" );
                } while( std::next_permutation( order.begin(), order.end() ) );
            }
        }
    }
}

void test_degenerated_tetrahedron_vertex_order()
{
    // Each tetrahedron lies in a slab of minimum thickness
    // factor * GLOBAL_EPSILON
    for( const auto factor : { 0.5, 0.9, 1.1, 2. } )
    {
        const auto width = factor * geode::GLOBAL_EPSILON;
        const std::array< std::array< std::array< double, 3 >, 4 >, 3 >
            configurations{ {
                // Cap: the apex projects inside the opposite facet
                { { { 0, 0, 0 }, { 1, 0, 0 }, { 0, 1, 0 },
                    { 0.3, 0.3, width } } },
                // Wedge: the apex projects outside the opposite facet, the
                // thinnest slab is between edges 03 and 12
                { { { 0, 0, 0 }, { 1, 0, 0 }, { 0, 1, 0 },
                    { 1, 1, 2 * width } } },
                // Sliver: thinnest between two opposite edges
                { { { 0, 0, 0 }, { 1, 1, 0 }, { 1, 0, width },
                    { 0, 1, width } } },
            } };
        for( const auto offset : { 0., 1e3, 1e6 } )
        {
            for( const auto& configuration : configurations )
            {
                std::array< geode::local_index_t, 4 > order{ 0, 1, 2, 3 };
                do
                {
                    const geode::Tetrahedron tetra{
                        to_global( configuration[order[0]], offset ),
                        to_global( configuration[order[1]], offset ),
                        to_global( configuration[order[2]], offset ),
                        to_global( configuration[order[3]], offset )
                    };
                    geode::OpenGeodeGeometryException::test(
                        tetra.is_degenerated() == ( factor < 1 ),
                        "Wrong degeneracy for tetrahedron ", tetra.string(),
                        " of thickness ", factor, " * GLOBAL_EPSILON" );
                } while( std::next_permutation( order.begin(), order.end() ) );
            }
        }
    }
}

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

void test()
{
    test_set_point();
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
    test_degenerated_consistency();
    test_degenerated_triangle_vertex_order();
    test_degenerated_tetrahedron_vertex_order();
}

OPENGEODE_TEST( "distance" )
