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
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_3D( Point );
} // namespace geode

namespace geode
{
    template < typename PointType, index_t dimension >
    class GenericSegment
    {
    public:
        GenericSegment(
            const Point< dimension >& p0, const Point< dimension >& p1 )
            : vertices_{ { { p0 }, { p1 } } }
        {
        }
        GenericSegment( const GenericSegment< PointType, dimension >& other )
            : vertices_( other.vertices_ )
        {
        }
        GenericSegment< PointType, dimension >& operator=(
            const GenericSegment< PointType, dimension >& other )
        {
            vertices_ = other.vertices_;
            return *this;
        }
        GenericSegment( GenericSegment< PointType, dimension >&& other )
            : vertices_( std::move( other.vertices_ ) )
        {
        }
        GenericSegment< PointType, dimension >& operator=(
            GenericSegment< PointType, dimension >&& other )
        {
            vertices_ = std::move( other.vertices_ );
            return *this;
        }
        Vector< dimension > direction() const
        {
            Vector< dimension > direction{ vertices_[0], vertices_[1] };
            OPENGEODE_EXCEPTION( direction.length() > global_epsilon,
                "[Segment::direction] Segment length too small" );
            return direction;
        }
        Vector< dimension > normalized_direction() const
        {
            return direction().normalize();
        }
        Point< dimension > barycenter() const
        {
            return ( vertices_[0].get() + vertices_[1].get() ) / 2.;
        }
        double length() const
        {
            return Vector< dimension >{ vertices_[0].get(), vertices_[1].get() }
                .length();
        }
        void set_point( index_t vertex, const Point< dimension >& point )
        {
            vertices_.at( vertex ) = point;
        }
        const std::array< PointType, 2 >& vertices() const
        {
            return vertices_;
        }

    private:
        std::array< PointType, 2 > vertices_;
    };

