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

#include <geode/geometry/basic_objects/circle.h>
#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/sphere.h>
#include <geode/geometry/basic_objects/tetra.h>
#include <geode/geometry/basic_objects/triangle.h>

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

    template < typename PointType, index_t dimension >
    GenericInfiniteLine< PointType, dimension >::GenericInfiniteLine(
        const Vector< dimension >& direction, const Point< dimension >& origin )
        : origin_( origin ), direction_( direction.normalize() )
    {
    }
    template < typename PointType, index_t dimension >
    GenericInfiniteLine< PointType, dimension >::GenericInfiniteLine(
        const GenericSegment< PointType, dimension >& segment )
        : GenericInfiniteLine(
            segment.normalized_direction(), segment.vertices()[0] )
    {
    }
    template < typename PointType, index_t dimension >
    GenericInfiniteLine< PointType, dimension >::GenericInfiniteLine(
        const GenericInfiniteLine< PointType, dimension >& other )
        : origin_( other.origin_ ), direction_( other.direction_ )
    {
    }
    template < typename PointType, index_t dimension >
    GenericInfiniteLine< PointType, dimension >&
        GenericInfiniteLine< PointType, dimension >::operator=(
            const GenericInfiniteLine< PointType, dimension >& other )
    {
        origin_ = other.origin_;
        direction_ = other.direction_;
        return *this;
    }
    template < typename PointType, index_t dimension >
    GenericInfiniteLine< PointType, dimension >::GenericInfiniteLine(
        GenericInfiniteLine< PointType, dimension >&& other )
        : origin_( std::move( other.origin_ ) ),
          direction_( std::move( other.direction_ ) )
    {
    }
    template < typename PointType, index_t dimension >
    GenericInfiniteLine< PointType, dimension >&
        GenericInfiniteLine< PointType, dimension >::operator=(
            GenericInfiniteLine< PointType, dimension >&& other )
    {
        origin_ = std::move( other.origin_ );
        direction_ = std::move( other.direction_ );
        return *this;
    }
    template < typename PointType, index_t dimension >
    const Point< dimension >&
        GenericInfiniteLine< PointType, dimension >::origin() const
    {
        return origin_;
    }
    template < typename PointType, index_t dimension >
    const Vector< dimension >&
        GenericInfiniteLine< PointType, dimension >::direction() const
    {
        return direction_;
    }
    template < typename PointType, index_t dimension >
    GenericInfiniteLine< PointType, dimension >::GenericInfiniteLine(
        const OwnerInfiniteLine< dimension >& other )
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

    template < typename PointType >
    GenericPlane< PointType >::GenericPlane(
        const Vector3D& normal, const Point3D& origin )
        : normal_( normal.normalize() ), origin_( origin )
    {
    }
    template < typename PointType >
    GenericPlane< PointType >::GenericPlane( const GenericPlane& other )
        : normal_( other.normal_ ), origin_( other.origin_ )
    {
    }
    template < typename PointType >
    GenericPlane< PointType >& GenericPlane< PointType >::operator=(
        const GenericPlane& other )
    {
        normal_ = other.normal_;
        origin_ = other.origin_;
        return *this;
    }
    template < typename PointType >
    GenericPlane< PointType >::GenericPlane( GenericPlane&& other )
        : normal_( std::move( other.normal_ ) ),
          origin_( std::move( other.origin_ ) )
    {
    }
    template < typename PointType >
    GenericPlane< PointType >& GenericPlane< PointType >::operator=(
        GenericPlane&& other )
    {
        normal_ = std::move( other.normal_ );
        origin_ = std::move( other.origin_ );
        return *this;
    }
    template < typename PointType >
    const Vector3D& GenericPlane< PointType >::normal() const
    {
        return normal_;
    }
    template < typename PointType >
    const Point3D& GenericPlane< PointType >::origin() const
    {
        return origin_;
    }
    template < typename PointType >
    double GenericPlane< PointType >::plane_constant() const
    {
        double plane_constant{ 0.0 };
        for( const auto i : LRange{ 3 } )
        {
            plane_constant -= origin().value( i ) * normal().value( i );
        }
        return plane_constant;
    }

    template < typename PointType >
    GenericPlane< PointType >::GenericPlane( const OwnerPlane& other )
        : normal_( other.normal() ), origin_( other.origin() )
    {
    }

    OwnerPlane::OwnerPlane( const Vector3D& normal, const Point3D& origin )
        : Base( normal, origin )
    {
    }
    OwnerPlane::OwnerPlane( const OwnerPlane& other ) : Base( other ) {}
    OwnerPlane& OwnerPlane::operator=( const OwnerPlane& other )
    {
        Base::operator=( other );
        return *this;
    }
    OwnerPlane::OwnerPlane( OwnerPlane&& other ) : Base( other ) {}
    OwnerPlane& OwnerPlane::operator=( OwnerPlane&& other )
    {
        Base::operator=( other );
        return *this;
    }

    Plane::Plane( const Vector3D& normal, const Point3D& origin )
        : Base( normal, origin )
    {
    }
    Plane::Plane( const Plane& other ) : Base( other ) {}
    Plane::Plane( const OwnerPlane& other ) : Base( other ) {}
    Plane& Plane::operator=( const Plane& other )
    {
        Base::operator=( other );
        return *this;
    }
    Plane::Plane( Plane&& other ) : Base( other ) {}
    Plane& Plane::operator=( Plane&& other )
    {
        Base::operator=( other );
        return *this;
    }

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
        return { this->normal(), vertices_[0] };
    }
    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, absl::optional< Vector3D > >::type
        GenericTriangle< PointType, dimension >::new_normal() const
    {
        try
        {
            for( const auto v : LRange{ 3 } )
            {
                const auto next = v + 1 == 3 ? 0 : v + 1;
                const auto edge0 =
                    Vector3D{ vertices_[v], vertices_[next] }.normalize();
                const auto prev = v == 0 ? 2 : v - 1;
                const auto edge1 =
                    Vector3D{ vertices_[v], vertices_[prev] }.normalize();

                const auto normal = edge0.cross( edge1 );
                const auto length = normal.length();
                if( length > M_PI / 180 ) // 1 degree
                {
                    return normal / length;
                }
            }
            return absl::nullopt;
        }
        catch( const OpenGeodeException& /*unused*/ )
        {
            return absl::nullopt;
        }
    }
    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, absl::optional< Plane > >::type
        GenericTriangle< PointType, dimension >::new_plane() const
    {
        if( const auto triangle_normal = this->new_normal() )
        {
            return Plane{ triangle_normal.value(), vertices_[0] };
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
            return OwnerPlane{ triangle_normal.value(), vertices_[0] };
        }
        return absl::nullopt;
    }
    template < typename PointType, index_t dimension >
    void GenericTriangle< PointType, dimension >::set_point(
        index_t vertex, const Point< dimension >& point )
    {
        vertices_.at( vertex ) = point;
    }
    template < typename PointType, index_t dimension >
    const std::array< PointType, 3 >&
        GenericTriangle< PointType, dimension >::vertices() const
    {
        return vertices_;
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

    template < typename PointType >
    GenericTetra< PointType >::GenericTetra( const Point3D& p0,
        const Point3D& p1,
        const Point3D& p2,
        const Point3D& p3 )
        : vertices_{ { { p0 }, { p1 }, { p2 }, { p3 } } }
    {
    }
    template < typename PointType >
    GenericTetra< PointType >::GenericTetra( const GenericTetra& other )
        : vertices_( other.vertices_ )
    {
    }
    template < typename PointType >
    GenericTetra< PointType >& GenericTetra< PointType >::operator=(
        const GenericTetra& other )
    {
        vertices_ = other.vertices_;
        return *this;
    }
    template < typename PointType >
    GenericTetra< PointType >::GenericTetra( GenericTetra&& other )
        : vertices_( std::move( other.vertices_ ) )
    {
    }
    template < typename PointType >
    GenericTetra< PointType >& GenericTetra< PointType >::operator=(
        GenericTetra&& other )
    {
        vertices_ = std::move( other.vertices_ );
        return *this;
    }
    template < typename PointType >
    Point3D GenericTetra< PointType >::barycenter() const
    {
        const Point3D& p0 = vertices_[0];
        const Point3D& p1 = vertices_[1];
        const Point3D& p2 = vertices_[2];
        const Point3D& p3 = vertices_[3];
        return ( p0 + p1 + p2 + p3 ) / 4.;
    }
    template < typename PointType >
    void GenericTetra< PointType >::set_point(
        index_t vertex, const Point3D& point )
    {
        vertices_.at( vertex ) = point;
    }
    template < typename PointType >
    const std::array< PointType, 4 >&
        GenericTetra< PointType >::vertices() const
    {
        return vertices_;
    }

    template < typename PointType >
    GenericTetra< PointType >::GenericTetra( const OwnerTetra& other )
        : vertices_{ { { other.vertices()[0] }, { other.vertices()[1] },
            { other.vertices()[2] }, { other.vertices()[3] } } }
    {
    }

    OwnerTetra::OwnerTetra( const Point3D& p0,
        const Point3D& p1,
        const Point3D& p2,
        const Point3D& p3 )
        : Base( p0, p1, p2, p3 )
    {
    }
    OwnerTetra::OwnerTetra( const OwnerTetra& other ) : Base( other ) {}
    OwnerTetra& OwnerTetra::operator=( const OwnerTetra& other )
    {
        Base::operator=( other );
        return *this;
    }
    OwnerTetra::OwnerTetra( OwnerTetra&& other ) : Base( other ) {}
    OwnerTetra& OwnerTetra::operator=( OwnerTetra&& other )
    {
        Base::operator=( other );
        return *this;
    }

    Tetra::Tetra( const Point3D& p0,
        const Point3D& p1,
        const Point3D& p2,
        const Point3D& p3 )
        : Base( p0, p1, p2, p3 )
    {
    }
    Tetra::Tetra( const Tetra& other ) : Base( other ) {}
    Tetra::Tetra( const OwnerTetra& other ) : Base( other ) {}
    Tetra& Tetra::operator=( const Tetra& other )
    {
        Base::operator=( other );
        return *this;
    }
    Tetra::Tetra( Tetra&& other ) : Base( other ) {}
    Tetra& Tetra::operator=( Tetra&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >::GenericSphere(
        const Point< dimension >& origin, double radius )
        : origin_( origin ), radius_( std::move( radius ) )
    {
    }
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >::GenericSphere(
        const GenericSphere< PointType, dimension >& other )
        : origin_( other.origin_ ), radius_( other.radius_ )
    {
    }
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >&
        GenericSphere< PointType, dimension >::operator=(
            const GenericSphere< PointType, dimension >& other )
    {
        origin_ = other.origin_;
        radius_ = other.radius_;
        return *this;
    }
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >::GenericSphere(
        GenericSphere< PointType, dimension >&& other )
        : origin_( std::move( other.origin_ ) ),
          radius_( std::move( other.radius_ ) )
    {
    }
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >&
        GenericSphere< PointType, dimension >::operator=(
            GenericSphere< PointType, dimension >&& other )
    {
        origin_ = std::move( other.origin_ );
        radius_ = std::move( other.radius_ );
        return *this;
    }
    template < typename PointType, index_t dimension >
    const Point< dimension >&
        GenericSphere< PointType, dimension >::origin() const
    {
        return origin_;
    }
    template < typename PointType, index_t dimension >
    double GenericSphere< PointType, dimension >::radius() const
    {
        return radius_;
    }

    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >::GenericSphere(
        const OwnerSphere< dimension >& other )
        : origin_( other.origin() ), radius_( other.radius() )
    {
    }

    template < index_t dimension >
    OwnerSphere< dimension >::OwnerSphere(
        const Point< dimension >& origin, double radius )
        : Base( origin, radius )
    {
    }
    template < index_t dimension >
    OwnerSphere< dimension >::OwnerSphere(
        const OwnerSphere< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    OwnerSphere< dimension >& OwnerSphere< dimension >::operator=(
        const OwnerSphere< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    OwnerSphere< dimension >::OwnerSphere( OwnerSphere< dimension >&& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    OwnerSphere< dimension >& OwnerSphere< dimension >::operator=(
        OwnerSphere< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < index_t dimension >
    Sphere< dimension >::Sphere(
        const Point< dimension >& origin, double radius )
        : Base( origin, radius )
    {
    }
    template < index_t dimension >
    Sphere< dimension >::Sphere( const Sphere< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    Sphere< dimension >::Sphere( const OwnerSphere< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    Sphere< dimension >& Sphere< dimension >::operator=(
        const Sphere< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    Sphere< dimension >::Sphere( Sphere< dimension >&& other ) : Base( other )
    {
    }
    template < index_t dimension >
    Sphere< dimension >& Sphere< dimension >::operator=(
        Sphere< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < typename PointType >
    GenericCircle< PointType >::GenericCircle(
        const GenericPlane< PointType >& plane, double radius )
        : plane_( plane ), radius_( std::move( radius ) )
    {
    }
    template < typename PointType >
    GenericCircle< PointType >::GenericCircle( const GenericCircle& other )
        : plane_( other.plane_ ), radius_( other.radius_ )
    {
    }
    template < typename PointType >
    GenericCircle< PointType >& GenericCircle< PointType >::operator=(
        const GenericCircle& other )
    {
        plane_ = other.plane_;
        radius_ = other.radius_;
        return *this;
    }
    template < typename PointType >
    GenericCircle< PointType >::GenericCircle( GenericCircle&& other )
        : plane_( std::move( other.plane_ ) ),
          radius_( std::move( other.radius_ ) )
    {
    }
    template < typename PointType >
    GenericCircle< PointType >& GenericCircle< PointType >::operator=(
        GenericCircle&& other )
    {
        plane_ = std::move( other.plane_ );
        radius_ = std::move( other.radius_ );
        return *this;
    }
    template < typename PointType >
    const GenericPlane< PointType >& GenericCircle< PointType >::plane() const
    {
        return plane_;
    }
    template < typename PointType >
    double GenericCircle< PointType >::radius() const
    {
        return radius_;
    }

    template < typename PointType >
    GenericCircle< PointType >::GenericCircle( const OwnerCircle& other )
        : plane_( other.plane().normal(), other.plane().origin() ),
          radius_( other.radius() )
    {
    }

    OwnerCircle::OwnerCircle( const OwnerPlane& plane, double radius )
        : Base( plane, radius )
    {
    }
    OwnerCircle::OwnerCircle( const OwnerCircle& other ) : Base( other ) {}
    OwnerCircle& OwnerCircle::operator=( const OwnerCircle& other )
    {
        Base::operator=( other );
        return *this;
    }
    OwnerCircle::OwnerCircle( OwnerCircle&& other ) : Base( other ) {}
    OwnerCircle& OwnerCircle::operator=( OwnerCircle&& other )
    {
        Base::operator=( other );
        return *this;
    }

    Circle::Circle( const Plane& plane, double radius ) : Base( plane, radius )
    {
    }
    Circle::Circle( const Circle& other ) : Base( other ) {}
    Circle::Circle( const OwnerCircle& other ) : Base( other ) {}
    Circle& Circle::operator=( const Circle& other )
    {
        Base::operator=( other );
        return *this;
    }
    Circle::Circle( Circle&& other ) : Base( other ) {}
    Circle& Circle::operator=( Circle&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < typename PointType >
    constexpr std::array< std::array< index_t, 3 >, 4 >
        GenericTetra< PointType >::tetra_facet_vertex;

    template const std::array< std::array< index_t, 3 >, 4 >
        GenericTetra< RefPoint< 3 > >::tetra_facet_vertex;

    template const std::array< std::array< index_t, 3 >, 4 >
        GenericTetra< Point< 3 > >::tetra_facet_vertex;

    template class opengeode_basic_api GenericSegment< Point< 2 >, 2 >;
    template class opengeode_basic_api GenericSegment< Point< 3 >, 3 >;
    template class opengeode_basic_api GenericSegment< RefPoint< 2 >, 2 >;
    template class opengeode_basic_api GenericSegment< RefPoint< 3 >, 3 >;
    template class opengeode_basic_api OwnerSegment< 2 >;
    template class opengeode_basic_api OwnerSegment< 3 >;
    template class opengeode_basic_api Segment< 2 >;
    template class opengeode_basic_api Segment< 3 >;

    template class opengeode_basic_api GenericInfiniteLine< Point< 2 >, 2 >;
    template class opengeode_basic_api GenericInfiniteLine< Point< 3 >, 3 >;
    template class opengeode_basic_api GenericInfiniteLine< RefPoint< 2 >, 2 >;
    template class opengeode_basic_api GenericInfiniteLine< RefPoint< 3 >, 3 >;
    template class opengeode_basic_api OwnerInfiniteLine< 2 >;
    template class opengeode_basic_api OwnerInfiniteLine< 3 >;
    template class opengeode_basic_api InfiniteLine< 2 >;
    template class opengeode_basic_api InfiniteLine< 3 >;

    template class opengeode_basic_api GenericPlane< Point< 3 > >;
    template class opengeode_basic_api GenericPlane< RefPoint< 3 > >;

    template class opengeode_basic_api GenericTriangle< Point< 2 >, 2 >;
    template class opengeode_basic_api GenericTriangle< Point< 3 >, 3 >;
    template class opengeode_basic_api GenericTriangle< RefPoint< 2 >, 2 >;
    template class opengeode_basic_api GenericTriangle< RefPoint< 3 >, 3 >;
    template class opengeode_basic_api OwnerTriangle< 2 >;
    template class opengeode_basic_api OwnerTriangle< 3 >;
    template class opengeode_basic_api Triangle< 2 >;
    template class opengeode_basic_api Triangle< 3 >;

    template opengeode_basic_api Vector3D
        GenericTriangle< Point< 3 >, 3 >::normal< 3 >() const;
    template opengeode_basic_api Plane
        GenericTriangle< Point< 3 >, 3 >::plane< 3 >() const;
    template opengeode_basic_api OwnerPlane
        GenericTriangle< Point< 3 >, 3 >::owner_plane< 3 >() const;
    template opengeode_basic_api absl::optional< Vector3D >
        GenericTriangle< Point< 3 >, 3 >::new_normal< 3 >() const;
    template opengeode_basic_api absl::optional< Plane >
        GenericTriangle< Point< 3 >, 3 >::new_plane< 3 >() const;
    template opengeode_basic_api absl::optional< OwnerPlane >
        GenericTriangle< Point< 3 >, 3 >::new_owner_plane< 3 >() const;

    template opengeode_basic_api Vector3D
        GenericTriangle< RefPoint< 3 >, 3 >::normal< 3 >() const;
    template opengeode_basic_api Plane
        GenericTriangle< RefPoint< 3 >, 3 >::plane< 3 >() const;
    template opengeode_basic_api OwnerPlane
        GenericTriangle< RefPoint< 3 >, 3 >::owner_plane< 3 >() const;
    template opengeode_basic_api absl::optional< Vector3D >
        GenericTriangle< RefPoint< 3 >, 3 >::new_normal< 3 >() const;
    template opengeode_basic_api absl::optional< Plane >
        GenericTriangle< RefPoint< 3 >, 3 >::new_plane< 3 >() const;
    template opengeode_basic_api absl::optional< OwnerPlane >
        GenericTriangle< RefPoint< 3 >, 3 >::new_owner_plane< 3 >() const;

    template class opengeode_basic_api GenericTetra< Point< 3 > >;
    template class opengeode_basic_api GenericTetra< RefPoint< 3 > >;

    template class opengeode_basic_api GenericSphere< Point< 2 >, 2 >;
    template class opengeode_basic_api GenericSphere< Point< 3 >, 3 >;
    template class opengeode_basic_api GenericSphere< RefPoint< 2 >, 2 >;
    template class opengeode_basic_api GenericSphere< RefPoint< 3 >, 3 >;
    template class opengeode_basic_api OwnerSphere< 2 >;
    template class opengeode_basic_api OwnerSphere< 3 >;
    template class opengeode_basic_api Sphere< 2 >;
    template class opengeode_basic_api Sphere< 3 >;

    template class opengeode_basic_api GenericCircle< Point< 3 > >;
    template class opengeode_basic_api GenericCircle< RefPoint< 3 > >;

} // namespace geode
