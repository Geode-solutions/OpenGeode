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

#include <algorithm>
#include <array>
#include <cmath>

#include <geode/basic/range.hpp>

#include <geode/geometry/basic_objects/ellipse.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/frame.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include "brute-force.hpp"

/*
 * Random geometric configurations for brute force tests: generic and nearly
 * degenerated objects (cap, needle and flat triangles, crossing, coplanar and
 * nearly parallel objects)
 */
namespace
{
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

    /*
     * Orthonormal axes of the plane orthogonal to the given unit normal
     * (independent of the library implementation on purpose)
     */
    std::array< geode::Vector3D, 2 > plane_axes( const geode::Vector3D& normal )
    {
        const auto other = std::fabs( normal.value( 0 ) ) < 0.9
                               ? geode::Vector3D{ { 1, 0, 0 } }
                               : geode::Vector3D{ { 0, 1, 0 } };
        const auto axis0 = normal.cross( other ).normalize();
        return { axis0, normal.cross( axis0 ) };
    }

    /*
     * Query point: random, close to the object (given point and scale), or
     * exactly at its center
     */
    template < geode::index_t dimension >
    geode::Point< dimension > random_query_around(
        const geode::Point< dimension >& center,
        double radius,
        geode::index_t trial )
    {
        switch( trial % 4 )
        {
            case 1:
                return center
                       + random_direction< dimension >()
                             * ( radius
                                 + uniform( -1, 1 ) * log_uniform( -8, 0 ) );
            case 2:
                return center
                       + random_direction< dimension >()
                             * log_uniform( -9, -5 );
            case 3:
                return center;
            default:
                return random_point< dimension >( 8 );
        }
    }
    template < geode::index_t dimension >
    geode::Frame< dimension > random_frame(
        double min_exponent, double max_exponent )
    {
        std::array< geode::Vector< dimension >, dimension > directions;
        const auto first = random_direction< dimension >();
        directions[0] = first;
        if constexpr( dimension == 2 )
        {
            directions[1] =
                geode::Vector2D{ { -first.value( 1 ), first.value( 0 ) } };
        }
        else
        {
            const auto axes = plane_axes( first );
            directions[1] = axes[0];
            directions[2] = axes[1];
        }
        for( auto& direction : directions )
        {
            direction = direction * log_uniform( min_exponent, max_exponent );
        }
        return geode::Frame< dimension >{ directions };
    }

    template < geode::index_t dimension >
    double ellipse_level( const geode::Ellipse< dimension >& ellipse,
        const geode::Point< dimension >& point )
    {
        const geode::Vector< dimension > center_to_point{ ellipse.center(),
            point };
        double level{ 0 };
        for( const auto d : geode::LRange{ dimension } )
        {
            const auto& axis = ellipse.axes().direction( d );
            const auto coordinate =
                center_to_point.dot( axis ) / axis.length2();
            level += coordinate * coordinate;
        }
        return level;
    }
} // namespace
