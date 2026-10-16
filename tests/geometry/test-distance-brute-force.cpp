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
#include <cmath>
#include <limits>

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

#include "brute-force.hpp"
#include "random-geometry.hpp"

/*
 * Each distance is checked against a brute force minimum over samples of
 * the objects. The exact distance is below the brute force minimum and above
 * it minus the sampling resolution (largest distance from a point of the
 * sampled object to its closest sample). All failures are reported with the
 * exact (hexadecimal) inputs before failing.
 */
namespace
{

    /*
     * Calls functor with nb_samples + 1 regular samples of the segment
     * and returns the sampling resolution
     */
    template < geode::index_t dimension, typename Functor >
    double sample_segment( const geode::Point< dimension >& p0,
        const geode::Point< dimension >& p1,
        geode::index_t nb_samples,
        Functor&& functor )
    {
        for( const auto i : geode::Range{ nb_samples + 1 } )
        {
            functor(
                p0
                + ( p1 - p0 ) * ( static_cast< double >( i ) / nb_samples ) );
        }
        return geode::point_point_distance( p0, p1 ) / ( 2. * nb_samples );
    }

    /*
     * Calls functor with the nodes of a regular grid of the triangle with
     * nb_samples subdivisions per edge and returns the sampling resolution
     */
    template < geode::index_t dimension, typename Functor >
    double sample_triangle(
        const std::array< geode::Point< dimension >, 3 >& vertices,
        geode::index_t nb_samples,
        Functor&& functor )
    {
        for( const auto i : geode::Range{ nb_samples + 1 } )
        {
            for( const auto j : geode::Range{ nb_samples + 1 - i } )
            {
                const auto u = static_cast< double >( i ) / nb_samples;
                const auto v = static_cast< double >( j ) / nb_samples;
                functor( vertices[0] * ( 1. - u - v ) + vertices[1] * u
                         + vertices[2] * v );
            }
        }
        return max_edge_length( vertices ) / nb_samples;
    }

    template < typename... Args >
    void check_bounds( double distance,
        double brute_force,
        double resolution,
        const Args&... message )
    {
        check(
            distance <= brute_force + geode::GLOBAL_EPSILON
                && distance >= brute_force - resolution - geode::GLOBAL_EPSILON,
            message..., ": ", distance, " | brute force: ", brute_force,
            " | resolution: ", resolution );
    }

    template < geode::index_t dimension, typename... Args >
    void check_closest_points( double distance,
        const geode::Point< dimension >& closest0,
        const geode::Point< dimension >& closest1,
        const Args&... message )
    {
        check( std::fabs( geode::point_point_distance( closest0, closest1 )
                          - distance )
                   < geode::GLOBAL_EPSILON,
            message..., ": distance ", distance,
            " does not match closest points ", hex( closest0 ), " and ",
            hex( closest1 ) );
    }

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

    template < geode::index_t dimension, typename Functor >
    double sample_sphere( const geode::Point< dimension >& center,
        double radius,
        geode::index_t nb_samples,
        Functor&& functor )
    {
        if constexpr( dimension == 2 )
        {
            for( const auto i : geode::Range{ 2 * nb_samples } )
            {
                const auto angle = M_PI * i / nb_samples;
                functor( center
                         + geode::Point2D{ { radius * std::cos( angle ),
                             radius * std::sin( angle ) } } );
            }
        }
        else
        {
            for( const auto i : geode::Range{ nb_samples + 1 } )
            {
                const auto theta = M_PI * i / nb_samples;
                for( const auto j : geode::Range{ 2 * nb_samples } )
                {
                    const auto phi = M_PI * j / nb_samples;
                    functor( center
                             + geode::Point3D{ { radius * std::sin( theta )
                                                     * std::cos( phi ),
                                 radius * std::sin( theta ) * std::sin( phi ),
                                 radius * std::cos( theta ) } } );
                }
            }
        }
        return M_PI * radius / nb_samples;
    }

