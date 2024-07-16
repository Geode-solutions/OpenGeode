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

#include <geode/geometry/basic_objects/circle.hpp>

#include <geode/geometry/bounding_box.hpp>

namespace geode
{
    template < typename PlaneType >
    GenericCircle< PlaneType >::GenericCircle( PlaneType plane, double radius )
        : plane_( std::move( plane ) ), radius_( radius )
    {
    }
    template < typename PlaneType >
    GenericCircle< PlaneType >::GenericCircle(
        const GenericCircle& other ) = default;
    template < typename PlaneType >
    GenericCircle< PlaneType >& GenericCircle< PlaneType >::operator=(
        const GenericCircle& other ) = default;
    template < typename PlaneType >
    GenericCircle< PlaneType >::GenericCircle(
        GenericCircle&& ) noexcept = default;

    template < typename PlaneType >
    GenericCircle< PlaneType >& GenericCircle< PlaneType >::operator=(
        GenericCircle&& ) noexcept = default;

    template < typename PlaneType >
    const PlaneType& GenericCircle< PlaneType >::plane() const
    {
        return plane_;
    }
    template < typename PlaneType >
    double GenericCircle< PlaneType >::radius() const
    {
        return radius_;
    }
    template < typename PlaneType >
    BoundingBox3D GenericCircle< PlaneType >::bounding_box() const
    {
        const auto sin_angle = [this]( Vector3D direction ) {
            const auto dP = plane_.normal().dot( direction );
            if( dP <= -1.0 )
            {
                return M_PI;
            }
            if( dP >= 1.0 )
            {
                return 0.;
            }
            return std::sqrt( 1 - dP * dP );
        };
        const auto x = sin_angle( Vector3D{ { 1, 0, 0 } } );
        const auto y = sin_angle( Vector3D{ { 0, 1, 0 } } );
        const auto z = sin_angle( Vector3D{ { 0, 0, 1 } } );
        const auto translation = Vector3D{ { x, y, z } } * radius_;
        BoundingBox3D bbox;
        bbox.add_point( plane_.origin() + translation );
        bbox.add_point( plane_.origin() - translation );
        return bbox;
    }

    OwnerCircle::OwnerCircle( OwnerPlane plane, double radius )
        : Base( std::move( plane ), radius )
    {
    }
    OwnerCircle::OwnerCircle( const OwnerCircle& other ) : Base( other ) {}
    OwnerCircle& OwnerCircle::operator=( const OwnerCircle& other )
    {
        Base::operator=( other );
        return *this;
    }
    OwnerCircle::OwnerCircle( OwnerCircle&& ) noexcept = default;
    OwnerCircle& OwnerCircle::operator=( OwnerCircle&& ) noexcept = default;

    Circle::Circle( Plane plane, double radius )
        : Base( std::move( plane ), radius )
    {
    }
    Circle::Circle( const Circle& ) = default;
    Circle::Circle( const OwnerCircle& other )
        : Base(
            { other.plane().normal(), other.plane().origin() }, other.radius() )
    {
    }
    Circle& Circle::operator=( const Circle& ) = default;
    Circle::Circle( Circle&& ) noexcept = default;
    Circle& Circle::operator=( Circle&& ) noexcept = default;

    template class opengeode_geometry_api GenericCircle< Plane >;
    template class opengeode_geometry_api GenericCircle< OwnerPlane >;
} // namespace geode
