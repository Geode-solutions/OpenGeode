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
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerInfiniteLine );
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerSegment );
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerSphere );
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerTriangle );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_3D( Point );
    class OwnerCircle;
    class OwnerPlane;
    class OwnerTetra;

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
    ALIAS_2D_AND_3D( RefPoint );
} // namespace geode

namespace geode
{
    template < typename PointType, index_t dimension >
    class GenericSegment
    {
    public:
        GenericSegment(
            const Point< dimension >& p0, const Point< dimension >& p1 );

        GenericSegment( const GenericSegment< PointType, dimension >& other );
        GenericSegment< PointType, dimension >& operator=(
            const GenericSegment< PointType, dimension >& other );
        GenericSegment( GenericSegment< PointType, dimension >&& other );
        GenericSegment< PointType, dimension >& operator=(
            GenericSegment< PointType, dimension >&& other );

        Vector< dimension > direction() const;
        Vector< dimension > normalized_direction() const;
        Point< dimension > barycenter() const;
        double length() const;
        void set_point( index_t vertex, const Point< dimension >& point );
        const std::array< PointType, 2 >& vertices() const;

    protected:
        GenericSegment( const OwnerSegment< dimension >& other );

    private:
        std::array< PointType, 2 > vertices_;
    };

    template < index_t dimension >
    class OwnerSegment : public GenericSegment< Point< dimension >, dimension >
    {
        using Base = GenericSegment< Point< dimension >, dimension >;

    public:
        OwnerSegment(
            const Point< dimension >& p0, const Point< dimension >& p1 );

        OwnerSegment( const OwnerSegment< dimension >& other );
        OwnerSegment< dimension >& operator=(
            const OwnerSegment< dimension >& other );
        OwnerSegment( OwnerSegment< dimension >&& other );
        OwnerSegment< dimension >& operator=(
            OwnerSegment< dimension >&& other );
    };
    ALIAS_2D_AND_3D( OwnerSegment );

    template < index_t dimension >
    class Segment : public GenericSegment< RefPoint< dimension >, dimension >
    {
        using Base = GenericSegment< RefPoint< dimension >, dimension >;

    public:
        Segment( const Point< dimension >& p0, const Point< dimension >& p1 );

        Segment( const Segment< dimension >& other );
        Segment( const OwnerSegment< dimension >& other );
        Segment< dimension >& operator=( const Segment< dimension >& other );
        Segment( Segment< dimension >&& other );
        Segment< dimension >& operator=( Segment< dimension >&& other );
    };
    ALIAS_2D_AND_3D( Segment );

    template < typename PointType, index_t dimension >
    class GenericInfiniteLine
    {
    public:
        GenericInfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin );

        GenericInfiniteLine(
            const GenericSegment< PointType, dimension >& segment );
        GenericInfiniteLine(
            const GenericInfiniteLine< PointType, dimension >& other );
        GenericInfiniteLine< PointType, dimension >& operator=(
            const GenericInfiniteLine< PointType, dimension >& other );
        GenericInfiniteLine(
            GenericInfiniteLine< PointType, dimension >&& other );
        GenericInfiniteLine< PointType, dimension >& operator=(
            GenericInfiniteLine< PointType, dimension >&& other );

        const Point< dimension >& origin() const;
        const Vector< dimension >& direction() const;

    protected:
        GenericInfiniteLine( const OwnerInfiniteLine< dimension >& other );

