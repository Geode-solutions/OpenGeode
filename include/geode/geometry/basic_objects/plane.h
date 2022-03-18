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
    class OwnerPlane;

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
    ALIAS_2D_AND_3D( RefPoint );
} // namespace geode

namespace geode
{
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
} // namespace geode
