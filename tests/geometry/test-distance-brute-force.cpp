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

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/common.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/tests/common.hpp>

/*
 * Each distance is checked against a brute force minimum over samples of
 * the objects. The exact distance is below the brute force minimum and above
 * it minus the sampling resolution (largest distance from a point of the
 * sampled object to its closest sample).
 */
namespace
{
    std::mt19937 generator{ 42 };

    double uniform( double min, double max )
    {
        return std::uniform_real_distribution< double >{ min, max }(
            generator );
    }

    double log_uniform( double min_exponent, double max_exponent )
    {
        return std::pow( 10., uniform( min_exponent, max_exponent ) );
    }

    template < geode::index_t dimension >
    geode::Point< dimension > random_point( double scale )
    {
        geode::Point< dimension > point;
        for( const auto d : geode::LRange{ dimension } )
        {
            point.set_value( d, uniform( -scale, scale ) );
        }
        return point;
    }

    template < geode::index_t dimension >
    geode::Vector< dimension > random_direction()
    {
        geode::Vector< dimension > direction;
        do
        {
            direction =
                geode::Vector< dimension >{ random_point< dimension >( 1 ) };
        } while( direction.length() < 0.1 );
        return direction.normalize();
    }

    /*
     * Random triangle: generic, cap (one angle close to 180 degrees), needle
     * (one short edge) or flat (three aligned vertices)
     */
    template < geode::index_t dimension >
    std::array< geode::Point< dimension >, 3 > random_triangle(
        geode::index_t trial )
    {
        auto p0 = random_point< dimension >( 5 );
        auto p1 = random_point< dimension >( 5 );
        auto p2 = random_point< dimension >( 5 );
        switch( trial % 4 )
        {
            case 1:
                p2 = ( p0 + p1 ) / 2.
                     + random_direction< dimension >() * log_uniform( -7, -1 );
                break;
            case 2:
                p1 = p0
                     + random_direction< dimension >() * log_uniform( -7, -2 );
                break;
            case 3:
                p2 = p0 + ( p1 - p0 ) * uniform( -0.5, 1.5 );
                break;
            default:
                break;
        }
        return { p0, p1, p2 };
    }

    /*
     * Random point of the plane of the triangle (affine combination of its
     * vertices, possibly outside of the triangle)
     */
    template < geode::index_t dimension >
    geode::Point< dimension > random_point_in_plane(
        const std::array< geode::Point< dimension >, 3 >& vertices,
        double min,
        double max )
    {
        const auto u = uniform( min, max );
        const auto v = uniform( min, max );
        return vertices[0] + ( vertices[1] - vertices[0] ) * u
               + ( vertices[2] - vertices[0] ) * v;
    }

    /*
     * Query point: random, close to the triangle or in its plane
     */
    template < geode::index_t dimension >
    geode::Point< dimension > random_query(
        const std::array< geode::Point< dimension >, 3 >& vertices,
        geode::index_t trial )
    {
        switch( trial % 3 )
        {
            case 1:
                return random_point_in_plane( vertices, -0.2, 1.2 )
                       + random_direction< dimension >() * log_uniform( -8, 0 );
            case 2:
                return random_point_in_plane( vertices, -1, 2 );
            default:
                return random_point< dimension >( 8 );
        }
    }

    template < geode::index_t dimension >
    double max_edge_length(
        const std::array< geode::Point< dimension >, 3 >& vertices )
    {
        return std::max(
            { geode::point_point_distance( vertices[0], vertices[1] ),
                geode::point_point_distance( vertices[1], vertices[2] ),
                geode::point_point_distance( vertices[2], vertices[0] ) } );
    }

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

    /*
     * Random segment around the triangle: random, crossing the triangle,
     * nearly parallel to its plane, in its plane, or with an endpoint on it
     */
    std::array< geode::Point3D, 2 > random_segment(
        const std::array< geode::Point3D, 3 >& vertices, geode::index_t trial )
    {
        switch( trial % 5 )
        {
            case 1: {
                const auto on_triangle =
                    random_point_in_plane( vertices, -0.2, 0.7 );
                const auto direction = random_direction< 3 >();
                return { on_triangle - direction * uniform( 0.1, 3 ),
                    on_triangle + direction * uniform( 0.1, 3 ) };
            }
            case 2: {
                const auto offset =
                    random_direction< 3 >() * log_uniform( -8, -1 );
                return { random_point_in_plane( vertices, -1, 2 ) + offset,
                    random_point_in_plane( vertices, -1, 2 ) + offset
                        + random_direction< 3 >() * log_uniform( -10, -6 ) };
            }
            case 3:
                return { random_point_in_plane( vertices, -1, 2 ),
                    random_point_in_plane( vertices, -1, 2 ) };
            case 4:
                return { random_point_in_plane( vertices, 0, 0.5 ),
                    random_point< 3 >( 8 ) };
            default:
                return { random_point< 3 >( 8 ), random_point< 3 >( 8 ) };
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

    /*
     * Second triangle: random, translated copy of the first one (parallel),
     * in the same plane, or crossing the first one
     */
    std::array< geode::Point3D, 3 > random_other_triangle(
        const std::array< geode::Point3D, 3 >& vertices, geode::index_t trial )
    {
        switch( trial % 4 )
        {
            case 1: {
                const auto offset =
                    random_direction< 3 >() * log_uniform( -8, 0 );
                return { vertices[0] + offset, vertices[1] + offset,
                    vertices[2] + offset };
            }
            case 2:
                return { random_point_in_plane( vertices, -1, 2 ),
                    random_point_in_plane( vertices, -1, 2 ),
                    random_point_in_plane( vertices, -1, 2 ) };
            case 3: {
                const auto on_triangle =
                    random_point_in_plane( vertices, 0.1, 0.4 );
                return { on_triangle + random_direction< 3 >(),
                    on_triangle - random_direction< 3 >(),
                    random_point< 3 >( 5 ) };
            }
            default:
                return random_triangle< 3 >( trial );
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
} // namespace

void test()
{
    // Exact predicates need the library initialization
    geode::OpenGeodeGeometryLibrary::initialize();
    test_point_segment_distance< 2 >();
    test_point_segment_distance< 3 >();
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
