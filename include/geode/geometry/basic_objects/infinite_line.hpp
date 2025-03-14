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

#include <geode/geometry/common.hpp>
#include <geode/geometry/vector.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerInfiniteLine );
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerRay );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Segment );

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
} // namespace geode

namespace geode
{
    template < typename PointType, index_t dimension >
    class GenericLine
    {
    public:
        GenericLine( const Vector< dimension >& direction, PointType origin );
        explicit GenericLine( const Segment< dimension >& segment );
        GenericLine( const GenericLine< PointType, dimension >& other );
        GenericLine< PointType, dimension >& operator=(
            const GenericLine< PointType, dimension >& other );
        GenericLine( GenericLine< PointType, dimension >&& other ) noexcept;
        GenericLine< PointType, dimension >& operator=(
            GenericLine< PointType, dimension >&& other ) noexcept;

        [[nodiscard]] const Point< dimension >& origin() const;
        [[nodiscard]] const Vector< dimension >& direction() const;
        template < index_t T = dimension >
        [[nodiscard]] typename std::enable_if< T == 2, double >::type
            line_constant() const;

    private:
        Vector< dimension > direction_;
        PointType origin_;
    };

    template < index_t dimension >
    class OwnerInfiniteLine
        : public GenericLine< Point< dimension >, dimension >
    {
        using Base = GenericLine< Point< dimension >, dimension >;

    public:
        explicit OwnerInfiniteLine(
            const Vector< dimension >& direction, Point< dimension > origin );

        explicit OwnerInfiniteLine( const Segment< dimension >& segment );
        OwnerInfiniteLine( const OwnerInfiniteLine< dimension >& other );
        OwnerInfiniteLine< dimension >& operator=(
            const OwnerInfiniteLine< dimension >& other );
        OwnerInfiniteLine( OwnerInfiniteLine< dimension >&& other ) noexcept;
        OwnerInfiniteLine< dimension >& operator=(
            OwnerInfiniteLine< dimension >&& other ) noexcept;
    };
    ALIAS_1D_AND_2D_AND_3D( OwnerInfiniteLine );

    template < index_t dimension >
    class OwnerRay : public GenericLine< Point< dimension >, dimension >
    {
        using Base = GenericLine< Point< dimension >, dimension >;

    public:
        explicit OwnerRay(
            const Vector< dimension >& direction, Point< dimension > origin );

        explicit OwnerRay( const Segment< dimension >& segment );
        OwnerRay( const OwnerRay< dimension >& other );
        OwnerRay< dimension >& operator=( const OwnerRay< dimension >& other );
        OwnerRay( OwnerRay< dimension >&& other ) noexcept;
        OwnerRay< dimension >& operator=(
            OwnerRay< dimension >&& other ) noexcept;
    };
    ALIAS_1D_AND_2D_AND_3D( OwnerRay );

    template < index_t dimension >
    class InfiniteLine : public GenericLine< RefPoint< dimension >, dimension >
    {
        using Base = GenericLine< RefPoint< dimension >, dimension >;

    public:
        InfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin );
        explicit InfiniteLine( const Segment< dimension >& segment );

        InfiniteLine( const InfiniteLine< dimension >& other );
        InfiniteLine( const OwnerInfiniteLine< dimension >& other );
        InfiniteLine< dimension >& operator=(
            const InfiniteLine< dimension >& other );
        InfiniteLine( InfiniteLine< dimension >&& other ) noexcept;
        InfiniteLine< dimension >& operator=(
            InfiniteLine< dimension >&& other ) noexcept;
    };
    ALIAS_1D_AND_2D_AND_3D( InfiniteLine );

    template < index_t dimension >
    class Ray : public GenericLine< RefPoint< dimension >, dimension >
    {
        using Base = GenericLine< RefPoint< dimension >, dimension >;

    public:
        Ray( const Vector< dimension >& direction,
            const Point< dimension >& origin );
        Ray( const Segment< dimension >& segment );

        Ray( const Ray< dimension >& other );
        Ray( const OwnerRay< dimension >& other );
        Ray< dimension >& operator=( const Ray< dimension >& other );
        Ray( Ray< dimension >&& other ) noexcept;
        Ray< dimension >& operator=( Ray< dimension >&& other ) noexcept;
    };
    ALIAS_1D_AND_2D_AND_3D( Ray );
} // namespace geode
