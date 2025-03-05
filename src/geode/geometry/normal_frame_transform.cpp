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

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/frame.hpp>
#include <geode/geometry/normal_frame_transform.hpp>
#include <geode/geometry/vector.hpp>

namespace geode
{
    template < index_t dimension >
    class NormalFrameTransform< dimension >::Impl
    {
    public:
        Impl( const NormalFrameTransform< dimension >& normal_frame_transform,
            const Frame< dimension >& from,
            const Frame< dimension >& to )
        {
            for( const auto d_from : LRange{ dimension } )
            {
                directions_[d_from] =
                    normal_frame_transform.direction( d_from );
                orientations_[d_from] =
                    normal_frame_transform.orientation( d_from );
                magnitudes_[d_from] =
                    to.direction( normal_frame_transform.direction( d_from ) )
                        .length()
                    / from.direction( d_from ).length();
            }
        }

        Frame< dimension > apply( const Frame< dimension >& frame ) const
        {
            Frame< dimension > result;
            for( const auto d : LRange{ dimension } )
            {
                result.set_direction( directions_[d],
                    frame.direction( d ) * orientations_[d] * magnitudes_[d] );
            }
            return result;
        }

        Point< dimension > apply( const Point< dimension >& point ) const
        {
            Point< dimension > result;
            for( const auto d : LRange{ dimension } )
            {
                result.set_value( d, orientations_[d]
                                         * point.value( directions_[d] )
                                         * magnitudes_[d] );
            }
            return result;
        }

        Vector< dimension > apply( const Vector< dimension >& vector ) const
        {
            Vector< dimension > result;
            for( const auto d : LRange{ dimension } )
            {
                result.set_value( d, orientations_[d]
                                         * vector.value( directions_[d] )
                                         * magnitudes_[d] );
            }
            return result;
        }

    private:
        std::array< local_index_t, dimension > directions_;
        std::array< signed_index_t, dimension > orientations_;
        std::array< double, dimension > magnitudes_;
    };

    template < index_t dimension >
    NormalFrameTransform< dimension >::NormalFrameTransform(
        const Frame< dimension >& from, const Frame< dimension >& to )
        : FrameTransform< dimension >( from, to ), impl_{ *this, from, to }
    {
    }

    template < index_t dimension >
    NormalFrameTransform< dimension >::~NormalFrameTransform() = default;

    template < index_t dimension >
    Frame< dimension > NormalFrameTransform< dimension >::apply(
        const Frame< dimension >& frame ) const
    {
        return impl_->apply( frame );
    }

    template < index_t dimension >
    Vector< dimension > NormalFrameTransform< dimension >::apply(
        const Vector< dimension >& vector ) const
    {
        return impl_->apply( vector );
    }

    template < index_t dimension >
    Point< dimension > NormalFrameTransform< dimension >::apply(
        const Point< dimension >& point ) const
    {
        return impl_->apply( point );
    }
    template opengeode_geometry_api class NormalFrameTransform< 2 >;
    template opengeode_geometry_api class NormalFrameTransform< 3 >;
} // namespace geode
