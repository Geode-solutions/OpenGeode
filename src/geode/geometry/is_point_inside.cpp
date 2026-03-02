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

#include <geode/geometry/is_point_inside.hpp>

#include <geode/basic/logger.hpp>

namespace
{
    double is_left( const geode::Point2D& p0,
        const geode::Point2D& p1,
        const geode::Point2D& p2 )
    {
        return ( p1.value( 0 ) - p0.value( 0 ) )
                   * ( p2.value( 1 ) - p0.value( 1 ) )
               - ( p2.value( 0 ) - p0.value( 0 ) )
                     * ( p1.value( 1 ) - p0.value( 1 ) );
    }
} // namespace

namespace geode
{
    bool opengeode_geometry_api is_point_inside_polygon(
        const Point2D& point, absl::Span< const Point2D > polygon_points )
    {
        double widing_number{ 0 };
        DEBUG( "is_point_inside_polygon" );
        SDEBUG( point );
        for( const auto polygon_vertex :
            geode::Range{ 0, polygon_points.size() } )
        {
            const auto v0 = polygon_points[polygon_vertex];
            const auto next_polygon_vertex =
                ( polygon_vertex + 1 ) % polygon_points.size();
            const auto v1 = polygon_points[next_polygon_vertex];
            SDEBUG( v0 );
            SDEBUG( v1 );
            if( v0.value( 1 ) <= point.value( 1 ) )
            {
                if( v1.value( 1 ) > point.value( 1 ) )
                {
                    if( is_left( v0, v1, point ) > 0 )
                    {
                        DEBUG( "widing_number++" );
                        widing_number++;
                    }
                }
            }
            else
            {
                if( v1.value( 1 ) <= point.value( 1 ) )
                {
                    if( is_left( v0, v1, point ) < 0 )
                    {
                        DEBUG( "widing_number--" );
                        widing_number--;
                    }
                }
            }
        }
        DEBUG( widing_number );
        return widing_number != 0;
    }
} // namespace geode