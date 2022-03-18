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

#include <geode/geometry/basic_objects/plane.h>

namespace geode
{
    template < typename PointType >
    GenericPlane< PointType >::GenericPlane(
        const Vector3D& normal, const Point3D& origin )
        : normal_( normal.normalize() ), origin_( origin )
    {
    }
    template < typename PointType >
    GenericPlane< PointType >::GenericPlane( const GenericPlane& other )
        : normal_( other.normal_ ), origin_( other.origin_ )
    {
    }
    template < typename PointType >
    GenericPlane< PointType >& GenericPlane< PointType >::operator=(
        const GenericPlane& other )
    {
        normal_ = other.normal_;
        origin_ = other.origin_;
        return *this;
    }
    template < typename PointType >
    GenericPlane< PointType >::GenericPlane( GenericPlane&& other )
        : normal_( std::move( other.normal_ ) ),
          origin_( std::move( other.origin_ ) )
    {
    }
    template < typename PointType >
    GenericPlane< PointType >& GenericPlane< PointType >::operator=(
        GenericPlane&& other )
    {
        normal_ = std::move( other.normal_ );
        origin_ = std::move( other.origin_ );
        return *this;
    }
    template < typename PointType >
    const Vector3D& GenericPlane< PointType >::normal() const
    {
        return normal_;
    }
    template < typename PointType >
    const Point3D& GenericPlane< PointType >::origin() const
    {
        return origin_;
    }
    template < typename PointType >
    double GenericPlane< PointType >::plane_constant() const
    {
        double plane_constant{ 0.0 };
        for( const auto i : LRange{ 3 } )
        {
            plane_constant -= origin().value( i ) * normal().value( i );
        }
        return plane_constant;
    }

    template < typename PointType >
    GenericPlane< PointType >::GenericPlane( const OwnerPlane& other )
        : normal_( other.normal() ), origin_( other.origin() )
    {
    }

    OwnerPlane::OwnerPlane( const Vector3D& normal, const Point3D& origin )
        : Base( normal, origin )
    {
    }
    OwnerPlane::OwnerPlane( const OwnerPlane& other ) : Base( other ) {}
    OwnerPlane& OwnerPlane::operator=( const OwnerPlane& other )
    {
        Base::operator=( other );
        return *this;
    }
    OwnerPlane::OwnerPlane( OwnerPlane&& other ) : Base( other ) {}
    OwnerPlane& OwnerPlane::operator=( OwnerPlane&& other )
    {
        Base::operator=( other );
        return *this;
    }

    Plane::Plane( const Vector3D& normal, const Point3D& origin )
        : Base( normal, origin )
    {
    }
    Plane::Plane( const Plane& other ) : Base( other ) {}
    Plane::Plane( const OwnerPlane& other ) : Base( other ) {}
    Plane& Plane::operator=( const Plane& other )
    {
        Base::operator=( other );
        return *this;
    }
    Plane::Plane( Plane&& other ) : Base( other ) {}
    Plane& Plane::operator=( Plane&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template class opengeode_geometry_api GenericPlane< Point< 3 > >;
    template class opengeode_geometry_api GenericPlane< RefPoint< 3 > >;
} // namespace geode
