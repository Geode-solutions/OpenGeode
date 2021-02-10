/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/geometry/basic_objects.h>
#include <geode/geometry/signed_mensuration.h>

namespace geode
{
    std::array< double, 4 > tetra_barycentric_coordinates(
        const Point3D& point, const Tetra& tetra )
    {
        const auto volume0 = tetra_signed_volume( Tetra{ tetra.vertices()[1],
            tetra.vertices()[3], tetra.vertices()[2], point } );
        const auto volume1 = tetra_signed_volume( Tetra{ tetra.vertices()[0],
            tetra.vertices()[2], tetra.vertices()[3], point } );
        const auto volume2 = tetra_signed_volume( Tetra{ tetra.vertices()[0],
            tetra.vertices()[3], tetra.vertices()[1], point } );
        const auto volume3 = tetra_signed_volume( Tetra{ tetra.vertices()[0],
            tetra.vertices()[1], tetra.vertices()[2], point } );
        const auto total_volume = volume0 + volume1 + volume2 + volume3;
        OPENGEODE_EXCEPTION( std::fabs( total_volume ) > global_epsilon3,
            "[tetra_barycentric_coordinates] Volume of input tetrahedron too "
            "small" );

        const auto lambda0 = volume0 / total_volume;
        const auto lambda1 = volume1 / total_volume;
        const auto lambda2 = volume2 / total_volume;
        const auto lambda3 = volume3 / total_volume;
        return { { lambda0, lambda1, lambda2, lambda3 } };
    }

    template <>
    std::array< double, 3 > triangle_barycentric_coordinates(
        const Point3D& point, const Triangle3D& triangle )
    {
        const auto triangle_normal = triangle.normal();
        const auto area0 = triangle_signed_area(
            { triangle.vertices()[1], triangle.vertices()[2], point },
            triangle_normal );
        const auto area1 = triangle_signed_area(
            { triangle.vertices()[2], triangle.vertices()[0], point },
            triangle_normal );
        const auto area2 = triangle_signed_area(
            { triangle.vertices()[0], triangle.vertices()[1], point },
            triangle_normal );

        const auto total_area = area0 + area1 + area2;
        OPENGEODE_EXCEPTION( std::fabs( total_area ) > global_epsilon2,
            "[triangle_barycentric_coordinates] Area of input triangle too "
            "small" );
        const auto lambda0 = area0 / total_area;
        const auto lambda1 = area1 / total_area;
        const auto lambda2 = area2 / total_area;
        return { { lambda0, lambda1, lambda2 } };
    }

    template <>
    std::array< double, 3 > triangle_barycentric_coordinates(
        const Point2D& point, const Triangle2D& triangle )
    {
        const auto area0 = triangle_signed_area(
            { triangle.vertices()[1], triangle.vertices()[2], point } );
        const auto area1 = triangle_signed_area(
            { triangle.vertices()[2], triangle.vertices()[0], point } );
        const auto area2 = triangle_signed_area(
            { triangle.vertices()[0], triangle.vertices()[1], point } );

        const auto total_area = area0 + area1 + area2;
        OPENGEODE_EXCEPTION( std::fabs( total_area ) > global_epsilon2,
            "[triangle_barycentric_coordinates] Area of input triangle too "
            "small" );
        const auto lambda0 = area0 / total_area;
        const auto lambda1 = area1 / total_area;
        const auto lambda2 = area2 / total_area;
        return { { lambda0, lambda1, lambda2 } };
    }

    template < index_t dimension >
    std::array< double, 2 > segment_barycentric_coordinates(
        const Point< dimension >& point, const Segment< dimension >& segment )
    {
        const auto dir = segment.direction();
        const auto length = dir.length();
        const Vector< dimension > v0p{ segment.vertices()[0], point };
        const auto dot0 = v0p.dot( dir ) / length;
        const Vector< dimension > v1p{ segment.vertices()[1], point };
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
        segment_barycentric_coordinates( const Point2D&, const Segment2D& );

    template std::array< double, 2 > opengeode_geometry_api
        segment_barycentric_coordinates( const Point3D&, const Segment3D& );
} // namespace geode
