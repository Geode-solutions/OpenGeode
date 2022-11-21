/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geometry/sign.h>

#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/private/position_from_sides.h>
#include <geode/geometry/private/predicates.h>

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
    Sign tetrahedron_volume_sign( const Tetrahedron& tetra )
    {
        const auto& vertices = tetra.vertices();
        return detail::side( GEO::PCK::orient_3d(
            vertices[0], vertices[1], vertices[2], vertices[3] ) );
    }

    Sign triangle_area_sign( const Triangle2D& triangle )
    {
        const auto& vertices = triangle.vertices();
        return detail::side(
            GEO::PCK::orient_2d( vertices[0], vertices[1], vertices[2] ) );
    }

    Sign triangle_area_sign( const Triangle3D& triangle, local_index_t axis )
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
