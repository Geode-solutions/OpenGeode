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

#pragma once

#include <geode/geometry/common.h>
#include <geode/geometry/vector.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerSegment );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
} // namespace geode

namespace geode
{
    template < typename PointType, index_t dimension >
    class GenericSegment
    {
    public:
        GenericSegment( PointType p0, PointType p1 );

        GenericSegment( const GenericSegment< PointType, dimension >& other );
        GenericSegment< PointType, dimension >& operator=(
            const GenericSegment< PointType, dimension >& other );
        GenericSegment( GenericSegment< PointType, dimension >&& other );
        GenericSegment< PointType, dimension >& operator=(
            GenericSegment< PointType, dimension >&& other );

        Vector< dimension > direction() const;
        Vector< dimension > normalized_direction() const;
        Point< dimension > barycenter() const;
        double length() const;
        void set_point( index_t vertex, PointType point );
        const std::array< PointType, 2 >& vertices() const;
        BoundingBox< dimension > bounding_box() const;

    private:
        std::array< PointType, 2 > vertices_;
    };

    template < index_t dimension >
    class OwnerSegment : public GenericSegment< Point< dimension >, dimension >
    {
        using Base = GenericSegment< Point< dimension >, dimension >;

    public:
        explicit OwnerSegment( Point< dimension > p0, Point< dimension > p1 );

        OwnerSegment( const OwnerSegment< dimension >& other );
        OwnerSegment< dimension >& operator=(
            const OwnerSegment< dimension >& other );
        OwnerSegment( OwnerSegment< dimension >&& other );
        OwnerSegment< dimension >& operator=(
            OwnerSegment< dimension >&& other );
    };
    ALIAS_1D_AND_2D_AND_3D( OwnerSegment );

    template < index_t dimension >
    class Segment : public GenericSegment< RefPoint< dimension >, dimension >
    {
        using Base = GenericSegment< RefPoint< dimension >, dimension >;

    public:
        Segment( const Point< dimension >& p0, const Point< dimension >& p1 );

        Segment( const Segment< dimension >& other );
        Segment( const OwnerSegment< dimension >& other );
        Segment< dimension >& operator=( const Segment< dimension >& other );
        Segment( Segment< dimension >&& other );
        Segment< dimension >& operator=( Segment< dimension >&& other );
    };
    ALIAS_1D_AND_2D_AND_3D( Segment );
} // namespace geode
