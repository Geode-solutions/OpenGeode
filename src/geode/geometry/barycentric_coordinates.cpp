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

#include <geode/geometry/barycentric_coordinates.h>

#include <geode/basic/logger.h>

#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/mensuration.h>

namespace geode
{
    std::array< double, 4 > tetrahedron_barycentric_coordinates(
        const Point3D& point, const Tetrahedron& tetra )
    {
        const auto& vertices = tetra.vertices();
        const Vector3D v0{ vertices[0], vertices[1] };
        const Vector3D v1{ vertices[0], vertices[2] };
        const Vector3D v2{ vertices[0], vertices[3] };
        const Vector3D v3{ vertices[0], point };
        const auto a = v0.dot( v3 );
        const auto b = v0.dot( v0 );
        const auto c = v0.dot( v1 );
        const auto d = v0.dot( v2 );
        const auto e = v1.dot( v3 );
        const auto f = v1.dot( v1 );
        const auto g = v1.dot( v2 );
        const auto h = v2.dot( v3 );
        const auto i = v2.dot( v2 );
        const auto denom =
            2 * c * d * g - b * g * g + i * b * f - i * c * c - d * d * f;
            OPENGEODE_EXCEPTION(denom != 0, "[tetrahedron_barycentric_coordinates] Computation failed" );
        const auto dg_ci = d * g - c * i;
        const auto cg_df = c * g - d * f;
        const auto cd_bg = c * d - b * g;
        const auto lambda1 =
            ( a * ( i * f - g * g ) + e * dg_ci + h * cg_df ) / denom;
        const auto lambda2 =
            ( a * dg_ci + e * ( -d * d + b * i ) + h * cd_bg ) / denom;
        const auto lambda3 =
            ( a * cg_df + e * cd_bg + h * ( b * f - c * c ) ) / denom;
        const auto lambda0 = 1. - lambda1 - lambda2 - lambda3;
        return { { lambda0, lambda1, lambda2, lambda3 } };
    }

    template < index_t dimension >
    std::array< double, 3 > triangle_barycentric_coordinates(
        const Point< dimension >& point, const Triangle< dimension >& triangle )
    {
        const auto& vertices = triangle.vertices();
        const Vector< dimension > v0{ vertices[0], vertices[1] };
        const Vector< dimension > v1{ vertices[0], vertices[2] };
        const Vector< dimension > v2{ vertices[0], point };
        const auto d00 = v0.dot( v0 );
        const auto d01 = v0.dot( v1 );
        const auto d11 = v1.dot( v1 );
        const auto d20 = v2.dot( v0 );
        const auto d21 = v2.dot( v1 );
        const auto denom = d00 * d11 - d01 * d01;
        OPENGEODE_EXCEPTION( denom != 0,
            "[triangle_barycentric_coordinates] Computation failed" );
        const auto lambda1 = ( d11 * d20 - d01 * d21 ) / denom;
        const auto lambda2 = ( d00 * d21 - d01 * d20 ) / denom;
        const auto lambda0 = 1. - lambda1 - lambda2;
        return { { lambda0, lambda1, lambda2 } };
    }

    template < index_t dimension >
    std::array< double, 2 > segment_barycentric_coordinates(
        const Point< dimension >& point, const Segment< dimension >& segment )
    {
        const auto dir = segment.direction();
        const auto length = dir.length();
        const auto& vertices = segment.vertices();
        const Vector< dimension > v0p{ vertices[0], point };
        const auto dot0 = v0p.dot( dir ) / length;
        const Vector< dimension > v1p{ vertices[1], point };
        const auto dot1 = -v1p.dot( dir ) / length;
        const auto sum = dot0 + dot1;
        OPENGEODE_EXCEPTION( std::fabs( sum ) > global_epsilon,
            "[segment_barycentric_coordinates] Length of input segment too "
            "small" );
        const auto lambda0 = dot1 / sum;
        const auto lambda1 = dot0 / sum;
        return { { lambda0, lambda1 } };
    }

    template std::array< double, 3 > opengeode_geometry_api
        triangle_barycentric_coordinates( const Point2D&, const Triangle2D& );
    template std::array< double, 3 > opengeode_geometry_api
        triangle_barycentric_coordinates( const Point3D&, const Triangle3D& );

    template std::array< double, 2 > opengeode_geometry_api
        segment_barycentric_coordinates( const Point1D&, const Segment1D& );
    template std::array< double, 2 > opengeode_geometry_api
        segment_barycentric_coordinates( const Point2D&, const Segment2D& );
    template std::array< double, 2 > opengeode_geometry_api
        segment_barycentric_coordinates( const Point3D&, const Segment3D& );
} // namespace geode
