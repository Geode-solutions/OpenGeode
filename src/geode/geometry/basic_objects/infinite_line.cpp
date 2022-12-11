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

#include <geode/geometry/basic_objects/infinite_line.h>

#include <geode/geometry/basic_objects/segment.h>

namespace geode
{
    template < typename PointType, index_t dimension >
    GenericLine< PointType, dimension >::GenericLine(
        const Vector< dimension >& direction, const Point< dimension >& origin )
        : origin_( origin ), direction_( direction.normalize() )
    {
    }
    template < typename PointType, index_t dimension >
    GenericLine< PointType, dimension >::GenericLine(
        const GenericSegment< PointType, dimension >& segment )
        : GenericLine( segment.normalized_direction(), segment.vertices()[0] )
    {
    }
    template < typename PointType, index_t dimension >
    GenericLine< PointType, dimension >::GenericLine(
        const GenericLine< PointType, dimension >& other )
        : origin_( other.origin_ ), direction_( other.direction_ )
    {
    }
    template < typename PointType, index_t dimension >
    GenericLine< PointType, dimension >&
        GenericLine< PointType, dimension >::operator=(
            const GenericLine< PointType, dimension >& other )
    {
        origin_ = other.origin_;
        direction_ = other.direction_;
        return *this;
    }
    template < typename PointType, index_t dimension >
    GenericLine< PointType, dimension >::GenericLine(
        GenericLine< PointType, dimension >&& other )
        : origin_( std::move( other.origin_ ) ),
          direction_( std::move( other.direction_ ) )
    {
    }
    template < typename PointType, index_t dimension >
    GenericLine< PointType, dimension >&
        GenericLine< PointType, dimension >::operator=(
            GenericLine< PointType, dimension >&& other )
    {
        origin_ = std::move( other.origin_ );
        direction_ = std::move( other.direction_ );
        return *this;
    }
    template < typename PointType, index_t dimension >
    const Point< dimension >&
        GenericLine< PointType, dimension >::origin() const
    {
        return origin_;
    }
    template < typename PointType, index_t dimension >
    const Vector< dimension >&
        GenericLine< PointType, dimension >::direction() const
    {
        return direction_;
    }
    template < typename PointType, index_t dimension >
    GenericLine< PointType, dimension >::GenericLine(
        const OwnerInfiniteLine< dimension >& other )
        : origin_( other.origin() ), direction_( other.direction() )
    {
    }
    template < typename PointType, index_t dimension >
    GenericLine< PointType, dimension >::GenericLine(
        const OwnerRay< dimension >& other )
        : origin_( other.origin() ), direction_( other.direction() )
    {
    }

    template < index_t dimension >
    OwnerInfiniteLine< dimension >::OwnerInfiniteLine(
        const Vector< dimension >& direction, const Point< dimension >& origin )
        : Base( direction, origin )
    {
    }
    template < index_t dimension >
    OwnerInfiniteLine< dimension >::OwnerInfiniteLine(
        const Segment< dimension >& segment )
        : OwnerInfiniteLine(
            segment.normalized_direction(), segment.vertices()[0] )
    {
    }
    template < index_t dimension >
    OwnerInfiniteLine< dimension >::OwnerInfiniteLine(
        const OwnerInfiniteLine< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    OwnerInfiniteLine< dimension >& OwnerInfiniteLine< dimension >::operator=(
        const OwnerInfiniteLine< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    OwnerInfiniteLine< dimension >::OwnerInfiniteLine(
        OwnerInfiniteLine< dimension >&& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    OwnerInfiniteLine< dimension >& OwnerInfiniteLine< dimension >::operator=(
        OwnerInfiniteLine< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < index_t dimension >
    InfiniteLine< dimension >::InfiniteLine(
        const Vector< dimension >& direction, const Point< dimension >& origin )
        : Base( direction, origin )
    {
    }
    template < index_t dimension >
    InfiniteLine< dimension >::InfiniteLine(
        const Segment< dimension >& segment )
        : InfiniteLine( segment.normalized_direction(), segment.vertices()[0] )
    {
    }
    template < index_t dimension >
    InfiniteLine< dimension >::InfiniteLine(
        const InfiniteLine< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    InfiniteLine< dimension >::InfiniteLine(
        const OwnerInfiniteLine< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    InfiniteLine< dimension >& InfiniteLine< dimension >::operator=(
        const InfiniteLine< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    InfiniteLine< dimension >::InfiniteLine( InfiniteLine< dimension >&& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    InfiniteLine< dimension >& InfiniteLine< dimension >::operator=(
        InfiniteLine< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < index_t dimension >
    OwnerRay< dimension >::OwnerRay(
        const Vector< dimension >& direction, const Point< dimension >& origin )
        : Base( direction, origin )
    {
    }
    template < index_t dimension >
    OwnerRay< dimension >::OwnerRay( const Segment< dimension >& segment )
        : OwnerRay( segment.normalized_direction(), segment.vertices()[0] )
    {
    }
    template < index_t dimension >
    OwnerRay< dimension >::OwnerRay( const OwnerRay< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    OwnerRay< dimension >& OwnerRay< dimension >::operator=(
        const OwnerRay< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    OwnerRay< dimension >::OwnerRay( OwnerRay< dimension >&& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    OwnerRay< dimension >& OwnerRay< dimension >::operator=(
        OwnerRay< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < index_t dimension >
    Ray< dimension >::Ray(
        const Vector< dimension >& direction, const Point< dimension >& origin )
        : Base( direction, origin )
    {
    }
    template < index_t dimension >
    Ray< dimension >::Ray( const Segment< dimension >& segment )
        : Ray( segment.normalized_direction(), segment.vertices()[0] )
    {
    }
    template < index_t dimension >
    Ray< dimension >::Ray( const Ray< dimension >& other ) : Base( other )
    {
    }
    template < index_t dimension >
    Ray< dimension >::Ray( const OwnerRay< dimension >& other ) : Base( other )
    {
    }
    template < index_t dimension >
    Ray< dimension >& Ray< dimension >::operator=(
        const Ray< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    Ray< dimension >::Ray( Ray< dimension >&& other ) : Base( other )
    {
    }
    template < index_t dimension >
    Ray< dimension >& Ray< dimension >::operator=( Ray< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template class opengeode_geometry_api GenericLine< Point< 1 >, 1 >;
    template class opengeode_geometry_api GenericLine< Point< 2 >, 2 >;
    template class opengeode_geometry_api GenericLine< Point< 3 >, 3 >;
    template class opengeode_geometry_api GenericLine< RefPoint< 1 >, 1 >;
    template class opengeode_geometry_api GenericLine< RefPoint< 2 >, 2 >;
    template class opengeode_geometry_api GenericLine< RefPoint< 3 >, 3 >;
    template class opengeode_geometry_api OwnerInfiniteLine< 1 >;
    template class opengeode_geometry_api OwnerInfiniteLine< 2 >;
    template class opengeode_geometry_api OwnerInfiniteLine< 3 >;
    template class opengeode_geometry_api InfiniteLine< 1 >;
    template class opengeode_geometry_api InfiniteLine< 2 >;
    template class opengeode_geometry_api InfiniteLine< 3 >;
    template class opengeode_geometry_api OwnerRay< 1 >;
    template class opengeode_geometry_api OwnerRay< 2 >;
    template class opengeode_geometry_api OwnerRay< 3 >;
    template class opengeode_geometry_api Ray< 1 >;
    template class opengeode_geometry_api Ray< 2 >;
    template class opengeode_geometry_api Ray< 3 >;
} // namespace geode