    template < index_t dimension >
    class Segment : public GenericSegment<
                        std::reference_wrapper< const Point< dimension > >,
                        dimension >
    {
        using Base =
            GenericSegment< std::reference_wrapper< const Point< dimension > >,
                dimension >;

    public:
        Segment( const Point< dimension >& p0, const Point< dimension >& p1 )
            : Base( p0, p1 )
        {
        }
        Segment( const Segment< dimension >& other ) : Base( other ) {}
        Segment< dimension >& operator=( const Segment< dimension >& other )
        {
            Base::operator=( other );
            return *this;
        }
        Segment( Segment< dimension >&& other ) : Base( std::move( other ) ) {}
        Segment< dimension >& operator=( Segment< dimension >&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };
    ALIAS_2D_AND_3D( Segment );

    template < index_t dimension >
    class OwnerSegment : public GenericSegment< Point< dimension >, dimension >
    {
        using Base = GenericSegment< Point< dimension >, dimension >;

    public:
        OwnerSegment(
            const Point< dimension >& p0, const Point< dimension >& p1 )
            : Base( p0, p1 )
        {
        }
        OwnerSegment( const OwnerSegment< dimension >& other ) : Base( other )
        {
        }
        OwnerSegment< dimension >& operator=(
            const OwnerSegment< dimension >& other )
        {
            Base::operator=( other );
            return *this;
        }
        OwnerSegment( OwnerSegment< dimension >&& other )
            : Base( std::move( other ) )
        {
        }
        OwnerSegment< dimension >& operator=(
            OwnerSegment< dimension >&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };
    ALIAS_2D_AND_3D( OwnerSegment );

    template < typename PointType, index_t dimension >
    class GenericInfiniteLine
    {
    public:
        GenericInfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin )
            : origin_( origin ), direction_( direction.normalize() )
        {
        }
        GenericInfiniteLine(
            const GenericSegment< PointType, dimension >& segment )
            : GenericInfiniteLine(
                segment.normalized_direction(), segment.vertices()[0] )
        {
        }
        GenericInfiniteLine(
            const GenericInfiniteLine< PointType, dimension >& other )
            : origin_( other.origin_ ), direction_( other.direction_ )
        {
        }
        GenericInfiniteLine< PointType, dimension >& operator=(
            const GenericInfiniteLine< PointType, dimension >& other )
        {
            origin_ = other.origin_;
            direction_ = other.direction_;
            return *this;
        }
        GenericInfiniteLine(
            GenericInfiniteLine< PointType, dimension >&& other )
            : origin_( std::move( other.origin_ ) ),
              direction_( std::move( other.direction_ ) )
        {
        }
        GenericInfiniteLine< PointType, dimension >& operator=(
            GenericInfiniteLine< PointType, dimension >&& other )
        {
            origin_ = std::move( other.origin_ );
            direction_ = std::move( other.direction_ );
            return *this;
        }
        const Point< dimension >& origin() const
        {
            return origin_;
        }
        const Vector< dimension >& direction() const
        {
            return direction_;
        }

    private:
        PointType origin_;
        Vector< dimension > direction_;
    };

    template < index_t dimension >
    class InfiniteLine : public GenericInfiniteLine<
                             std::reference_wrapper< const Point< dimension > >,
                             dimension >
    {
        using Base = GenericInfiniteLine<
            std::reference_wrapper< const Point< dimension > >,
            dimension >;

    public:
        InfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin )
            : Base( direction, origin )
        {
        }
        InfiniteLine( const Segment< dimension >& segment )
            : InfiniteLine(
                segment.normalized_direction(), segment.vertices()[0] )
        {
        }
        InfiniteLine( const InfiniteLine< dimension >& other ) : Base( other )
        {
        }
        InfiniteLine< dimension >& operator=(
            const InfiniteLine< dimension >& other )
        {
            Base::operator=( other );
            return *this;
        }
        InfiniteLine( InfiniteLine< dimension >&& other ) : Base( other ) {}
        InfiniteLine< dimension >& operator=(
            InfiniteLine< dimension >&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };
    ALIAS_2D_AND_3D( InfiniteLine );
    template < index_t dimension >
    using Ray = InfiniteLine< dimension >;
    ALIAS_2D_AND_3D( Ray );

    template < index_t dimension >
    class OwnerInfiniteLine
        : public GenericInfiniteLine< Point< dimension >, dimension >
    {
        using Base = GenericInfiniteLine< Point< dimension >, dimension >;

    public:
        OwnerInfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin )
            : Base( direction, origin )
        {
        }
        OwnerInfiniteLine( const Segment< dimension >& segment )
            : OwnerInfiniteLine(
                segment.normalized_direction(), segment.vertices()[0] )
        {
        }
        OwnerInfiniteLine( const InfiniteLine< dimension >& other )
            : Base( other )
        {
        }
        OwnerInfiniteLine< dimension >& operator=(
            const OwnerInfiniteLine< dimension >& other )
        {
            Base::operator=( other );
            return *this;
        }
        OwnerInfiniteLine( InfiniteLine< dimension >&& other ) : Base( other )
        {
        }
        OwnerInfiniteLine< dimension >& operator=(
            OwnerInfiniteLine< dimension >&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };
    ALIAS_2D_AND_3D( OwnerInfiniteLine );
    template < index_t dimension >
    using OwnerRay = OwnerInfiniteLine< dimension >;
    ALIAS_2D_AND_3D( OwnerRay );

    template < typename PointType >
    class GenericPlane
    {
    public:
        GenericPlane( const Vector3D& normal, const Point3D& origin )
            : normal_( normal.normalize() ), origin_( origin )
        {
        }
        GenericPlane( const GenericPlane& other )
            : normal_( other.normal_ ), origin_( other.origin_ )
        {
        }
        GenericPlane& operator=( const GenericPlane& other )
        {
            normal_ = other.normal_;
            origin_ = other.origin_;
            return *this;
        }
        GenericPlane( GenericPlane&& other )
            : normal_( std::move( other.normal_ ) ),
              origin_( std::move( other.origin_ ) )
        {
        }
        GenericPlane& operator=( GenericPlane&& other )
        {
            normal_ = std::move( other.normal_ );
            origin_ = std::move( other.origin_ );
            return *this;
        }
        const Vector3D& normal() const
        {
            return normal_;
        }
        const Point3D& origin() const
        {
            return origin_;
        }
        double plane_constant() const
        {
            double plane_constant{ 0.0 };
            for( const auto i : LRange{ 3 } )
            {
                plane_constant -= origin_.get().value( i ) * normal_.value( i );
            }
            return plane_constant;
        }

    private:
        Vector3D normal_;
        PointType origin_;
    };

    class Plane : public GenericPlane< std::reference_wrapper< const Point3D > >
    {
        using Base = GenericPlane< std::reference_wrapper< const Point3D > >;

    public:
        Plane( const Vector3D& normal, const Point3D& origin )
            : Base( normal, origin )
        {
        }
        Plane( const Plane& other ) : Base( other ) {}
        Plane& operator=( const Plane& other )
        {
            Base::operator=( other );
            return *this;
        }
        Plane( Plane&& other ) : Base( other ) {}
        Plane& operator=( Plane&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };

    class OwnerPlane : public GenericPlane< Point3D >
    {
        using Base = GenericPlane< Point3D >;

    public:
        OwnerPlane( const Vector3D& normal, const Point3D& origin )
            : Base( normal, origin )
        {
        }
        OwnerPlane( const OwnerPlane& other ) : Base( other ) {}
        OwnerPlane& operator=( const OwnerPlane& other )
        {
            Base::operator=( other );
            return *this;
        }
        OwnerPlane( OwnerPlane&& other ) : Base( other ) {}
        OwnerPlane& operator=( OwnerPlane&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };

    template < typename PointType, index_t dimension >
    class GenericTriangle
    {
    public:
        GenericTriangle( const Point< dimension >& p0,
            const Point< dimension >& p1,
            const Point< dimension >& p2 )
            : vertices_{ { { p0 }, { p1 }, { p2 } } }
        {
        }
        GenericTriangle( const GenericTriangle< PointType, dimension >& other )
            : vertices_( other.vertices_ )
        {
        }
        GenericTriangle< PointType, dimension >& operator=(
            const GenericTriangle< PointType, dimension >& other )
        {
            vertices_ = other.vertices_;
            return *this;
        }
        GenericTriangle( GenericTriangle< PointType, dimension >&& other )
            : vertices_( std::move( other.vertices_ ) )
        {
        }
        GenericTriangle< PointType, dimension >& operator=(
            GenericTriangle< PointType, dimension >&& other )
        {
            vertices_ = std::move( other.vertices_ );
            return *this;
        }
        Point< dimension > barycenter() const
        {
            return ( vertices_[0].get() + vertices_[1].get()
                       + vertices_[2].get() )
                   / 3.;
        }
        template < index_t T = dimension >
        typename std::enable_if< T == 3, Vector3D >::type normal() const
        {
            return Vector3D{ vertices_[0], vertices_[1] }
                .cross( Vector3D{ vertices_[0], vertices_[2] } )
                .normalize();
        }
        template < index_t T = dimension >
        typename std::enable_if< T == 3, Plane >::type plane() const
        {
            return { this->normal(), vertices_[0] };
        }
        template < index_t T = dimension >
        typename std::enable_if< T == 3, OwnerPlane >::type owner_plane() const
        {
            return { this->normal(), vertices_[0] };
        }
        template < index_t T = dimension >
        typename std::enable_if< T == 3, absl::optional< Vector3D > >::type
            new_normal() const
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
        template < index_t T = dimension >
        typename std::enable_if< T == 3, absl::optional< Plane > >::type
            new_plane() const
        {
            if( const auto triangle_normal = this->new_normal() )
            {
                return Plane{ triangle_normal.value(), vertices_[0] };
            }
            return absl::nullopt;
        }
        template < index_t T = dimension >
        typename std::enable_if< T == 3, absl::optional< OwnerPlane > >::type
            new_owner_plane() const
        {
            if( const auto triangle_normal = this->new_normal() )
            {
                return OwnerPlane{ triangle_normal.value(), vertices_[0] };
            }
            return absl::nullopt;
        }
        void set_point( index_t vertex, const Point< dimension >& point )
        {
            vertices_.at( vertex ) = point;
        }
        const std::array< PointType, 3 >& vertices() const
        {
            return vertices_;
        }

    private:
        std::array< PointType, 3 > vertices_;
    };

    template < index_t dimension >
    class Triangle : public GenericTriangle<
                         std::reference_wrapper< const Point< dimension > >,
                         dimension >
    {
        using Base =
            GenericTriangle< std::reference_wrapper< const Point< dimension > >,
                dimension >;

    public:
        Triangle( const Point< dimension >& p0,
            const Point< dimension >& p1,
            const Point< dimension >& p2 )
            : Base( p0, p1, p2 )
        {
        }
        Triangle( const Triangle< dimension >& other ) : Base( other ) {}
        Triangle< dimension >& operator=( const Triangle< dimension >& other )
        {
            Base::operator=( other );
            return *this;
        }
        Triangle( Triangle< dimension >&& other ) : Base( other ) {}
        Triangle< dimension >& operator=( Triangle< dimension >&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };
    ALIAS_2D_AND_3D( Triangle );

    template < index_t dimension >
    class OwnerTriangle
        : public GenericTriangle< Point< dimension >, dimension >
    {
        using Base = GenericTriangle< Point< dimension >, dimension >;

    public:
        OwnerTriangle( const Point< dimension >& p0,
            const Point< dimension >& p1,
            const Point< dimension >& p2 )
            : Base( p0, p1, p2 )
        {
        }
        OwnerTriangle( const OwnerTriangle< dimension >& other ) : Base( other )
        {
        }
        OwnerTriangle< dimension >& operator=(
            const OwnerTriangle< dimension >& other )
        {
            Base::operator=( other );
            return *this;
        }
        OwnerTriangle( OwnerTriangle< dimension >&& other ) : Base( other ) {}
        OwnerTriangle< dimension >& operator=(
            OwnerTriangle< dimension >&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };
    ALIAS_2D_AND_3D( OwnerTriangle );

    template < typename PointType >
    class GenericTetra
    {
    public:
        static constexpr std::array< std::array< index_t, 3 >, 4 >
            tetra_facet_vertex{ { { { 1, 3, 2 } }, { { 0, 2, 3 } },
                { { 3, 1, 0 } }, { { 0, 1, 2 } } } };

        GenericTetra( const Point3D& p0,
            const Point3D& p1,
            const Point3D& p2,
            const Point3D& p3 )
            : vertices_{ { { p0 }, { p1 }, { p2 }, { p3 } } }
        {
        }
        GenericTetra( const GenericTetra& other ) : vertices_( other.vertices_ )
        {
        }
        GenericTetra& operator=( const GenericTetra& other )
        {
            vertices_ = other.vertices_;
            return *this;
        }
        GenericTetra( GenericTetra&& other )
            : vertices_( std::move( other.vertices_ ) )
        {
        }
        GenericTetra& operator=( GenericTetra&& other )
        {
            vertices_ = std::move( other.vertices_ );
            return *this;
        }
        Point3D barycenter() const
        {
            return ( vertices_[0].get() + vertices_[1].get()
                       + vertices_[2].get() + vertices_[3].get() )
                   / 4.;
        }
        void set_point( index_t vertex, const Point3D& point )
        {
            vertices_.at( vertex ) = point;
        }
        const std::array< PointType, 4 >& vertices() const
        {
            return vertices_;
        }

    private:
        std::array< PointType, 4 > vertices_;
    };

    class Tetra : public GenericTetra< std::reference_wrapper< const Point3D > >
    {
        using Base = GenericTetra< std::reference_wrapper< const Point3D > >;

    public:
        Tetra( const Point3D& p0,
            const Point3D& p1,
            const Point3D& p2,
            const Point3D& p3 )
            : Base( p0, p1, p2, p3 )
        {
        }
        Tetra( const Tetra& other ) : Base( other ) {}
        Tetra& operator=( const Tetra& other )
        {
            Base::operator=( other );
            return *this;
        }
        Tetra( Tetra&& other ) : Base( other ) {}
        Tetra& operator=( Tetra&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };

    class OwnerTetra : public GenericTetra< Point3D >
    {
        using Base = GenericTetra< Point3D >;

    public:
        OwnerTetra( const Point3D& p0,
            const Point3D& p1,
            const Point3D& p2,
            const Point3D& p3 )
            : Base( p0, p1, p2, p3 )
        {
        }
        OwnerTetra( const OwnerTetra& other ) : Base( other ) {}
        OwnerTetra& operator=( const OwnerTetra& other )
        {
            Base::operator=( other );
            return *this;
        }
        OwnerTetra( OwnerTetra&& other ) : Base( other ) {}
        OwnerTetra& operator=( OwnerTetra&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };

    template < typename PointType, index_t dimension >
    class GenericSphere
    {
    public:
        GenericSphere( const Point< dimension >& origin, double radius )
            : origin_( origin ), radius_( std::move( radius ) )
        {
        }
        GenericSphere( const GenericSphere& other )
            : origin_( other.origin_ ), radius_( other.radius_ )
        {
        }
        GenericSphere& operator=( const GenericSphere& other )
        {
            origin_ = other.origin_;
            radius_ = other.radius_;
            return *this;
        }
        GenericSphere( GenericSphere&& other )
            : origin_( std::move( other.origin_ ) ),
              radius_( std::move( other.radius_ ) )
        {
        }
        GenericSphere& operator=( GenericSphere&& other )
        {
            origin_ = std::move( other.origin_ );
            radius_ = std::move( other.radius_ );
            return *this;
        }
        const Point< dimension >& origin() const
        {
            return origin_;
        }
        double radius() const
        {
            return radius_;
        }

    private:
        PointType origin_;
        double radius_{ 0 };
    };

    template < index_t dimension >
    class Sphere : public GenericSphere<
                       std::reference_wrapper< const Point< dimension > >,
                       dimension >
    {
        using Base =
            GenericSphere< std::reference_wrapper< const Point< dimension > >,
                dimension >;

    public:
        Sphere( const Point< dimension >& origin, double radius )
            : Base( origin, radius )
        {
        }
        Sphere( const Sphere& other ) : Base( other ) {}
        Sphere& operator=( const Sphere& other )
        {
            Base::operator=( other );
            return *this;
        }
        Sphere( Sphere&& other ) : Base( other ) {}
        Sphere& operator=( Sphere&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };

    template < index_t dimension >
    using Ball = Sphere< dimension >;
    ALIAS_2D_AND_3D( Sphere );
    ALIAS_2D_AND_3D( Ball );

    template < index_t dimension >
    class OwnerSphere : public GenericSphere< Point< dimension >, dimension >
    {
        using Base = GenericSphere< Point< dimension >, dimension >;

    public:
        OwnerSphere( const Point< dimension >& origin, double radius )
            : Base( origin, radius )
        {
        }
        OwnerSphere( const OwnerSphere& other ) : Base( other ) {}
        OwnerSphere& operator=( const OwnerSphere& other )
        {
            Base::operator=( other );
            return *this;
        }
        OwnerSphere( OwnerSphere&& other ) : Base( other ) {}
        OwnerSphere& operator=( OwnerSphere&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };

    template < index_t dimension >
    using OwnerBall = OwnerSphere< dimension >;
    ALIAS_2D_AND_3D( OwnerSphere );
    ALIAS_2D_AND_3D( OwnerBall );

    template < typename PointType >
    class GenericCircle
    {
    public:
        GenericCircle( const GenericPlane< PointType >& plane, double radius )
            : plane_( plane ), radius_( std::move( radius ) )
        {
        }
        GenericCircle( const GenericCircle& other )
            : plane_( other.plane_ ), radius_( other.radius_ )
        {
        }
        GenericCircle& operator=( const GenericCircle& other )
        {
            plane_ = other.plane_;
            radius_ = other.radius_;
            return *this;
        }
        GenericCircle( GenericCircle&& other )
            : plane_( std::move( other.plane_ ) ),
              radius_( std::move( other.radius_ ) )
        {
        }
        GenericCircle& operator=( GenericCircle&& other )
        {
            plane_ = std::move( other.plane_ );
            radius_ = std::move( other.radius_ );
            return *this;
        }
        const GenericPlane< PointType >& plane() const
        {
            return plane_;
        }
        double radius() const
        {
            return radius_;
        }

    private:
        GenericPlane< PointType > plane_;
        double radius_{ 0 };
    };

    class Circle
        : public GenericCircle< std::reference_wrapper< const Point3D > >
    {
        using Base = GenericCircle< std::reference_wrapper< const Point3D > >;

    public:
        Circle( const Plane& plane, double radius ) : Base( plane, radius ) {}
        Circle( const Circle& other ) : Base( other ) {}
        Circle& operator=( const Circle& other )
        {
            Base::operator=( other );
            return *this;
        }
        Circle( Circle&& other ) : Base( other ) {}
        Circle& operator=( Circle&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };
    using Disk = Circle;

    class OwnerCircle : public GenericCircle< Point3D >
    {
        using Base = GenericCircle< Point3D >;

    public:
        OwnerCircle( const OwnerPlane& plane, double radius )
            : Base( plane, radius )
        {
        }
        OwnerCircle( const OwnerCircle& other ) : Base( other ) {}
        OwnerCircle& operator=( const OwnerCircle& other )
        {
            Base::operator=( other );
            return *this;
        }
        OwnerCircle( OwnerCircle&& other ) : Base( other ) {}
        OwnerCircle& operator=( OwnerCircle&& other )
        {
            Base::operator=( other );
            return *this;
        }
    };
    using OwnerDisk = OwnerCircle;

    enum struct Side
    {
        positive,
        negative,
        zero
    };

    using Sign = Side;

    enum struct Position
    {
        // Strictly outside
        outside,
        // Strictly inside
        inside,
        // Vertices
        vertex0,
        vertex1,
        vertex2,
        vertex3,
        // Three edges of a triangle
        edge0,
        edge1,
        edge2,
        // Six edges of a tetra
        edge01,
        edge02,
        edge03,
        edge12,
        edge13,
        edge23,
        // Four facets of a tetra
        facet0,
        facet1,
        facet2,
        facet3,
        // Parallel or coplanar configuration
        parallel
    };
} // namespace geode
