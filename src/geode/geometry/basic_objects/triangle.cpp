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
        geode::Vector3D normal;
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
                const auto edge0 = edge.normalize();
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
        const geode::Point3D& p1,
        const geode::Point3D& p2 )
    {
        const auto result_right =
            simple_pivot_and_normal( { new_point, p1, p2 } );
        if( !result_right || result_right->pivot == geode::NO_LID )
        {
            return false;
        }
        return result_left.normal.cross( result_right->normal ).length()
               < geode::global_angular_epsilon;
    }

    absl::optional< std::pair< geode::local_index_t, geode::Vector3D > >
        pivot_and_normal( const std::array< geode::RefPoint3D, 3 >& points )
    {
        const auto result = simple_pivot_and_normal( points );
        if( !result )
        {
            return absl::nullopt;
        }
        if( result->pivot != geode::NO_LID )
        {
            return std::make_pair( result->pivot, result->normal );
        }
        const auto max = absl::c_max_element( result->lengths );
        const geode::local_index_t longest_e =
            std::distance( result->lengths.begin(), max );
        const auto& p0 = points[longest_e].get();
        const auto e1 = longest_e == 2 ? 0 : longest_e + 1;
        const auto& p1 = points[e1].get();
        const auto e2 = e1 == 2 ? 0 : e1 + 1;
        const auto& p2 = points[e2].get();
        if( geode::point_segment_distance( p2, { p0, p1 } )
            > geode::global_epsilon )
        {
            const auto ratio = result->lengths[e2]
                               / ( result->lengths[e2] + result->lengths[e1] );
            const auto new_point = p0 * ( 1. - ratio ) + p1 * ratio;
            const auto result_left =
                simple_pivot_and_normal( { p0, new_point, p2 } );
            if( !result_left || result_left->pivot == geode::NO_LID )
            {
                return absl::nullopt;
            }
            OPENGEODE_ASSERT(
                check_right( result_left.value(), new_point, p1, p2 ),
                "[Triangle::pivot_and_normal] Wrong sub-triangle computation" );
            return std::make_pair( e2, result_left->normal );
        }
        return absl::nullopt;
    }
} // namespace

namespace geode
{
    template < typename PointType, index_t dimension >
    GenericTriangle< PointType, dimension >::GenericTriangle(
        PointType p0, PointType p1, PointType p2 )
        : vertices_{ { std::move( p0 ), std::move( p1 ), std::move( p2 ) } }
    {
    }

    template < typename PointType, index_t dimension >
    GenericTriangle< PointType, dimension >::GenericTriangle(
        const GenericTriangle< PointType, dimension >& other )
        : vertices_( other.vertices_ )
    {
    }

    template < typename PointType, index_t dimension >
    GenericTriangle< PointType, dimension >&
        GenericTriangle< PointType, dimension >::operator=(
            const GenericTriangle< PointType, dimension >& other )
    {
        vertices_ = other.vertices_;
        return *this;
    }

    template < typename PointType, index_t dimension >
    GenericTriangle< PointType, dimension >::GenericTriangle(
        GenericTriangle< PointType, dimension >&& other )
        : vertices_( std::move( other.vertices_ ) )
    {
    }

    template < typename PointType, index_t dimension >
    GenericTriangle< PointType, dimension >&
        GenericTriangle< PointType, dimension >::operator=(
            GenericTriangle< PointType, dimension >&& other )
    {
        vertices_ = std::move( other.vertices_ );
        return *this;
    }

    template < typename PointType, index_t dimension >
    Point< dimension >
        GenericTriangle< PointType, dimension >::barycenter() const
    {
        const Point< dimension >& p0 = vertices_[0];
        const Point< dimension >& p1 = vertices_[1];
        const Point< dimension >& p2 = vertices_[2];
        return ( p0 + p1 + p2 ) / 3.;
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, absl::optional< Vector3D > >::type
        GenericTriangle< PointType, dimension >::normal() const
    {
        if( const auto result = pivot_and_normal(
                { vertices_[0], vertices_[1], vertices_[2] } ) )
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
        if( const auto result = pivot_and_normal(
                { vertices_[0], vertices_[1], vertices_[2] } ) )
        {
            return result->first;
        }
        return absl::nullopt;
    }

    template < typename PointType, index_t dimension >
    void GenericTriangle< PointType, dimension >::set_point(
        index_t vertex, PointType point )
    {
        vertices_[vertex] = std::move( point );
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

    template < index_t dimension >
    OwnerTriangle< dimension >::OwnerTriangle(
        Point< dimension > p0, Point< dimension > p1, Point< dimension > p2 )
        : Base( std::move( p0 ), std::move( p1 ), std::move( p2 ) )
    {
    }
    template < index_t dimension >
    OwnerTriangle< dimension >::OwnerTriangle(
        const OwnerTriangle< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    OwnerTriangle< dimension >& OwnerTriangle< dimension >::operator=(
        const OwnerTriangle< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    OwnerTriangle< dimension >::OwnerTriangle(
        OwnerTriangle< dimension >&& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    OwnerTriangle< dimension >& OwnerTriangle< dimension >::operator=(
        OwnerTriangle< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < index_t dimension >
    Triangle< dimension >::Triangle( const Point< dimension >& p0,
        const Point< dimension >& p1,
        const Point< dimension >& p2 )
        : Base( p0, p1, p2 )
    {
    }
    template < index_t dimension >
    Triangle< dimension >::Triangle( const Triangle< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    Triangle< dimension >::Triangle( const OwnerTriangle< dimension >& other )
        : Base( other.vertices()[0], other.vertices()[1], other.vertices()[2] )
    {
    }
    template < index_t dimension >
    Triangle< dimension >& Triangle< dimension >::operator=(
        const Triangle< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    Triangle< dimension >::Triangle( Triangle< dimension >&& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    Triangle< dimension >& Triangle< dimension >::operator=(
        Triangle< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

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

    template opengeode_geometry_api absl::optional< Vector3D >
        GenericTriangle< RefPoint< 3 >, 3 >::normal< 3 >() const;
    template opengeode_geometry_api absl::optional< Plane >
        GenericTriangle< RefPoint< 3 >, 3 >::plane< 3 >() const;
    template opengeode_geometry_api absl::optional< OwnerPlane >
        GenericTriangle< RefPoint< 3 >, 3 >::owner_plane< 3 >() const;
    template opengeode_geometry_api absl::optional< local_index_t >
        GenericTriangle< RefPoint< 3 >, 3 >::pivot< 3 >() const;
} // namespace geode
