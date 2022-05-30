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

#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/common.h>
#include <geode/geometry/vector.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_3D( Point );
    class OwnerCircle;

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
    ALIAS_2D_AND_3D( RefPoint );
} // namespace geode

namespace geode
{
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

    class opengeode_geometry_api OwnerCircle : public GenericCircle< Point3D >
    {
        using Base = GenericCircle< Point3D >;

    public:
        explicit OwnerCircle( const OwnerPlane& plane, double radius );

        OwnerCircle( const OwnerCircle& other );
        OwnerCircle& operator=( const OwnerCircle& other );
        OwnerCircle( OwnerCircle&& other );
        OwnerCircle& operator=( OwnerCircle&& other );
    };
    using OwnerDisk = OwnerCircle;

    class opengeode_geometry_api Circle : public GenericCircle< RefPoint3D >
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
} // namespace geode
