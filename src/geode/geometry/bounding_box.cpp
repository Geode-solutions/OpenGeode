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

#include <geode/geometry/bounding_box.hpp>

#include <limits>

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/perpendicular.hpp>
#include <geode/geometry/position.hpp>

namespace
{
    template < geode::index_t dimension >
    bool line_intersects( const geode::BoundingBox< dimension >& box,
        const geode::GenericLine< geode::RefPoint< dimension >, dimension >&
            line );

    template <>
    bool line_intersects< 3 >( const geode::BoundingBox< 3 >& box,
        const geode::GenericLine< geode::RefPoint< 3 >, 3 >& line )
    {
        const auto box_half_extent = box.diagonal() / 2.;
        const geode::Vector3D line_translated_origin{ box.center(),
            line.origin() };
        const auto origin_cross_direction =
            line.direction().cross( line_translated_origin );
        const geode::Point3D abs_line_direction{
            { std::fabs( line.direction().value( 0 ) ),
                std::fabs( line.direction().value( 1 ) ),
                std::fabs( line.direction().value( 2 ) ) }
        };
        for( const auto i : geode::LRange{ 3 } )
        {
            const auto ii = i == 2 ? 0 : i + 1;
            const auto iii = ii == 2 ? 0 : ii + 1;
            if( std::fabs( origin_cross_direction.value( i ) )
                    - ( box_half_extent.value( ii )
                            * abs_line_direction.value( iii )
                        + box_half_extent.value( iii )
                              * abs_line_direction.value( ii ) )
                > geode::GLOBAL_EPSILON )
            {
                return false;
            }
        }
        return true;
    }

    template <>
    bool line_intersects< 2 >( const geode::BoundingBox< 2 >& box,
        const geode::GenericLine< geode::RefPoint< 2 >, 2 >& line )
    {
        const auto box_center = box.center();
        const auto box_half_extent = box.diagonal() / 2.;
        const geode::Vector2D line_translated_origin{ box_center,
            line.origin() };
        const auto lhs = std::fabs(
            dot_perpendicular( line.direction(), line_translated_origin ) );
        const auto rhs = box_half_extent.value( 0 )
                             * std::fabs( line.direction().value( 1 ) )
                         + box_half_extent.value( 1 )
                               * std::fabs( line.direction().value( 0 ) );
        return lhs - rhs <= geode::GLOBAL_EPSILON;
    }