    template < geode::index_t dimension >
    void test_point_sphere_distance()
    {
        for( const auto trial : geode::Range{ 400 } )
        {
            const auto center = random_point< dimension >( 5 );
            const auto radius = log_uniform( -7, 1 );
            const geode::Sphere< dimension > sphere{ center, radius };
            const auto query = random_query_around( center, radius, trial );
            const auto [distance, closest] =
                geode::point_sphere_distance( query, sphere );
            check_closest_points( distance, query, closest,
                "Wrong result for point_sphere_distance ", dimension,
                "D with query ", hex( query ) );
            check( std::fabs(
                       geode::point_point_distance( center, closest ) - radius )
                       < geode::GLOBAL_EPSILON,
                "Wrong result for point_sphere_distance ", dimension,
                "D with query ", hex( query ), ": closest point ",
                hex( closest ), " not on sphere" );
            auto brute_force = std::numeric_limits< double >::max();
            const auto resolution = sample_sphere( center, radius, 100,
                [&]( const geode::Point< dimension >& sample ) {
                    brute_force = std::min( brute_force,
                        geode::point_point_distance( query, sample ) );
                } );
            check_bounds( distance, brute_force, resolution,
                "Wrong result for point_sphere_distance ", dimension,
                "D with query ", hex( query ), " and sphere ", hex( center ),
                " radius ", radius );
            const auto center_distance =
                geode::point_point_distance( center, query );
            const auto [signed_distance, signed_closest] =
                geode::point_sphere_signed_distance( query, sphere );
            check( std::fabs( std::fabs( signed_distance ) - distance )
                           < geode::GLOBAL_EPSILON
                       && ( distance <= geode::GLOBAL_EPSILON
                            || ( signed_distance > 0 )
                                   == ( center_distance > radius ) ),
                "Wrong result for point_sphere_signed_distance ", dimension,
                "D with query ", hex( query ), ": ", signed_distance, " | ",
                distance );
            const auto [ball_distance, ball_closest] =
                geode::point_ball_distance( query, sphere );
            const auto expected_ball_distance =
                center_distance <= radius ? 0. : distance;
            check( std::fabs( ball_distance - expected_ball_distance )
                           < geode::GLOBAL_EPSILON
                       && ball_closest.inexact_equal(
                           ball_distance == 0 ? query : closest ),
                "Wrong result for point_ball_distance ", dimension,
                "D with query ", hex( query ), ": ", ball_distance, " | ",
                expected_ball_distance );
        }
    }

