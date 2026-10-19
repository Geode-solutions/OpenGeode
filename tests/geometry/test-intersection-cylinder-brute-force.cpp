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
    void test_cylinder()
    {
        const std::string name = "cylinder";
        for( const auto trial : geode::Range{ 2000 } )
        {
            const auto base = random_point< 3 >( 5 );
            const auto axis_direction = random_direction< 3 >();
            const auto length = log_uniform( -3, 2.5 );
            const auto radius = log_uniform( -3, 2.5 );
            const geode::Point3D top = base + axis_direction * length;
            const geode::Cylinder cylinder{ geode::Segment3D{ base, top },
                radius };
            const auto center = ( base + top ) / 2.;
            // Radius of the cylinder bounding sphere
            const auto size =
                std::sqrt( length * length / 4 + radius * radius );
            static const std::array< std::string, 3 > modes{ " [random]",
                " [through center]", " [near boundary]" };
            current_mode = modes[trial % 3];
            auto points = random_line_points( center, size, trial );
            if( trial % 7 == 6 )
            {
                // Parallel to the axis
                current_mode += " [parallel]";
                const auto offset = plane_axes( axis_direction )[0]
                                    * ( radius * uniform( 0, 2 ) );
                points = { base + offset - axis_direction * uniform( 0, 2 ),
                    top + offset + axis_direction * uniform( 0, 2 ) };
            }
            if( geode::point_point_distance( points[0], points[1] )
                <= geode::GLOBAL_EPSILON )
            {
                continue;
            }
            const auto signed_distance = [&]( const geode::Point3D& point ) {
                const geode::Vector3D base_to_point{ base, point };
                const auto axial = base_to_point.dot( axis_direction );
                const auto radial =
                    ( base_to_point - axis_direction * axial ).length();
                const auto axial_out =
                    std::max( { -axial, axial - length, 0. } );
                const auto radial_out = std::max( radial - radius, 0. );
                if( axial_out > 0 || radial_out > 0 )
                {
                    return std::sqrt(
                        axial_out * axial_out + radial_out * radial_out );
                }
                return -std::min( { radius - radial, axial, length - axial } );
            };
            const auto on_cylinder = [&]( const geode::Point3D& point ) {
                return std::fabs( signed_distance( point ) );
            };
            const auto context = "trial " + std::to_string( trial ) + " B "
                                 + hex( base ) + "T " + hex( top ) + "r "
                                 + hex( geode::Point1D{ { radius } } ) + "P "
                                 + hex( points[0] ) + hex( points[1] );
            const geode::Segment3D segment{ points[0], points[1] };
            const geode::InfiniteLine3D line{ segment };
            for( const auto is_line : { false, true } )
            {
                double resolution;
                const auto distances = sample_signed_distances< 3 >( points,
                    is_line, center, size, 4000, signed_distance, resolution );
                const auto expected = expected_crossings(
                    distances, resolution + geode::GLOBAL_EPSILON );
                if( is_line )
                {
                    check_points< 3 >(
                        "line_" + name,
                        geode::line_cylinder_intersection( line, cylinder ),
                        expected,
                        [&line]( const geode::Point3D& point ) {
                            return geode::point_line_distance( point, line );
                        },
                        on_cylinder, context );
                }
                else
                {
                    check_points< 3 >(
                        "segment_" + name,
                        geode::segment_cylinder_intersection(
                            segment, cylinder ),
                        expected,
                        [&segment]( const geode::Point3D& point ) {
                            return geode::point_segment_distance(
                                point, segment );
                        },
                        on_cylinder, context );
                }
            }
        }
    }
} // namespace

void test()
{
    // Exact predicates need the library initialization
    geode::OpenGeodeGeometryLibrary::initialize();
    test_cylinder();
    report_coverage();
    report_issues( "Intersection" );
}

OPENGEODE_TEST( "intersection-cylinder-brute-force" )
