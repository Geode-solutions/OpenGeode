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

#include <geode/geometry/mensuration.hpp>

#include <algorithm>
#include <optional>

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/perpendicular.hpp>

namespace geode
{
    namespace
    {
        struct TriangleCross3D
        {
            std::array< double, 3 > cross;
            double length2;
        };

        struct TriangleCross2D
        {
            double cross;
            double length2;
        };

        // Cross product of the two edges adjacent to the vertex opposite to
        // the longest edge (all three edge pairs give the same result in exact
        // arithmetic, this one is the most accurate), with the orientation of
        // the triangle, or nullopt if the triangle height is below
        // GLOBAL_EPSILON. Selections are branchless.
        std::optional< TriangleCross3D > triangle_cross_product(
            const Triangle3D& triangle )
        {
            const auto& vertices = triangle.vertices();
            const Point3D& v0 = vertices[0];
            const Point3D& v1 = vertices[1];
            const Point3D& v2 = vertices[2];
            const auto e0x = v1.value( 0 ) - v0.value( 0 );
            const auto e0y = v1.value( 1 ) - v0.value( 1 );
            const auto e0z = v1.value( 2 ) - v0.value( 2 );
            const auto e1x = v2.value( 0 ) - v1.value( 0 );
            const auto e1y = v2.value( 1 ) - v1.value( 1 );
            const auto e1z = v2.value( 2 ) - v1.value( 2 );
            const auto e2x = v0.value( 0 ) - v2.value( 0 );
            const auto e2y = v0.value( 1 ) - v2.value( 1 );
            const auto e2z = v0.value( 2 ) - v2.value( 2 );
            const auto length2_0 = e0x * e0x + e0y * e0y + e0z * e0z;
            const auto length2_1 = e1x * e1x + e1y * e1y + e1z * e1z;
            const auto length2_2 = e2x * e2x + e2y * e2y + e2z * e2z;
            const auto longest0 =
                length2_0 >= length2_1 && length2_0 >= length2_2;
            const auto longest1 = !longest0 && length2_1 >= length2_2;
            // Pick the two edges following the longest one
            const auto ax = longest0 ? e1x : ( longest1 ? e2x : e0x );
            const auto ay = longest0 ? e1y : ( longest1 ? e2y : e0y );
            const auto az = longest0 ? e1z : ( longest1 ? e2z : e0z );
            const auto bx = longest0 ? e2x : ( longest1 ? e0x : e1x );
            const auto by = longest0 ? e2y : ( longest1 ? e0y : e1y );
            const auto bz = longest0 ? e2z : ( longest1 ? e0z : e1z );
            const auto max_length2 =
                std::max( { length2_0, length2_1, length2_2 } );
            const auto cx = ay * bz - az * by;
            const auto cy = az * bx - ax * bz;
            const auto cz = ax * by - ay * bx;
            const auto length2 = cx * cx + cy * cy + cz * cz;
            // height = |cross| / longest length
            if( length2 <= GLOBAL_EPSILON * GLOBAL_EPSILON * max_length2 )
            {
                return std::nullopt;
            }
            return TriangleCross3D{ { cx, cy, cz }, length2 };
        }

        std::optional< TriangleCross2D > triangle_cross_product(
            const Triangle2D& triangle )
        {
            const auto& vertices = triangle.vertices();
            const Point2D& v0 = vertices[0];
            const Point2D& v1 = vertices[1];
            const Point2D& v2 = vertices[2];
            const auto e0x = v1.value( 0 ) - v0.value( 0 );
            const auto e0y = v1.value( 1 ) - v0.value( 1 );
            const auto e1x = v2.value( 0 ) - v1.value( 0 );
            const auto e1y = v2.value( 1 ) - v1.value( 1 );
            const auto e2x = v0.value( 0 ) - v2.value( 0 );
            const auto e2y = v0.value( 1 ) - v2.value( 1 );
            const auto length2_0 = e0x * e0x + e0y * e0y;
            const auto length2_1 = e1x * e1x + e1y * e1y;
            const auto length2_2 = e2x * e2x + e2y * e2y;
            const auto longest0 =
                length2_0 >= length2_1 && length2_0 >= length2_2;
            const auto longest1 = !longest0 && length2_1 >= length2_2;
            const auto ax = longest0 ? e1x : ( longest1 ? e2x : e0x );
            const auto ay = longest0 ? e1y : ( longest1 ? e2y : e0y );
            const auto bx = longest0 ? e2x : ( longest1 ? e0x : e1x );
            const auto by = longest0 ? e2y : ( longest1 ? e0y : e1y );
            const auto max_length2 =
                std::max( { length2_0, length2_1, length2_2 } );
            const auto cross = ax * by - ay * bx;
            const auto length2 = cross * cross;
            if( length2 <= GLOBAL_EPSILON * GLOBAL_EPSILON * max_length2 )
            {
                return std::nullopt;
            }
            return TriangleCross2D{ cross, length2 };
        }
    } // namespace

    template < index_t dimension >
    double triangle_area( const Triangle< dimension >& triangle )
    {
        const auto cross = triangle_cross_product( triangle );
        if( !cross )
        {
            return 0;
        }
        return std::sqrt( cross->length2 ) / 2.;
    }

    double triangle_signed_area(
        const Triangle3D& triangle, const Vector3D& direction )
    {
        const auto cross = triangle_cross_product( triangle );
        if( !cross )
        {
            return 0;
        }
        const auto area = std::sqrt( cross->length2 ) / 2.;
        const auto dot = direction.value( 0 ) * cross->cross[0]
                         + direction.value( 1 ) * cross->cross[1]
                         + direction.value( 2 ) * cross->cross[2];
        return dot > 0 ? area : -area;
    }

    double triangle_signed_area( const Triangle2D& triangle )
    {
        const auto& vertices = triangle.vertices();
        return dot_perpendicular(
                   { vertices[0], vertices[1] }, { vertices[0], vertices[2] } )
               / 2.;
    }

    double tetrahedron_signed_volume( const Tetrahedron& tetra )
    {
        const auto& vertices = tetra.vertices();
        const Vector3D edge01{ vertices[0], vertices[1] };
        const Vector3D edge02{ vertices[0], vertices[2] };
        const Vector3D edge03{ vertices[0], vertices[3] };
        const Vector3D edge12{ vertices[1], vertices[2] };
        const Vector3D edge13{ vertices[1], vertices[3] };
        const auto cross02_03 = edge02.cross( edge03 );
        const auto volume6 = edge01.dot( cross02_03 );
        // height = |6 * volume| / (2 * largest facet area), compared on
        // squared values
        const auto max_facet_double_area2 = std::max( { cross02_03.length2(),
            edge01.cross( edge02 ).length2(), edge01.cross( edge03 ).length2(),
            edge12.cross( edge13 ).length2() } );
        if( volume6 * volume6
            <= GLOBAL_EPSILON * GLOBAL_EPSILON * max_facet_double_area2 )
        {
            return 0.;
        }
        return volume6 / 6.;
    }

    double tetrahedron_volume( const Tetrahedron& tetra )
    {
        return std::fabs( tetrahedron_signed_volume( tetra ) );
    }

    template double opengeode_geometry_api triangle_area( const Triangle2D& );
    template double opengeode_geometry_api triangle_area( const Triangle3D& );
} // namespace geode
