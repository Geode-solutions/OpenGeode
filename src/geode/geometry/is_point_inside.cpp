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

#include <geode/geometry/basic_objects/polygon.hpp>
#include <geode/geometry/detail/predicates.hpp>
#include <geode/geometry/point.hpp>

namespace
{
    // Returns true if the point lies exactly on the edge, given the exact
    // orientation of the point relative to the edge line.
    bool is_point_on_edge( const geode::Point2D& point,
        const geode::Point2D& edge_vertex0,
        const geode::Point2D& edge_vertex1,
        GEO::SIGN point_side )
    {
        if( point_side != GEO::SIGN::zero )
        {
            return false;
        }
        // The point is on the edge line: it is on the edge if and only if it
        // is inside the edge bounding box
        for( const auto d : geode::LRange{ 2 } )
        {
            const auto value = point.value( d );
            const auto value0 = edge_vertex0.value( d );
            const auto value1 = edge_vertex1.value( d );
            if( value < std::min( value0, value1 )
                || value > std::max( value0, value1 ) )
            {
                return false;
            }
        }
        return true;
    }
} // namespace

namespace geode
{
    bool is_point_inside_polygon(
        const Point2D& point, const Polygon2D& polygon )
    {
        // Winding number using exact orientation predicates. Points on the
        // polygon boundary are considered inside, whatever the polygon
        // orientation.
        int winding_number{ 0 };
        const auto& vertices = polygon.vertices();
        const auto nb_vertices = polygon.nb_vertices();
        for( const auto polygon_vertex : geode::Range{ nb_vertices } )
        {
            const Point2D& v0 = vertices[polygon_vertex];
            const Point2D& v1 = vertices[polygon_vertex + 1 == nb_vertices
                                             ? 0
                                             : polygon_vertex + 1];
            const auto y = point.value( 1 );
            const auto y0 = v0.value( 1 );
            const auto y1 = v1.value( 1 );
            if( ( y0 < y && y1 < y ) || ( y0 > y && y1 > y ) )
            {
                // The edge neither crosses the point row nor contains it
                continue;
            }
            const auto side = GEO::PCK::orient_2d( v0, v1, point );
            if( is_point_on_edge( point, v0, v1, side ) )
            {
                return true;
            }
            if( y0 <= y )
            {
                if( y1 > y && side == GEO::SIGN::positive )
                {
                    winding_number++;
                }
            }
            else if( y1 <= y && side == GEO::SIGN::negative )
            {
                winding_number--;
            }
        }
        return winding_number != 0;
    }
} // namespace geode
