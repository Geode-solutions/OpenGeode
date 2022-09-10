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

#include <geode/geometry/basic_objects/segment.h>

#include <geode/geometry/distance.h>

namespace geode
{
    template < typename PointType, index_t dimension >
    GenericSegment< PointType, dimension >::GenericSegment(
        const Point< dimension >& p0, const Point< dimension >& p1 )
        : vertices_{ { { p0 }, { p1 } } }
    {
    }
    template < typename PointType, index_t dimension >
    GenericSegment< PointType, dimension >::GenericSegment(
        const GenericSegment< PointType, dimension >& other )
        : vertices_( other.vertices_ )
    {
    }
    template < typename PointType, index_t dimension >
    GenericSegment< PointType, dimension >&
        GenericSegment< PointType, dimension >::operator=(
            const GenericSegment< PointType, dimension >& other )
    {
        vertices_ = other.vertices_;
        return *this;
    }
    template < typename PointType, index_t dimension >
    GenericSegment< PointType, dimension >::GenericSegment(
        GenericSegment< PointType, dimension >&& other )
        : vertices_( std::move( other.vertices_ ) )
    {
    }
    template < typename PointType, index_t dimension >
    GenericSegment< PointType, dimension >&
        GenericSegment< PointType, dimension >::operator=(
            GenericSegment< PointType, dimension >&& other )
    {
        vertices_ = std::move( other.vertices_ );
        return *this;
    }
    template < typename PointType, index_t dimension >
    Vector< dimension >
        GenericSegment< PointType, dimension >::direction() const
    {
        Vector< dimension > direction{ vertices_[0], vertices_[1] };
        OPENGEODE_EXCEPTION( direction.length() > global_epsilon,
            "[Segment::direction] Segment length too small" );
        return direction;
    }
    template < typename PointType, index_t dimension >
    Vector< dimension >
        GenericSegment< PointType, dimension >::normalized_direction() const
    {
        return direction().normalize();
    }
    template < typename PointType, index_t dimension >
    Point< dimension >
        GenericSegment< PointType, dimension >::barycenter() const
    {
        const Point< dimension >& p0 = vertices_[0];
        const Point< dimension >& p1 = vertices_[1];
        return ( p0 + p1 ) / 2.;
    }
    template < typename PointType, index_t dimension >
    double GenericSegment< PointType, dimension >::length() const
    {
        const Point< dimension >& p0 = vertices_[0];
        const Point< dimension >& p1 = vertices_[1];
        return point_point_distance( p0, p1 );
    }
    template < typename PointType, index_t dimension >
    void GenericSegment< PointType, dimension >::set_point(
        index_t vertex, const Point< dimension >& point )
    {
        vertices_.at( vertex ) = point;
    }
    template < typename PointType, index_t dimension >
    const std::array< PointType, 2 >&
        GenericSegment< PointType, dimension >::vertices() const
    {
        return vertices_;
    }

    template < typename PointType, index_t dimension >
    GenericSegment< PointType, dimension >::GenericSegment(
        const OwnerSegment< dimension >& other )
        : vertices_{ { { other.vertices()[0] }, { other.vertices()[1] } } }
    {
    }

    template < index_t dimension >
    OwnerSegment< dimension >::OwnerSegment(
        const Point< dimension >& p0, const Point< dimension >& p1 )
        : Base( p0, p1 )
    {
    }
    template < index_t dimension >
    OwnerSegment< dimension >::OwnerSegment(
        const OwnerSegment< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    OwnerSegment< dimension >& OwnerSegment< dimension >::operator=(
        const OwnerSegment< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    OwnerSegment< dimension >::OwnerSegment( OwnerSegment< dimension >&& other )
        : Base( std::move( other ) )
    {
    }
    template < index_t dimension >
    OwnerSegment< dimension >& OwnerSegment< dimension >::operator=(
        OwnerSegment< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < index_t dimension >
    Segment< dimension >::Segment(
        const Point< dimension >& p0, const Point< dimension >& p1 )
        : Base( p0, p1 )
    {
    }
    template < index_t dimension >
    Segment< dimension >::Segment( const Segment< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    Segment< dimension >::Segment( const OwnerSegment< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    Segment< dimension >& Segment< dimension >::operator=(
        const Segment< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    Segment< dimension >::Segment( Segment< dimension >&& other )
        : Base( std::move( other ) )
    {
    }
    template < index_t dimension >
    Segment< dimension >& Segment< dimension >::operator=(
        Segment< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template class opengeode_geometry_api GenericSegment< Point< 1 >, 1 >;
    template class opengeode_geometry_api GenericSegment< Point< 2 >, 2 >;
    template class opengeode_geometry_api GenericSegment< Point< 3 >, 3 >;
    template class opengeode_geometry_api GenericSegment< RefPoint< 1 >, 1 >;
    template class opengeode_geometry_api GenericSegment< RefPoint< 2 >, 2 >;
    template class opengeode_geometry_api GenericSegment< RefPoint< 3 >, 3 >;
    template class opengeode_geometry_api OwnerSegment< 1 >;
    template class opengeode_geometry_api OwnerSegment< 2 >;
    template class opengeode_geometry_api OwnerSegment< 3 >;
    template class opengeode_geometry_api Segment< 1 >;
    template class opengeode_geometry_api Segment< 2 >;
    template class opengeode_geometry_api Segment< 3 >;
} // namespace geode
