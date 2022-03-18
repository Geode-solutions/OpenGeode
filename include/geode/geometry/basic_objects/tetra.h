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
    class OwnerTetra;

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
    ALIAS_2D_AND_3D( RefPoint );
} // namespace geode

namespace geode
{

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
} // namespace geode
