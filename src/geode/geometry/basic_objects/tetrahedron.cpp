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

#include <geode/geometry/basic_objects/tetrahedron.h>

#include <geode/geometry/bounding_box.h>

namespace geode
{
    template < typename PointType >
    GenericTetrahedron< PointType >::GenericTetrahedron( PointType point0,
        PointType point1,
        PointType point2,
        PointType point3 ) noexcept
        : vertices_{ { point0, point1, point2, point3 } }
    {
    }
    template < typename PointType >
    GenericTetrahedron< PointType >::GenericTetrahedron(
        const GenericTetrahedron& ) noexcept = default;
    template < typename PointType >
    GenericTetrahedron< PointType >& GenericTetrahedron< PointType >::operator=(
        const GenericTetrahedron& ) noexcept = default;
    template < typename PointType >
    GenericTetrahedron< PointType >::GenericTetrahedron(
        GenericTetrahedron&& ) noexcept = default;
    template < typename PointType >
    GenericTetrahedron< PointType >& GenericTetrahedron< PointType >::operator=(
        GenericTetrahedron&& ) noexcept = default;
    template < typename PointType >
    Point3D GenericTetrahedron< PointType >::barycenter() const
    {
        const Point3D& point0 = vertices_[0];
        const Point3D& point1 = vertices_[1];
        const Point3D& point2 = vertices_[2];
        const Point3D& point3 = vertices_[3];
        return ( point0 + point1 + point2 + point3 ) / 4.;
    }
    template < typename PointType >
    void GenericTetrahedron< PointType >::set_point(
        index_t vertex, PointType point )
    {
        vertices_[vertex] = point;
    }
    template < typename PointType >
    const std::array< PointType, 4 >&
        GenericTetrahedron< PointType >::vertices() const
    {
        return vertices_;
    }
    template < typename PointType >
    BoundingBox3D GenericTetrahedron< PointType >::bounding_box() const
    {
        BoundingBox3D bbox;
        for( const auto& point : vertices_ )
        {
            bbox.add_point( point );
        }
        return bbox;
    }

    OwnerTetrahedron::OwnerTetrahedron( Point3D point0,
        Point3D point1,
        Point3D point2,
        Point3D point3 ) noexcept
        : Base( point0, point1, point2, point3 )
    {
    }
    OwnerTetrahedron::OwnerTetrahedron(
        const OwnerTetrahedron& ) noexcept = default;
    OwnerTetrahedron& OwnerTetrahedron::operator=(
        const OwnerTetrahedron& ) noexcept = default;
    OwnerTetrahedron::OwnerTetrahedron( OwnerTetrahedron&& ) noexcept = default;
    OwnerTetrahedron& OwnerTetrahedron::operator=(
        OwnerTetrahedron&& ) noexcept = default;

    Tetrahedron::Tetrahedron( const Point3D& point0,
        const Point3D& point1,
        const Point3D& point2,
        const Point3D& point3 ) noexcept
        : Base( point0, point1, point2, point3 )
    {
    }
    Tetrahedron::Tetrahedron( const Tetrahedron& other ) noexcept = default;
    Tetrahedron::Tetrahedron( const OwnerTetrahedron& other ) noexcept
        : Base( other.vertices()[0],
              other.vertices()[1],
              other.vertices()[2],
              other.vertices()[3] )
    {
    }
    Tetrahedron& Tetrahedron::operator=(
        const Tetrahedron& ) noexcept = default;
    Tetrahedron::Tetrahedron( Tetrahedron&& ) noexcept = default;
    Tetrahedron& Tetrahedron::operator=( Tetrahedron&& ) noexcept = default;

    template class opengeode_geometry_api GenericTetrahedron< Point< 3 > >;
    template class opengeode_geometry_api GenericTetrahedron< RefPoint< 3 > >;
} // namespace geode
