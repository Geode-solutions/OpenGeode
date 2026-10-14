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

#include <cstdio>
#include <map>
#include <optional>
#include <string>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/common.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/intersection.hpp>
#include <geode/geometry/intersection_detection.hpp>
#include <geode/geometry/point.hpp>

#include <geode/tests/common.hpp>

#include "random-geometry.hpp"

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
    std::map< std::string, int > issues;
    std::string current_mode;

    template < geode::index_t dimension >
    std::string hex( const geode::Point< dimension >& point )
    {
        std::string result;
        for( const auto d : geode::LRange{ dimension } )
        {
            char buffer[64];
            std::snprintf( buffer, 64, "%a ", point.value( d ) );
            result += buffer;
        }
        return result;
    }

    void record( const std::string& category, const std::string& details )
    {
        if( issues[category + current_mode]++ < 2 )
        {
            geode::Logger::warning( category, current_mode, " | ", details );
        }
    }

    const char* type_name( geode::INTERSECTION_TYPE type )
    {
        switch( type )
        {
            case geode::INTERSECTION_TYPE::none:
                return "none";
            case geode::INTERSECTION_TYPE::intersect:
                return "intersect";
            case geode::INTERSECTION_TYPE::parallel:
                return "parallel";
            default:
                return "incorrect";
        }
    }

    constexpr double TOUCHING{ 1e-3 * geode::GLOBAL_EPSILON };

    std::string segment_mode( geode::index_t trial )
    {
        static const std::array< std::string, 5 > segment_modes{ "random",
            "crossing", "near-parallel", "coplanar", "endpoint" };
        static const std::array< std::string, 4 > triangle_modes{ "generic",
            "cap", "needle", "flat" };
        return " [" + triangle_modes[trial % 4] + "/"
               + segment_modes[( trial / 4 ) % 5] + "]";
    }

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
    for( const auto& [category, count] : issues )
    {
        geode::Logger::info( category, ": ", count );
    }
    geode::OpenGeodeGeometryException::test(
        issues.empty(), "Intersection issues found" );
}

OPENGEODE_TEST( "intersection-brute-force" )
