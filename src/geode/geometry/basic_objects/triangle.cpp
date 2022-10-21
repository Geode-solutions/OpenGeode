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

#include <geode/geometry/basic_objects/triangle.h>

#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/bounding_box.h>

namespace
{
    template < typename GenericTriangle >
    absl::optional< std::pair< geode::local_index_t, geode::Vector3D > > normal(
        const GenericTriangle& triangle )
    {
        try
        {
            for( const auto pivot : geode::LRange{ 3 } )
            {
                const auto next = pivot + 1 == 3 ? 0 : pivot + 1;
                const auto edge0 = geode::Vector3D{ triangle.vertices()[pivot],
                    triangle.vertices()[next] }
                                       .normalize();
                const auto prev = pivot == 0 ? 2 : pivot - 1;
                const auto edge1 = geode::Vector3D{ triangle.vertices()[pivot],
                    triangle.vertices()[prev] }
                                       .normalize();

                const auto normal = edge0.cross( edge1 );
                const auto length = normal.length();
                if( length > M_PI / 180 ) // 1 degree
                {
                    return std::make_pair( pivot, normal / length );
                }
            }
            return absl::nullopt;
        }
        catch( const geode::OpenGeodeException& /*unused*/ )
        {
            return absl::nullopt;
        }
    }
} // namespace

namespace geode
{
    template < typename PointType, index_t dimension >
    GenericTriangle< PointType, dimension >::GenericTriangle(
        const Point< dimension >& p0,
        const Point< dimension >& p1,
        const Point< dimension >& p2 )
        : vertices_{ { { p0 }, { p1 }, { p2 } } }
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
    typename std::enable_if< T == 3, Vector3D >::type
        GenericTriangle< PointType, dimension >::normal() const
    {
        return Vector3D{ vertices_[0], vertices_[1] }
            .cross( Vector3D{ vertices_[0], vertices_[2] } )
            .normalize();
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, Plane >::type
        GenericTriangle< PointType, dimension >::plane() const
    {
        return { this->normal(), vertices_[0] };
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, OwnerPlane >::type
        GenericTriangle< PointType, dimension >::owner_plane() const
    {
        return OwnerPlane{ this->normal(), vertices_[0] };
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, absl::optional< Vector3D > >::type
        GenericTriangle< PointType, dimension >::new_normal() const
    {
        if( auto normal = ::normal( *this ) )
        {
            return std::move( normal->second );
        }
        return absl::nullopt;
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, absl::optional< Plane > >::type
        GenericTriangle< PointType, dimension >::new_plane() const
    {
        if( const auto triangle_normal = this->new_normal() )
        {
            return absl::optional< Plane >{ absl::in_place,
                triangle_normal.value(), vertices_[0] };
        }
        return absl::nullopt;
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, absl::optional< OwnerPlane > >::type
        GenericTriangle< PointType, dimension >::new_owner_plane() const
    {
        if( const auto triangle_normal = this->new_normal() )
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
        if( auto normal = ::normal( *this ) )
        {
            return std::move( normal->first );
        }
        return absl::nullopt;
    }

    template < typename PointType, index_t dimension >
    void GenericTriangle< PointType, dimension >::set_point(
        index_t vertex, const Point< dimension >& point )
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
    GenericTriangle< PointType, dimension >::GenericTriangle(
        const OwnerTriangle< dimension >& other )
        : vertices_{ { { other.vertices()[0] }, { other.vertices()[1] },
            { other.vertices()[2] } } }
    {
    }

    template < index_t dimension >
    OwnerTriangle< dimension >::OwnerTriangle( const Point< dimension >& p0,
        const Point< dimension >& p1,
        const Point< dimension >& p2 )
        : Base( p0, p1, p2 )
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
        : Base( other )
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

    template opengeode_geometry_api Vector3D
        GenericTriangle< Point< 3 >, 3 >::normal< 3 >() const;
    template opengeode_geometry_api Plane
        GenericTriangle< Point< 3 >, 3 >::plane< 3 >() const;
    template opengeode_geometry_api OwnerPlane
        GenericTriangle< Point< 3 >, 3 >::owner_plane< 3 >() const;
    template opengeode_geometry_api absl::optional< Vector3D >
        GenericTriangle< Point< 3 >, 3 >::new_normal< 3 >() const;
    template opengeode_geometry_api absl::optional< Plane >
        GenericTriangle< Point< 3 >, 3 >::new_plane< 3 >() const;
    template opengeode_geometry_api absl::optional< OwnerPlane >
        GenericTriangle< Point< 3 >, 3 >::new_owner_plane< 3 >() const;
    template opengeode_geometry_api absl::optional< local_index_t >
        GenericTriangle< Point< 3 >, 3 >::pivot< 3 >() const;

    template opengeode_geometry_api Vector3D
        GenericTriangle< RefPoint< 3 >, 3 >::normal< 3 >() const;
    template opengeode_geometry_api Plane
        GenericTriangle< RefPoint< 3 >, 3 >::plane< 3 >() const;
    template opengeode_geometry_api OwnerPlane
        GenericTriangle< RefPoint< 3 >, 3 >::owner_plane< 3 >() const;
    template opengeode_geometry_api absl::optional< Vector3D >
        GenericTriangle< RefPoint< 3 >, 3 >::new_normal< 3 >() const;
    template opengeode_geometry_api absl::optional< Plane >
        GenericTriangle< RefPoint< 3 >, 3 >::new_plane< 3 >() const;
    template opengeode_geometry_api absl::optional< OwnerPlane >
        GenericTriangle< RefPoint< 3 >, 3 >::new_owner_plane< 3 >() const;
    template opengeode_geometry_api absl::optional< local_index_t >
        GenericTriangle< RefPoint< 3 >, 3 >::pivot< 3 >() const;
} // namespace geode
