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
    void test_point_segment_distance()
    {
        for( const auto trial : geode::Range{ 1000 } )
        {
            const auto p0 = random_point< dimension >( 5 );
            const auto p1 = trial % 2 == 0
                                ? random_point< dimension >( 5 )
                                : p0
                                      + random_direction< dimension >()
                                            * log_uniform( -8, -2 );
            const geode::Segment< dimension > segment{ p0, p1 };
            const auto query = trial % 3 == 0
                                   ? p0 + ( p1 - p0 ) * uniform( -1, 2 )
                                         + random_direction< dimension >()
                                               * log_uniform( -8, 0 )
                                   : random_point< dimension >( 8 );
            const auto distance =
                geode::point_segment_distance( query, segment );
            auto brute_force = std::numeric_limits< double >::max();
            const auto resolution = sample_segment(
                p0, p1, 1000, [&]( const geode::Point< dimension >& sample ) {
                    brute_force = std::min( brute_force,
                        geode::point_point_distance( query, sample ) );
                } );
            check_bounds( distance, brute_force, resolution,
                "Wrong result for point_segment_distance ", dimension,
                "D with query ", hex( query ), " and segment ", hex( p0 ),
                " - ", hex( p1 ) );
        }
    }

    template < geode::index_t dimension >
    void test_point_line_distance()
    {
        for( const auto trial : geode::Range{ 1000 } )
        {
            const auto origin = random_point< dimension >( 5 );
            const auto direction =
                random_direction< dimension >() * uniform( 0.1, 10 );
            const geode::InfiniteLine< dimension > line{ direction, origin };
            const auto unit = direction.normalize();
            // Random, close to the line or on the line
            auto query = random_point< dimension >( 8 );
            if( trial % 3 == 1 )
            {
                query =
                    origin + unit * uniform( -5, 5 )
                    + random_direction< dimension >() * log_uniform( -8, 0 );
            }
            else if( trial % 3 == 2 )
            {
                query = origin + unit * uniform( -5, 5 );
            }
            const auto distance = geode::point_line_distance( query, line );
            const auto half_length =
                geode::point_point_distance( query, origin ) + 1;
            auto brute_force = std::numeric_limits< double >::max();
            const auto resolution = sample_segment< dimension >(
                origin - unit * half_length, origin + unit * half_length, 4000,
                [&]( const geode::Point< dimension >& sample ) {
                    brute_force = std::min( brute_force,
                        geode::point_point_distance( query, sample ) );
                } );
            check_bounds( distance, brute_force, resolution,
                "Wrong result for point_line_distance ", dimension,
                "D with query ", hex( query ), " and line ", hex( origin ),
                " direction ", hex( direction ) );
            if constexpr( dimension == 2 )
            {
                const auto signed_distance =
                    geode::point_line_signed_distance( query, line );
                check( std::fabs( std::fabs( signed_distance ) - distance )
                           < geode::GLOBAL_EPSILON,
                    "Wrong result for point_line_signed_distance with query ",
                    hex( query ), ": ", signed_distance, " | ", distance );
                if( distance > geode::GLOBAL_EPSILON )
                {
                    const auto side = geode::point_side_to_line( query, line );
                    check( ( signed_distance > 0 )
                               == ( side == geode::SIDE::positive ),
                        "Wrong sign for point_line_signed_distance with "
                        "query ",
                        hex( query ), ": ", signed_distance );
                }
            }
        }
    }

    void test_point_plane_distance()
    {
        for( const auto trial : geode::Range{ 300 } )
        {
            const auto origin = random_point< 3 >( 5 );
            const auto normal = random_direction< 3 >();
            const geode::Plane plane{ normal * uniform( 0.1, 10 ), origin };
            const auto axes = plane_axes( normal );
            auto query = random_point< 3 >( 8 );
            if( trial % 2 == 1 )
            {
                query = origin + axes[0] * uniform( -5, 5 )
                        + axes[1] * uniform( -5, 5 )
                        + normal * ( uniform( -1, 1 ) * log_uniform( -8, 0 ) );
            }
            const auto [distance, closest] =
                geode::point_plane_distance( query, plane );
            check_closest_points( distance, query, closest,
                "Wrong result for point_plane_distance with query ",
                hex( query ) );
            check( std::fabs( geode::Vector3D{ origin, closest }.dot( normal ) )
                       < geode::GLOBAL_EPSILON,
                "Wrong result for point_plane_distance with query ",
                hex( query ), ": closest point not on plane" );
            const auto half_length =
                geode::point_point_distance( query, origin ) + 1;
            constexpr geode::index_t nb_samples{ 200 };
            auto brute_force = std::numeric_limits< double >::max();
            for( const auto i : geode::Range{ nb_samples + 1 } )
            {
                for( const auto j : geode::Range{ nb_samples + 1 } )
                {
                    const auto sample =
                        origin
                        + axes[0]
                              * ( half_length * ( 2. * i / nb_samples - 1 ) )
                        + axes[1]
                              * ( half_length * ( 2. * j / nb_samples - 1 ) );
                    brute_force = std::min( brute_force,
                        geode::point_point_distance( query, sample ) );
                }
            }
            check_bounds( distance, brute_force,
                std::sqrt( 2. ) * half_length / nb_samples,
                "Wrong result for point_plane_distance with query ",
                hex( query ), " and plane ", hex( origin ), " normal ",
                hex( normal ) );
            const auto [signed_distance, signed_closest] =
                geode::point_plane_signed_distance( query, plane );
            check( std::fabs( std::fabs( signed_distance ) - distance )
                           < geode::GLOBAL_EPSILON
                       && signed_closest.inexact_equal( closest ),
                "Wrong result for point_plane_signed_distance with query ",
                hex( query ), ": ", signed_distance, " | ", distance );
            const auto height = geode::Vector3D{ origin, query }.dot( normal );
            check( std::fabs( height ) <= geode::GLOBAL_EPSILON
                       || ( signed_distance > 0 ) == ( height > 0 ),
                "Wrong sign for point_plane_signed_distance with query ",
                hex( query ), ": ", signed_distance );
        }
    }

    template < geode::index_t dimension >
    void test_point_triangle_distance()
    {
        for( const auto trial : geode::Range{ 600 } )
        {
            const auto vertices = random_triangle< dimension >( trial );
            const geode::Triangle< dimension > triangle{ vertices[0],
                vertices[1], vertices[2] };
            const auto query = random_query( vertices, trial / 4 );
            const auto output =
                geode::point_triangle_distance( query, triangle );
            const auto distance = std::get< 0 >( output );
            const auto& closest = std::get< 1 >( output );
            check_closest_points( distance, query, closest,
                "Wrong result for point_triangle_distance ", dimension,
                "D with query ", hex( query ) );
            auto brute_force = std::numeric_limits< double >::max();
            auto closest_to_samples = std::numeric_limits< double >::max();
            const auto resolution = sample_triangle(
                vertices, 200, [&]( const geode::Point< dimension >& sample ) {
                    brute_force = std::min( brute_force,
                        geode::point_point_distance( query, sample ) );
                    closest_to_samples = std::min( closest_to_samples,
                        geode::point_point_distance( closest, sample ) );
                } );
            check_bounds( distance, brute_force, resolution,
                "Wrong result for point_triangle_distance ", dimension,
                "D with query ", hex( query ), " and triangle ",
                hex( vertices[0] ), " - ", hex( vertices[1] ), " - ",
                hex( vertices[2] ) );
            check( closest_to_samples <= resolution + geode::GLOBAL_EPSILON,
                "Wrong result for point_triangle_distance ", dimension,
                "D with query ", hex( query ), ": closest point ",
                hex( closest ), " not on triangle" );
        }
    }

    void test_point_tetrahedron_distance()
    {
        for( const auto trial : geode::Range{ 2000 } )
        {
            const auto triangle = random_triangle< 3 >( trial );
            // Apex: random, close to the base plane or a base vertex
            auto apex = random_point< 3 >( 5 );
            if( trial / 4 % 3 == 1 )
            {
                apex = random_point_in_plane( triangle, -0.5, 1.5 )
                       + random_direction< 3 >() * log_uniform( -7, -1 );
            }
            else if( trial / 4 % 3 == 2 )
            {
                apex = triangle[0]
                       + random_direction< 3 >() * log_uniform( -7, -1 );
            }
            const geode::Tetrahedron tetra{ triangle[0], triangle[1],
                triangle[2], apex };
            const auto query = random_query( triangle, trial / 12 );
            const auto context = "query " + hex( query ) + " and tetrahedron "
                                 + hex( triangle[0] ) + " - "
                                 + hex( triangle[1] ) + " - "
                                 + hex( triangle[2] ) + " - " + hex( apex );
            const auto [distance, closest] =
                geode::point_tetrahedron_distance( query, tetra );
            check_closest_points( distance, query, closest,
                "Wrong result for point_tetrahedron_distance with ", context );
            const auto& vertices = tetra.vertices();
            auto reference = std::numeric_limits< double >::max();
            for( const auto& facet :
                geode::Tetrahedron::tetrahedron_facet_vertex )
            {
                reference = std::min( reference,
                    std::get< 0 >( geode::point_triangle_distance( query,
                        geode::Triangle3D{ vertices[facet[0]],
                            vertices[facet[1]], vertices[facet[2]] } ) ) );
            }
            if( geode::point_tetrahedron_position( query, tetra )
                != geode::POSITION::outside )
            {
                reference = 0;
            }
            check( std::fabs( distance - reference ) < geode::GLOBAL_EPSILON,
                "Wrong result for point_tetrahedron_distance with ", context,
                ": ", distance, " | expected: ", reference );
        }
    }
} // namespace

void test()
{
    // Exact predicates need the library initialization
    geode::OpenGeodeGeometryLibrary::initialize();
    test_point_segment_distance< 2 >();
    test_point_segment_distance< 3 >();
    test_point_line_distance< 2 >();
    test_point_line_distance< 3 >();
    test_point_plane_distance();
    test_point_triangle_distance< 2 >();
    test_point_triangle_distance< 3 >();
    test_point_tetrahedron_distance();
    report_issues( "Distance" );
}

OPENGEODE_TEST( "distance-point-brute-force" )
