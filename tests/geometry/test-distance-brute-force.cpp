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
#include <random>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/common.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/position.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/tests/common.hpp>

#include "random-geometry.hpp"

/*
 * Each distance is checked against a brute force minimum over samples of
 * the objects. The exact distance is below the brute force minimum and above
 * it minus the sampling resolution (largest distance from a point of the
 * sampled object to its closest sample).
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
        geode::OpenGeodeGeometryException::test(
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
        geode::OpenGeodeGeometryException::test(
            std::fabs(
                geode::point_point_distance( closest0, closest1 ) - distance )
                < geode::GLOBAL_EPSILON,
            message..., ": distance ", distance,
            " does not match closest points ", closest0.string(), " and ",
            closest1.string() );
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
                "D with query ", query.string(), " and segment ", p0.string(),
                " - ", p1.string() );
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
            const auto [distance, closest] =
                geode::point_triangle_distance( query, triangle );
            check_closest_points( distance, query, closest,
                "Wrong result for point_triangle_distance ", dimension,
                "D with query ", query.string() );
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
                "D with query ", query.string(), " and triangle ",
                vertices[0].string(), " - ", vertices[1].string(), " - ",
                vertices[2].string() );
            geode::OpenGeodeGeometryException::test(
                closest_to_samples <= resolution + geode::GLOBAL_EPSILON,
                "Wrong result for point_triangle_distance ", dimension,
                "D with query ", query.string(), ": closest point ",
                closest.string(), " not on triangle" );
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
                points[0].string(), " - ", points[1].string() );
            geode::OpenGeodeGeometryException::test(
                geode::point_segment_distance( closest_segment, segment )
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
                points[0].string(), " - ", points[1].string(), " and triangle ",
                vertices[0].string(), " - ", vertices[1].string(), " - ",
                vertices[2].string() );
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
                points[0].string(), " and ", points[1].string() );
            geode::OpenGeodeGeometryException::test(
                geode::point_line_distance( closest_line, line )
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
                points[0].string(), " and ", points[1].string(),
                " and triangle ", vertices[0].string(), " - ",
                vertices[1].string(), " - ", vertices[2].string() );
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
                "D with segment ", p0.string(), " - ", p1.string() );
            geode::OpenGeodeGeometryException::test(
                geode::point_segment_distance( closest_segment, segment )
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
                "D with segment ", p0.string(), " - ", p1.string(),
                " and line ", origin.string(), " direction ",
                direction.string() );
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
            geode::OpenGeodeGeometryException::test(
                std::get< 0 >(
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
                vertices0[0].string(), " - ", vertices0[1].string(), " - ",
                vertices0[2].string(), " and ", vertices1[0].string(), " - ",
                vertices1[1].string(), " - ", vertices1[2].string() );
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
                "D with query ", query.string(), " and line ", origin.string(),
                " direction ", direction.string() );
            if constexpr( dimension == 2 )
            {
                const auto signed_distance =
                    geode::point_line_signed_distance( query, line );
                geode::OpenGeodeGeometryException::test(
                    std::fabs( std::fabs( signed_distance ) - distance )
                        < geode::GLOBAL_EPSILON,
                    "Wrong result for point_line_signed_distance with query ",
                    query.string(), ": ", signed_distance, " | ", distance );
                if( distance > geode::GLOBAL_EPSILON )
                {
                    const auto side = geode::point_side_to_line( query, line );
                    geode::OpenGeodeGeometryException::test(
                        ( signed_distance > 0 )
                            == ( side == geode::SIDE::positive ),
                        "Wrong sign for point_line_signed_distance with "
                        "query ",
                        query.string(), ": ", signed_distance );
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
                query.string() );
            geode::OpenGeodeGeometryException::test(
                std::fabs( geode::Vector3D{ origin, closest }.dot( normal ) )
                    < geode::GLOBAL_EPSILON,
                "Wrong result for point_plane_distance with query ",
                query.string(), ": closest point not on plane" );
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
                query.string(), " and plane ", origin.string(), " normal ",
                normal.string() );
            const auto [signed_distance, signed_closest] =
                geode::point_plane_signed_distance( query, plane );
            geode::OpenGeodeGeometryException::test(
                std::fabs( std::fabs( signed_distance ) - distance )
                        < geode::GLOBAL_EPSILON
                    && signed_closest.inexact_equal( closest ),
                "Wrong result for point_plane_signed_distance with query ",
                query.string(), ": ", signed_distance, " | ", distance );
            const auto height = geode::Vector3D{ origin, query }.dot( normal );
            geode::OpenGeodeGeometryException::test(
                std::fabs( height ) <= geode::GLOBAL_EPSILON
                    || ( signed_distance > 0 ) == ( height > 0 ),
                "Wrong sign for point_plane_signed_distance with query ",
                query.string(), ": ", signed_distance );
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
                "D with query ", query.string() );
            geode::OpenGeodeGeometryException::test(
                std::fabs(
                    geode::point_point_distance( center, closest ) - radius )
                    < geode::GLOBAL_EPSILON,
                "Wrong result for point_sphere_distance ", dimension,
                "D with query ", query.string(), ": closest point ",
                closest.string(), " not on sphere" );
            auto brute_force = std::numeric_limits< double >::max();
            const auto resolution = sample_sphere( center, radius, 100,
                [&]( const geode::Point< dimension >& sample ) {
                    brute_force = std::min( brute_force,
                        geode::point_point_distance( query, sample ) );
                } );
            check_bounds( distance, brute_force, resolution,
                "Wrong result for point_sphere_distance ", dimension,
                "D with query ", query.string(), " and sphere ",
                center.string(), " radius ", radius );
            const auto center_distance =
                geode::point_point_distance( center, query );
            const auto [signed_distance, signed_closest] =
                geode::point_sphere_signed_distance( query, sphere );
            geode::OpenGeodeGeometryException::test(
                std::fabs( std::fabs( signed_distance ) - distance )
                        < geode::GLOBAL_EPSILON
                    && ( distance <= geode::GLOBAL_EPSILON
                         || ( signed_distance > 0 )
                                == ( center_distance > radius ) ),
                "Wrong result for point_sphere_signed_distance ", dimension,
                "D with query ", query.string(), ": ", signed_distance, " | ",
                distance );
            const auto [ball_distance, ball_closest] =
                geode::point_ball_distance( query, sphere );
            const auto expected_ball_distance =
                center_distance <= radius ? 0. : distance;
            geode::OpenGeodeGeometryException::test(
                std::fabs( ball_distance - expected_ball_distance )
                        < geode::GLOBAL_EPSILON
                    && ball_closest.inexact_equal(
                        ball_distance == 0 ? query : closest ),
                "Wrong result for point_ball_distance ", dimension,
                "D with query ", query.string(), ": ", ball_distance, " | ",
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
                query.string() );
            geode::OpenGeodeGeometryException::test(
                std::fabs(
                    geode::point_point_distance( center, closest ) - radius )
                        < geode::GLOBAL_EPSILON
                    && std::fabs(
                           geode::Vector3D{ center, closest }.dot( normal ) )
                           < geode::GLOBAL_EPSILON,
                "Wrong result for point_circle_distance with query ",
                query.string(), ": closest point ", closest.string(),
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
                query.string(), " and circle ", center.string(), " normal ",
                normal.string(), " radius ", radius );
            const auto height = geode::Vector3D{ center, query }.dot( normal );
            const auto [signed_distance, signed_closest] =
                geode::point_circle_signed_distance( query, circle );
            geode::OpenGeodeGeometryException::test(
                std::fabs( std::fabs( signed_distance ) - distance )
                        < geode::GLOBAL_EPSILON
                    && ( std::fabs( height ) <= geode::GLOBAL_EPSILON
                         || ( signed_distance > 0 ) == ( height > 0 ) ),
                "Wrong result for point_circle_signed_distance with query ",
                query.string(), ": ", signed_distance, " | ", distance );

            const auto [disk_distance, disk_closest] =
                geode::point_disk_distance( query, circle );
            check_closest_points( disk_distance, query, disk_closest,
                "Wrong result for point_disk_distance with query ",
                query.string() );
            geode::OpenGeodeGeometryException::test(
                geode::point_point_distance( center, disk_closest )
                        <= radius + geode::GLOBAL_EPSILON
                    && std::fabs( geode::Vector3D{ center, disk_closest }.dot(
                           normal ) )
                           < geode::GLOBAL_EPSILON,
                "Wrong result for point_disk_distance with query ",
                query.string(), ": closest point ", disk_closest.string(),
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
                query.string(), " and disk ", center.string(), " normal ",
                normal.string(), " radius ", radius );
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
    test_triangle_triangle_distance();

    geode::Logger::info( "TEST SUCCESS" );
}

OPENGEODE_TEST( "distance-brute-force" )
