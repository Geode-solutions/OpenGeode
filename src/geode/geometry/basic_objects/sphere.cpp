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

#include <geode/geometry/basic_objects/sphere.h>

namespace geode
{
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >::GenericSphere(
        const Point< dimension >& origin, double radius )
        : origin_( origin ), radius_( std::move( radius ) )
    {
    }
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >::GenericSphere(
        const GenericSphere< PointType, dimension >& other )
        : origin_( other.origin_ ), radius_( other.radius_ )
    {
    }
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >&
        GenericSphere< PointType, dimension >::operator=(
            const GenericSphere< PointType, dimension >& other )
    {
        origin_ = other.origin_;
        radius_ = other.radius_;
        return *this;
    }
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >::GenericSphere(
        GenericSphere< PointType, dimension >&& other )
        : origin_( std::move( other.origin_ ) ),
          radius_( std::move( other.radius_ ) )
    {
    }
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >&
        GenericSphere< PointType, dimension >::operator=(
            GenericSphere< PointType, dimension >&& other )
    {
        origin_ = std::move( other.origin_ );
        radius_ = std::move( other.radius_ );
        return *this;
    }
    template < typename PointType, index_t dimension >
    const Point< dimension >&
        GenericSphere< PointType, dimension >::origin() const
    {
        return origin_;
    }
    template < typename PointType, index_t dimension >
    double GenericSphere< PointType, dimension >::radius() const
    {
        return radius_;
    }

    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >::GenericSphere(
        const OwnerSphere< dimension >& other )
        : origin_( other.origin() ), radius_( other.radius() )
    {
    }

    template < index_t dimension >
    OwnerSphere< dimension >::OwnerSphere(
        const Point< dimension >& origin, double radius )
        : Base( origin, radius )
    {
    }
    template < index_t dimension >
    OwnerSphere< dimension >::OwnerSphere(
        const OwnerSphere< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    OwnerSphere< dimension >& OwnerSphere< dimension >::operator=(
        const OwnerSphere< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    OwnerSphere< dimension >::OwnerSphere( OwnerSphere< dimension >&& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    OwnerSphere< dimension >& OwnerSphere< dimension >::operator=(
        OwnerSphere< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < index_t dimension >
    Sphere< dimension >::Sphere(
        const Point< dimension >& origin, double radius )
        : Base( origin, radius )
    {
    }
    template < index_t dimension >
    Sphere< dimension >::Sphere( const Sphere< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    Sphere< dimension >::Sphere( const OwnerSphere< dimension >& other )
        : Base( other )
    {
    }
    template < index_t dimension >
    Sphere< dimension >& Sphere< dimension >::operator=(
        const Sphere< dimension >& other )
    {
        Base::operator=( other );
        return *this;
    }
    template < index_t dimension >
    Sphere< dimension >::Sphere( Sphere< dimension >&& other ) : Base( other )
    {
    }
    template < index_t dimension >
    Sphere< dimension >& Sphere< dimension >::operator=(
        Sphere< dimension >&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template class opengeode_geometry_api GenericSphere< Point< 2 >, 2 >;
    template class opengeode_geometry_api GenericSphere< Point< 3 >, 3 >;
    template class opengeode_geometry_api GenericSphere< RefPoint< 2 >, 2 >;
    template class opengeode_geometry_api GenericSphere< RefPoint< 3 >, 3 >;
    template class opengeode_geometry_api OwnerSphere< 2 >;
    template class opengeode_geometry_api OwnerSphere< 3 >;
    template class opengeode_geometry_api Sphere< 2 >;
    template class opengeode_geometry_api Sphere< 3 >;
} // namespace geode
