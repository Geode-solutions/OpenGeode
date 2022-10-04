/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geometry/common.h>
#include <geode/geometry/vector.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerInfiniteLine );
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerRay );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    template < typename PointType, index_t dimension >
    class GenericSegment;
    FORWARD_DECLARATION_DIMENSION_CLASS( Segment );
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerSegment );

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
} // namespace geode

namespace geode
{
    template < typename PointType, index_t dimension >
    class GenericInfiniteLine
    {
    public:
        GenericInfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin );

        GenericInfiniteLine(
            const GenericSegment< PointType, dimension >& segment );
        GenericInfiniteLine(
            const GenericInfiniteLine< PointType, dimension >& other );
        GenericInfiniteLine< PointType, dimension >& operator=(
            const GenericInfiniteLine< PointType, dimension >& other );
        GenericInfiniteLine(
            GenericInfiniteLine< PointType, dimension >&& other );
        GenericInfiniteLine< PointType, dimension >& operator=(
            GenericInfiniteLine< PointType, dimension >&& other );

        const Point< dimension >& origin() const;
        const Vector< dimension >& direction() const;

    protected:
        GenericInfiniteLine( const OwnerInfiniteLine< dimension >& other );
        GenericInfiniteLine( const OwnerRay< dimension >& other );

    private:
        PointType origin_;
        Vector< dimension > direction_;
    };

    template < index_t dimension >
    class OwnerInfiniteLine
        : public GenericInfiniteLine< Point< dimension >, dimension >
    {
        using Base = GenericInfiniteLine< Point< dimension >, dimension >;

    public:
        explicit OwnerInfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin );

        OwnerInfiniteLine( const Segment< dimension >& segment );
        OwnerInfiniteLine( const OwnerInfiniteLine< dimension >& other );
        OwnerInfiniteLine< dimension >& operator=(
            const OwnerInfiniteLine< dimension >& other );
        OwnerInfiniteLine( OwnerInfiniteLine< dimension >&& other );
        OwnerInfiniteLine< dimension >& operator=(
            OwnerInfiniteLine< dimension >&& other );
    };
    ALIAS_1D_AND_2D_AND_3D( OwnerInfiniteLine );

    template < index_t dimension >
    class OwnerRay : public GenericInfiniteLine< Point< dimension >, dimension >
    {
        using Base = GenericInfiniteLine< Point< dimension >, dimension >;

    public:
        explicit OwnerRay( const Vector< dimension >& direction,
            const Point< dimension >& origin );

        OwnerRay( const Segment< dimension >& segment );
        OwnerRay( const OwnerRay< dimension >& other );
        OwnerRay< dimension >& operator=( const OwnerRay< dimension >& other );
        OwnerRay( OwnerRay< dimension >&& other );
        OwnerRay< dimension >& operator=( OwnerRay< dimension >&& other );
    };
    ALIAS_1D_AND_2D_AND_3D( OwnerRay );

    template < index_t dimension >
    class InfiniteLine
        : public GenericInfiniteLine< RefPoint< dimension >, dimension >
    {
        using Base = GenericInfiniteLine< RefPoint< dimension >, dimension >;

    public:
        InfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin );
        InfiniteLine( const Segment< dimension >& segment );

        InfiniteLine( const InfiniteLine< dimension >& other );
        InfiniteLine( const OwnerInfiniteLine< dimension >& other );
        InfiniteLine< dimension >& operator=(
            const InfiniteLine< dimension >& other );
        InfiniteLine( InfiniteLine< dimension >&& other );
        InfiniteLine< dimension >& operator=(
            InfiniteLine< dimension >&& other );
    };
    ALIAS_1D_AND_2D_AND_3D( InfiniteLine );

    template < index_t dimension >
    class Ray : public GenericInfiniteLine< RefPoint< dimension >, dimension >
    {
        using Base = GenericInfiniteLine< RefPoint< dimension >, dimension >;

    public:
        Ray( const Vector< dimension >& direction,
            const Point< dimension >& origin );
        Ray( const Segment< dimension >& segment );

        Ray( const Ray< dimension >& other );
        Ray( const OwnerRay< dimension >& other );
        Ray< dimension >& operator=( const Ray< dimension >& other );
        Ray( Ray< dimension >&& other );
        Ray< dimension >& operator=( Ray< dimension >&& other );
    };
    ALIAS_1D_AND_2D_AND_3D( Ray );
} // namespace geode
