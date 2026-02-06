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

#pragma once

#include <optional>

#include <geode/geometry/common.hpp>
#include <geode/geometry/vector.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerPolygon );
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
    class GenericPolygon
    {
    public:
        static constexpr auto dim = dimension;

        GenericPolygon( std::vector< PointType > vertices ) noexcept;

        GenericPolygon(
            const GenericPolygon< PointType, dimension >& other ) noexcept;
        GenericPolygon< PointType, dimension >& operator=(
            const GenericPolygon< PointType, dimension >& other ) noexcept;
        GenericPolygon(
            GenericPolygon< PointType, dimension >&& other ) noexcept;
        GenericPolygon< PointType, dimension >& operator=(
            GenericPolygon< PointType, dimension >&& other ) noexcept;

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
        [[nodiscard]] index_t nb_vertices() const;
        void set_point( index_t vertex, PointType point );
        [[nodiscard]] const std::vector< PointType >& vertices() const;
        [[nodiscard]] BoundingBox< dimension > bounding_box() const;
        [[nodiscard]] std::vector< std::array< index_t, 3 > >
            triangulate() const;
        [[nodiscard]] double minimum_height() const;
        [[nodiscard]] bool is_degenerated() const;
        [[nodiscard]] std::string string() const;

    private:
        std::vector< PointType > vertices_;
    };

    template < index_t dimension >
    class OwnerPolygon : public GenericPolygon< Point< dimension >, dimension >
    {
        using Base = GenericPolygon< Point< dimension >, dimension >;

    public:
        explicit OwnerPolygon(
            std::vector< Point< dimension > > vertices ) noexcept;

        OwnerPolygon( const OwnerPolygon< dimension >& other ) noexcept;
        OwnerPolygon< dimension >& operator=(
            const OwnerPolygon< dimension >& other ) noexcept;
        OwnerPolygon( OwnerPolygon< dimension >&& other ) noexcept;
        OwnerPolygon< dimension >& operator=(
            OwnerPolygon< dimension >&& other ) noexcept;
    };
    ALIAS_2D_AND_3D( OwnerPolygon );

    template < index_t dimension >
    class Polygon : public GenericPolygon< RefPoint< dimension >, dimension >
    {
        using Base = GenericPolygon< RefPoint< dimension >, dimension >;

    public:
        Polygon( std::vector< RefPoint< dimension > > vertices ) noexcept;

        Polygon( const Polygon< dimension >& other ) noexcept;
        Polygon< dimension >& operator=(
            const Polygon< dimension >& other ) noexcept;
        Polygon( Polygon< dimension >&& other ) noexcept;
        Polygon< dimension >& operator=(
            Polygon< dimension >&& other ) noexcept;
    };
    ALIAS_2D_AND_3D( Polygon );
} // namespace geode
