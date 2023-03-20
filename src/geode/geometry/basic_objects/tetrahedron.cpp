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

#include <geode/geometry/basic_objects/tetrahedron.h>

#include <geode/geometry/bounding_box.h>

namespace geode
{
    template < typename PointType >
    GenericTetrahedron< PointType >::GenericTetrahedron(
        PointType p0, PointType p1, PointType p2, PointType p3 )
        : vertices_{ { std::move( p0 ), std::move( p1 ), std::move( p2 ),
            std::move( p3 ) } }
    {
    }
    template < typename PointType >
    GenericTetrahedron< PointType >::GenericTetrahedron(
        const GenericTetrahedron& other )
        : vertices_( other.vertices_ )
    {
    }
    template < typename PointType >
    GenericTetrahedron< PointType >& GenericTetrahedron< PointType >::operator=(
        const GenericTetrahedron& other )
    {
        vertices_ = other.vertices_;
        return *this;
    }
    template < typename PointType >
    GenericTetrahedron< PointType >::GenericTetrahedron(
        GenericTetrahedron&& other )
        : vertices_( std::move( other.vertices_ ) )
    {
    }
    template < typename PointType >
    GenericTetrahedron< PointType >& GenericTetrahedron< PointType >::operator=(
        GenericTetrahedron&& other )
    {
        vertices_ = std::move( other.vertices_ );
        return *this;
    }
    template < typename PointType >
    Point3D GenericTetrahedron< PointType >::barycenter() const
    {
        const Point3D& p0 = vertices_[0];
        const Point3D& p1 = vertices_[1];
        const Point3D& p2 = vertices_[2];
        const Point3D& p3 = vertices_[3];
        return ( p0 + p1 + p2 + p3 ) / 4.;
    }
    template < typename PointType >
    void GenericTetrahedron< PointType >::set_point(
        index_t vertex, PointType point )
    {
        vertices_[vertex] = std::move( point );
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

    OwnerTetrahedron::OwnerTetrahedron(
        Point3D p0, Point3D p1, Point3D p2, Point3D p3 )
        : Base(
            std::move( p0 ), std::move( p1 ), std::move( p2 ), std::move( p3 ) )
    {
    }
    OwnerTetrahedron::OwnerTetrahedron( const OwnerTetrahedron& other )
        : Base( other )
    {
    }
    OwnerTetrahedron& OwnerTetrahedron::operator=(
        const OwnerTetrahedron& other )
    {
        Base::operator=( other );
        return *this;
    }
    OwnerTetrahedron::OwnerTetrahedron( OwnerTetrahedron&& other )
        : Base( other )
    {
    }
    OwnerTetrahedron& OwnerTetrahedron::operator=( OwnerTetrahedron&& other )
    {
        Base::operator=( other );
        return *this;
    }

    Tetrahedron::Tetrahedron( const Point3D& p0,
        const Point3D& p1,
        const Point3D& p2,
        const Point3D& p3 )
        : Base( p0, p1, p2, p3 )
    {
    }
    Tetrahedron::Tetrahedron( const Tetrahedron& other ) : Base( other ) {}
    Tetrahedron::Tetrahedron( const OwnerTetrahedron& other )
        : Base( other.vertices()[0],
            other.vertices()[1],
            other.vertices()[2],
            other.vertices()[3] )
    {
    }
    Tetrahedron& Tetrahedron::operator=( const Tetrahedron& other )
    {
        Base::operator=( other );
        return *this;
    }
    Tetrahedron::Tetrahedron( Tetrahedron&& other ) : Base( other ) {}
    Tetrahedron& Tetrahedron::operator=( Tetrahedron&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < typename PointType >
    constexpr std::array< std::array< index_t, 3 >, 4 >
        GenericTetrahedron< PointType >::tetrahedron_facet_vertex;
    template opengeode_geometry_api const
        std::array< std::array< index_t, 3 >, 4 >
            GenericTetrahedron< RefPoint< 3 > >::tetrahedron_facet_vertex;
    template opengeode_geometry_api const
        std::array< std::array< index_t, 3 >, 4 >
            GenericTetrahedron< Point< 3 > >::tetrahedron_facet_vertex;

    template class opengeode_geometry_api GenericTetrahedron< Point< 3 > >;
    template class opengeode_geometry_api GenericTetrahedron< RefPoint< 3 > >;
} // namespace geode
