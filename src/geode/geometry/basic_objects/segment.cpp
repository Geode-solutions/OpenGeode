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

#include <geode/geometry/basic_objects/segment.hpp>

#include <geode/basic/logger.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/distance.hpp>

namespace geode
{
    template < typename PointType, index_t dimension >
    GenericSegment< PointType, dimension >::GenericSegment(
        PointType point0, PointType point1 ) noexcept
        : vertices_{ { point0, point1 } }
    {
    }
    template < typename PointType, index_t dimension >
    GenericSegment< PointType, dimension >::GenericSegment(
        const GenericSegment< PointType, dimension >& ) noexcept = default;
    template < typename PointType, index_t dimension >
    GenericSegment< PointType, dimension >&
        GenericSegment< PointType, dimension >::operator=(
            const GenericSegment< PointType, dimension >& ) noexcept = default;
    template < typename PointType, index_t dimension >
    GenericSegment< PointType, dimension >::GenericSegment(
        GenericSegment< PointType, dimension >&& ) noexcept = default;
    template < typename PointType, index_t dimension >
    GenericSegment< PointType, dimension >&
        GenericSegment< PointType, dimension >::operator=(
            GenericSegment< PointType, dimension >&& ) noexcept = default;
    template < typename PointType, index_t dimension >
    Vector< dimension >
        GenericSegment< PointType, dimension >::direction() const
    {
        Vector< dimension > direction{ vertices_[0], vertices_[1] };
        OPENGEODE_EXCEPTION( direction.length() > GLOBAL_EPSILON,
            "[Segment::direction] Segment length too small (",
            direction.length(), ")" );
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
        const Point< dimension >& point0 = vertices_[0];
        const Point< dimension >& point1 = vertices_[1];
        return ( point0 + point1 ) / 2.;
    }
    template < typename PointType, index_t dimension >
    double GenericSegment< PointType, dimension >::length() const
    {
        const Point< dimension >& point0 = vertices_[0];
        const Point< dimension >& point1 = vertices_[1];
        return point_point_distance( point0, point1 );
    }
    template < typename PointType, index_t dimension >
    void GenericSegment< PointType, dimension >::set_point(
        local_index_t vertex, PointType point )
    {
        vertices_[vertex] = point;
    }
    template < typename PointType, index_t dimension >
    const std::array< PointType, 2 >&
        GenericSegment< PointType, dimension >::vertices() const
    {
        return vertices_;
    }
    template < typename PointType, index_t dimension >
    BoundingBox< dimension >
        GenericSegment< PointType, dimension >::bounding_box() const
    {
        BoundingBox< dimension > bbox;
        for( const auto& point : vertices_ )
        {
            bbox.add_point( point );
        }
        return bbox;
    }

    template < typename PointType, index_t dimension >
    bool GenericSegment< PointType, dimension >::is_degenerated() const
    {
        return length() <= GLOBAL_EPSILON;
    }

    template < typename PointType, index_t dimension >
    std::string GenericSegment< PointType, dimension >::string() const
    {
        const Point< dimension >& point0 = vertices_[0];
        const Point< dimension >& point1 = vertices_[1];
        return absl::StrCat( "[", point0.string(), ", ", point1.string(), "]" );
    }

    template < index_t dimension >
    OwnerSegment< dimension >::OwnerSegment(
        Point< dimension > point0, Point< dimension > point1 ) noexcept
        : Base( point0, point1 )
    {
    }
    template < index_t dimension >
    OwnerSegment< dimension >::OwnerSegment(
        const OwnerSegment< dimension >& ) noexcept = default;
    template < index_t dimension >
    OwnerSegment< dimension >& OwnerSegment< dimension >::operator=(
        const OwnerSegment< dimension >& ) noexcept = default;
    template < index_t dimension >
    OwnerSegment< dimension >::OwnerSegment(
        OwnerSegment< dimension >&& ) noexcept = default;
    template < index_t dimension >
    OwnerSegment< dimension >& OwnerSegment< dimension >::operator=(
        OwnerSegment< dimension >&& ) noexcept = default;

    template < index_t dimension >
    Segment< dimension >::Segment( const Point< dimension >& point0,
        const Point< dimension >& point1 ) noexcept
        : Base( point0, point1 )
    {
    }
    template < index_t dimension >
    Segment< dimension >::Segment(
        const Segment< dimension >& ) noexcept = default;
    template < index_t dimension >
    Segment< dimension >::Segment(
        const OwnerSegment< dimension >& other ) noexcept
        : Base( other.vertices()[0], other.vertices()[1] )
    {
    }
    template < index_t dimension >
    Segment< dimension >& Segment< dimension >::operator=(
        const Segment< dimension >& ) noexcept = default;
    template < index_t dimension >
    Segment< dimension >::Segment( Segment< dimension >&& ) noexcept = default;
    template < index_t dimension >
    Segment< dimension >& Segment< dimension >::operator=(
        Segment< dimension >&& ) noexcept = default;

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
