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

#include <geode/geometry/bounding_box.h>

#include <limits>

#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/perpendicular.h>
#include <geode/geometry/point.h>

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
            const auto box_half_extent = ( diagonal() ) / 2.;
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
        bool line_intersects( const InfiniteLine< dimension >& line ) const;

    private:
        Point< dimension > min_;
        Point< dimension > max_;
    };

    template <>
    bool BoundingBox< 3 >::Impl::line_intersects(
        const InfiniteLine3D& line ) const
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
        const InfiniteLine2D& line ) const
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

    template class opengeode_geometry_api BoundingBox< 2 >;
    template class opengeode_geometry_api BoundingBox< 3 >;
} // namespace geode
