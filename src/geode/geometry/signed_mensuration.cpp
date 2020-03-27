/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/geometry/signed_mensuration.h>

#include <geode/geometry/perpendicular.h>

namespace geode
{
    template <>
    double triangle_area( const Triangle3D& triangle )
    {
        const auto Ux = triangle.vertices()[1].get().value( 0 )
                        - triangle.vertices()[0].get().value( 0 );
        const auto Uy = triangle.vertices()[1].get().value( 1 )
                        - triangle.vertices()[0].get().value( 1 );
        const auto Uz = triangle.vertices()[1].get().value( 2 )
                        - triangle.vertices()[0].get().value( 2 );

        const auto Vx = triangle.vertices()[2].get().value( 0 )
                        - triangle.vertices()[0].get().value( 0 );
        const auto Vy = triangle.vertices()[2].get().value( 1 )
                        - triangle.vertices()[0].get().value( 1 );
        const auto Vz = triangle.vertices()[2].get().value( 2 )
                        - triangle.vertices()[0].get().value( 2 );

        const auto Nx = Uy * Vz - Uz * Vy;
        const auto Ny = Uz * Vx - Ux * Vz;
        const auto Nz = Ux * Vy - Uy * Vx;
        return std::sqrt( Nx * Nx + Ny * Ny + Nz * Nz ) / 2.;
    }

    template <>
    double triangle_area( const Triangle2D& triangle )
    {
        return std::fabs( triangle_signed_area( triangle ) );
    }

    double triangle_signed_area(
        const Triangle3D& triangle, const Vector3D& direction )
    {
        const auto area = triangle_area( triangle );
        const auto area_normal = triangle.normal();
        return direction.dot( area_normal ) > 0 ? area : -area;
    }

    double triangle_signed_area( const Triangle2D& triangle )
    {
        return dot_perpendicular(
                   { triangle.vertices()[0], triangle.vertices()[1] },
                   { triangle.vertices()[0], triangle.vertices()[2] } )
               / 2.;
    }

    double tetra_signed_volume( const Tetra& tetra )
    {
        return Vector3D{ tetra.vertices()[0], tetra.vertices()[1] }.dot(
                   Vector3D{ tetra.vertices()[0], tetra.vertices()[2] }.cross(
                       { tetra.vertices()[0], tetra.vertices()[3] } ) )
               / 6.;
    }

    template double opengeode_geometry_api triangle_area( const Triangle2D& );

    template double opengeode_geometry_api triangle_area( const Triangle3D& );
} // namespace geode
