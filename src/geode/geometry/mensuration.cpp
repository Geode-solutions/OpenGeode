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
        double cross_product_length( const Vector3D& cross )
        {
            return cross.length();
        }

        double cross_product_length( double cross )
        {
            return std::fabs( cross );
        }

        double cross_product_length2( const Vector3D& cross )
        {
            return cross.length2();
        }

        double cross_product_length2( double cross )
        {
            return cross * cross;
        }

        Vector3D cross_product( const Vector3D& lhs, const Vector3D& rhs )
        {
            return lhs.cross( rhs );
        }

        double cross_product( const Vector2D& lhs, const Vector2D& rhs )
        {
            return dot_perpendicular( lhs, rhs );
        }

        // Cross product of the two edges adjacent to the vertex opposite to
        // the longest edge (same orientation as the triangle), or nullopt if
        // the triangle height is below GLOBAL_EPSILON.
        template < index_t dimension >
        auto triangle_cross_product( const Triangle< dimension >& triangle )
            -> std::optional< decltype( cross_product(
                std::declval< Vector< dimension > >(),
                std::declval< Vector< dimension > >() ) ) >
        {
            const auto& vertices = triangle.vertices();
            std::array< double, 3 > lengths2;
            for( const auto e : LRange{ 3 } )
            {
                lengths2[e] = Vector< dimension >{ vertices[e],
                    vertices[e == 2 ? 0 : e + 1] }
                                  .length2();
            }
            local_index_t longest{ 0 };
            if( lengths2[1] > lengths2[longest] )
            {
                longest = 1;
            }
            if( lengths2[2] > lengths2[longest] )
            {
                longest = 2;
            }
            const auto next = longest == 2 ? 0 : longest + 1;
            const auto opposite = next == 2 ? 0 : next + 1;
            const auto cross = cross_product(
                Vector< dimension >{ vertices[opposite], vertices[longest] },
                Vector< dimension >{ vertices[opposite], vertices[next] } );
            // height = |cross| / longest length
            if( cross_product_length2( cross )
                <= GLOBAL_EPSILON * GLOBAL_EPSILON * lengths2[longest] )
            {
                return std::nullopt;
            }
            return cross;
        }
    } // namespace

    template < index_t dimension >
    double triangle_area( const Triangle< dimension >& triangle )
    {
        const auto cross = triangle_cross_product< dimension >( triangle );
        if( !cross )
        {
            return 0;
        }
        return cross_product_length( cross.value() ) / 2.;
    }

    double triangle_signed_area(
        const Triangle3D& triangle, const Vector3D& direction )
    {
        const auto cross = triangle_cross_product< 3 >( triangle );
        if( !cross )
        {
            return 0;
        }
        const auto area = cross->length() / 2.;
        return direction.dot( cross.value() ) > 0 ? area : -area;
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
