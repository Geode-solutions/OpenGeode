/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/geometry/point.h>
#include <geode/geometry/vector.h>

#include <geode/geometry/common.h>

namespace geode
{
    template < index_t dimension >
    class Segment
    {
    public:
        Segment( const Point< dimension >& p0, const Point< dimension >& p1 )
            : vertices_{ { { p0 }, { p1 } } }
        {
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
        const std::array<
            const std::reference_wrapper< const Point< dimension > >,
            2 >&
            vertices() const
        {
            return vertices_;
        }

    private:
        const std::
            array< const std::reference_wrapper< const Point< dimension > >, 2 >
                vertices_;
    };
    ALIAS_2D_AND_3D( Segment );

    template < index_t dimension >
    class InfiniteLine
    {
    public:
        InfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin )
            : origin_( origin ), direction_( direction.normalize() )
        {
        }
        explicit InfiniteLine( const Segment< dimension >& segment )
            : InfiniteLine(
                segment.normalized_direction(), segment.vertices()[0] )
        {
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
        const Point< dimension >& origin_;
        Vector< dimension > direction_;
    };
    ALIAS_2D_AND_3D( InfiniteLine );

    class Plane
    {
    public:
        Plane( const Vector3D& normal, const Point3D& origin )
            : normal_( normal.normalize() ), origin_( origin )
        {
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
            for( auto i : Range{ 3 } )
            {
                plane_constant -= origin_.value( i ) * normal_.value( i );
            }
            return plane_constant;
        }

    private:
        Vector3D normal_;
        const Point3D& origin_;
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
        const std::array<
            const std::reference_wrapper< const Point< dimension > >,
            3 >&
            vertices() const
        {
            return vertices_;
        }

    private:
        const std::
            array< const std::reference_wrapper< const Point< dimension > >, 3 >
                vertices_;
    };
    template <>
    class Triangle< 3 >
    {
    public:
        Triangle( const Point3D& p0, const Point3D& p1, const Point3D& p2 )
            : vertices_{ { { p0 }, { p1 }, { p2 } } }
        {
        }
        Vector3D normal() const
        {
            return Vector3D{ vertices_[0], vertices_[1] }.cross(
                Vector3D{ vertices_[0], vertices_[2] } );
        }
        Plane plane() const
        {
            return { this->normal(), vertices_[0] };
        }
        const std::array< const std::reference_wrapper< const Point3D >, 3 >&
            vertices() const
        {
            return vertices_;
        }

    private:
        const std::array< const std::reference_wrapper< const Point3D >, 3 >
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
        const std::array< const std::reference_wrapper< const Point3D >, 4 >&
            vertices() const
        {
            return vertices_;
        }

    private:
        const std::array< const std::reference_wrapper< const Point3D >, 4 >
            vertices_;
    };

    template < index_t dimension >
    class Sphere
    {
    public:
        Sphere( const Point< dimension >& origin, double radius )
            : origin_( origin ), radius_( std::move( radius ) )
        {
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
        const Point< dimension >& origin_;
        const double radius_{ 0 };
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
        const Plane& plane() const
        {
            return plane_;
        }
        double radius() const
        {
            return radius_;
        }

    private:
        const Plane& plane_;
        const double radius_{ 0 };
    };

    using Disk = Circle;

    enum struct Side
    {
        positive,
        negative,
        zero
    };

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
        facet3
    };
} // namespace geode
