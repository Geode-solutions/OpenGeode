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
} // namespace

void test()
{
    // Exact predicates need the library initialization
    geode::OpenGeodeGeometryLibrary::initialize();
    test_point_sphere_distance< 2 >();
    test_point_sphere_distance< 3 >();
    test_point_circle_distance();
    test_point_ellipse_distance< 2 >();
    test_point_ellipse_distance< 3 >();
    report_issues( "Distance" );
}

OPENGEODE_TEST( "distance-curve-brute-force" )
