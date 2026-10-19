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
    void test_segment_triangle()
    {
        for( const auto trial : geode::Range{ 4000 } )
        {
            const auto vertices = random_triangle< 3 >( trial );
            const geode::Triangle3D triangle{ vertices[0], vertices[1],
                vertices[2] };
            const auto points = random_segment( vertices, trial / 4 );
            const geode::Segment3D segment{ points[0], points[1] };
            current_mode = segment_mode( trial );
            const auto reference = std::get< 0 >(
                geode::segment_triangle_distance( segment, triangle ) );
            const auto context = "trial " + std::to_string( trial ) + " T "
                                 + hex( vertices[0] ) + hex( vertices[1] )
                                 + hex( vertices[2] ) + "S " + hex( points[0] )
                                 + hex( points[1] ) + " ref "
                                 + std::to_string( reference );
            const auto result =
                geode::segment_triangle_intersection( segment, triangle );
            if( result.type == geode::INTERSECTION_TYPE::incorrect )
            {
                record( "incorrect", context );
            }
            else if( result.type == geode::INTERSECTION_TYPE::intersect )
            {
                const auto& point = result.result.value();
                const auto on_segment =
                    geode::point_segment_distance( point, segment );
                const auto on_triangle = std::get< 0 >(
                    geode::point_triangle_distance( point, triangle ) );
                if( on_segment > geode::GLOBAL_EPSILON
                    || on_triangle > geode::GLOBAL_EPSILON )
                {
                    record( std::string{ "segment_triangle point off (" }
                                + type_name( result.type ) + ")",
                        context + " seg " + std::to_string( on_segment )
                            + " tri " + std::to_string( on_triangle ) );
                }
                if( reference > geode::GLOBAL_EPSILON )
                {
                    record(
                        std::string{
                            "segment_triangle intersect but separated" },
                        context );
                }
            }
            else if( reference <= TOUCHING
                     && result.type != geode::INTERSECTION_TYPE::parallel )
            {
                record( std::string{ "segment_triangle missed (" }
                            + type_name( result.type ) + ")",
                    context );
            }
            const auto detection =
                geode::segment_triangle_intersection_detection(
                    segment, triangle );
            const auto detected =
                detection.first != geode::POSITION::outside
                && detection.first != geode::POSITION::parallel;
            if( detected && reference > geode::GLOBAL_EPSILON )
            {
                record(
                    std::string{
                        "segment_triangle_detection detected but separated" },
                    context );
            }
        }
    }

    void test_line_triangle()
    {
        for( const auto trial : geode::Range{ 4000 } )
        {
            const auto vertices = random_triangle< 3 >( trial );
            const geode::Triangle3D triangle{ vertices[0], vertices[1],
                vertices[2] };
            const auto points = random_segment( vertices, trial / 4 );
            if( geode::point_point_distance( points[0], points[1] )
                <= geode::GLOBAL_EPSILON )
            {
                continue;
            }
            current_mode = segment_mode( trial );
            const geode::InfiniteLine3D line{ geode::Segment3D{
                points[0], points[1] } };
            const auto reference = std::get< 0 >(
                geode::line_triangle_distance( line, triangle ) );
            const auto context = "trial " + std::to_string( trial ) + " T "
                                 + hex( vertices[0] ) + hex( vertices[1] )
                                 + hex( vertices[2] ) + "L " + hex( points[0] )
                                 + hex( points[1] ) + " ref "
                                 + std::to_string( reference );
            const auto result =
                geode::line_triangle_intersection( line, triangle );
            if( result.type == geode::INTERSECTION_TYPE::incorrect )
            {
                record( "incorrect", context );
            }
            else if( result.type == geode::INTERSECTION_TYPE::intersect )
            {
                const auto& point = result.result.value();
                const auto on_line = geode::point_line_distance( point, line );
                const auto on_triangle = std::get< 0 >(
                    geode::point_triangle_distance( point, triangle ) );
                if( on_line > geode::GLOBAL_EPSILON
                    || on_triangle > geode::GLOBAL_EPSILON )
                {
                    record( std::string{ "line_triangle point off (" }
                                + type_name( result.type ) + ")",
                        context + " line " + std::to_string( on_line ) + " tri "
                            + std::to_string( on_triangle ) );
                }
                if( reference > geode::GLOBAL_EPSILON )
                {
                    record(
                        std::string{ "line_triangle intersect but separated" },
                        context );
                }
            }
            else if( reference <= TOUCHING
                     && result.type != geode::INTERSECTION_TYPE::parallel )
            {
                record( std::string{ "line_triangle missed (" }
                            + type_name( result.type ) + ")",
                    context );
            }
            const auto detection =
                geode::line_triangle_intersection_detection( line, triangle );
            const auto detected = detection != geode::POSITION::outside
                                  && detection != geode::POSITION::parallel;
            if( detected && reference > geode::GLOBAL_EPSILON )
            {
                record(
                    std::string{
                        "line_triangle_detection detected but separated" },
                    context );
            }
        }
    }

    void test_segment_plane()
    {
        for( const auto trial : geode::Range{ 4000 } )
        {
            const auto vertices = random_triangle< 3 >( 0 );
            const auto normal = random_direction< 3 >();
            const auto& origin = vertices[0];
            const geode::Plane plane{ normal, origin };
            // Reuse the triangle segments with a non degenerated triangle
            // in the plane
            const auto axes = plane_axes( normal );
            const std::array< geode::Point3D, 3 > in_plane{ origin,
                origin + axes[0] * uniform( 0.5, 3 ),
                origin + axes[1] * uniform( 0.5, 3 ) };
            const auto points = random_segment( in_plane, trial );
            current_mode = segment_mode( trial * 4 );
            const geode::Segment3D segment{ points[0], points[1] };
            const auto height0 =
                geode::Vector3D{ origin, points[0] }.dot( normal );
            const auto height1 =
                geode::Vector3D{ origin, points[1] }.dot( normal );
            const auto reference =
                height0 * height1 <= 0
                    ? 0.
                    : std::min( std::fabs( height0 ), std::fabs( height1 ) );
            const auto context = "trial " + std::to_string( trial ) + " P "
                                 + hex( origin ) + hex( normal ) + "S "
                                 + hex( points[0] ) + hex( points[1] ) + " ref "
                                 + std::to_string( reference );
            const auto result =
                geode::segment_plane_intersection( segment, plane );
            if( result.type == geode::INTERSECTION_TYPE::incorrect )
            {
                record( "incorrect", context );
            }
            else if( result.type == geode::INTERSECTION_TYPE::intersect )
            {
                const auto& point = result.result.value();
                const auto on_segment =
                    geode::point_segment_distance( point, segment );
                const auto on_plane =
                    std::fabs( geode::Vector3D{ origin, point }.dot( normal ) );
                if( on_segment > geode::GLOBAL_EPSILON
                    || on_plane > geode::GLOBAL_EPSILON )
                {
                    record( std::string{ "segment_plane point off (" }
                                + type_name( result.type ) + ")",
                        context + " seg " + std::to_string( on_segment )
                            + " plane " + std::to_string( on_plane ) );
                }
                if( reference > geode::GLOBAL_EPSILON )
                {
                    record( "segment_plane intersect but separated", context );
                }
            }
            else if( reference <= TOUCHING
                     && result.type != geode::INTERSECTION_TYPE::parallel )
            {
                record( std::string{ "segment_plane missed (" }
                            + type_name( result.type ) + ")",
                    context );
            }
            const auto detection =
                geode::segment_plane_intersection_detection( segment, plane );
            const auto detected = detection != geode::POSITION::outside
                                  && detection != geode::POSITION::parallel;
            if( detected && reference > geode::GLOBAL_EPSILON )
            {
                record(
                    "segment_plane_detection detected but separated", context );
            }
        }
    }

    void test_segment_segment_2d()
    {
        for( const auto trial : geode::Range{ 4000 } )
        {
            const auto p0 = random_point< 2 >( 5 );
            const auto p1 = random_point< 2 >( 5 );
            std::array< geode::Point2D, 2 > other;
            switch( trial % 4 )
            {
                case 1: {
                    // Crossing at a random point, possibly with a small angle
                    const auto crossing =
                        p0 + ( p1 - p0 ) * uniform( -0.2, 1.2 );
                    auto direction = random_direction< 2 >();
                    if( trial % 8 == 1 )
                    {
                        direction = ( geode::Vector2D{ p0, p1 }.normalize()
                                      + direction * log_uniform( -10, -2 ) )
                                        .normalize();
                    }
                    other = { crossing - direction * uniform( 0, 3 ),
                        crossing + direction * uniform( 0, 3 ) };
                    break;
                }
                case 2:
                    // Collinear
                    other = { p0 + ( p1 - p0 ) * uniform( -1, 2 ),
                        p0 + ( p1 - p0 ) * uniform( -1, 2 ) };
                    break;
                case 3:
                    // Touching at an endpoint
                    other = { p0 + ( p1 - p0 ) * uniform( 0, 1 ),
                        random_point< 2 >( 5 ) };
                    break;
                default:
                    other = { random_point< 2 >( 5 ), random_point< 2 >( 5 ) };
            }
            static const std::array< std::string, 8 > segment_segment_modes{
                " [random]", " [small angle]", " [collinear]", " [endpoint]",
                " [random]", " [crossing]", " [collinear]", " [endpoint]"
            };
            current_mode = segment_segment_modes[trial % 8];
            const geode::Segment2D segment0{ p0, p1 };
            const geode::Segment2D segment1{ other[0], other[1] };
            const auto reference = std::get< 0 >(
                geode::segment_segment_distance( segment0, segment1 ) );
            const auto context = "trial " + std::to_string( trial ) + " S0 "
                                 + hex( p0 ) + hex( p1 ) + "S1 "
                                 + hex( other[0] ) + hex( other[1] ) + " ref "
                                 + std::to_string( reference );
            const auto result =
                geode::segment_segment_intersection( segment0, segment1 );
            if( result.type == geode::INTERSECTION_TYPE::incorrect )
            {
                record( "incorrect", context );
            }
            else if( result.type == geode::INTERSECTION_TYPE::intersect )
            {
                const auto& point = result.result.value();
                const auto on0 =
                    geode::point_segment_distance( point, segment0 );
                const auto on1 =
                    geode::point_segment_distance( point, segment1 );
                if( on0 > geode::GLOBAL_EPSILON || on1 > geode::GLOBAL_EPSILON )
                {
                    record( std::string{ "segment_segment point off (" }
                                + type_name( result.type ) + ")",
                        context + " on0 " + std::to_string( on0 ) + " on1 "
                            + std::to_string( on1 ) );
                }
                if( reference > geode::GLOBAL_EPSILON )
                {
                    record(
                        "segment_segment intersect but separated", context );
                }
            }
            else if( reference <= TOUCHING
                     && result.type != geode::INTERSECTION_TYPE::parallel )
            {
                record( std::string{ "segment_segment missed (" }
                            + type_name( result.type ) + ")",
                    context );
            }
            const auto detection =
                geode::segment_segment_intersection_detection(
                    segment0, segment1 );
            const auto detected =
                detection.first != geode::POSITION::outside
                && detection.first != geode::POSITION::parallel;
            if( detected && reference > geode::GLOBAL_EPSILON )
            {
                record( "segment_segment_detection detected but separated",
                    context );
            }
            const auto line_result = geode::segment_line_intersection(
                segment0, geode::InfiniteLine2D{ segment1 } );
            const auto line_reference =
                std::get< 0 >( geode::segment_line_distance(
                    segment0, geode::InfiniteLine2D{ segment1 } ) );
            if( line_result.type == geode::INTERSECTION_TYPE::incorrect )
            {
                record( "incorrect", context );
            }
            else if( line_result.type == geode::INTERSECTION_TYPE::intersect )
            {
                const auto& point = line_result.result.value();
                const auto on0 =
                    geode::point_segment_distance( point, segment0 );
                const auto on1 = geode::point_line_distance(
                    point, geode::InfiniteLine2D{ segment1 } );
                if( on0 > geode::GLOBAL_EPSILON || on1 > geode::GLOBAL_EPSILON )
                {
                    record( std::string{ "segment_line point off (" }
                                + type_name( line_result.type ) + ")",
                        context + " on0 " + std::to_string( on0 ) + " on1 "
                            + std::to_string( on1 ) );
                }
                if( line_reference > geode::GLOBAL_EPSILON )
                {
                    record( "segment_line intersect but separated", context );
                }
            }
            else if( line_reference <= TOUCHING
                     && line_result.type != geode::INTERSECTION_TYPE::parallel )
            {
                record( std::string{ "segment_line missed (" }
                            + type_name( line_result.type ) + ")",
                    context );
            }
            const auto line_detection =
                geode::segment_line_intersection_detection(
                    segment0, geode::InfiniteLine2D{ segment1 } );
            if( line_detection != geode::POSITION::outside
                && line_detection != geode::POSITION::parallel
                && line_reference > geode::GLOBAL_EPSILON )
            {
                record(
                    "segment_line_detection detected but separated", context );
            }
        }
    }

    template < geode::index_t dimension >
    void test_segment_sphere()
    {
        for( const auto trial : geode::Range{ 4000 } )
        {
            const auto center = random_point< dimension >( 5 );
            const auto radius = log_uniform( -7, 1 );
            const geode::Sphere< dimension > sphere{ center, radius };
            static const std::array< std::string, 4 > sphere_modes{ " [random]",
                " [near surface]", " [near center]", " [center]" };
            current_mode = sphere_modes[trial % 4];
            const auto p0 = random_query_around( center, radius, trial );
            const auto p1 = trial % 3 == 0 ? random_query_around(
                                                 center, radius, trial / 3 )
                                           : random_point< dimension >( 8 );
            const geode::Segment< dimension > segment{ p0, p1 };
            // Distance between the segment and the sphere surface
            const auto closest =
                geode::point_segment_distance( center, segment );
            const auto farthest =
                std::max( geode::point_point_distance( center, p0 ),
                    geode::point_point_distance( center, p1 ) );
            const auto reference = closest > radius    ? closest - radius
                                   : farthest < radius ? radius - farthest
                                                       : 0.;
            const auto context =
                "trial " + std::to_string( trial ) + " C " + hex( center )
                + " r " + hex( geode::Point1D{ { radius } } ) + " S "
                + hex( p0 ) + hex( p1 ) + " ref " + std::to_string( reference );
            std::optional< geode::IntersectionResult<
                absl::InlinedVector< geode::Point< dimension >, 2 > > >
                maybe_result;
            try
            {
                maybe_result.emplace(
                    geode::segment_sphere_intersection( segment, sphere ) );
            }
            catch( const geode::OpenGeodeException& )
            {
                record( "segment_sphere " + std::to_string( dimension )
                            + "D throws",
                    context );
                continue;
            }
            const auto& result = maybe_result.value();
            if( result.type == geode::INTERSECTION_TYPE::incorrect )
            {
                record( "incorrect", context );
            }
            else if( result.type == geode::INTERSECTION_TYPE::intersect )
            {
                if( !result.result )
                {
                    record( "segment_sphere " + std::to_string( dimension )
                                + "D no points (" + type_name( result.type )
                                + ")",
                        context );
                }
                for( const auto& point : result.result.value_or(
                         absl::InlinedVector< geode::Point< dimension >,
                             2 >{} ) )
                {
                    const auto on_segment =
                        geode::point_segment_distance( point, segment );
                    const auto on_sphere = std::fabs(
                        geode::point_point_distance( point, center ) - radius );
                    if( on_segment > geode::GLOBAL_EPSILON
                        || on_sphere > geode::GLOBAL_EPSILON )
                    {
                        record( "segment_sphere " + std::to_string( dimension )
                                    + "D point off (" + type_name( result.type )
                                    + ")",
                            context + " seg " + std::to_string( on_segment )
                                + " sphere " + std::to_string( on_sphere ) );
                    }
                }
                if( reference > geode::GLOBAL_EPSILON )
                {
                    record( "segment_sphere " + std::to_string( dimension )
                                + "D intersect but separated",
                        context );
                }
            }
            else if( reference <= TOUCHING
                     && geode::point_point_distance( p0, p1 )
                            > geode::GLOBAL_EPSILON )
            {
                record( "segment_sphere " + std::to_string( dimension )
                            + "D missed (" + type_name( result.type ) + ")",
                    context );
            }
            if( geode::point_point_distance( p0, p1 ) <= geode::GLOBAL_EPSILON )
            {
                continue;
            }
            const geode::InfiniteLine< dimension > line{ segment };
            const auto line_closest =
                geode::point_line_distance( center, line );
            const auto line_reference =
                line_closest > radius ? line_closest - radius : 0.;
            const auto line_result =
                geode::line_sphere_intersection( line, sphere );
            if( line_result.type == geode::INTERSECTION_TYPE::incorrect )
            {
                record( "incorrect", context );
            }
            else if( line_result.type == geode::INTERSECTION_TYPE::intersect )
            {
                if( !line_result.result )
                {
                    record( "line_sphere " + std::to_string( dimension )
                                + "D no points ("
                                + type_name( line_result.type ) + ")",
                        context );
                }
                for( const auto& point : line_result.result.value_or(
                         absl::InlinedVector< geode::Point< dimension >,
                             2 >{} ) )
                {
                    const auto on_line =
                        geode::point_line_distance( point, line );
                    const auto on_sphere = std::fabs(
                        geode::point_point_distance( point, center ) - radius );
                    if( on_line > geode::GLOBAL_EPSILON
                        || on_sphere > geode::GLOBAL_EPSILON )
                    {
                        record( "line_sphere " + std::to_string( dimension )
                                    + "D point off ("
                                    + type_name( line_result.type ) + ")",
                            context + " line " + std::to_string( on_line )
                                + " sphere " + std::to_string( on_sphere ) );
                    }
                }
                if( line_reference > geode::GLOBAL_EPSILON )
                {
                    record( "line_sphere " + std::to_string( dimension )
                                + "D intersect but separated",
                        context );
                }
            }
            else if( line_reference <= TOUCHING )
            {
                record( "line_sphere " + std::to_string( dimension )
                            + "D missed (" + type_name( line_result.type )
                            + ")",
                    context );
            }
        }
    }

    template < geode::index_t dimension >
    void test_ellipse()
    {
        const std::string name = "ellipse " + std::to_string( dimension ) + "D";
        for( const auto trial : geode::Range{ 600 } )
        {
            const auto center = random_point< dimension >( 5 );
            const auto axes = random_frame< dimension >( -3, 2.5 );
            const geode::Ellipse< dimension > ellipse{ center, axes };
            double size{ 0 };
            for( const auto d : geode::LRange{ dimension } )
            {
                size = std::max( size, axes.direction( d ).length() );
            }
            static const std::array< std::string, 3 > modes{ " [random]",
                " [through center]", " [near boundary]" };
            current_mode = modes[trial % 3];
            const auto points = random_line_points( center, size, trial );
            if( geode::point_point_distance( points[0], points[1] )
                <= geode::GLOBAL_EPSILON )
            {
                continue;
            }
            const auto signed_distance =
                [&ellipse]( const geode::Point< dimension >& point ) {
                    const auto distance = std::get< 0 >(
                        geode::point_ellipse_distance( point, ellipse ) );
                    return ellipse_level( ellipse, point ) < 1 ? -distance
                                                               : distance;
                };
            const auto on_ellipse =
                [&ellipse]( const geode::Point< dimension >& point ) {
                    return std::get< 0 >(
                        geode::point_ellipse_distance( point, ellipse ) );
                };
            std::string context = "trial " + std::to_string( trial ) + " C "
                                  + hex( center ) + " axes ";
            for( const auto d : geode::LRange{ dimension } )
            {
                context += hex( axes.direction( d ) );
            }
            context += "P " + hex( points[0] ) + hex( points[1] );
            const geode::Segment< dimension > segment{ points[0], points[1] };
            const geode::InfiniteLine< dimension > line{ segment };
            for( const auto is_line : { false, true } )
            {
                double resolution;
                const auto distances =
                    sample_signed_distances< dimension >( points, is_line,
                        center, size, 400, signed_distance, resolution );
                const auto expected = expected_crossings(
                    distances, resolution + geode::GLOBAL_EPSILON );
                if( is_line )
                {
                    check_points< dimension >(
                        "line_" + name,
                        geode::line_ellipse_intersection( line, ellipse ),
                        expected,
                        [&line]( const geode::Point< dimension >& point ) {
                            return geode::point_line_distance( point, line );
                        },
                        on_ellipse, context );
                }
                else
                {
                    check_points< dimension >(
                        "segment_" + name,
                        geode::segment_ellipse_intersection( segment, ellipse ),
                        expected,
                        [&segment]( const geode::Point< dimension >& point ) {
                            return geode::point_segment_distance(
                                point, segment );
                        },
                        on_ellipse, context );
                }
            }
        }
    }

    void test_line_plane()
    {
        for( const auto trial : geode::Range{ 2000 } )
        {
            const auto origin = random_point< 3 >( 5 );
            const auto normal = random_direction< 3 >();
            const geode::Plane plane{ normal, origin };
            const auto axes = plane_axes( normal );
            static const std::array< std::string, 3 > modes{ " [random]",
                " [near parallel]", " [in plane]" };
            current_mode = modes[trial % 3];
            auto direction = random_direction< 3 >();
            auto line_origin = random_point< 3 >( 8 );
            if( trial % 3 != 0 )
            {
                direction =
                    ( axes[0] * uniform( -1, 1 ) + axes[1] * uniform( -1, 1 )
                        + normal
                              * ( uniform( -1, 1 ) * log_uniform( -10, -2 ) ) )
                        .normalize();
            }
            if( trial % 3 == 2 )
            {
                line_origin = origin + axes[0] * uniform( -5, 5 )
                              + axes[1] * uniform( -5, 5 );
            }
            const geode::InfiniteLine3D line{ direction, line_origin };
            const auto sine = std::fabs( direction.dot( normal ) );
            const auto context = "trial " + std::to_string( trial ) + " O "
                                 + hex( origin ) + "N " + hex( normal ) + "L "
                                 + hex( line_origin ) + hex( direction );
            const auto result = geode::line_plane_intersection( line, plane );
            const std::optional< geode::index_t > expected =
                sine > 10 * geode::GLOBAL_EPSILON
                    ? std::optional< geode::index_t >{ 1 }
                    : std::nullopt;
            const auto on_plane = [&]( const geode::Point3D& point ) {
                return std::fabs(
                    geode::Vector3D{ origin, point }.dot( normal ) );
            };
            if( result.type == geode::INTERSECTION_TYPE::parallel && expected )
            {
                record( "line_plane parallel but crossing", context );
            }
            if( result.type == geode::INTERSECTION_TYPE::incorrect )
            {
                record( "line_plane incorrect", context );
            }
            else if( result.type == geode::INTERSECTION_TYPE::intersect )
            {
                const auto& point = result.result.value();
                if( geode::point_line_distance( point, line )
                        > geode::GLOBAL_EPSILON
                    || on_plane( point ) > geode::GLOBAL_EPSILON )
                {
                    record( "line_plane point off", context );
                }
            }
            else if( expected )
            {
                record( std::string{ "line_plane missed (" }
                            + type_name( result.type ) + ")",
                    context );
            }

            // Plane / plane
            const auto other_normal =
                trial % 3 == 0
                    ? random_direction< 3 >()
                    : ( normal
                          + random_direction< 3 >() * log_uniform( -10, -2 ) )
                          .normalize();
            const auto other_origin = random_point< 3 >( 5 );
            const geode::Plane other{ other_normal, other_origin };
            const auto planes = geode::plane_plane_intersection( plane, other );
            const auto planes_sine = normal.cross( other_normal ).length();
            const auto planes_context = context + " N2 " + hex( other_normal )
                                        + "O2 " + hex( other_origin );
            if( planes.type == geode::INTERSECTION_TYPE::intersect
                || planes.type == geode::INTERSECTION_TYPE::incorrect )
            {
                const auto& planes_line = planes.result.value();
                const auto on_other = [&]( const geode::Point3D& point ) {
                    return std::fabs(
                        geode::Vector3D{ other_origin, point }.dot(
                            other_normal ) );
                };
                for( const auto t : { 0., 1., -10. } )
                {
                    const auto point =
                        planes_line.origin() + planes_line.direction() * t;
                    if( on_plane( point ) > geode::GLOBAL_EPSILON
                        || on_other( point ) > geode::GLOBAL_EPSILON )
                    {
                        record( std::string{ "plane_plane line off (" }
                                    + type_name( planes.type ) + ")",
                            planes_context + " d0 "
                                + std::to_string( on_plane( point ) ) + " d1 "
                                + std::to_string( on_other( point ) ) );
                        break;
                    }
                }
            }
            else if( planes_sine > 10 * geode::GLOBAL_EPSILON )
            {
                record( std::string{ "plane_plane missed (" }
                            + type_name( planes.type ) + ")",
                    planes_context );
            }
        }
    }
} // namespace

void test()
{
    // Exact predicates need the library initialization
    geode::OpenGeodeGeometryLibrary::initialize();
    test_segment_triangle();
    test_line_triangle();
    test_segment_plane();
    test_segment_segment_2d();
    test_segment_sphere< 2 >();
    test_segment_sphere< 3 >();
    test_ellipse< 2 >();
    test_ellipse< 3 >();
    test_line_plane();
    report_coverage();
    report_issues( "Intersection" );
}

OPENGEODE_TEST( "intersection-brute-force" )
