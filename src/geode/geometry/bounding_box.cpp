/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

        void add_box( const BoundingBox< dimension >& box );

    private:
        Point< dimension > min_;
        Point< dimension > max_;
    };

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
        add_point( box.min() );
        add_point( box.max() );
    }

    template < index_t dimension >
    bool BoundingBox< dimension >::contains(
        const Point< dimension >& point ) const
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

    template < index_t dimension >
    bool BoundingBox< dimension >::intersects(
        const BoundingBox< dimension >& box ) const
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

    template class opengeode_geometry_api BoundingBox< 2 >;
    template class opengeode_geometry_api BoundingBox< 3 >;
} // namespace geode
