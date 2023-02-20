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

#include <geode/geometry/bounding_box.h>

#include <limits>

#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/information.h>
#include <geode/geometry/perpendicular.h>
#include <geode/geometry/point.h>
#include <geode/geometry/position.h>

namespace geode
{
    template < index_t dimension >
    class BoundingBox< dimension >::Impl
    {
    public:
        Impl()
        {
            for( const auto i : LRange{ dimension } )
            {
                min_.set_value( i, std::numeric_limits< double >::max() );
                max_.set_value( i, std::numeric_limits< double >::lowest() );
            }
        }

        const Point< dimension >& min() const
        {
            return min_;
        }

        const Point< dimension >& max() const
        {
            return max_;
        }

        void add_point( const Point< dimension >& point )
        {
            for( const auto i : LRange{ dimension } )
            {
                min_.set_value(
                    i, std::min( min_.value( i ), point.value( i ) ) );
                max_.set_value(
                    i, std::max( max_.value( i ), point.value( i ) ) );
            }
        }

        void add_box( const BoundingBox< dimension >& box )
        {
            add_point( box.min() );
            add_point( box.max() );
        }

        bool intersects( const BoundingBox< dimension >& box ) const
        {
            for( const auto i : LRange{ dimension } )
            {
                if( max().value( i ) < box.min().value( i ) )
                {
                    return false;
                }
                if( min().value( i ) > box.max().value( i ) )
                {
                    return false;
                }
            }
            return true;
        }

        bool contains( const Point< dimension >& point ) const
        {
            for( const auto i : LRange{ dimension } )
            {
                if( point.value( i ) < min().value( i ) )
                {
                    return false;
                }
                if( point.value( i ) > max().value( i ) )
                {
                    return false;
                }
            }
            return true;
        }

        bool intersects( const Ray< dimension >& ray ) const
        {
            const auto box_half_extent = diagonal() / 2.;
            const auto ray_translated_origin = ray.origin() - center();
            for( const auto i : LRange{ dimension } )
            {
                if( std::fabs( ray_translated_origin.value( i ) )
                            - box_half_extent.value( i )
                        > global_epsilon
                    && ray_translated_origin.value( i )
                               * ray.direction().value( i )
                           > global_epsilon )
                {
                    return false;
                }
            }
            return line_intersects( ray );
        }

        bool intersects( const InfiniteLine< dimension >& line ) const
        {
            return line_intersects( line );
        }

        template < index_t T = dimension >
        typename std::enable_if< T == 2 || T == 3, bool >::type intersects(
            const Triangle< T >& triangle ) const;

        template < index_t T = dimension >
        typename std::enable_if< T == 3, bool >::type intersects(
            const Tetrahedron& tetra ) const
        {
            if( point_tetrahedron_position( center(), tetra )
                == Position::inside )
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
            return intersects( { vertices[0].get(), vertices[1].get(),
                       vertices[2].get() } )
                   || intersects( { vertices[0].get(), vertices[1].get(),
                       vertices[3].get() } )
                   || intersects( { vertices[0].get(), vertices[2].get(),
                       vertices[3].get() } )
                   || intersects( { vertices[1].get(), vertices[2].get(),
                       vertices[3].get() } );
        }

        template < index_t T = dimension >
        typename std::enable_if< T == 2, bool >::type intersects(
            const Segment< T >& segment ) const
        {
            const auto& vertices = segment.vertices();
            for( const auto v : LRange{ 2 } )
            {
                if( contains( vertices[v].get() ) )
                {
                    return true;
                }
            }
            const auto box_center = center();
            const auto box_extent = diagonal() / 2.;
            const Segment2D transformed_segment{ vertices[0].get() - box_center,
                vertices[1].get() - box_center };
            const auto segment_origin = transformed_segment.barycenter();
            const auto segment_extent = transformed_segment.length() / 2.;
            const auto segment_direction =
                transformed_segment.normalized_direction();
            for( const auto i : LRange{ 2 } )
            {
                const auto lhs = std::fabs( segment_origin.value( i ) );
                const auto rhs =
                    box_extent.value( i )
                    + segment_extent
                          * std::fabs( segment_direction.value( i ) );
                if( lhs > rhs )
                {
                    return false;
                }
            }
            const auto lhs = std::fabs(
                dot_perpendicular( segment_direction, segment_origin ) );
            const auto rhs = box_extent.value( 0 )
                                 * std::fabs( segment_direction.value( 1 ) )
                             + box_extent.value( 1 )
                                   * std::fabs( segment_direction.value( 0 ) );
            return lhs <= rhs;
        }

