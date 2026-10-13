/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <absl/algorithm/container.h>

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
            std::array< std::array< double, dimension >, dimension > cosines;
            for( const auto d_from : LRange{ dimension } )
            {
                const auto from_vector = from.direction( d_from ).normalize();
                for( const auto d_to : LRange{ dimension } )
                {
                    cosines[d_from][d_to] =
                        from_vector.dot( to.direction( d_to ).normalize() );
                }
            }
            // Choose the permutation maximizing the sum of |cos|, the first
            // one in lexicographic order on ties
            std::array< local_index_t, dimension > permutation;
            absl::c_iota( permutation, 0 );
            double max_sum{ -1 };
            do
            {
                double sum{ 0 };
                for( const auto d_from : LRange{ dimension } )
                {
                    sum += std::fabs( cosines[d_from][permutation[d_from]] );
                }
                if( sum > max_sum )
                {
                    max_sum = sum;
                    directions_ = permutation;
                }
            } while( absl::c_next_permutation( permutation ) );
            for( const auto d_from : LRange{ dimension } )
            {
                orientations_[d_from] =
                    cosines[d_from][directions_[d_from]] > 0 ? 1 : -1;
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
    template class opengeode_geometry_api FrameTransform< 2 >;
    template class opengeode_geometry_api FrameTransform< 3 >;
} // namespace geode
