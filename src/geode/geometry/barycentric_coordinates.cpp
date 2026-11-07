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

#include <geode/geometry/barycentric_coordinates.hpp>

#include <optional>

#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/perpendicular.hpp>

namespace
{
    // The computations below return std::nullopt where the public functions
    // throw, so that the safe_* functions do not rely on exceptions
    std::optional< std::array< double, 4 > > try_tetrahedron_barycentric(
        const geode::Point3D& point, const geode::Tetrahedron& tetra )
    {
        using geode::Vector3D;
        const auto& vertices = tetra.vertices();
        const Vector3D edge01{ vertices[0], vertices[1] };
        const Vector3D edge02{ vertices[0], vertices[2] };
        const Vector3D edge03{ vertices[0], vertices[3] };
        const auto normal1 = edge02.cross( edge03 );
        const auto six_volume = edge01.dot( normal1 );
        if( six_volume == 0 )
        {
            return std::nullopt;
        }
        const Vector3D edge0p{ vertices[0], point };
        const auto lambda1 = edge0p.dot( normal1 ) / six_volume;
        const auto lambda2 = edge0p.dot( edge03.cross( edge01 ) ) / six_volume;
        const auto lambda3 = edge0p.dot( edge01.cross( edge02 ) ) / six_volume;
        const auto lambda0 = 1. - lambda1 - lambda2 - lambda3;
        return std::array< double, 4 >{ { lambda0, lambda1, lambda2,
            lambda3 } };
    }

    template < geode::index_t dimension >
    std::optional< std::array< double, 3 > > try_triangle_barycentric(
        const geode::Point< dimension >& point,
        const geode::Triangle< dimension >& triangle )
    {
        const auto& vertices = triangle.vertices();
        const geode::Vector< dimension > edge01{ vertices[0], vertices[1] };
        const geode::Vector< dimension > edge02{ vertices[0], vertices[2] };
        const geode::Vector< dimension > edge0p{ vertices[0], point };
        double lambda1;
        double lambda2;
        if constexpr( dimension == 2 )
        {
            const auto double_area = geode::dot_perpendicular( edge01, edge02 );
            if( double_area == 0 )
            {
                return std::nullopt;
            }
            lambda1 = geode::dot_perpendicular( edge0p, edge02 ) / double_area;
            lambda2 = geode::dot_perpendicular( edge01, edge0p ) / double_area;
        }
        else
        {
            const auto normal = edge01.cross( edge02 );
            const auto squared_double_area = normal.length2();
            if( squared_double_area == 0 )
            {
                return std::nullopt;
            }
            lambda1 =
                normal.dot( edge0p.cross( edge02 ) ) / squared_double_area;
            lambda2 =
                normal.dot( edge01.cross( edge0p ) ) / squared_double_area;
        }
        const auto lambda0 = 1. - lambda1 - lambda2;
        return std::array< double, 3 >{ { lambda0, lambda1, lambda2 } };
    }

    template < geode::index_t dimension >
    std::optional< std::array< double, 2 > > try_segment_barycentric(
        const geode::Point< dimension >& point,
        const geode::Segment< dimension >& segment )
    {
        const auto& vertices = segment.vertices();
        // Same computation as Segment::direction
        const geode::Vector< dimension > direction{ vertices[0], vertices[1] };
        const auto length = direction.length();
        if( !( length > geode::GLOBAL_EPSILON ) )
        {
            return std::nullopt;
        }
        const auto dir = direction / length;
        const geode::Vector< dimension > v0p{ vertices[0], point };
        const auto dot0 = v0p.dot( dir );
        const geode::Vector< dimension > v1p{ vertices[1], point };
        const auto dot1 = -v1p.dot( dir );
        const auto sum = dot0 + dot1;
        if( sum == 0 )
        {
            return std::nullopt;
        }
        const auto lambda0 = dot1 / sum;
        const auto lambda1 = dot0 / sum;
        return std::array< double, 2 >{ { lambda0, lambda1 } };
    }
} // namespace

namespace geode
{
    std::array< double, 4 > tetrahedron_barycentric_coordinates(
        const Point3D& point, const Tetrahedron& tetra )
    {
        const auto result = try_tetrahedron_barycentric( point, tetra );
        OpenGeodeGeometryException::check_exception( result.has_value(), point,
            OpenGeodeException::TYPE::internal,
            "[tetrahedron_barycentric_coordinates] Computation failed" );
        return result.value();
    }

