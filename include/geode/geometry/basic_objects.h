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
    public:
        Segment( const Point< dimension >& p0, const Point< dimension >& p1 )
            : GenericSegment<
                std::reference_wrapper< const Point< dimension > >,
                dimension >( p0, p1 )
        {
        }
        Segment( const Segment< dimension >& other )
            : GenericSegment<
                std::reference_wrapper< const Point< dimension > >,
                dimension >( other )
        {
        }
        Segment< dimension >& operator=( const Segment< dimension >& other )
        {
            GenericSegment< std::reference_wrapper< const Point< dimension > >,
                dimension >::operator=( other );
            return *this;
        }
        Segment( Segment< dimension >&& other )
            : GenericSegment<
                std::reference_wrapper< const Point< dimension > >,
                dimension >( std::move( other ) )
        {
        }
        Segment< dimension >& operator=( Segment< dimension >&& other )
        {
            GenericSegment< std::reference_wrapper< const Point< dimension > >,
                dimension >::operator=( other );
            return *this;
        }
    };
    ALIAS_2D_AND_3D( Segment );

    template < index_t dimension >
    class OwnerSegment : public GenericSegment< Point< dimension >, dimension >
    {
    public:
        OwnerSegment(
            const Point< dimension >& p0, const Point< dimension >& p1 )
            : GenericSegment< Point< dimension >, dimension >( p0, p1 )
        {
        }
        OwnerSegment( const OwnerSegment< dimension >& other )
            : GenericSegment< Point< dimension >, dimension >( other )
        {
        }
        OwnerSegment< dimension >& operator=(
            const OwnerSegment< dimension >& other )
        {
            GenericSegment< Point< dimension >, dimension >::operator=( other );
            return *this;
        }
        OwnerSegment( OwnerSegment< dimension >&& other )
            : GenericSegment< Point< dimension >, dimension >(
                std::move( other ) )
        {
        }
        OwnerSegment< dimension >& operator=(
            OwnerSegment< dimension >&& other )
        {
            GenericSegment< Point< dimension >, dimension >::operator=( other );
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
    public:
        InfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin )
            : GenericInfiniteLine<
                std::reference_wrapper< const Point< dimension > >,
                dimension >( direction, origin )
        {
        }
        InfiniteLine( const Segment< dimension >& segment )
            : InfiniteLine(
                segment.normalized_direction(), segment.vertices()[0] )
        {
        }
        InfiniteLine( const InfiniteLine< dimension >& other )
            : GenericInfiniteLine<
                std::reference_wrapper< const Point< dimension > >,
                dimension >( other )
        {
        }
        InfiniteLine< dimension >& operator=(
            const InfiniteLine< dimension >& other )
        {
            GenericInfiniteLine<
                std::reference_wrapper< const Point< dimension > >,
                dimension >::operator=( other );
            return *this;
        }
        InfiniteLine( InfiniteLine< dimension >&& other )
            : GenericInfiniteLine<
                std::reference_wrapper< const Point< dimension > >,
                dimension >( other )
        {
        }
        InfiniteLine< dimension >& operator=(
            InfiniteLine< dimension >&& other )
        {
            GenericInfiniteLine<
                std::reference_wrapper< const Point< dimension > >,
                dimension >::operator=( other );
            return *this;
        }
    };
    ALIAS_2D_AND_3D( InfiniteLine );
    template < index_t dimension >
    using Ray = InfiniteLine< dimension >;
    ALIAS_2D_AND_3D( Ray );

    class Plane
    {
    public:
        Plane( const Vector3D& normal, const Point3D& origin )
            : normal_( normal.normalize() ), origin_( origin )
        {
        }
        Plane( const Plane& other )
            : normal_( other.normal_ ), origin_( other.origin_ )
        {
        }
        Plane& operator=( const Plane& other )
        {
            normal_ = other.normal_;
            origin_ = other.origin_;
            return *this;
        }
        Plane( Plane&& other )
            : normal_( std::move( other.normal_ ) ),
              origin_( std::move( other.origin_ ) )
        {
        }
        Plane& operator=( Plane&& other )
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
            for( const auto i : Range{ 3 } )
            {
                plane_constant -= origin_.get().value( i ) * normal_.value( i );
            }
            return plane_constant;
        }

    private:
        Vector3D normal_;
        std::reference_wrapper< const Point3D > origin_;
    };

    template < index_t dimension >
    class Triangle
    {
    public:
        Triangle( const Point< dimension >& p0,
            const Point< dimension >& p1,
            const Point< dimension >& p2 )
            : vertices_{ { { p0 }, { p1 }, { p2 } } }
        {
        }
        Triangle( const Triangle< dimension >& other )
            : vertices_( other.vertices_ )
        {
        }
        Triangle< dimension >& operator=( const Triangle< dimension >& other )
        {
            vertices_ = other.vertices_;
            return *this;
        }
        Triangle( Triangle< dimension >&& other )
            : vertices_( std::move( other.vertices_ ) )
        {
        }
        Triangle< dimension >& operator=( Triangle< dimension >&& other )
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
        void set_point( index_t vertex, const Point< dimension >& point )
        {
            vertices_.at( vertex ) = point;
        }
        const std::array< std::reference_wrapper< const Point< dimension > >,
            3 >&
            vertices() const
        {
            return vertices_;
        }

    private:
        std::array< std::reference_wrapper< const Point< dimension > >, 3 >
            vertices_;
    };
    ALIAS_2D_AND_3D( Triangle );

    class Tetra
    {
    public:
        static constexpr std::array< std::array< index_t, 3 >, 4 >
            tetra_facet_vertex{ { { { 1, 3, 2 } }, { { 0, 2, 3 } },
                { { 3, 1, 0 } }, { { 0, 1, 2 } } } };

        Tetra( const Point3D& p0,
            const Point3D& p1,
            const Point3D& p2,
            const Point3D& p3 )
            : vertices_{ { { p0 }, { p1 }, { p2 }, { p3 } } }
        {
        }
        Tetra( const Tetra& other ) : vertices_( other.vertices_ ) {}
        Tetra& operator=( const Tetra& other )
        {
            vertices_ = other.vertices_;
            return *this;
        }
        Tetra( Tetra&& other ) : vertices_( std::move( other.vertices_ ) ) {}
        Tetra& operator=( Tetra&& other )
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
        const std::array< std::reference_wrapper< const Point3D >, 4 >&
            vertices() const
        {
            return vertices_;
        }

    private:
        std::array< std::reference_wrapper< const Point3D >, 4 > vertices_;
    };

    template < index_t dimension >
    class Sphere
    {
    public:
        Sphere( const Point< dimension >& origin, double radius )
            : origin_( origin ), radius_( std::move( radius ) )
        {
        }
        Sphere( const Sphere& other )
            : origin_( other.origin_ ), radius_( other.radius_ )
        {
        }
        Sphere& operator=( const Sphere& other )
        {
            origin_ = other.origin_;
            radius_ = other.radius_;
            return *this;
        }
        Sphere( Sphere&& other )
            : origin_( std::move( other.origin_ ) ),
              radius_( std::move( other.radius_ ) )
        {
        }
        Sphere& operator=( Sphere&& other )
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
        std::reference_wrapper< const Point< dimension > > origin_;
        double radius_{ 0 };
    };

    template < index_t dimension >
    using Ball = Sphere< dimension >;

    ALIAS_2D_AND_3D( Sphere );
    ALIAS_2D_AND_3D( Ball );

    class Circle
    {
    public:
        Circle( const Plane& plane, double radius )
            : plane_( plane ), radius_( std::move( radius ) )
        {
        }
        Circle( const Circle& other )
            : plane_( other.plane_ ), radius_( other.radius_ )
        {
        }
        Circle& operator=( const Circle& other )
        {
            plane_ = other.plane_;
            radius_ = other.radius_;
            return *this;
        }
        Circle( Circle&& other )
            : plane_( std::move( other.plane_ ) ),
              radius_( std::move( other.radius_ ) )
        {
        }
        Circle& operator=( Circle&& other )
        {
            plane_ = std::move( other.plane_ );
            radius_ = std::move( other.radius_ );
            return *this;
        }
        const Plane& plane() const
        {
            return plane_;
        }
        double radius() const
        {
            return radius_;
        }

    private:
        std::reference_wrapper< const Plane > plane_;
        double radius_{ 0 };
    };

    using Disk = Circle;

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
