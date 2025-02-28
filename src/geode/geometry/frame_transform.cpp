/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geometry/frame_transform.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/frame.hpp>
#include <geode/geometry/vector.hpp>

namespace geode
{
    template < index_t dimension >
    class FrameTransform< dimension >::Impl
    {
    public:
        Impl( const Frame< dimension >& from, const Frame< dimension >& to )
        {
            for( const auto d_from : LRange{ dimension } )
            {
                const auto from_vector = from.direction( d_from ).normalize();
                double max_dot{ 0 };
                for( const auto d_to : LRange{ dimension } )
                {
                    const auto to_vector = to.direction( d_to ).normalize();
                    const auto dot = from_vector.dot( to_vector );
                    const auto abs_dot = std::fabs( dot );
                    if( abs_dot > max_dot )
                    {
                        directions_[d_from] = d_to;
                        max_dot = abs_dot;
                        orientations_[d_from] = dot > 0 ? 1 : -1;
                    }
                }
            }
        }

        local_index_t direction( local_index_t index ) const
        {
            return directions_[index];
        }

        signed_index_t orientation( local_index_t index ) const
        {
            return orientations_[index];
        }

        Frame< dimension > apply( const Frame< dimension >& frame ) const
        {
            Frame< dimension > result;
            for( const auto d : LRange{ dimension } )
            {
                result.set_direction(
                    direction( d ), frame.direction( d ) * orientation( d ) );
            }
            return result;
        }

        Point< dimension > apply( const Point< dimension >& point ) const
        {
            Point< dimension > result;
            for( const auto d : LRange{ dimension } )
            {
                result.set_value(
                    d, orientations_[d] * point.value( directions_[d] ) );
            }
            return result;
        }

        Vector< dimension > apply( const Vector< dimension >& vector ) const
        {
            Vector< dimension > result;
            for( const auto d : LRange{ dimension } )
            {
                result.set_value(
                    d, orientations_[d] * vector.value( directions_[d] ) );
            }
            return result;
        }

    private:
        std::array< local_index_t, dimension > directions_;
        std::array< signed_index_t, dimension > orientations_;
    };

    template < index_t dimension >
    FrameTransform< dimension >::FrameTransform(
        const Frame< dimension >& from, const Frame< dimension >& to )
        : impl_{ from, to }
    {
    }

    template < index_t dimension >
    FrameTransform< dimension >::~FrameTransform() = default;

    template < index_t dimension >
    local_index_t FrameTransform< dimension >::direction(
        local_index_t index ) const
    {
        return impl_->direction( index );
    }

    template < index_t dimension >
    signed_index_t FrameTransform< dimension >::orientation(
        local_index_t index ) const
    {
        return impl_->orientation( index );
    }

    template < index_t dimension >
    Frame< dimension > FrameTransform< dimension >::apply(
        const Frame< dimension >& frame ) const
    {
        return impl_->apply( frame );
    }

    template < index_t dimension >
    Vector< dimension > FrameTransform< dimension >::apply(
        const Vector< dimension >& vector ) const
    {
        return impl_->apply( vector );
    }

    template < index_t dimension >
    Point< dimension > FrameTransform< dimension >::apply(
        const Point< dimension >& point ) const
    {
        return impl_->apply( point );
    }
    template opengeode_geometry_api class FrameTransform< 2 >;
    template opengeode_geometry_api class FrameTransform< 3 >;
} // namespace geode