    void test_point_circle_distance()
    {
        for( const auto trial : geode::Range{ 600 } )
        {
            const auto center = random_point< 3 >( 5 );
            const auto normal = random_direction< 3 >();
            const auto radius = log_uniform( -7, 1 );
            const geode::Plane plane{ normal, center };
            const geode::Circle circle{ plane, radius };
            const auto axes = plane_axes( normal );
            // Random, close to the circle, close to its center or on its axis
            auto query = random_query_around( center, radius, trial );
            if( trial % 5 == 4 )
            {
                query = center + normal * uniform( -2, 2 );
            }
            const auto [distance, closest] =
                geode::point_circle_distance( query, circle );
            check_closest_points( distance, query, closest,
                "Wrong result for point_circle_distance with query ",
                hex( query ) );
            check( std::fabs(
                       geode::point_point_distance( center, closest ) - radius )
                           < geode::GLOBAL_EPSILON
                       && std::fabs(
                              geode::Vector3D{ center, closest }.dot( normal ) )
                              < geode::GLOBAL_EPSILON,
                "Wrong result for point_circle_distance with query ",
                hex( query ), ": closest point ", hex( closest ),
                " not on circle" );
            constexpr geode::index_t nb_angles{ 4000 };
            auto brute_force = std::numeric_limits< double >::max();
            for( const auto i : geode::Range{ nb_angles } )
            {
                const auto angle = 2 * M_PI * i / nb_angles;
                brute_force = std::min( brute_force,
                    geode::point_point_distance( query,
                        center + axes[0] * ( radius * std::cos( angle ) )
                            + axes[1] * ( radius * std::sin( angle ) ) ) );
            }
            check_bounds( distance, brute_force, M_PI * radius / nb_angles,
                "Wrong result for point_circle_distance with query ",
                hex( query ), " and circle ", hex( center ), " normal ",
                hex( normal ), " radius ", radius );
            const auto height = geode::Vector3D{ center, query }.dot( normal );
            const auto [signed_distance, signed_closest] =
                geode::point_circle_signed_distance( query, circle );
            check( std::fabs( std::fabs( signed_distance ) - distance )
                           < geode::GLOBAL_EPSILON
                       && ( std::fabs( height ) <= geode::GLOBAL_EPSILON
                            || ( signed_distance > 0 ) == ( height > 0 ) ),
                "Wrong result for point_circle_signed_distance with query ",
                hex( query ), ": ", signed_distance, " | ", distance );

            const auto [disk_distance, disk_closest] =
                geode::point_disk_distance( query, circle );
            check_closest_points( disk_distance, query, disk_closest,
                "Wrong result for point_disk_distance with query ",
                hex( query ) );
            check(
                geode::point_point_distance( center, disk_closest )
                        <= radius + geode::GLOBAL_EPSILON
                    && std::fabs( geode::Vector3D{ center, disk_closest }.dot(
                           normal ) )
                           < geode::GLOBAL_EPSILON,
                "Wrong result for point_disk_distance with query ",
                hex( query ), ": closest point ", hex( disk_closest ),
                " not on disk" );
            constexpr geode::index_t nb_radii{ 100 };
            constexpr geode::index_t nb_disk_angles{ 400 };
            std::array< geode::Vector3D, nb_disk_angles > unit_directions;
            for( const auto i : geode::Range{ nb_disk_angles } )
            {
                const auto angle = 2 * M_PI * i / nb_disk_angles;
                unit_directions[i] =
                    axes[0] * std::cos( angle ) + axes[1] * std::sin( angle );
            }
            auto disk_brute_force =
                geode::point_point_distance( query, center );
            for( const auto r : geode::Range{ 1, nb_radii + 1 } )
            {
                const auto sample_radius =
                    radius * static_cast< double >( r ) / nb_radii;
                for( const auto& unit_direction : unit_directions )
                {
                    disk_brute_force = std::min( disk_brute_force,
                        geode::point_point_distance(
                            query, center + unit_direction * sample_radius ) );
                }
            }
            check_bounds( disk_distance, disk_brute_force,
                radius / ( 2. * nb_radii ) + M_PI * radius / nb_disk_angles,
                "Wrong result for point_disk_distance with query ",
                hex( query ), " and disk ", hex( center ), " normal ",
                hex( normal ), " radius ", radius );
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

    template < geode::index_t dimension, typename Functor >
    double sample_ellipse( const geode::Ellipse< dimension >& ellipse,
        geode::index_t nb_samples,
        Functor&& functor )
    {
        const auto& axes = ellipse.axes();
        double max_axis{ 0 };
        for( const auto d : geode::LRange{ dimension } )
        {
            max_axis = std::max( max_axis, axes.direction( d ).length() );
        }
        if constexpr( dimension == 2 )
        {
            for( const auto i : geode::Range{ 2 * nb_samples } )
            {
                const auto angle = M_PI * i / nb_samples;
                functor( ellipse.center()
                         + axes.direction( 0 ) * std::cos( angle )
                         + axes.direction( 1 ) * std::sin( angle ) );
            }
        }
        else
        {
            for( const auto i : geode::Range{ nb_samples + 1 } )
            {
                const auto theta = M_PI * i / nb_samples;
                for( const auto j : geode::Range{ 2 * nb_samples } )
                {
                    const auto phi = M_PI * j / nb_samples;
                    functor( ellipse.center()
                             + axes.direction( 0 )
                                   * ( std::sin( theta ) * std::cos( phi ) )
                             + axes.direction( 1 )
                                   * ( std::sin( theta ) * std::sin( phi ) )
                             + axes.direction( 2 ) * std::cos( theta ) );
                }
            }
        }
        return M_PI * max_axis / nb_samples;
    }

    template < geode::index_t dimension >
    void test_point_ellipse_distance()
    {
        for( const auto trial : geode::Range{ 300 } )
        {
            const auto center = random_point< dimension >( 5 );
            const auto axes = random_frame< dimension >( -2, 1.5 );
            const geode::Ellipse< dimension > ellipse{ center, axes };
            double max_axis{ 0 };
            double min_axis{ std::numeric_limits< double >::max() };
            for( const auto d : geode::LRange{ dimension } )
            {
                max_axis = std::max( max_axis, axes.direction( d ).length() );
                min_axis = std::min( min_axis, axes.direction( d ).length() );
            }
            // Random, close to the ellipse, or close to its center
            auto query = random_point< dimension >( 8 );
            if( trial % 3 == 1 )
            {
                geode::Point< dimension > on_ellipse;
                sample_ellipse(
                    ellipse, 3, [&]( const geode::Point< dimension >& sample ) {
                        if( uniform( 0, 1 ) < 0.2 )
                        {
                            on_ellipse = sample;
                        }
                    } );
                query =
                    on_ellipse
                    + random_direction< dimension >() * log_uniform( -8, 0 );
            }
            else if( trial % 3 == 2 )
            {
                query = center
                        + random_direction< dimension >()
                              * ( min_axis * log_uniform( -8, -0.1 ) );
            }
            std::string context = "query " + hex( query ) + " and ellipse "
                                  + hex( center ) + " axes";
            for( const auto d : geode::LRange{ dimension } )
            {
                context += " " + axes.direction( d ).string();
            }
            const auto [distance, closest] =
                geode::point_ellipse_distance( query, ellipse );
            check_closest_points( distance, query, closest,
                "Wrong result for point_ellipse_distance with ", context );
            // Distance of the closest point to the ellipse, estimated as
            // |level - 1| / |gradient(level)|
            const geode::Vector< dimension > center_to_closest{ center,
                closest };
            geode::Vector< dimension > gradient;
            for( const auto d : geode::LRange{ dimension } )
            {
                const auto& axis = axes.direction( d );
                gradient += axis
                            * ( 2 * center_to_closest.dot( axis )
                                / ( axis.length2() * axis.length2() ) );
            }
            check( std::fabs( ellipse_level( ellipse, closest ) - 1 )
                           / gradient.length()
                       < geode::GLOBAL_EPSILON,
                "Wrong result for point_ellipse_distance with ", context,
                ": closest point ", hex( closest ), " not on ellipse" );
            auto brute_force = std::numeric_limits< double >::max();
            const auto resolution =
                sample_ellipse( ellipse, dimension == 2 ? 2000 : 150,
                    [&]( const geode::Point< dimension >& sample ) {
                        brute_force = std::min( brute_force,
                            geode::point_point_distance( query, sample ) );
                    } );
            check_bounds( distance, brute_force, resolution,
                "Wrong result for point_ellipse_distance with ", context );
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
    test_point_sphere_distance< 2 >();
    test_point_sphere_distance< 3 >();
    test_point_circle_distance();
    test_point_triangle_distance< 2 >();
    test_point_triangle_distance< 3 >();
    test_segment_line_distance< 2 >();
    test_segment_line_distance< 3 >();
    test_segment_triangle_distance();
    test_line_triangle_distance();
    test_segment_segment_distance();
    test_point_ellipse_distance< 2 >();
    test_point_ellipse_distance< 3 >();
    test_point_tetrahedron_distance();
    test_triangle_triangle_distance();
    test_non_conformal_distance();
    report_issues( "Distance" );
}

OPENGEODE_TEST( "distance-brute-force" )
