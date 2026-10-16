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
    template < geode::index_t dimension >
    void test_segment_line_distance()
    {
        for( const auto trial : geode::Range{ 1000 } )
        {
            const auto p0 = random_point< dimension >( 5 );
            const auto p1 = random_point< dimension >( 5 );
            const geode::Segment< dimension > segment{ p0, p1 };
            // Random, crossing or nearly parallel line
            const auto origin = trial % 3 == 0
                                    ? p0 + ( p1 - p0 ) * uniform( -0.5, 1.5 )
                                    : random_point< dimension >( 8 );
            auto direction = random_direction< dimension >();
            if( trial % 3 == 1 )
            {
                direction = ( geode::Vector< dimension >{ p0, p1 }.normalize()
                              + direction * log_uniform( -10, -2 ) )
                                .normalize();
            }
            const geode::InfiniteLine< dimension > line{ direction, origin };
            const auto [distance, closest_segment, closest_line] =
                geode::segment_line_distance( segment, line );
            check_closest_points( distance, closest_segment, closest_line,
                "Wrong result for segment_line_distance ", dimension,
                "D with segment ", hex( p0 ), " - ", hex( p1 ) );
            check( geode::point_segment_distance( closest_segment, segment )
                           < geode::GLOBAL_EPSILON
                       && geode::point_line_distance( closest_line, line )
                              < geode::GLOBAL_EPSILON,
                "Wrong result for segment_line_distance: closest points not "
                "on objects" );
            auto brute_force = std::numeric_limits< double >::max();
            const auto resolution = sample_segment(
                p0, p1, 1000, [&]( const geode::Point< dimension >& sample ) {
                    brute_force = std::min( brute_force,
                        geode::point_line_distance( sample, line ) );
                } );
            check_bounds( distance, brute_force, resolution,
                "Wrong result for segment_line_distance ", dimension,
                "D with segment ", hex( p0 ), " - ", hex( p1 ), " and line ",
                hex( origin ), " direction ", hex( direction ) );
        }
    }

    void test_segment_segment_distance()
    {
        for( const auto trial : geode::Range{ 2000 } )
        {
            const auto p0 = random_point< 3 >( 5 );
            const auto p1 =
                trial % 6 == 5
                    ? p0 + random_direction< 3 >() * log_uniform( -8, -2 )
                    : random_point< 3 >( 5 );
            std::array< geode::Point3D, 2 > other;
            switch( trial % 5 )
            {
                case 1: {
                    // Crossing segment
                    const auto crossing =
                        p0 + ( p1 - p0 ) * uniform( -0.2, 1.2 );
                    const auto direction = random_direction< 3 >();
                    other = { crossing - direction * uniform( 0, 3 ),
                        crossing + direction * uniform( 0, 3 ) };
                    break;
                }
                case 2: {
                    // Nearly parallel segment
                    const auto offset =
                        random_direction< 3 >() * log_uniform( -8, 0 );
                    const auto direction = geode::Vector3D{ p0,
                        p1 } + random_direction< 3 >() * log_uniform( -12, -3 );
                    const auto start =
                        p0 + ( p1 - p0 ) * uniform( -1, 1 ) + offset;
                    other = { start, start + direction * uniform( 0.2, 1.5 ) };
                    break;
                }
                case 3:
                    // Collinear segment
                    other = { p0 + ( p1 - p0 ) * uniform( -1, 2 ),
                        p0 + ( p1 - p0 ) * uniform( -1, 2 ) };
                    break;
                case 4:
                    // Tiny segment
                    other[0] = random_point< 3 >( 5 );
                    other[1] =
                        other[0]
                        + random_direction< 3 >() * log_uniform( -9, -5 );
                    break;
                default:
                    other = { random_point< 3 >( 5 ), random_point< 3 >( 5 ) };
            }
            const geode::Segment3D segment0{ p0, p1 };
            const geode::Segment3D segment1{ other[0], other[1] };
            const auto context = "segments " + hex( p0 ) + " - " + hex( p1 )
                                 + " and " + hex( other[0] ) + " - "
                                 + hex( other[1] );
            const auto [distance, closest0, closest1] =
                geode::segment_segment_distance( segment0, segment1 );
            check_closest_points( distance, closest0, closest1,
                "Wrong result for segment_segment_distance with ", context );
            check( geode::point_segment_distance( closest0, segment0 )
                           < geode::GLOBAL_EPSILON
                       && geode::point_segment_distance( closest1, segment1 )
                              < geode::GLOBAL_EPSILON,
                "Wrong result for segment_segment_distance with ", context,
                ": closest points not on segments" );
            auto brute_force = std::numeric_limits< double >::max();
            const auto resolution0 = sample_segment< 3 >(
                p0, p1, 1000, [&]( const geode::Point3D& sample ) {
                    brute_force = std::min( brute_force,
                        geode::point_segment_distance( sample, segment1 ) );
                } );
            const auto resolution1 = sample_segment< 3 >(
                other[0], other[1], 1000, [&]( const geode::Point3D& sample ) {
                    brute_force = std::min( brute_force,
                        geode::point_segment_distance( sample, segment0 ) );
                } );
            check_bounds( distance, brute_force,
                std::min( resolution0, resolution1 ),
                "Wrong result for segment_segment_distance with ", context );
        }
    }

    void test_non_conformal_distance()
    {
        for( const auto trial : geode::Range{ 2000 } )
        {
            const auto vertices0 = random_triangle< 3 >( trial );
            auto vertices1 = random_other_triangle( vertices0, trial / 4 );
            // 0: no shared vertex, 1: shared vertex, 2: shared edge
            const auto shared = trial % 3;
            for( const auto v : geode::LRange{ shared } )
            {
                vertices1[v] = vertices0[v];
            }
            const geode::Triangle3D triangle0{ vertices0[0], vertices0[1],
                vertices0[2] };
            const geode::Triangle3D triangle1{ vertices1[0], vertices1[1],
                vertices1[2] };
            const auto context =
                "triangles " + hex( vertices0[0] ) + " - " + hex( vertices0[1] )
                + " - " + hex( vertices0[2] ) + " and " + hex( vertices1[0] )
                + " - " + hex( vertices1[1] ) + " - " + hex( vertices1[2] );
            const auto result =
                geode::triangle_triangle_distance_between_non_conformal_parts(
                    triangle0, triangle1 );
            const auto reference = std::get< 0 >(
                geode::triangle_triangle_distance( triangle0, triangle1 ) );
            if( !result )
            {
                continue;
            }
            const auto& [distance, closest0, closest1] = result.value();
            check_closest_points( distance, closest0, closest1,
                "Wrong result for "
                "triangle_triangle_distance_between_non_conformal_parts with ",
                context );
            check( std::get< 0 >(
                       geode::point_triangle_distance( closest0, triangle0 ) )
                           < geode::GLOBAL_EPSILON
                       && std::get< 0 >( geode::point_triangle_distance(
                              closest1, triangle1 ) )
                              < geode::GLOBAL_EPSILON
                       && distance >= reference - geode::GLOBAL_EPSILON,
                "Wrong result for "
                "triangle_triangle_distance_between_non_conformal_parts with ",
                context, ": ", distance, " | distance ", reference );
            if( shared == 0 && trial % 4 != 1 )
            {
                // No shared vertex (and no colocated vertices)
                check(
                    std::fabs( distance - reference ) < geode::GLOBAL_EPSILON,
                    "Wrong result for "
                    "triangle_triangle_distance_between_non_conformal_parts "
                    "without common vertex with ",
                    context, ": ", distance, " | distance ", reference );
            }
        }

        // Constructed configurations in a plane
        for( const auto trial : geode::Range{ 500 } )
        {
            const auto origin = random_point< 3 >( 5 );
            const auto axes = plane_axes( random_direction< 3 >() );
            const auto in_plane = [&]( double u, double v ) {
                return origin + axes[0] * u + axes[1] * v;
            };
            const auto a = in_plane( 0, 0 );
            const auto b = in_plane( uniform( 0.5, 2 ), 0 );
            const auto c = in_plane( uniform( 0.2, 1 ), uniform( 0.5, 2 ) );
            // Shared edge ab, other vertex on the same side, inside abc
            const auto inner = a + ( ( b - a ) * 0.3 + ( c - a ) * 0.3 );
            const geode::Triangle3D abc{ a, b, c };
            const geode::Triangle3D abd{ a, b, inner };
            const auto folded =
                geode::triangle_triangle_distance_between_non_conformal_parts(
                    abc, abd );
            check(
                folded
                    && std::get< 0 >( folded.value() ) < geode::GLOBAL_EPSILON,
                "Wrong result for "
                "triangle_triangle_distance_between_non_conformal_parts with "
                "overlapping triangles sharing an edge, trial ",
                trial );
            // Shared edge ab, other vertex on the other side
            const auto opposite =
                in_plane( uniform( 0.2, 1 ), -uniform( 0.5, 2 ) );
            const geode::Triangle3D abe{ a, b, opposite };
            const auto conformal =
                geode::triangle_triangle_distance_between_non_conformal_parts(
                    abc, abe );
            check( !conformal
                       || std::get< 0 >( conformal.value() )
                              > geode::GLOBAL_EPSILON,
                "Wrong result for "
                "triangle_triangle_distance_between_non_conformal_parts with "
                "conformal triangles sharing an edge, trial ",
                trial );
            // Shared vertex a, other edge crossing abc
            const auto f = in_plane( uniform( 0.6, 1.2 ), uniform( -1, -0.2 ) );
            const auto g = b + ( c - b ) * 0.5 + ( c - a ) * uniform( 0.2, 1 );
            const geode::Triangle3D afg{ a, f, g };
            const auto crossing =
                geode::triangle_triangle_distance_between_non_conformal_parts(
                    abc, afg );
            check( crossing
                       && std::get< 0 >( crossing.value() )
                              < geode::GLOBAL_EPSILON,
                "Wrong result for "
                "triangle_triangle_distance_between_non_conformal_parts with "
                "overlapping triangles sharing a vertex, trial ",
                trial );
        }
    }
} // namespace

void test()
{
    // Exact predicates need the library initialization
    geode::OpenGeodeGeometryLibrary::initialize();
    test_segment_line_distance< 2 >();
    test_segment_line_distance< 3 >();
    test_segment_segment_distance();
    test_non_conformal_distance();
    report_issues( "Distance" );
}

OPENGEODE_TEST( "distance-segment-brute-force" )
