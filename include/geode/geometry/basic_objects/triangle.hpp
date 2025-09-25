/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#pragma once

#include <optional>

#include <geode/geometry/common.hpp>
#include <geode/geometry/vector.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerTriangle );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    class OwnerPlane;
    class Plane;

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
    ALIAS_2D_AND_3D( RefPoint );
} // namespace geode

namespace geode
{
    template < typename PointType, index_t dimension >
    class GenericTriangle
    {
    public:
        static constexpr auto dim = dimension;

        GenericTriangle(
            PointType point0, PointType point1, PointType point2 ) noexcept;

        GenericTriangle(
            const GenericTriangle< PointType, dimension >& other ) noexcept;
        GenericTriangle< PointType, dimension >& operator=(
            const GenericTriangle< PointType, dimension >& other ) noexcept;
        GenericTriangle(
            GenericTriangle< PointType, dimension >&& other ) noexcept;
        GenericTriangle< PointType, dimension >& operator=(
            GenericTriangle< PointType, dimension >&& other ) noexcept;

        [[nodiscard]] Point< dimension > barycenter() const;
        template < index_t T = dimension >
        [[nodiscard]]
        typename std::enable_if< T == 3, std::optional< Vector3D > >::type
            normal() const;
        template < index_t T = dimension >
        [[nodiscard]]
        typename std::enable_if< T == 3, std::optional< Plane > >::type
            plane() const;
        template < index_t T = dimension >
        [[nodiscard]]
        typename std::enable_if< T == 3, std::optional< OwnerPlane > >::type
            owner_plane() const;
        template < index_t T = dimension >
        [[nodiscard]]
        typename std::enable_if< T == 3, std::optional< local_index_t > >::type
            pivot() const;
        template < index_t T = dimension >
        [[nodiscard]] typename std::enable_if< T == 3,
            std::optional< std::pair< local_index_t, Vector3D > > >::type
            pivot_and_normal() const;
        void set_point( local_index_t vertex, PointType point );
        [[nodiscard]] const std::array< PointType, 3 >& vertices() const;
        [[nodiscard]] BoundingBox< dimension > bounding_box() const;
        [[nodiscard]] local_index_t longest_edge_index() const;
        [[nodiscard]] local_index_t smallest_edge_index() const;
        [[nodiscard]] double minimum_height() const;
        [[nodiscard]] bool is_degenerated() const;
        [[nodiscard]] std::string string() const;

    private:
        std::array< PointType, 3 > vertices_;
    };

    template < index_t dimension >
    class OwnerTriangle
        : public GenericTriangle< Point< dimension >, dimension >
    {
        using Base = GenericTriangle< Point< dimension >, dimension >;

    public:
        explicit OwnerTriangle( Point< dimension > point0,
            Point< dimension > point1,
            Point< dimension > point2 ) noexcept;

        OwnerTriangle( const OwnerTriangle< dimension >& other ) noexcept;
        OwnerTriangle< dimension >& operator=(
            const OwnerTriangle< dimension >& other ) noexcept;
        OwnerTriangle( OwnerTriangle< dimension >&& other ) noexcept;
        OwnerTriangle< dimension >& operator=(
            OwnerTriangle< dimension >&& other ) noexcept;
    };
    ALIAS_2D_AND_3D( OwnerTriangle );
    template < index_t dimension >
    class Triangle : public GenericTriangle< RefPoint< dimension >, dimension >
    {
        using Base = GenericTriangle< RefPoint< dimension >, dimension >;

    public:
        Triangle( const Point< dimension >& point0,
            const Point< dimension >& point1,
            const Point< dimension >& point2 ) noexcept;

        Triangle( const Triangle< dimension >& other ) noexcept;
        Triangle( const OwnerTriangle< dimension >& other ) noexcept;
        Triangle< dimension >& operator=(
            const Triangle< dimension >& other ) noexcept;
        Triangle( Triangle< dimension >&& other ) noexcept;
        Triangle< dimension >& operator=(
            Triangle< dimension >&& other ) noexcept;
    };
    ALIAS_2D_AND_3D( Triangle );

    template < index_t dimension >
    class OpenGeodeTriangleException : public OpenGeodeException
    {
    public:
        template < typename... Args >
        explicit OpenGeodeTriangleException(
            Triangle< dimension > triangle_in, const Args&... message )
            : OpenGeodeException{ absl::StrCat(
                  message..., " at ", triangle_in.string() ) },
              triangle{ std::move( triangle_in ) }
        {
        }

        OwnerTriangle< dimension > triangle;
    };
    ALIAS_2D_AND_3D( OpenGeodeTriangleException );
} // namespace geode

// NOLINTNEXTLINE
#define OPENGEODE_TRIANGLE_EXCEPTION( dimension, condition, triangle, ... )    \
    if( ABSL_PREDICT_FALSE( !( condition ) ) )                                 \
        throw geode::OpenGeodeTriangleException< dimension >                   \
        {                                                                      \
            triangle, __VA_ARGS__                                              \
        }

// NOLINTNEXTLINE
#define OPENGEODE_TRIANGLE2D_EXCEPTION( condition, triangle, ... )             \
    OPENGEODE_TRIANGLE_EXCEPTION( 2, condition, triangle, __VA_ARGS__ )

// NOLINTNEXTLINE
#define OPENGEODE_TRIANGLE3D_EXCEPTION( condition, triangle, ... )             \
    OPENGEODE_TRIANGLE_EXCEPTION( 3, condition, triangle, __VA_ARGS__ )