        double signed_distance( const Point< dimension >& point ) const
        {
            bool inside{ true };
            Vector< dimension > result;
            for( const auto c : LRange{ dimension } )
            {
                const auto value = point.value( c );
                if( value < min().value( c ) )
                {
                    inside = false;
                    result.set_value( c, value - min().value( c ) );
                }
                else if( value > max().value( c ) )
                {
                    inside = false;
                    result.set_value( c, value - max().value( c ) );
                }
            }
            if( !inside )
            {
                return result.length();
            }
            const auto Pmin = point - min();
            const auto Pmax = point - max();
            auto inner_distance = std::numeric_limits< double >::max();
            for( const auto c : geode::LRange{ dimension } )
            {
                const auto local_distance = std::min(
                    std::abs( Pmin.value( c ) ), std::abs( Pmax.value( c ) ) );
                inner_distance = std::min( inner_distance, local_distance );
            }
            return -inner_distance;
        }

        Point< dimension > center() const
        {
            return ( min() + max() ) / 2.;
        }

        Vector< dimension > diagonal() const
        {
            return { min(), max() };
        }

    private:
        bool line_intersects(
            const GenericLine< RefPoint< dimension >, dimension >& line ) const;

    private:
        Point< dimension > min_;
        Point< dimension > max_;
    };

    template <>
    bool BoundingBox< 3 >::Impl::line_intersects(
        const GenericLine< RefPoint< 3 >, 3 >& line ) const
    {
        const auto box_half_extent = diagonal() / 2.;
        const auto line_translated_origin = line.origin() - center();
        const auto origin_cross_direction =
            line.direction().cross( line_translated_origin );
        const Point3D abs_line_direction = {
            { std::fabs( line.direction().value( 0 ) ),
                std::fabs( line.direction().value( 1 ) ),
                std::fabs( line.direction().value( 2 ) ) }
        };
        for( const auto i : LRange{ 3 } )
        {
            const auto ii = i == 2 ? 0 : i + 1;
            const auto iii = ii == 2 ? 0 : ii + 1;
            if( std::fabs( origin_cross_direction.value( i ) )
                    - ( box_half_extent.value( ii )
                            * abs_line_direction.value( iii )
                        + box_half_extent.value( iii )
                              * abs_line_direction.value( ii ) )
                > global_epsilon )
            {
                return false;
            }
        }
        return true;
    }

    template <>
    bool BoundingBox< 2 >::Impl::line_intersects(
        const GenericLine< RefPoint< 2 >, 2 >& line ) const
    {
        const auto box_center = center();
        const auto box_half_extent = diagonal() / 2.;
        const auto line_translated_origin = line.origin() - box_center;
        const auto lhs = std::fabs(
            dot_perpendicular( line.direction(), line_translated_origin ) );
        const auto rhs = box_half_extent.value( 0 )
                             * std::fabs( line.direction().value( 1 ) )
                         + box_half_extent.value( 1 )
                               * std::fabs( line.direction().value( 0 ) );
        return lhs - rhs <= global_epsilon;
    }

    template <>
    bool BoundingBox< 1 >::Impl::line_intersects(
        const GenericLine< RefPoint< 1 >, 1 >& line ) const
    {
        if( diagonal().dot( line.direction() ) > 0 )
        {
            return line.origin().value( 0 ) < min().value( 0 );
        }
        return line.origin().value( 0 ) > max().value( 0 );
    }

