/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geometry/mensuration.h>

#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/perpendicular.h>

namespace geode
{
    static constexpr std::array< std::array< geode::local_index_t, 3 >, 4 >
        signed_volume_vertices_order{ { { 1, 2, 3 }, { 3, 2, 0 }, { 3, 0, 1 },
            { 1, 0, 2 } } };

    template < index_t dimension >
    double triangle_area( const Triangle< dimension >& triangle )
    {
        const auto& tri_v = triangle.vertices();
        // Heron's formula
        const auto l0 = point_point_distance( tri_v[0].get(), tri_v[1].get() );
        const auto l1 = point_point_distance( tri_v[1].get(), tri_v[2].get() );
        const auto l2 = point_point_distance( tri_v[2].get(), tri_v[0].get() );
        const auto p = ( l0 + l1 + l2 ) / 2;
        if( p - l0 <= global_epsilon )
        {
            return 0;
        }
        if( p - l1 <= global_epsilon )
        {
            return 0;
        }
        if( p - l2 <= global_epsilon )
        {
            return 0;
        }
        const auto area2 = p * ( p - l0 ) * ( p - l1 ) * ( p - l2 );
        return std::sqrt( area2 );
    }

    double triangle_signed_area(
        const Triangle3D& triangle, const Vector3D& direction )
    {
        const auto area = triangle_area( triangle );
        if( const auto normal = triangle.normal() )
        {
            return direction.dot( normal.value() ) > 0 ? area : -area;
        }
        return area;
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
        for( const auto v : LRange{ 4 } )
        {
            const auto v1 = signed_volume_vertices_order[v][0];
            const auto v2 = signed_volume_vertices_order[v][1];
            const auto v3 = signed_volume_vertices_order[v][2];
            const Vector3D edge02{ vertices[v], vertices[v2] };
            const Vector3D edge03{ vertices[v], vertices[v3] };
            const auto cross02_03 = edge02.cross( edge03 );
            if( cross02_03.length()
                > global_angular_epsilon * edge02.length() * edge03.length() )
            {
                return Vector3D{ vertices[v], vertices[v1] }.dot( cross02_03 )
                       / 6.;
            }
        }
        OPENGEODE_ASSERT_NOT_REACHED(
            "[tetrahedron_signed_volume] Tetrahedron signed volume uses "
            "below angular threshold cross products" );
        return 0.;
    }

    double tetrahedron_volume( const Tetrahedron& tetra )
    {
        return std::fabs( tetrahedron_signed_volume( tetra ) );
    }

    template double opengeode_geometry_api triangle_area( const Triangle2D& );
    template double opengeode_geometry_api triangle_area( const Triangle3D& );
} // namespace geode
