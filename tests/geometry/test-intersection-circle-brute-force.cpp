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

#include "random-geometry.hpp"
#include <functional>
#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/cylinder.hpp>
#include <geode/geometry/basic_objects/ellipse.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/common.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/frame.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/intersection.hpp>
#include <geode/geometry/intersection_detection.hpp>
#include <geode/geometry/point.hpp>
#include <geode/tests/common.hpp>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "intersection-brute-force.hpp"

/*
 * Each intersection is checked against the (brute force checked) distance
 * between the objects: objects farther than GLOBAL_EPSILON must not
 * intersect, touching objects must intersect (or be reported parallel when
 * coplanar or collinear), and intersection points must be on both objects.
 * The exact detections must not detect separated objects. All failures are
 * reported with the exact (hexadecimal) inputs before failing.
 */
namespace
{
    void test_circle()
    {
        for( const auto trial : geode::Range{ 2000 } )
        {
            const auto origin = random_point< 3 >( 5 );
            const auto normal = random_direction< 3 >();
            const geode::Plane plane{ normal, origin };
            const auto axes = plane_axes( normal );
            const auto radius = log_uniform( -3, 0.5 );
            static const std::array< std::string, 4 > modes{ " [random]",
                " [crossing]", " [tangent]", " [coplanar]" };
            current_mode = modes[trial % 4];
            auto center = origin + axes[0] * uniform( -2, 2 )
                          + axes[1] * uniform( -2, 2 );
            auto circle_normal = random_direction< 3 >();
            switch( trial % 4 )
            {
                case 1:
                    center += normal * ( radius * uniform( -0.9, 0.9 ) );
                    break;
                case 2: {
                    // Circle plane containing the plane normal: tangent when
                    // the center height is the radius
                    circle_normal = axes[0];
                    center +=
                        normal
                        * ( radius
                            * ( 1
                                + uniform( -1, 1 ) * log_uniform( -9, -3 ) ) );
                    break;
                }
                case 3:
                    circle_normal =
                        ( normal
                            + random_direction< 3 >() * log_uniform( -10, -6 ) )
                            .normalize();
                    center +=
                        normal * ( uniform( -1, 1 ) * log_uniform( -10, -6 ) );
                    break;
                default:
                    center = random_point< 3 >( 5 );
            }
            const geode::Plane circle_plane{ circle_normal, center };
            const geode::Circle circle{ circle_plane, radius };
            const auto circle_axes = plane_axes( circle_normal );
            const auto context = "trial " + std::to_string( trial ) + " O "
                                 + hex( origin ) + "N " + hex( normal ) + "C "
                                 + hex( center ) + "CN " + hex( circle_normal )
                                 + "r " + hex( geode::Point1D{ { radius } } );
            constexpr geode::index_t nb_samples{ 4000 };
            std::vector< double > heights;
            std::vector< geode::Point3D > samples;
            for( const auto i : geode::Range{ nb_samples + 1 } )
            {
                const auto angle = 2 * M_PI * i / nb_samples;
                samples.push_back(
                    center + circle_axes[0] * ( radius * std::cos( angle ) )
                    + circle_axes[1] * ( radius * std::sin( angle ) ) );
                heights.push_back(
                    geode::Vector3D{ origin, samples.back() }.dot( normal ) );
            }
            // Start the closed curve at the farthest point from the plane
            const auto start = static_cast< geode::index_t >(
                std::max_element( heights.begin(), heights.end(),
                    []( double a, double b ) {
                        return std::fabs( a ) < std::fabs( b );
                    } )
                - heights.begin() );
            std::vector< double > closed_heights;
            for( const auto i : geode::Range{ nb_samples + 1 } )
            {
                closed_heights.push_back( heights[( start + i ) % nb_samples] );
            }
            const auto resolution = 2 * M_PI * radius / nb_samples;
            const auto expected = expected_crossings(
                closed_heights, resolution + geode::GLOBAL_EPSILON );
            const auto on_circle = [&circle]( const geode::Point3D& point ) {
                return std::get< 0 >(
                    geode::point_circle_distance( point, circle ) );
            };
            const auto on_plane = [&]( const geode::Point3D& point ) {
                return std::fabs(
                    geode::Vector3D{ origin, point }.dot( normal ) );
            };
            check_points< 3 >( "plane_circle",
                geode::plane_circle_intersection( plane, circle ), expected,
                on_plane, on_circle, context );

            // Triangle in the plane: crossings inside the triangle
            const geode::Point3D t0 = origin + axes[0] * uniform( -3, 3 )
                                      + axes[1] * uniform( -3, 3 );
            const geode::Point3D t1 = origin + axes[0] * uniform( -3, 3 )
                                      + axes[1] * uniform( -3, 3 );
            const geode::Point3D t2 = origin + axes[0] * uniform( -3, 3 )
                                      + axes[1] * uniform( -3, 3 );
            const geode::Triangle3D triangle{ t0, t1, t2 };
            if( !triangle.normal() )
            {
                continue;
            }
            std::optional< geode::index_t > triangle_expected;
            if( expected )
            {
                triangle_expected = 0;
                const auto margin = resolution + geode::GLOBAL_EPSILON;
                for( const auto i : geode::Range{ nb_samples } )
                {
                    const auto h0 = heights[i];
                    const auto h1 = heights[i + 1];
                    if( ( h0 <= 0 ) == ( h1 <= 0 ) )
                    {
                        continue;
                    }
                    const auto crossing = samples[i]
                                          + ( samples[i + 1] - samples[i] )
                                                * ( h0 / ( h0 - h1 ) );
                    const auto triangle_distance = std::get< 0 >(
                        geode::point_triangle_distance( crossing, triangle ) );
                    double boundary_distance =
                        std::min( { geode::point_segment_distance(
                                        crossing, geode::Segment3D{ t0, t1 } ),
                            geode::point_segment_distance(
                                crossing, geode::Segment3D{ t1, t2 } ),
                            geode::point_segment_distance(
                                crossing, geode::Segment3D{ t2, t0 } ) } );
                    if( triangle_distance <= geode::GLOBAL_EPSILON
                        && boundary_distance > margin )
                    {
                        triangle_expected.value()++;
                    }
                    else if( triangle_distance <= margin )
                    {
                        triangle_expected.reset();
                        break;
                    }
                }
            }
            const auto on_triangle = [&triangle](
                                         const geode::Point3D& point ) {
                return std::get< 0 >(
                    geode::point_triangle_distance( point, triangle ) );
            };
            check_points< 3 >( "triangle_circle",
                geode::triangle_circle_intersection( triangle, circle ),
                triangle_expected, on_triangle, on_circle,
                context + " T " + hex( t0 ) + hex( t1 ) + hex( t2 ) );
        }
    }
} // namespace

void test()
{
    // Exact predicates need the library initialization
    geode::OpenGeodeGeometryLibrary::initialize();
    test_circle();
    report_coverage();
    report_issues( "Intersection" );
}

OPENGEODE_TEST( "intersection-circle-brute-force" )