    private:
        PointType origin_;
        Vector< dimension > direction_;
    };

    template < index_t dimension >
    class OwnerInfiniteLine
        : public GenericInfiniteLine< Point< dimension >, dimension >
    {
        using Base = GenericInfiniteLine< Point< dimension >, dimension >;

    public:
        OwnerInfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin );

        OwnerInfiniteLine( const Segment< dimension >& segment );
        OwnerInfiniteLine( const OwnerInfiniteLine< dimension >& other );
        OwnerInfiniteLine< dimension >& operator=(
            const OwnerInfiniteLine< dimension >& other );
        OwnerInfiniteLine( OwnerInfiniteLine< dimension >&& other );
        OwnerInfiniteLine< dimension >& operator=(
            OwnerInfiniteLine< dimension >&& other );
    };
    ALIAS_2D_AND_3D( OwnerInfiniteLine );
    template < index_t dimension >
    using OwnerRay = OwnerInfiniteLine< dimension >;
    ALIAS_2D_AND_3D( OwnerRay );

    template < index_t dimension >
    class InfiniteLine
        : public GenericInfiniteLine< RefPoint< dimension >, dimension >
    {
        using Base = GenericInfiniteLine< RefPoint< dimension >, dimension >;

    public:
        InfiniteLine( const Vector< dimension >& direction,
            const Point< dimension >& origin );
        InfiniteLine( const Segment< dimension >& segment );

        InfiniteLine( const InfiniteLine< dimension >& other );
        InfiniteLine( const OwnerInfiniteLine< dimension >& other );
        InfiniteLine< dimension >& operator=(
            const InfiniteLine< dimension >& other );
        InfiniteLine( InfiniteLine< dimension >&& other );
        InfiniteLine< dimension >& operator=(
            InfiniteLine< dimension >&& other );
    };
    ALIAS_2D_AND_3D( InfiniteLine );
    template < index_t dimension >
    using Ray = InfiniteLine< dimension >;
    ALIAS_2D_AND_3D( Ray );

    template < typename PointType >
    class GenericPlane
    {
    public:
        GenericPlane( const Vector3D& normal, const Point3D& origin );

        GenericPlane( const GenericPlane& other );
        GenericPlane& operator=( const GenericPlane& other );
        GenericPlane( GenericPlane&& other );
        GenericPlane& operator=( GenericPlane&& other );

        const Vector3D& normal() const;
        const Point3D& origin() const;
        double plane_constant() const;

    protected:
        GenericPlane( const OwnerPlane& other );

    private:
        Vector3D normal_;
        PointType origin_;
    };

    class opengeode_basic_api OwnerPlane : public GenericPlane< Point3D >
    {
        using Base = GenericPlane< Point3D >;

    public:
        OwnerPlane( const Vector3D& normal, const Point3D& origin );
        OwnerPlane( const OwnerPlane& other );
        OwnerPlane& operator=( const OwnerPlane& other );
        OwnerPlane( OwnerPlane&& other );
        OwnerPlane& operator=( OwnerPlane&& other );
    };

    class opengeode_basic_api Plane : public GenericPlane< RefPoint3D >
    {
        using Base = GenericPlane< RefPoint3D >;

    public:
        Plane( const Vector3D& normal, const Point3D& origin );

        Plane( const Plane& other );
        Plane( const OwnerPlane& other );
        Plane& operator=( const Plane& other );
        Plane( Plane&& other );
        Plane& operator=( Plane&& other );
    };

    template < typename PointType, index_t dimension >
    class GenericTriangle
    {
    public:
        GenericTriangle( const Point< dimension >& p0,
            const Point< dimension >& p1,
            const Point< dimension >& p2 );

        GenericTriangle( const GenericTriangle< PointType, dimension >& other );
        GenericTriangle< PointType, dimension >& operator=(
            const GenericTriangle< PointType, dimension >& other );
        GenericTriangle( GenericTriangle< PointType, dimension >&& other );
        GenericTriangle< PointType, dimension >& operator=(
            GenericTriangle< PointType, dimension >&& other );

        Point< dimension > barycenter() const;
        template < index_t T = dimension >
        typename std::enable_if< T == 3, Vector3D >::type normal() const;
        template < index_t T = dimension >
        typename std::enable_if< T == 3, Plane >::type plane() const;
        template < index_t T = dimension >
        typename std::enable_if< T == 3, OwnerPlane >::type owner_plane() const;
        template < index_t T = dimension >
        typename std::enable_if< T == 3, absl::optional< Vector3D > >::type
            new_normal() const;
        template < index_t T = dimension >
        typename std::enable_if< T == 3, absl::optional< Plane > >::type
            new_plane() const;
        template < index_t T = dimension >
        typename std::enable_if< T == 3, absl::optional< OwnerPlane > >::type
            new_owner_plane() const;
        void set_point( index_t vertex, const Point< dimension >& point );
        const std::array< PointType, 3 >& vertices() const;

    protected:
        GenericTriangle( const OwnerTriangle< dimension >& other );

    private:
        std::array< PointType, 3 > vertices_;
    };

    template < index_t dimension >
    class OwnerTriangle
        : public GenericTriangle< Point< dimension >, dimension >
    {
        using Base = GenericTriangle< Point< dimension >, dimension >;

    public:
        OwnerTriangle( const Point< dimension >& p0,
            const Point< dimension >& p1,
            const Point< dimension >& p2 );

        OwnerTriangle( const OwnerTriangle< dimension >& other );
        OwnerTriangle< dimension >& operator=(
            const OwnerTriangle< dimension >& other );
        OwnerTriangle( OwnerTriangle< dimension >&& other );
        OwnerTriangle< dimension >& operator=(
            OwnerTriangle< dimension >&& other );
    };
    ALIAS_2D_AND_3D( OwnerTriangle );
    template < index_t dimension >
    class Triangle : public GenericTriangle< RefPoint< dimension >, dimension >
    {
        using Base = GenericTriangle< RefPoint< dimension >, dimension >;

    public:
        Triangle( const Point< dimension >& p0,
            const Point< dimension >& p1,
            const Point< dimension >& p2 );

        Triangle( const Triangle< dimension >& other );
        Triangle( const OwnerTriangle< dimension >& other );
        Triangle< dimension >& operator=( const Triangle< dimension >& other );
        Triangle( Triangle< dimension >&& other );
        Triangle< dimension >& operator=( Triangle< dimension >&& other );
    };
    ALIAS_2D_AND_3D( Triangle );

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
            const Point3D& p3 );

        GenericTetra( const GenericTetra& other );
        GenericTetra& operator=( const GenericTetra& other );
        GenericTetra( GenericTetra&& other );
        GenericTetra& operator=( GenericTetra&& other );

        Point3D barycenter() const;
        void set_point( index_t vertex, const Point3D& point );
        const std::array< PointType, 4 >& vertices() const;

    protected:
        GenericTetra( const OwnerTetra& other );

    private:
        std::array< PointType, 4 > vertices_;
    };

    class opengeode_basic_api OwnerTetra : public GenericTetra< Point3D >
    {
        using Base = GenericTetra< Point3D >;

    public:
        OwnerTetra( const Point3D& p0,
            const Point3D& p1,
            const Point3D& p2,
            const Point3D& p3 );

        OwnerTetra( const OwnerTetra& other );
        OwnerTetra& operator=( const OwnerTetra& other );
        OwnerTetra( OwnerTetra&& other );
        OwnerTetra& operator=( OwnerTetra&& other );
    };

    class opengeode_basic_api Tetra : public GenericTetra< RefPoint3D >
    {
        using Base = GenericTetra< RefPoint3D >;

    public:
        Tetra( const Point3D& p0,
            const Point3D& p1,
            const Point3D& p2,
            const Point3D& p3 );

        Tetra( const Tetra& other );
        Tetra( const OwnerTetra& other );
        Tetra& operator=( const Tetra& other );
        Tetra( Tetra&& other );
        Tetra& operator=( Tetra&& other );
    };

    template < typename PointType, index_t dimension >
    class GenericSphere
    {
    public:
        GenericSphere( const Point< dimension >& origin, double radius );

        GenericSphere( const GenericSphere< PointType, dimension >& other );
        GenericSphere< PointType, dimension >& operator=(
            const GenericSphere< PointType, dimension >& other );
        GenericSphere( GenericSphere< PointType, dimension >&& other );
        GenericSphere< PointType, dimension >& operator=(
            GenericSphere< PointType, dimension >&& other );

        const Point< dimension >& origin() const;
        double radius() const;

    protected:
        GenericSphere( const OwnerSphere< dimension >& other );

    private:
        PointType origin_;
        double radius_{ 0 };
    };

    template < index_t dimension >
    class OwnerSphere : public GenericSphere< Point< dimension >, dimension >
    {
        using Base = GenericSphere< Point< dimension >, dimension >;

    public:
        OwnerSphere( const Point< dimension >& origin, double radius );

        OwnerSphere( const OwnerSphere< dimension >& other );
        OwnerSphere< dimension >& operator=(
            const OwnerSphere< dimension >& other );
        OwnerSphere( OwnerSphere< dimension >&& other );
        OwnerSphere< dimension >& operator=( OwnerSphere< dimension >&& other );
    };

    template < index_t dimension >
    using OwnerBall = OwnerSphere< dimension >;
    ALIAS_2D_AND_3D( OwnerSphere );
    ALIAS_2D_AND_3D( OwnerBall );

    template < index_t dimension >
    class Sphere : public GenericSphere< RefPoint< dimension >, dimension >
    {
        using Base = GenericSphere< RefPoint< dimension >, dimension >;

    public:
        Sphere( const Point< dimension >& origin, double radius );

        Sphere( const Sphere< dimension >& other );
        Sphere( const OwnerSphere< dimension >& other );
        Sphere< dimension >& operator=( const Sphere< dimension >& other );
        Sphere( Sphere< dimension >&& other );
        Sphere< dimension >& operator=( Sphere< dimension >&& other );
    };

    template < index_t dimension >
    using Ball = Sphere< dimension >;
    ALIAS_2D_AND_3D( Sphere );
    ALIAS_2D_AND_3D( Ball );

    template < typename PointType >
    class GenericCircle
    {
    public:
        GenericCircle( const GenericPlane< PointType >& plane, double radius );

        GenericCircle( const GenericCircle& other );
        GenericCircle& operator=( const GenericCircle& other );
        GenericCircle( GenericCircle&& other );
        GenericCircle& operator=( GenericCircle&& other );

        const GenericPlane< PointType >& plane() const;
        double radius() const;

    protected:
        GenericCircle( const OwnerCircle& other );

    private:
        GenericPlane< PointType > plane_;
        double radius_{ 0 };
    };

    class opengeode_basic_api OwnerCircle : public GenericCircle< Point3D >
    {
        using Base = GenericCircle< Point3D >;

    public:
        OwnerCircle( const OwnerPlane& plane, double radius );

        OwnerCircle( const OwnerCircle& other );
        OwnerCircle& operator=( const OwnerCircle& other );
        OwnerCircle( OwnerCircle&& other );
        OwnerCircle& operator=( OwnerCircle&& other );
    };
    using OwnerDisk = OwnerCircle;

    class opengeode_basic_api Circle : public GenericCircle< RefPoint3D >
    {
        using Base = GenericCircle< RefPoint3D >;

    public:
        Circle( const Plane& plane, double radius );

        Circle( const Circle& other );
        Circle( const OwnerCircle& other );
        Circle& operator=( const Circle& other );
        Circle( Circle&& other );
        Circle& operator=( Circle&& other );
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
