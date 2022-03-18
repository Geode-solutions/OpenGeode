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

#include <geode/geometry/basic_objects/tetra.h>

namespace geode
{
    template < typename PointType >
    GenericTetra< PointType >::GenericTetra( const Point3D& p0,
        const Point3D& p1,
        const Point3D& p2,
        const Point3D& p3 )
        : vertices_{ { { p0 }, { p1 }, { p2 }, { p3 } } }
    {
    }
    template < typename PointType >
    GenericTetra< PointType >::GenericTetra( const GenericTetra& other )
        : vertices_( other.vertices_ )
    {
    }
    template < typename PointType >
    GenericTetra< PointType >& GenericTetra< PointType >::operator=(
        const GenericTetra& other )
    {
        vertices_ = other.vertices_;
        return *this;
    }
    template < typename PointType >
    GenericTetra< PointType >::GenericTetra( GenericTetra&& other )
        : vertices_( std::move( other.vertices_ ) )
    {
    }
    template < typename PointType >
    GenericTetra< PointType >& GenericTetra< PointType >::operator=(
        GenericTetra&& other )
    {
        vertices_ = std::move( other.vertices_ );
        return *this;
    }
    template < typename PointType >
    Point3D GenericTetra< PointType >::barycenter() const
    {
        const Point3D& p0 = vertices_[0];
        const Point3D& p1 = vertices_[1];
        const Point3D& p2 = vertices_[2];
        const Point3D& p3 = vertices_[3];
        return ( p0 + p1 + p2 + p3 ) / 4.;
    }
    template < typename PointType >
    void GenericTetra< PointType >::set_point(
        index_t vertex, const Point3D& point )
    {
        vertices_.at( vertex ) = point;
    }
    template < typename PointType >
    const std::array< PointType, 4 >&
        GenericTetra< PointType >::vertices() const
    {
        return vertices_;
    }

    template < typename PointType >
    GenericTetra< PointType >::GenericTetra( const OwnerTetra& other )
        : vertices_{ { { other.vertices()[0] }, { other.vertices()[1] },
            { other.vertices()[2] }, { other.vertices()[3] } } }
    {
    }

    OwnerTetra::OwnerTetra( const Point3D& p0,
        const Point3D& p1,
        const Point3D& p2,
        const Point3D& p3 )
        : Base( p0, p1, p2, p3 )
    {
    }
    OwnerTetra::OwnerTetra( const OwnerTetra& other ) : Base( other ) {}
    OwnerTetra& OwnerTetra::operator=( const OwnerTetra& other )
    {
        Base::operator=( other );
        return *this;
    }
    OwnerTetra::OwnerTetra( OwnerTetra&& other ) : Base( other ) {}
    OwnerTetra& OwnerTetra::operator=( OwnerTetra&& other )
    {
        Base::operator=( other );
        return *this;
    }

    Tetra::Tetra( const Point3D& p0,
        const Point3D& p1,
        const Point3D& p2,
        const Point3D& p3 )
        : Base( p0, p1, p2, p3 )
    {
    }
    Tetra::Tetra( const Tetra& other ) : Base( other ) {}
    Tetra::Tetra( const OwnerTetra& other ) : Base( other ) {}
    Tetra& Tetra::operator=( const Tetra& other )
    {
        Base::operator=( other );
        return *this;
    }
    Tetra::Tetra( Tetra&& other ) : Base( other ) {}
    Tetra& Tetra::operator=( Tetra&& other )
    {
        Base::operator=( other );
        return *this;
    }

    template < typename PointType >
    constexpr std::array< std::array< index_t, 3 >, 4 >
        GenericTetra< PointType >::tetra_facet_vertex;
    template const std::array< std::array< index_t, 3 >, 4 >
        GenericTetra< RefPoint< 3 > >::tetra_facet_vertex;
    template const std::array< std::array< index_t, 3 >, 4 >
        GenericTetra< Point< 3 > >::tetra_facet_vertex;

    template class opengeode_basic_api GenericTetra< Point< 3 > >;
    template class opengeode_basic_api GenericTetra< RefPoint< 3 > >;
} // namespace geode
