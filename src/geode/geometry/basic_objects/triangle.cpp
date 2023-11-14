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

#include <geode/geometry/basic_objects/triangle.h>

#include <geode/basic/logger.h>

#include <absl/algorithm/container.h>

#include <geode/geometry/barycentric_coordinates.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/bounding_box.h>
#include <geode/geometry/distance.h>

namespace
{
    struct PivotNormalResult
    {
        geode::local_index_t pivot{ geode::NO_LID };
        geode::Vector3D normal{ { 0, 0, 0 } };
        std::array< double, 3 > lengths;
    };

    absl::optional< PivotNormalResult > simple_pivot_and_normal(
        const std::array< geode::RefPoint3D, 3 >& points )
    {
        try
        {
            PivotNormalResult result;
            for( const auto pivot : geode::LRange{ 3 } )
            {
                const auto next = pivot + 1 == 3 ? 0 : pivot + 1;
                const geode::Vector3D edge{ points[pivot], points[next] };
                result.lengths[pivot] = edge.length();
                const auto edge0 = edge / result.lengths[pivot];
                const auto prev = pivot == 0 ? 2 : pivot - 1;
                const auto edge1 =
                    geode::Vector3D{ points[pivot], points[prev] }.normalize();

                const auto normal = edge0.cross( edge1 );
                const auto length = normal.length();
                if( length > geode::global_angular_epsilon )
                {
                    result.pivot = pivot;
                    result.normal = normal / length;
                    return result;
                }
            }
            return result;
        }
        catch( const geode::OpenGeodeException& /*unused*/ )
        {
            return absl::nullopt;
        }
    }

    bool check_right( const PivotNormalResult& result_left,
        const geode::Point3D& new_point,
        const geode::Point3D& point1,
        const geode::Point3D& point2 )
    {
        const auto result_right =
            simple_pivot_and_normal( { new_point, point1, point2 } );
        if( !result_right || result_right->pivot == geode::NO_LID )
        {
            return false;
        }
        return result_left.normal.cross( result_right->normal ).length()
               < geode::global_angular_epsilon;
    }
} // namespace

namespace geode
{
    template < typename PointType, index_t dimension >
    GenericTriangle< PointType, dimension >::GenericTriangle(
        PointType point0, PointType point1, PointType point2 ) noexcept
        : vertices_{ { point0, point1, point2 } }
    {
    }

    template < typename PointType, index_t dimension >
    GenericTriangle< PointType, dimension >::GenericTriangle(
        const GenericTriangle< PointType, dimension >& ) noexcept = default;

    template < typename PointType, index_t dimension >
    GenericTriangle< PointType, dimension >&
        GenericTriangle< PointType, dimension >::operator=(
            const GenericTriangle< PointType, dimension >& ) noexcept = default;

    template < typename PointType, index_t dimension >
    GenericTriangle< PointType, dimension >::GenericTriangle(
        GenericTriangle< PointType, dimension >&& ) noexcept = default;

    template < typename PointType, index_t dimension >
    GenericTriangle< PointType, dimension >&
        GenericTriangle< PointType, dimension >::operator=(
            GenericTriangle< PointType, dimension >&& ) noexcept = default;

