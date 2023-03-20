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

#include <geode/geometry/basic_objects/circle.h>

namespace geode
{
    template < typename PointType >
    GenericCircle< PointType >::GenericCircle(
        GenericPlane< PointType > plane, double radius )
        : plane_( std::move( plane ) ), radius_( radius )
    {
    }
    template < typename PointType >
    GenericCircle< PointType >::GenericCircle( const GenericCircle& other )
        : plane_( other.plane_ ), radius_( other.radius_ )
    {
    }
    template < typename PointType >
    GenericCircle< PointType >& GenericCircle< PointType >::operator=(
        const GenericCircle& other )
    {
        plane_ = other.plane_;
        radius_ = other.radius_;
        return *this;
    }
    template < typename PointType >
    GenericCircle< PointType >::GenericCircle( GenericCircle&& other )
        : plane_( std::move( other.plane_ ) ),
          radius_( std::move( other.radius_ ) )
    {
    }
    template < typename PointType >
    GenericCircle< PointType >& GenericCircle< PointType >::operator=(
        GenericCircle&& other )
    {
        plane_ = std::move( other.plane_ );
        radius_ = std::move( other.radius_ );
        return *this;
    }
    template < typename PointType >
    const GenericPlane< PointType >& GenericCircle< PointType >::plane() const
    {
        return plane_;
    }
    template < typename PointType >
    double GenericCircle< PointType >::radius() const
    {
        return radius_;
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
    OwnerCircle::OwnerCircle( OwnerCircle&& other ) : Base( other ) {}
    OwnerCircle& OwnerCircle::operator=( OwnerCircle&& other )
    {
        Base::operator=( other );
        return *this;
    }

    Circle::Circle( Plane plane, double radius )
        : Base( std::move( plane ), radius )
    {
    }
    Circle::Circle( const Circle& other ) : Base( other ) {}
    Circle::Circle( const OwnerCircle& other )
        : Base(
            { other.plane().normal(), other.plane().origin() }, other.radius() )
    {
    }
    Circle& Circle::operator=( const Circle& other )
    {
        Base::operator=( other );
        return *this;
    }
    Circle::Circle( Circle&& other ) : Base( other ) {}
    Circle& Circle::operator=( Circle&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template class opengeode_geometry_api GenericCircle< Point< 3 > >;
    template class opengeode_geometry_api GenericCircle< RefPoint< 3 > >;
} // namespace geode
