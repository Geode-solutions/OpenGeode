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

#include "brute-force.hpp"
#include "random-geometry.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/common.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/position.hpp>
#include <geode/geometry/vector.hpp>
#include <geode/tests/common.hpp>
#include <limits>

#include "distance-brute-force.hpp"

/*
 * Each distance is checked against a brute force minimum over samples of
 * the objects. The exact distance is below the brute force minimum and above
 * it minus the sampling resolution (largest distance from a point of the
 * sampled object to its closest sample). All failures are reported with the
 * exact (hexadecimal) inputs before failing.
 */
namespace
{
    void test_segment_triangle_distance()
    {
        for( const auto trial : geode::Range{ 1000 } )
        {
            const auto vertices = random_triangle< 3 >( trial );
            const geode::Triangle3D triangle{ vertices[0], vertices[1],
                vertices[2] };
            const auto points = random_segment( vertices, trial / 4 );
            const geode::Segment3D segment{ points[0], points[1] };
            const auto [distance, closest_segment, closest_triangle] =
                geode::segment_triangle_distance( segment, triangle );
            check_closest_points( distance, closest_segment, closest_triangle,
                "Wrong result for segment_triangle_distance with segment ",
                hex( points[0] ), " - ", hex( points[1] ) );
            check( geode::point_segment_distance( closest_segment, segment )
                           < geode::GLOBAL_EPSILON
                       && std::get< 0 >( geode::point_triangle_distance(
                              closest_triangle, triangle ) )
                              < geode::GLOBAL_EPSILON,
                "Wrong result for segment_triangle_distance: closest points "
                "not on objects" );
            auto brute_force = std::numeric_limits< double >::max();
            const auto resolution = sample_segment(
                points[0], points[1], 300, [&]( const geode::Point3D& sample ) {
                    brute_force = std::min( brute_force,
                        std::get< 0 >( geode::point_triangle_distance(
                            sample, triangle ) ) );
                } );
            check_bounds( distance, brute_force, resolution,
                "Wrong result for segment_triangle_distance with segment ",
                hex( points[0] ), " - ", hex( points[1] ), " and triangle ",
                hex( vertices[0] ), " - ", hex( vertices[1] ), " - ",
                hex( vertices[2] ) );
        }
    }

    void test_line_triangle_distance()
    {
        for( const auto trial : geode::Range{ 1000 } )
        {
            const auto vertices = random_triangle< 3 >( trial );
            const geode::Triangle3D triangle{ vertices[0], vertices[1],
                vertices[2] };
            const auto points = random_segment( vertices, trial / 4 );
            if( geode::point_point_distance( points[0], points[1] )
                < geode::GLOBAL_EPSILON )
            {
                continue;
            }
            const geode::InfiniteLine3D line{
                geode::Vector3D{ points[0], points[1] }.normalize(), points[0]
            };
            const auto [distance, closest_line, closest_triangle] =
                geode::line_triangle_distance( line, triangle );
            check_closest_points( distance, closest_line, closest_triangle,
                "Wrong result for line_triangle_distance with line through ",
                hex( points[0] ), " and ", hex( points[1] ) );
            check( geode::point_line_distance( closest_line, line )
                           < geode::GLOBAL_EPSILON
                       && std::get< 0 >( geode::point_triangle_distance(
                              closest_triangle, triangle ) )
                              < geode::GLOBAL_EPSILON,
                "Wrong result for line_triangle_distance: closest points not "
                "on objects" );
            auto brute_force = std::numeric_limits< double >::max();
            const auto resolution = sample_triangle(
                vertices, 200, [&]( const geode::Point3D& sample ) {
                    brute_force = std::min( brute_force,
                        geode::point_line_distance( sample, line ) );
                } );
            check_bounds( distance, brute_force, resolution,
                "Wrong result for line_triangle_distance with line through ",
                hex( points[0] ), " and ", hex( points[1] ), " and triangle ",
                hex( vertices[0] ), " - ", hex( vertices[1] ), " - ",
                hex( vertices[2] ) );
        }
    }

    void test_triangle_triangle_distance()
    {
        for( const auto trial : geode::Range{ 400 } )
        {
            const auto vertices0 = random_triangle< 3 >( trial );
            const auto vertices1 =
                random_other_triangle( vertices0, trial / 4 );
            const geode::Triangle3D triangle0{ vertices0[0], vertices0[1],
                vertices0[2] };
            const geode::Triangle3D triangle1{ vertices1[0], vertices1[1],
                vertices1[2] };
            const auto [distance, closest0, closest1] =
                geode::triangle_triangle_distance( triangle0, triangle1 );
            check_closest_points( distance, closest0, closest1,
                "Wrong result for triangle_triangle_distance with trial ",
                trial );
            check( std::get< 0 >(
                       geode::point_triangle_distance( closest0, triangle0 ) )
                           < geode::GLOBAL_EPSILON
                       && std::get< 0 >( geode::point_triangle_distance(
                              closest1, triangle1 ) )
                              < geode::GLOBAL_EPSILON,
                "Wrong result for triangle_triangle_distance: closest points "
                "not on triangles" );
            auto brute_force = std::numeric_limits< double >::max();
            const auto resolution0 = sample_triangle(
                vertices0, 60, [&]( const geode::Point3D& sample ) {
                    brute_force = std::min( brute_force,
                        std::get< 0 >( geode::point_triangle_distance(
                            sample, triangle1 ) ) );
                } );
            const auto resolution1 = sample_triangle(
                vertices1, 60, [&]( const geode::Point3D& sample ) {
                    brute_force = std::min( brute_force,
                        std::get< 0 >( geode::point_triangle_distance(
                            sample, triangle0 ) ) );
                } );
            check_bounds( distance, brute_force,
                std::min( resolution0, resolution1 ),
                "Wrong result for triangle_triangle_distance with triangles ",
                hex( vertices0[0] ), " - ", hex( vertices0[1] ), " - ",
                hex( vertices0[2] ), " and ", hex( vertices1[0] ), " - ",
                hex( vertices1[1] ), " - ", hex( vertices1[2] ) );
        }
    }
} // namespace

void test()
{
    // Exact predicates need the library initialization
    geode::OpenGeodeGeometryLibrary::initialize();
    test_segment_triangle_distance();
    test_line_triangle_distance();
    test_triangle_triangle_distance();
    report_issues( "Distance" );
}

OPENGEODE_TEST( "distance-triangle-brute-force" )
