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

#include <geode/geometry/bounding_box.h>

namespace geode
{
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >::GenericSphere(
        PointType origin, double radius )
        : origin_( std::move( origin ) ), radius_( radius )
    {
    }
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >::GenericSphere(
        const GenericSphere< PointType, dimension >& ) = default;
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >&
        GenericSphere< PointType, dimension >::operator=(
            const GenericSphere< PointType, dimension >& ) = default;
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >::GenericSphere(
        GenericSphere< PointType, dimension >&& ) noexcept = default;
    template < typename PointType, index_t dimension >
    GenericSphere< PointType, dimension >&
        GenericSphere< PointType, dimension >::operator=(
            GenericSphere< PointType, dimension >&& ) noexcept = default;
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
    BoundingBox< dimension >
        GenericSphere< PointType, dimension >::bounding_box() const
    {
        Point< dimension > translation;
        for( const auto i : LRange{ dimension } )
        {
            translation.set_value( i, radius_ );
        }
        BoundingBox< dimension > bbox;
        const Point< dimension >& origin = origin_;
        bbox.add_point( origin + translation );
        bbox.add_point( origin - translation );
        return bbox;
    }

    template < index_t dimension >
    OwnerSphere< dimension >::OwnerSphere(
        Point< dimension > origin, double radius )
        : Base( std::move( origin ), radius )
    {
    }
    template < index_t dimension >
    OwnerSphere< dimension >::OwnerSphere(
        const OwnerSphere< dimension >& ) = default;
    template < index_t dimension >
    OwnerSphere< dimension >& OwnerSphere< dimension >::operator=(
        const OwnerSphere< dimension >& ) = default;
    template < index_t dimension >
    OwnerSphere< dimension >::OwnerSphere(
        OwnerSphere< dimension >&& ) noexcept = default;
    template < index_t dimension >
    OwnerSphere< dimension >& OwnerSphere< dimension >::operator=(
        OwnerSphere< dimension >&& ) noexcept = default;

    template < index_t dimension >
    Sphere< dimension >::Sphere(
        const Point< dimension >& origin, double radius )
        : Base( origin, radius )
    {
    }
    template < index_t dimension >
    Sphere< dimension >::Sphere( const Sphere< dimension >& ) = default;
    template < index_t dimension >
    Sphere< dimension >::Sphere( const OwnerSphere< dimension >& other )
        : Base( other.origin(), other.radius() )
    {
    }
    template < index_t dimension >
    Sphere< dimension >& Sphere< dimension >::operator=(
        const Sphere< dimension >& ) = default;
    template < index_t dimension >
    Sphere< dimension >::Sphere( Sphere< dimension >&& ) noexcept = default;
    template < index_t dimension >
    Sphere< dimension >& Sphere< dimension >::operator=(
        Sphere< dimension >&& ) noexcept = default;

    template class opengeode_geometry_api GenericSphere< Point< 2 >, 2 >;
    template class opengeode_geometry_api GenericSphere< Point< 3 >, 3 >;
    template class opengeode_geometry_api GenericSphere< RefPoint< 2 >, 2 >;
    template class opengeode_geometry_api GenericSphere< RefPoint< 3 >, 3 >;
    template class opengeode_geometry_api OwnerSphere< 2 >;
    template class opengeode_geometry_api OwnerSphere< 3 >;
    template class opengeode_geometry_api Sphere< 2 >;
    template class opengeode_geometry_api Sphere< 3 >;
} // namespace geode
