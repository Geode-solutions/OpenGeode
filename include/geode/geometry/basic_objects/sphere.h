/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerSphere );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
    ALIAS_2D_AND_3D( RefPoint );
} // namespace geode

namespace geode
{
    template < typename PointType, index_t dimension >
    class GenericSphere
    {
    public:
        GenericSphere( PointType origin, double radius );

        GenericSphere( const GenericSphere< PointType, dimension >& other );
        GenericSphere< PointType, dimension >& operator=(
            const GenericSphere< PointType, dimension >& other );
        GenericSphere( GenericSphere< PointType, dimension >&& other ) noexcept;
        GenericSphere< PointType, dimension >& operator=(
            GenericSphere< PointType, dimension >&& other ) noexcept;

        const Point< dimension >& origin() const;
        double radius() const;
        BoundingBox< dimension > bounding_box() const;

    private:
        PointType origin_;
        double radius_{ 0 };
    };

    template < index_t dimension >
    class OwnerSphere : public GenericSphere< Point< dimension >, dimension >
    {
        using Base = GenericSphere< Point< dimension >, dimension >;

    public:
        explicit OwnerSphere( Point< dimension > origin, double radius );

        OwnerSphere( const OwnerSphere< dimension >& other );
        OwnerSphere< dimension >& operator=(
            const OwnerSphere< dimension >& other );
        OwnerSphere( OwnerSphere< dimension >&& other ) noexcept;
        OwnerSphere< dimension >& operator=(
            OwnerSphere< dimension >&& other ) noexcept;
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
        Sphere( Sphere< dimension >&& other ) noexcept;
        Sphere< dimension >& operator=( Sphere< dimension >&& other ) noexcept;
    };

    template < index_t dimension >
    using Ball = Sphere< dimension >;
    ALIAS_2D_AND_3D( Sphere );
    ALIAS_2D_AND_3D( Ball );
} // namespace geode