    template <>
    template <>
    bool BoundingBox< 2 >::Impl::intersects< 2 >(
        const Triangle< 2 >& triangle ) const
    {
        if( point_triangle_position( center(), triangle ) == Position::inside )
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

    template <>
    template <>
    bool BoundingBox< 3 >::Impl::intersects< 3 >(
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
            interval.add_point( { { normal.dot( vertices[0].get() ) } } );
            interval.add_point( { { normal.dot( vertices[1].get() ) } } );
            interval.add_point( { { normal.dot( vertices[2].get() ) } } );
            return interval;
        };
        const auto bbox_projection = [this]( const Vector3D& normal ) {
            const auto origin = normal.dot( center() );
            const auto extent = diagonal();
            const auto maximum_extent =
                ( std::fabs( normal.value( 0 ) * extent.value( 0 ) )
                    + std::fabs( normal.value( 1 ) * extent.value( 1 ) )
                    + std::fabs( normal.value( 2 ) * extent.value( 2 ) ) )
                / 2.;
            BoundingBox1D interval;
            interval.add_point( { { origin - maximum_extent } } );
            interval.add_point( { { origin + maximum_extent } } );
            return interval;
        };
        const std::array< Vector3D, 3 > edges{ Vector3D{ vertices[0].get(),
                                                   vertices[1].get() },
            Vector3D{ vertices[0].get(), vertices[2].get() },
            Vector3D{ vertices[1].get(), vertices[2].get() } };

        // Test direction of triangle normal.
        const auto triangle_normal = edges[0].cross( edges[1] );
        if( !bbox_projection( triangle_normal )
                 .contains( { { triangle_normal.dot( vertices[0].get() ) } } ) )
        {
            return false;
        }

        // Test direction of box faces.
        const auto box_center = center();
        const auto box_diagonal = diagonal();
        for( const auto i : LRange{ 3 } )
        {
            Vector3D axis;
            axis.set_value( i, 1 );
            const auto triangle_interval = triangle_projection( axis );
            BoundingBox1D box_interval;
            const auto& center_value = box_center.value( i );
            const auto extent = box_diagonal.value( i ) / 2.;
            box_interval.add_point( { { center_value - extent } } );
            box_interval.add_point( { { center_value + extent } } );
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

    template < index_t dimension >
    BoundingBox< dimension >::BoundingBox() // NOLINT
    {
    }

    template < index_t dimension >
    BoundingBox< dimension >::~BoundingBox() // NOLINT
    {
    }

    template < index_t dimension >
    BoundingBox< dimension >::BoundingBox( const BoundingBox& other )
    {
        *impl_ = *other.impl_;
    }

    template < index_t dimension >
    BoundingBox< dimension >& BoundingBox< dimension >::operator=(
        const BoundingBox& other )
    {
        *impl_ = *other.impl_;
        return *this;
    }

    template < index_t dimension >
    BoundingBox< dimension >::BoundingBox( BoundingBox&& other ) noexcept
    {
        *impl_ = std::move( *other.impl_ );
    }

    template < index_t dimension >
    BoundingBox< dimension >& BoundingBox< dimension >::operator=(
        BoundingBox&& other ) noexcept
    {
        *impl_ = std::move( *other.impl_ );
        return *this;
    }

    template < index_t dimension >
    const Point< dimension >& BoundingBox< dimension >::min() const
    {
        return impl_->min();
    }

    template < index_t dimension >
    const Point< dimension >& BoundingBox< dimension >::max() const
    {
        return impl_->max();
    }

    template < index_t dimension >
    void BoundingBox< dimension >::add_point( const Point< dimension >& point )
    {
        impl_->add_point( point );
    }

    template < index_t dimension >
    void BoundingBox< dimension >::add_box(
        const BoundingBox< dimension >& box )
    {
        impl_->add_box( box );
    }

    template < index_t dimension >
    bool BoundingBox< dimension >::contains(
        const Point< dimension >& point ) const
    {
        return impl_->contains( point );
    }

    template < index_t dimension >
    bool BoundingBox< dimension >::intersects(
        const BoundingBox< dimension >& box ) const
    {
        return impl_->intersects( box );
    }

    template < index_t dimension >
    bool BoundingBox< dimension >::intersects(
        const Ray< dimension >& ray ) const
    {
        return impl_->intersects( ray );
    }

    template < index_t dimension >
    bool BoundingBox< dimension >::intersects(
        const InfiniteLine< dimension >& line ) const
    {
        return impl_->intersects( line );
    }

    template < index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 2, bool >::type
        BoundingBox< dimension >::intersects(
            const Segment< T >& segment ) const
    {
        return impl_->intersects( segment );
    }

    template < index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 2 || T == 3, bool >::type
        BoundingBox< dimension >::intersects(
            const Triangle< T >& triangle ) const
    {
        return impl_->intersects( triangle );
    }

    template < index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, bool >::type
        BoundingBox< dimension >::intersects( const Tetrahedron& tetra ) const
    {
        return impl_->intersects( tetra );
    }

    template < index_t dimension >
    Point< dimension > BoundingBox< dimension >::center() const
    {
        return impl_->center();
    }

    template < index_t dimension >
    Vector< dimension > BoundingBox< dimension >::diagonal() const
    {
        return impl_->diagonal();
    }

    template < index_t dimension >
    double BoundingBox< dimension >::signed_distance(
        const Point< dimension >& point ) const
    {
        return impl_->signed_distance( point );
    }

    template class opengeode_geometry_api BoundingBox< 1 >;
    template class opengeode_geometry_api BoundingBox< 2 >;
    template class opengeode_geometry_api BoundingBox< 3 >;

    template opengeode_geometry_api bool BoundingBox< 2 >::intersects< 2 >(
        const Segment< 2 >& ) const;
    template opengeode_geometry_api bool BoundingBox< 2 >::intersects< 2 >(
        const Triangle< 2 >& ) const;
    template opengeode_geometry_api bool BoundingBox< 3 >::intersects< 3 >(
        const Triangle< 3 >& ) const;
    template opengeode_geometry_api bool BoundingBox< 3 >::intersects< 3 >(
        const Tetrahedron& ) const;
} // namespace geode