    template < typename PointType, index_t dimension >
    Point< dimension >
        GenericTriangle< PointType, dimension >::barycenter() const
    {
        const Point< dimension >& point0 = vertices_[0];
        const Point< dimension >& point1 = vertices_[1];
        const Point< dimension >& point2 = vertices_[2];
        return ( point0 + point1 + point2 ) / 3.;
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, absl::optional< Vector3D > >::type
        GenericTriangle< PointType, dimension >::normal() const
    {
        if( const auto result = pivot_and_normal() )
        {
            return result->second;
        }
        return absl::nullopt;
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, absl::optional< Plane > >::type
        GenericTriangle< PointType, dimension >::plane() const
    {
        if( const auto triangle_normal = this->normal() )
        {
            return absl::optional< Plane >{ absl::in_place,
                triangle_normal.value(), vertices_[0] };
        }
        return absl::nullopt;
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, absl::optional< OwnerPlane > >::type
        GenericTriangle< PointType, dimension >::owner_plane() const
    {
        if( const auto triangle_normal = this->normal() )
        {
            return absl::optional< OwnerPlane >{ absl::in_place,
                triangle_normal.value(), vertices_[0] };
        }
        return absl::nullopt;
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, absl::optional< local_index_t > >::type
        GenericTriangle< PointType, dimension >::pivot() const
    {
        if( const auto result = pivot_and_normal() )
        {
            return result->first;
        }
        return absl::nullopt;
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3,
        absl::optional< std::pair< local_index_t, Vector3D > > >::type
        GenericTriangle< PointType, dimension >::pivot_and_normal() const
    {
        const auto result = simple_pivot_and_normal(
            { vertices_[0], vertices_[1], vertices_[2] } );
        if( !result )
        {
            return absl::nullopt;
        }
        if( result->pivot != NO_LID )
        {
            return std::make_pair( result->pivot, result->normal );
        }
        const auto max = absl::c_max_element( result->lengths );
        const local_index_t longest_e =
            std::distance( result->lengths.begin(), max );
        const Point3D& point0 = vertices_[longest_e];
        const auto e1 = longest_e == 2 ? 0 : longest_e + 1;
        const Point3D& point1 = vertices_[e1];
        const auto e2 = e1 == 2 ? 0 : e1 + 1;
        const Point3D& point2 = vertices_[e2];
        if( point_segment_distance( point2, { point0, point1 } )
            > global_epsilon )
        {
            const auto ratio = result->lengths[e2]
                               / ( result->lengths[e2] + result->lengths[e1] );
            const auto new_point = point0 * ( 1. - ratio ) + point1 * ratio;
            const auto result_left =
                simple_pivot_and_normal( { point0, new_point, point2 } );
            if( !result_left || result_left->pivot == NO_LID )
            {
                return absl::nullopt;
            }
            OPENGEODE_ASSERT(
                check_right( result_left.value(), new_point, point1, point2 ),
                "[Triangle::pivot_and_normal] Wrong sub-triangle computation" );
            return std::make_pair( e2, result_left->normal );
        }
        return absl::nullopt;
    }

    template < typename PointType, index_t dimension >
    void GenericTriangle< PointType, dimension >::set_point(
        index_t vertex, PointType point )
    {
        vertices_[vertex] = point;
    }

    template < typename PointType, index_t dimension >
    const std::array< PointType, 3 >&
        GenericTriangle< PointType, dimension >::vertices() const
    {
        return vertices_;
    }

    template < typename PointType, index_t dimension >
    BoundingBox< dimension >
        GenericTriangle< PointType, dimension >::bounding_box() const
    {
        BoundingBox< dimension > bbox;
        for( const auto& point : vertices_ )
        {
            bbox.add_point( point );
        }
        return bbox;
    }

    template < typename PointType, index_t dimension >
    local_index_t
        GenericTriangle< PointType, dimension >::longest_edge_index() const
    {
        local_index_t max_length_edge_id{ NO_LID };
        double edge_max_length{ 0 };
        for( const auto edge_id : LRange{ 3 } )
        {
            const auto next_vertex = edge_id == 2 ? 0 : edge_id + 1;
            const Point< dimension >& point0 = vertices_.at( edge_id );
            const Point< dimension >& point1 = vertices_.at( next_vertex );
            const auto edge_length = point_point_distance( point0, point1 );
            if( edge_length > edge_max_length )
            {
                max_length_edge_id = edge_id;
                edge_max_length = edge_length;
            }
        }
        return max_length_edge_id;
    }

    template < typename PointType, index_t dimension >
    double GenericTriangle< PointType, dimension >::minimum_height() const
    {
        const auto max_length_edge_id = longest_edge_index();
        const Point< dimension >& opposite_vertex = vertices_.at(
            max_length_edge_id == 0 ? 2 : max_length_edge_id - 1 );
        const Point< dimension >& next_vertex = vertices_.at(
            max_length_edge_id == 2 ? 0 : max_length_edge_id + 1 );
        const Segment< dimension > longest_edge{
            vertices_.at( max_length_edge_id ), next_vertex
        };
        return point_segment_distance( opposite_vertex, longest_edge );
    }

    template < index_t dimension >
    OwnerTriangle< dimension >::OwnerTriangle( Point< dimension > point0,
        Point< dimension > point1,
        Point< dimension > point2 ) noexcept
        : Base( point0, point1, point2 )
    {
    }
    template < index_t dimension >
    OwnerTriangle< dimension >::OwnerTriangle(
        const OwnerTriangle< dimension >& ) noexcept = default;
    template < index_t dimension >
    OwnerTriangle< dimension >& OwnerTriangle< dimension >::operator=(
        const OwnerTriangle< dimension >& ) noexcept = default;
    template < index_t dimension >
    OwnerTriangle< dimension >::OwnerTriangle(
        OwnerTriangle< dimension >&& ) noexcept = default;
    template < index_t dimension >
    OwnerTriangle< dimension >& OwnerTriangle< dimension >::operator=(
        OwnerTriangle< dimension >&& ) noexcept = default;

    template < index_t dimension >
    Triangle< dimension >::Triangle( const Point< dimension >& point0,
        const Point< dimension >& point1,
        const Point< dimension >& point2 ) noexcept
        : Base( point0, point1, point2 )
    {
    }
    template < index_t dimension >
    Triangle< dimension >::Triangle(
        const Triangle< dimension >& ) noexcept = default;
    template < index_t dimension >
    Triangle< dimension >::Triangle(
        const OwnerTriangle< dimension >& other ) noexcept
        : Base( other.vertices()[0], other.vertices()[1], other.vertices()[2] )
    {
    }
    template < index_t dimension >
    Triangle< dimension >& Triangle< dimension >::operator=(
        const Triangle< dimension >& ) noexcept = default;
    template < index_t dimension >
    Triangle< dimension >::Triangle(
        Triangle< dimension >&& ) noexcept = default;
    template < index_t dimension >
    Triangle< dimension >& Triangle< dimension >::operator=(
        Triangle< dimension >&& ) noexcept = default;

    template class opengeode_geometry_api GenericTriangle< Point< 2 >, 2 >;
    template class opengeode_geometry_api GenericTriangle< Point< 3 >, 3 >;
    template class opengeode_geometry_api GenericTriangle< RefPoint< 2 >, 2 >;
    template class opengeode_geometry_api GenericTriangle< RefPoint< 3 >, 3 >;
    template class opengeode_geometry_api OwnerTriangle< 2 >;
    template class opengeode_geometry_api OwnerTriangle< 3 >;
    template class opengeode_geometry_api Triangle< 2 >;
    template class opengeode_geometry_api Triangle< 3 >;

    template opengeode_geometry_api absl::optional< Vector3D >
        GenericTriangle< Point< 3 >, 3 >::normal< 3 >() const;
    template opengeode_geometry_api absl::optional< Plane >
        GenericTriangle< Point< 3 >, 3 >::plane< 3 >() const;
    template opengeode_geometry_api absl::optional< OwnerPlane >
        GenericTriangle< Point< 3 >, 3 >::owner_plane< 3 >() const;
    template opengeode_geometry_api absl::optional< local_index_t >
        GenericTriangle< Point< 3 >, 3 >::pivot< 3 >() const;
    template opengeode_geometry_api
        absl::optional< std::pair< local_index_t, Vector3D > >
        GenericTriangle< Point< 3 >, 3 >::pivot_and_normal< 3 >() const;

    template opengeode_geometry_api absl::optional< Vector3D >
        GenericTriangle< RefPoint< 3 >, 3 >::normal< 3 >() const;
    template opengeode_geometry_api absl::optional< Plane >
        GenericTriangle< RefPoint< 3 >, 3 >::plane< 3 >() const;
    template opengeode_geometry_api absl::optional< OwnerPlane >
        GenericTriangle< RefPoint< 3 >, 3 >::owner_plane< 3 >() const;
    template opengeode_geometry_api absl::optional< local_index_t >
        GenericTriangle< RefPoint< 3 >, 3 >::pivot< 3 >() const;
    template opengeode_geometry_api
        absl::optional< std::pair< local_index_t, Vector3D > >
        GenericTriangle< RefPoint< 3 >, 3 >::pivot_and_normal< 3 >() const;
} // namespace geode
