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

#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/common.hpp>
#include <geode/geometry/vector.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    ALIAS_3D( BoundingBox );
    class OwnerCircle;
} // namespace geode

namespace geode
{
    template < typename PlaneType >
    class GenericCircle
    {
    public:
        static constexpr auto dim = 3;

        GenericCircle( PlaneType plane, double radius );

        GenericCircle( const GenericCircle& other );
        GenericCircle& operator=( const GenericCircle& other );
        GenericCircle( GenericCircle&& other ) noexcept;
        GenericCircle& operator=( GenericCircle&& other ) noexcept;

        [[nodiscard]] const PlaneType& plane() const;
        [[nodiscard]] double radius() const;
        [[nodiscard]] BoundingBox3D bounding_box() const;

    private:
        PlaneType plane_;
        double radius_{ 0 };
    };

    class opengeode_geometry_api OwnerCircle
        : public GenericCircle< OwnerPlane >
    {
        using Base = GenericCircle< OwnerPlane >;

    public:
        explicit OwnerCircle( OwnerPlane plane, double radius );

        OwnerCircle( const OwnerCircle& other );
        OwnerCircle& operator=( const OwnerCircle& other );
        OwnerCircle( OwnerCircle&& other ) noexcept;
        OwnerCircle& operator=( OwnerCircle&& other ) noexcept;
    };
    using OwnerDisk = OwnerCircle;

    class opengeode_geometry_api Circle : public GenericCircle< Plane >
    {
        using Base = GenericCircle< Plane >;

    public:
        Circle( Plane plane, double radius );

        Circle( const Circle& other );
        Circle( const OwnerCircle& other );
        Circle& operator=( const Circle& other );
        Circle( Circle&& other ) noexcept;
        Circle& operator=( Circle&& other ) noexcept;
    };
    using Disk = Circle;
} // namespace geode
