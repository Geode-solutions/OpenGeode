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
#include <limits>

/*
 * Sampling and checking tools of the distance brute force tests
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
} // namespace