    template <>
    bool line_intersects< 1 >( const geode::BoundingBox< 1 >& box,
        const geode::GenericLine< geode::RefPoint< 1 >, 1 >& line )
    {
        if( box.diagonal().dot( line.direction() ) > 0 )
        {
            return line.origin().value( 0 ) < box.min().value( 0 );
        }
        return line.origin().value( 0 ) > box.max().value( 0 );
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    BoundingBox< dimension >::BoundingBox()
    {
        for( const auto i : LRange{ dimension } )
        {
            min_.set_value( i, std::numeric_limits< double >::max() );
            max_.set_value( i, std::numeric_limits< double >::lowest() );
        }
    }

    template < index_t dimension >
    BoundingBox< dimension >::BoundingBox(
        Point< dimension > min, Point< dimension > max )
        : min_{ std::move( min ) }, max_{ std::move( max ) }
    {
    }

    template < index_t dimension >
    BoundingBox< dimension >::~BoundingBox() = default;

    template < index_t dimension >
    BoundingBox< dimension >::BoundingBox( const BoundingBox& other ) = default;

    template < index_t dimension >
    BoundingBox< dimension >& BoundingBox< dimension >::operator=(
        const BoundingBox& other ) = default;

    template < index_t dimension >
    BoundingBox< dimension >::BoundingBox( BoundingBox&& ) noexcept = default;

    template < index_t dimension >
    BoundingBox< dimension >& BoundingBox< dimension >::operator=(
        BoundingBox&& ) noexcept = default;

    template < index_t dimension >
    const Point< dimension >& BoundingBox< dimension >::min() const
    {
        return min_;
    }

    template < index_t dimension >
    const Point< dimension >& BoundingBox< dimension >::max() const
    {
        return max_;
    }

    template < index_t dimension >
    void BoundingBox< dimension >::add_point( const Point< dimension >& point )
    {
        for( const auto i : LRange{ dimension } )
        {
            min_.set_value( i, std::min( min_.value( i ), point.value( i ) ) );
            max_.set_value( i, std::max( max_.value( i ), point.value( i ) ) );
        }
    }

    template < index_t dimension >
    void BoundingBox< dimension >::add_box(
        const BoundingBox< dimension >& box )
    {
        add_point( box.min_ );
        add_point( box.max_ );
    }

    template < index_t dimension >
    bool BoundingBox< dimension >::contains(
        const Point< dimension >& point ) const
    {
        for( const auto i : LRange{ dimension } )
        {
            if( point.value( i ) < min_.value( i )
                || point.value( i ) > max_.value( i ) )
            {
                return false;
            }
        }
        return true;
    }

    template < index_t dimension >
    bool BoundingBox< dimension >::intersects(
        const BoundingBox< dimension >& box ) const
    {
        for( const auto i : LRange{ dimension } )
        {
            if( max_.value( i ) < box.min_.value( i )
                || min_.value( i ) > box.max_.value( i ) )
            {
                return false;
            }
        }
        return true;
    }

    template < index_t dimension >
    bool BoundingBox< dimension >::intersects(
        const Ray< dimension >& ray ) const
    {
        const auto box_half_extent = diagonal() / 2.;
        const auto ray_translated_origin = ray.origin() - center();
        for( const auto i : LRange{ dimension } )
        {
            if( std::fabs( ray_translated_origin.value( i ) )
                        - box_half_extent.value( i )
                    > GLOBAL_EPSILON
                && ray_translated_origin.value( i ) * ray.direction().value( i )
                       > GLOBAL_EPSILON )
            {
                return false;
            }
        }
        return line_intersects( *this, ray );
    }

    template < index_t dimension >
    bool BoundingBox< dimension >::intersects(
        const InfiniteLine< dimension >& line ) const
    {
        return line_intersects( *this, line );
    }

    template < index_t dimension >
    bool BoundingBox< dimension >::intersects(
        const Segment< dimension >& segment ) const
    {
        const auto& vertices = segment.vertices();
        for( const auto v : LRange{ 2 } )
        {
            if( contains( vertices[v].get() ) )
            {
                return true;
            }
        }
        if( segment.length() < GLOBAL_EPSILON )
        {
            return false;
        }
        const auto box_center = center();
        const auto box_extent = diagonal() / 2.;
        const auto segment_origin = segment.barycenter() - box_center;
        const auto segment_extent = segment.length() / 2.;
        const auto segment_direction = segment.normalized_direction();
        for( const auto i : LRange{ dimension } )
        {
            const auto lhs = std::fabs( segment_origin.value( i ) );
            const auto rhs =
                box_extent.value( i )
                + segment_extent * std::fabs( segment_direction.value( i ) );
            if( lhs > rhs )
            {
                return false;
            }
        }
        return this->intersects( InfiniteLine< dimension >{ segment } );
    }

    template <>
    template <>
    bool BoundingBox< 3 >::intersects< 3 >(
        const Triangle< 3 >& triangle ) const
    {
        const auto& vertices = triangle.vertices();
        for( const auto v : LRange{ 3 } )
        {
            if( contains( vertices[v].get() ) )
            {
                return true;
            }
        }
        const auto triangle_projection = [&vertices]( const Vector3D& normal ) {
            BoundingBox1D interval;
            interval.add_point(
                Point1D{ { normal.dot( Vector3D{ vertices[0].get() } ) } } );
            interval.add_point(
                Point1D{ { normal.dot( Vector3D{ vertices[1].get() } ) } } );
            interval.add_point(
                Point1D{ { normal.dot( Vector3D{ vertices[2].get() } ) } } );
            return interval;
        };
        const Vector3D box_center{ center() };
        const auto box_diagonal = diagonal();
        const auto bbox_projection = [&box_center, &box_diagonal](
                                         const Vector3D& normal ) {
            const auto origin = normal.dot( box_center );
            const auto maximum_extent =
                ( std::fabs( normal.value( 0 ) * box_diagonal.value( 0 ) )
                    + std::fabs( normal.value( 1 ) * box_diagonal.value( 1 ) )
                    + std::fabs( normal.value( 2 ) * box_diagonal.value( 2 ) ) )
                / 2.;
            return BoundingBox1D{ Point1D{ { origin - maximum_extent } },
                Point1D{ { origin + maximum_extent } } };
        };
        const std::array< Vector3D, 3 > edges{ Vector3D{ vertices[0].get(),
                                                   vertices[1].get() },
            Vector3D{ vertices[0].get(), vertices[2].get() },
            Vector3D{ vertices[1].get(), vertices[2].get() } };

        // Test direction of triangle normal.
        const auto triangle_normal = edges[0].cross( edges[1] );
        if( !bbox_projection( triangle_normal )
                 .contains( Point1D{ { triangle_normal.dot(
                     Vector3D{ vertices[0].get() } ) } } ) )
        {
            return false;
        }

        // Test direction of box faces.
        for( const auto i : LRange{ 3 } )
        {
            Vector3D axis;
            axis.set_value( i, 1 );
            const auto triangle_interval = triangle_projection( axis );
            const auto center_value = box_center.value( i );
            const auto extent = box_diagonal.value( i ) / 2.;
            const BoundingBox1D box_interval{ Point1D{
                                                  { center_value - extent } },
                Point1D{ { center_value + extent } } };
            if( !triangle_interval.intersects( box_interval ) )
            {
                return false;
            }
        }

        // Test direction of triangle-box edge cross products.
        for( const auto i0 : LRange{ 3 } )
        {
            for( const auto i1 : LRange{ 3 } )
            {
                Vector3D axis;
                axis.set_value( i1, 1 );
                const auto normal = edges[i0].cross( axis );
                const auto triangle_interval = triangle_projection( normal );
                const auto box_interval = bbox_projection( normal );
                if( !triangle_interval.intersects( box_interval ) )
                {
                    return false;
                }
            }
        }
        return true;
    }

    template <>
    template <>
    bool BoundingBox< 2 >::intersects< 2 >(
        const Triangle< 2 >& triangle ) const
    {
        if( point_triangle_position( center(), triangle ) == POSITION::inside )
        {
            return true;
        }
        const auto& vertices = triangle.vertices();
        for( const auto v : LRange{ 3 } )
        {
            if( contains( vertices[v].get() ) )
            {
                return true;
            }
        }
        return intersects( Segment2D{ vertices[0].get(), vertices[1].get() } )
               || intersects(
                   Segment2D{ vertices[0].get(), vertices[2].get() } )
               || intersects(
                   Segment2D{ vertices[1].get(), vertices[2].get() } );
    }

    template < index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, bool >::type
        BoundingBox< dimension >::intersects( const Tetrahedron& tetra ) const
    {
        if( point_tetrahedron_position( center(), tetra ) == POSITION::inside )
        {
            return true;
        }
        const auto& vertices = tetra.vertices();
        for( const auto v : LRange{ 4 } )
        {
            if( contains( vertices[v].get() ) )
            {
                return true;
            }
        }
        return intersects(
                   { vertices[0].get(), vertices[1].get(), vertices[2].get() } )
               || intersects(
                   { vertices[0].get(), vertices[1].get(), vertices[3].get() } )
               || intersects(
                   { vertices[0].get(), vertices[2].get(), vertices[3].get() } )
               || intersects( { vertices[1].get(), vertices[2].get(),
                   vertices[3].get() } );
    }

    template < index_t dimension >
    Point< dimension > BoundingBox< dimension >::center() const
    {
        return ( min_ + max_ ) / 2.;
    }

    template < index_t dimension >
    Vector< dimension > BoundingBox< dimension >::diagonal() const
    {
        return { min_, max_ };
    }

    template < index_t dimension >
    double BoundingBox< dimension >::signed_distance(
        const Point< dimension >& point ) const
    {
        bool inside{ true };
        Vector< dimension > result;
        for( const auto c : LRange{ dimension } )
        {
            const auto value = point.value( c );
            if( value < min_.value( c ) )
            {
                inside = false;
                result.set_value( c, value - min_.value( c ) );
            }
            else if( value > max_.value( c ) )
            {
                inside = false;
                result.set_value( c, value - max_.value( c ) );
            }
        }
        if( !inside )
        {
            return result.length();
        }
        const auto Pmin = point - min_;
        const auto Pmax = point - max_;
        auto inner_distance = std::numeric_limits< double >::max();
        for( const auto c : geode::LRange{ dimension } )
        {
            const auto local_distance = std::min(
                std::abs( Pmin.value( c ) ), std::abs( Pmax.value( c ) ) );
            inner_distance = std::min( inner_distance, local_distance );
        }
        return -inner_distance;
    }

    template class opengeode_geometry_api BoundingBox< 1 >;
    template class opengeode_geometry_api BoundingBox< 2 >;
    template class opengeode_geometry_api BoundingBox< 3 >;

    template opengeode_geometry_api bool BoundingBox< 2 >::intersects< 2 >(
        const Triangle< 2 >& ) const;
    template opengeode_geometry_api bool BoundingBox< 3 >::intersects< 3 >(
        const Triangle< 3 >& ) const;
    template opengeode_geometry_api bool BoundingBox< 3 >::intersects< 3 >(
        const Tetrahedron& ) const;
} // namespace geode
