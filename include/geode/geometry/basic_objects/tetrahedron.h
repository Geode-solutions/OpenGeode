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

#pragma once

#include <geode/geometry/common.h>
#include <geode/geometry/vector.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_3D( BoundingBox );
    ALIAS_3D( Point );
    class OwnerTetrahedron;

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
    ALIAS_2D_AND_3D( RefPoint );
} // namespace geode

namespace geode
{
    template < typename PointType >
    class GenericTetrahedron
    {
    public:
        static constexpr std::array< std::array< index_t, 3 >, 4 >
            tetrahedron_facet_vertex{ { { { 1, 3, 2 } }, { { 0, 2, 3 } },
                { { 3, 1, 0 } }, { { 0, 1, 2 } } } };

        GenericTetrahedron(
            PointType p0, PointType p1, PointType p2, PointType p3 );

        GenericTetrahedron( const GenericTetrahedron& other );
        GenericTetrahedron& operator=( const GenericTetrahedron& other );
        GenericTetrahedron( GenericTetrahedron&& other );
        GenericTetrahedron& operator=( GenericTetrahedron&& other );

        Point3D barycenter() const;
        void set_point( index_t vertex, PointType point );
        const std::array< PointType, 4 >& vertices() const;
        BoundingBox3D bounding_box() const;

    private:
        std::array< PointType, 4 > vertices_;
    };

    class opengeode_geometry_api OwnerTetrahedron
        : public GenericTetrahedron< Point3D >
    {
        using Base = GenericTetrahedron< Point3D >;

    public:
        explicit OwnerTetrahedron(
            Point3D p0, Point3D p1, Point3D p2, Point3D p3 );

        OwnerTetrahedron( const OwnerTetrahedron& other );
        OwnerTetrahedron& operator=( const OwnerTetrahedron& other );
        OwnerTetrahedron( OwnerTetrahedron&& other );
        OwnerTetrahedron& operator=( OwnerTetrahedron&& other );
    };

    class opengeode_geometry_api Tetrahedron
        : public GenericTetrahedron< RefPoint3D >
    {
        using Base = GenericTetrahedron< RefPoint3D >;

    public:
        Tetrahedron( const Point3D& p0,
            const Point3D& p1,
            const Point3D& p2,
            const Point3D& p3 );

        Tetrahedron( const Tetrahedron& other );
        Tetrahedron( const OwnerTetrahedron& other );
        Tetrahedron& operator=( const Tetrahedron& other );
        Tetrahedron( Tetrahedron&& other );
        Tetrahedron& operator=( Tetrahedron&& other );
    };
} // namespace geode
