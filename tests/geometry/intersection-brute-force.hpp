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

#pragma once

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
#include <map>
#include <optional>
#include <string>
#include <vector>

/*
 * Checking tools of the intersection brute force tests
 */
namespace
{
    std::map< std::string, std::array< int, 3 > > coverage;

    inline const char* type_name( geode::INTERSECTION_TYPE type )
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

    inline std::string segment_mode( geode::index_t trial )
    {
        static const std::array< std::string, 5 > segment_modes{ "random",
            "crossing", "near-parallel", "coplanar", "endpoint" };
        static const std::array< std::string, 4 > triangle_modes{ "generic",
            "cap", "needle", "flat" };
        return " [" + triangle_modes[trial % 4] + "/"
               + segment_modes[( trial / 4 ) % 5] + "]";
    }

    /*
     * Expected number of boundary crossings along samples of a curve, given
     * the signed distance to the object boundary (negative inside). Samples
     * closer to the boundary than the margin are ambiguous. Returns nullopt
     * when the count is uncertain (tangent or ambiguous curve ends).
     */
    inline std::optional< geode::index_t > expected_crossings(
        const std::vector< double >& signed_distances, double margin )
    {
        geode::index_t crossings{ 0 };
        std::optional< bool > previous_inside;
        bool ambiguous_run{ false };
        for( const auto distance : signed_distances )
        {
            if( std::fabs( distance ) <= margin )
            {
                if( !previous_inside )
                {
                    return std::nullopt;
                }
                ambiguous_run = true;
                continue;
            }
            const auto inside = distance < 0;
            if( previous_inside && previous_inside.value() != inside )
            {
                crossings++;
            }
            else if( previous_inside && ambiguous_run )
            {
                // Tangent to the boundary
                return std::nullopt;
            }
            previous_inside = inside;
            ambiguous_run = false;
        }
        if( ambiguous_run || !previous_inside )
        {
            return std::nullopt;
        }
        return crossings;
    }

    template < geode::index_t dimension, typename Result >
    void check_points( const std::string& name,
        const Result& result,
        const std::optional< geode::index_t >& expected,
        const std::function< double( const geode::Point< dimension >& ) >&
            distance0,
        const std::function< double( const geode::Point< dimension >& ) >&
            distance1,
        const std::string& context )
    {
        auto& counts = coverage[name];
        counts[0]++;
        if( expected )
        {
            counts[1]++;
        }
        if( result.type == geode::INTERSECTION_TYPE::intersect )
        {
            counts[2]++;
        }
        if( result.type == geode::INTERSECTION_TYPE::incorrect )
        {
            record( name + " incorrect", context );
            return;
        }
        geode::index_t nb_points{ 0 };
        if( result.type == geode::INTERSECTION_TYPE::intersect )
        {
            if( !result.result )
            {
                record( name + " no points", context );
                return;
            }
            for( const auto& point : result.result.value() )
            {
                const auto on0 = distance0( point );
                const auto on1 = distance1( point );
                if( on0 > geode::GLOBAL_EPSILON || on1 > geode::GLOBAL_EPSILON )
                {
                    record( name + " point off",
                        context + " d0 " + std::to_string( on0 ) + " d1 "
                            + std::to_string( on1 ) );
                }
            }
            nb_points = result.result->size();
        }
        if( expected && result.type != geode::INTERSECTION_TYPE::parallel
            && nb_points != expected.value() )
        {
            record( name + " wrong count (" + type_name( result.type ) + ")",
                context + " got " + std::to_string( nb_points ) + " expected "
                    + std::to_string( expected.value() ) );
        }
    }

    /*
     * Random line through or around an object of given center and size:
     * random, close to the center, or at a distance close to the size
     */
    template < geode::index_t dimension >
    std::array< geode::Point< dimension >, 2 > random_line_points(
        const geode::Point< dimension >& center,
        double size,
        geode::index_t trial )
    {
        const auto direction = random_direction< dimension >();
        geode::Point< dimension > origin;
        switch( trial % 3 )
        {
            case 1:
                origin = center
                         + random_direction< dimension >()
                               * log_uniform( -9, -1 ) * size;
                break;
            case 2:
                origin =
                    center
                    + random_direction< dimension >()
                          * ( size
                              * ( 1
                                  + uniform( -1, 1 ) * log_uniform( -8, 0 ) ) );
                break;
            default:
                origin = random_point< dimension >( 8 );
        }
        const auto length = uniform( 0.1, 2 ) * size + uniform( 0, 3 );
        return { origin - direction * ( length * uniform( 0, 1 ) ),
            origin + direction * ( length * uniform( 0, 1 ) ) };
    }

    /*
     * Signed distances to the object boundary at samples of the segment
     * (or of the line around the object when is_line)
     */
    template < geode::index_t dimension >
    std::vector< double > sample_signed_distances(
        const std::array< geode::Point< dimension >, 2 >& points,
        bool is_line,
        const geode::Point< dimension >& center,
        double size,
        geode::index_t nb_samples,
        const std::function< double( const geode::Point< dimension >& ) >&
            signed_distance,
        double& resolution )
    {
        auto begin = points[0];
        auto end = points[1];
        if( is_line )
        {
            const auto unit =
                geode::Vector< dimension >{ points[0], points[1] }.normalize();
            const auto middle = points[0]
                                + unit
                                      * unit.dot( geode::Vector< dimension >{
                                          points[0], center } );
            const auto half_length =
                size + geode::point_point_distance( middle, center ) + 1;
            begin = middle - unit * half_length;
            end = middle + unit * half_length;
        }
        std::vector< double > distances;
        distances.reserve( nb_samples + 1 );
        for( const auto i : geode::Range{ nb_samples + 1 } )
        {
            distances.push_back( signed_distance(
                begin
                + ( end - begin )
                      * ( static_cast< double >( i ) / nb_samples ) ) );
        }
        resolution = geode::point_point_distance( begin, end ) / nb_samples;
        return distances;
    }

    inline void report_coverage()
    {
        for( const auto& [name, counts] : coverage )
        {
            geode::Logger::info( name, ": ", counts[0], " cases, ", counts[1],
                " with known count, ", counts[2], " intersecting" );
        }
    }
} // namespace
