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

#include <geode/geometry/sign.hpp>

#include <geode/geometry/basic_objects/polygon.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/internal/position_from_sides.hpp>
#include <geode/geometry/internal/predicates.hpp>

namespace
{
    const std::array< std::array< geode::local_index_t, 2 >, 3 > new_axis{
        { std::array< geode::local_index_t, 2 >{ { 1, 2 } },
            std::array< geode::local_index_t, 2 >{ { 2, 0 } },
            std::array< geode::local_index_t, 2 >{ { 0, 1 } } }
    };
} // namespace

namespace geode
{
    SIGN tetrahedron_volume_sign( const Tetrahedron& tetra )
    {
        const auto& vertices = tetra.vertices();
        return internal::side( GEO::PCK::orient_3d(
            vertices[0], vertices[1], vertices[2], vertices[3] ) );
    }

    SIGN triangle_area_sign( const Triangle2D& triangle )
    {
        const auto& vertices = triangle.vertices();
        return internal::side(
            GEO::PCK::orient_2d( vertices[0], vertices[1], vertices[2] ) );
    }

    SIGN polygon_area_sign( const Polygon2D& polygon )
    {
        const auto& polygon_vertices = polygon.vertices();
        const auto nb_vertices = polygon_vertices.size();
        // The lexicographically smallest vertex of a simple polygon is
        // convex: its corner orientation is the polygon orientation, even
        // when the polygon is concave.
        index_t lowest{ 0 };
        for( const auto v : Range{ 1, nb_vertices } )
        {
            const Point2D& point = polygon_vertices[v];
            if( point < polygon_vertices[lowest] )
            {
                lowest = v;
            }
        }
        const auto sign = triangle_area_sign(
            { polygon_vertices[( lowest + nb_vertices - 1 ) % nb_vertices],
                polygon_vertices[lowest],
                polygon_vertices[( lowest + 1 ) % nb_vertices] } );
        if( sign != SIGN::zero )
        {
            return sign;
        }
        // Degenerate corner (duplicated or collinear points): fallback on
        // the first non-degenerate fan triangle
        const auto& p1 = polygon_vertices[0];
        for( const auto other_index : LRange{ 1, nb_vertices - 1 } )
        {
            const auto& p2 = polygon_vertices[other_index];
            const auto& p3 = polygon_vertices[static_cast< local_index_t >(
                other_index + 1 )];
            const auto fan_sign = triangle_area_sign( { p1, p2, p3 } );
            if( fan_sign != SIGN::zero )
            {
                return fan_sign;
            }
        }
        return geode::SIGN::zero;
    }

    SIGN triangle_area_sign( const Triangle3D& triangle, local_index_t axis )
    {
        const auto axis1 = new_axis[axis][0];
        const auto axis2 = new_axis[axis][1];
        const auto& vertices = triangle.vertices();
        const Point2D pt0{ { vertices[0].get().value( axis1 ),
            vertices[0].get().value( axis2 ) } };
        const Point2D pt1{ { vertices[1].get().value( axis1 ),
            vertices[1].get().value( axis2 ) } };
        const Point2D pt2{ { vertices[2].get().value( axis1 ),
            vertices[2].get().value( axis2 ) } };
        return triangle_area_sign( { pt0, pt1, pt2 } );
    }
} // namespace geode
