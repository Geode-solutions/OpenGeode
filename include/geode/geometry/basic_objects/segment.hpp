/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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
        GenericSegment( PointType point0, PointType point1 ) noexcept;

        GenericSegment(
            const GenericSegment< PointType, dimension >& other ) noexcept;
        GenericSegment< PointType, dimension >& operator=(
            const GenericSegment< PointType, dimension >& other ) noexcept;
        GenericSegment(
            GenericSegment< PointType, dimension >&& other ) noexcept;
        GenericSegment< PointType, dimension >& operator=(
            GenericSegment< PointType, dimension >&& other ) noexcept;

        [[nodiscard]] Vector< dimension > direction() const;
        [[nodiscard]] Vector< dimension > normalized_direction() const;
        [[nodiscard]] Point< dimension > barycenter() const;
        [[nodiscard]] double length() const;
        void set_point( local_index_t vertex, PointType point );
        [[nodiscard]] const std::array< PointType, 2 >& vertices() const;
        [[nodiscard]] BoundingBox< dimension > bounding_box() const;
        [[nodiscard]] bool is_degenerated() const;
        [[nodiscard]] std::string string() const;

    private:
        std::array< PointType, 2 > vertices_;
    };

    template < index_t dimension >
    class OwnerSegment : public GenericSegment< Point< dimension >, dimension >
    {
        using Base = GenericSegment< Point< dimension >, dimension >;

    public:
        explicit OwnerSegment(
            Point< dimension > point0, Point< dimension > point1 ) noexcept;

        OwnerSegment( const OwnerSegment< dimension >& other ) noexcept;
        OwnerSegment< dimension >& operator=(
            const OwnerSegment< dimension >& other ) noexcept;
        OwnerSegment( OwnerSegment< dimension >&& other ) noexcept;
        OwnerSegment< dimension >& operator=(
            OwnerSegment< dimension >&& other ) noexcept;
    };
    ALIAS_1D_AND_2D_AND_3D( OwnerSegment );

    template < index_t dimension >
    class Segment : public GenericSegment< RefPoint< dimension >, dimension >
    {
        using Base = GenericSegment< RefPoint< dimension >, dimension >;

    public:
        Segment( const Point< dimension >& point0,
            const Point< dimension >& point1 ) noexcept;

        Segment( const Segment< dimension >& other ) noexcept;
        Segment( const OwnerSegment< dimension >& other ) noexcept;
        Segment< dimension >& operator=(
            const Segment< dimension >& other ) noexcept;
        Segment( Segment< dimension >&& other ) noexcept;
        Segment< dimension >& operator=(
            Segment< dimension >&& other ) noexcept;
    };
    ALIAS_1D_AND_2D_AND_3D( Segment );

    template < index_t dimension >
    class OpenGeodeSegmentException : public OpenGeodeException
    {
    public:
        template < typename... Args >
        explicit OpenGeodeSegmentException(
            Segment< dimension > segment_in, const Args&... message )
            : OpenGeodeException{ absl::StrCat(
                  message..., " at ", segment_in.string() ) },
              segment{ std::move( segment_in ) }
        {
        }

        OwnerSegment< dimension > segment;
    };
    ALIAS_1D_AND_2D_AND_3D( OpenGeodeSegmentException );
} // namespace geode

// NOLINTNEXTLINE
#define OPENGEODE_SEGMENT_EXCEPTION( dimension, condition, segment, ... )      \
    if( ABSL_PREDICT_FALSE( !( condition ) ) )                                 \
        throw geode::OpenGeodeSegmentException< dimension >                    \
        {                                                                      \
            segment, __VA_ARGS__                                               \
        }

// NOLINTNEXTLINE
#define OPENGEODE_SEGMENT1D_EXCEPTION( condition, segment, ... )               \
    OPENGEODE_SEGMENT_EXCEPTION( 1, condition, segment, __VA_ARGS__ )

// NOLINTNEXTLINE
#define OPENGEODE_SEGMENT2D_EXCEPTION( condition, segment, ... )               \
    OPENGEODE_SEGMENT_EXCEPTION( 2, condition, segment, __VA_ARGS__ )

// NOLINTNEXTLINE
#define OPENGEODE_SEGMENT3D_EXCEPTION( condition, segment, ... )               \
    OPENGEODE_SEGMENT_EXCEPTION( 3, condition, segment, __VA_ARGS__ )