    std::array< double, 4 > safe_tetrahedron_barycentric_coordinates(
        const Point3D& point, const Tetrahedron& tetrahedron ) noexcept
    {
        if( !tetrahedron.is_degenerated() )
        {
            if( const auto result =
                    try_tetrahedron_barycentric( point, tetrahedron ) )
            {
                return result.value();
            }
        }
        const auto& vertices = tetrahedron.vertices();
        auto facet = NO_LID;
        double best_area{ 0 };
        Triangle3D best_triangle{ vertices[0], vertices[0], vertices[0] };
        for( const auto f : LRange{ 4 } )
        {
            const auto v0 = f == 3 ? 0 : f + 1;
            const auto v1 = v0 == 3 ? 0 : v0 + 1;
            const auto v2 = v1 == 3 ? 0 : v1 + 1;
            Triangle3D triangle{ vertices[v0], vertices[v1], vertices[v2] };
            const auto area = triangle_area( triangle );
            if( area >= best_area )
            {
                facet = f;
                best_area = area;
                best_triangle = std::move( triangle );
            }
        }
        std::array< double, 4 > lambdas;
        lambdas.fill( 0 );
        const auto triangle_lambdas =
            safe_triangle_barycentric_coordinates( point, best_triangle );
        const auto v0 = facet == 3 ? 0 : facet + 1;
        const auto v1 = v0 == 3 ? 0 : v0 + 1;
        const auto v2 = v1 == 3 ? 0 : v1 + 1;
        lambdas[v0] = triangle_lambdas[0];
        lambdas[v1] = triangle_lambdas[1];
        lambdas[v2] = triangle_lambdas[2];
        return lambdas;
    }

    template < index_t dimension >
    std::array< double, 3 > triangle_barycentric_coordinates(
        const Point< dimension >& point, const Triangle< dimension >& triangle )
    {
        const auto result = try_triangle_barycentric( point, triangle );
        OpenGeodeGeometryException::check_exception( result.has_value(), point,
            OpenGeodeException::TYPE::internal,
            "[triangle_barycentric_coordinates] Computation failed" );
        return result.value();
    }

    template < index_t dimension >
    std::array< double, 3 > safe_triangle_barycentric_coordinates(
        const Point< dimension >& point,
        const Triangle< dimension >& triangle ) noexcept
    {
        if( !triangle.is_degenerated() )
        {
            if( const auto result =
                    try_triangle_barycentric( point, triangle ) )
            {
                return result.value();
            }
        }
        const auto& vertices = triangle.vertices();
        auto edge = NO_LID;
        double best_length{ 0 };
        Segment< dimension > best_segment{ vertices[0], vertices[0] };
        for( const auto e : LRange{ 3 } )
        {
            Segment< dimension > segment{ vertices[e],
                vertices[e == 2 ? 0 : e + 1] };
            const auto length = segment.length();
            if( length >= best_length )
            {
                edge = e;
                best_length = length;
                best_segment = std::move( segment );
            }
        }
        std::array< double, 3 > lambdas;
        lambdas.fill( 0 );
        const auto segment_lambdas =
            safe_segment_barycentric_coordinates( point, best_segment );
        lambdas[edge] = segment_lambdas[0];
        lambdas[edge == 2 ? 0 : edge + 1] = segment_lambdas[1];
        return lambdas;
    }

    template < index_t dimension >
    std::array< double, 2 > segment_barycentric_coordinates(
        const Point< dimension >& point, const Segment< dimension >& segment )
    {
        const auto result = try_segment_barycentric( point, segment );
        OpenGeodeGeometryException::check_exception( result.has_value(), point,
            OpenGeodeException::TYPE::data,
            "[segment_barycentric_coordinates] Length of input segment too "
            "small" );
        return result.value();
    }

    template < index_t dimension >
    std::array< double, 2 > safe_segment_barycentric_coordinates(
        const Point< dimension >& point,
        const Segment< dimension >& segment ) noexcept
    {
        if( const auto result = try_segment_barycentric( point, segment ) )
        {
            return result.value();
        }
        const auto& vertices = segment.vertices();
        if( point_point_distance( point, vertices[0].get() )
            < point_point_distance( point, vertices[1].get() ) )
        {
            return { { 1, 0 } };
        }
        return { { 0, 1 } };
    }

    template std::array< double, 3 > opengeode_geometry_api
        triangle_barycentric_coordinates( const Point2D&, const Triangle2D& );
    template std::array< double, 3 > opengeode_geometry_api
        triangle_barycentric_coordinates( const Point3D&, const Triangle3D& );

    template std::array< double, 3 >
        opengeode_geometry_api safe_triangle_barycentric_coordinates(
            const Point2D&, const Triangle2D& ) noexcept;
    template std::array< double, 3 >
        opengeode_geometry_api safe_triangle_barycentric_coordinates(
            const Point3D&, const Triangle3D& ) noexcept;

    template std::array< double, 2 > opengeode_geometry_api
        segment_barycentric_coordinates( const Point1D&, const Segment1D& );
    template std::array< double, 2 > opengeode_geometry_api
        segment_barycentric_coordinates( const Point2D&, const Segment2D& );
    template std::array< double, 2 > opengeode_geometry_api
        segment_barycentric_coordinates( const Point3D&, const Segment3D& );

    template std::array< double, 2 >
        opengeode_geometry_api safe_segment_barycentric_coordinates(
            const Point1D&, const Segment1D& ) noexcept;
    template std::array< double, 2 >
        opengeode_geometry_api safe_segment_barycentric_coordinates(
            const Point2D&, const Segment2D& ) noexcept;
    template std::array< double, 2 >
        opengeode_geometry_api safe_segment_barycentric_coordinates(
            const Point3D&, const Segment3D& ) noexcept;
} // namespace geode
