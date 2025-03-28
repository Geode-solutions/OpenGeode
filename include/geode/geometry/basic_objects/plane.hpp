/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geometry/common.hpp>
#include <geode/geometry/vector.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
    ALIAS_3D( Triangle );
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
        GenericPlane( const Vector3D& normal, PointType origin );
        explicit GenericPlane( const Triangle3D& triangle );
        GenericPlane( const GenericPlane& other );
        GenericPlane& operator=( const GenericPlane& other );
        GenericPlane( GenericPlane&& other ) noexcept;
        GenericPlane& operator=( GenericPlane&& other ) noexcept;

        [[nodiscard]] const Vector3D& normal() const;
        [[nodiscard]] const Point3D& origin() const;
        [[nodiscard]] double plane_constant() const;

    private:
        Vector3D normal_;
        PointType origin_;
    };

    class opengeode_geometry_api OwnerPlane : public GenericPlane< Point3D >
    {
        using Base = GenericPlane< Point3D >;

    public:
        explicit OwnerPlane( const Vector3D& normal, Point3D origin );
        explicit OwnerPlane( const Triangle3D& triangle );
        OwnerPlane( const OwnerPlane& other );
        OwnerPlane& operator=( const OwnerPlane& other );
        OwnerPlane( OwnerPlane&& other ) noexcept;
        OwnerPlane& operator=( OwnerPlane&& other ) noexcept;
    };

    class opengeode_geometry_api Plane : public GenericPlane< RefPoint3D >
    {
        using Base = GenericPlane< RefPoint3D >;

    public:
        Plane( const Vector3D& normal, const Point3D& origin );
        explicit Plane( const Triangle3D& triangle );
        Plane( const Plane& other );
        Plane( const OwnerPlane& other );
        Plane& operator=( const Plane& other );
        Plane( Plane&& other ) noexcept;
        Plane& operator=( Plane&& other ) noexcept;
    };
} // namespace geode
