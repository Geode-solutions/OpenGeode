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
        const auto nb_vertices = polygon.nb_vertices();
        if( nb_vertices < 3 )
        {
            return SIGN::zero;
        }
        // Doubly linked list of the remaining vertices
        std::vector< index_t > prev( nb_vertices );
        std::vector< index_t > next( nb_vertices );
        for( const auto v : Range{ nb_vertices } )
        {
            prev[v] = v == 0 ? nb_vertices - 1 : v - 1;
            next[v] = v + 1 == nb_vertices ? 0 : v + 1;
        }
        index_t start{ 0 };
        for( auto nb_remaining = nb_vertices; nb_remaining >= 3;
            nb_remaining-- )
        {
            // The lexicographically smallest vertex is on the convex hull:
            // the polygon orientation is the orientation of its corner
            auto min_vertex = start;
            for( auto v = next[start]; v != start; v = next[v] )
            {
                if( polygon_vertices[v].get()
                    < polygon_vertices[min_vertex].get() )
                {
                    min_vertex = v;
                }
            }
            const Point2D& corner = polygon_vertices[min_vertex];
            auto before = prev[min_vertex];
            while( before != min_vertex
                   && polygon_vertices[before].get() == corner )
            {
                before = prev[before];
            }
            if( before == min_vertex )
            {
                return SIGN::zero;
            }
            auto after = next[min_vertex];
            while( polygon_vertices[after].get() == corner )
            {
                after = next[after];
            }
            const auto corner_sign = triangle_area_sign(
                { polygon_vertices[before], corner, polygon_vertices[after] } );
            if( corner_sign != SIGN::zero )
            {
                return corner_sign;
            }
            // Flat corner (spike): removing it keeps the polygon signed area
            next[prev[min_vertex]] = next[min_vertex];
            prev[next[min_vertex]] = prev[min_vertex];
            start = next[min_vertex];
        }
        return SIGN::zero;
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
