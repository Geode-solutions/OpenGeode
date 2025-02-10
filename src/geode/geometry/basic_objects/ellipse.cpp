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

#include <geode/geometry/basic_objects/ellipse.hpp>

namespace geode
{
    template < typename PointType, typename FrameType, index_t dimension >
    GenericEllipse< PointType, FrameType, dimension >::GenericEllipse(
        PointType center, FrameType axes )
        : center_( std::move( center ) ), axes_( std::move( axes ) )
    {
    }
    template < typename PointType, typename FrameType, index_t dimension >
    GenericEllipse< PointType, FrameType, dimension >::GenericEllipse(
        const GenericEllipse< PointType, FrameType, dimension >& ) = default;
    template < typename PointType, typename FrameType, index_t dimension >
    GenericEllipse< PointType, FrameType, dimension >&
        GenericEllipse< PointType, FrameType, dimension >::operator=(
            const GenericEllipse< PointType, FrameType, dimension >& ) =
            default;
    template < typename PointType, typename FrameType, index_t dimension >
    GenericEllipse< PointType, FrameType, dimension >::GenericEllipse(
        GenericEllipse< PointType, FrameType, dimension >&& ) noexcept =
        default;

    template < typename PointType, typename FrameType, index_t dimension >
    GenericEllipse< PointType, FrameType, dimension >&
        GenericEllipse< PointType, FrameType, dimension >::operator=(
            GenericEllipse< PointType, FrameType, dimension >&& ) noexcept =
            default;

    template < typename PointType, typename FrameType, index_t dimension >
    const Point< dimension >&
        GenericEllipse< PointType, FrameType, dimension >::center() const
    {
        return center_;
    }
    template < typename PointType, typename FrameType, index_t dimension >
    const Frame< dimension >&
        GenericEllipse< PointType, FrameType, dimension >::axes() const
    {
        return axes_;
    }

    template < index_t dimension >
    OwnerEllipse< dimension >::OwnerEllipse(
        Point< dimension > center, Frame< dimension > axes )
        : Base( std::move( center ), std::move( axes ) )
    {
    }
    template < index_t dimension >
    OwnerEllipse< dimension >::OwnerEllipse(
        const OwnerEllipse< dimension >& ) = default;
    template < index_t dimension >
    OwnerEllipse< dimension >& OwnerEllipse< dimension >::operator=(
        const OwnerEllipse< dimension >& ) = default;
    template < index_t dimension >
    OwnerEllipse< dimension >::OwnerEllipse(
        OwnerEllipse< dimension >&& ) noexcept = default;
    template < index_t dimension >
    OwnerEllipse< dimension >& OwnerEllipse< dimension >::operator=(
        OwnerEllipse< dimension >&& ) noexcept = default;

    template < index_t dimension >
    Ellipse< dimension >::Ellipse(
        const Point< dimension >& center, const Frame< dimension >& axes )
        : Base( center, axes )
    {
    }
    template < index_t dimension >
    Ellipse< dimension >::Ellipse( const Ellipse< dimension >& ) = default;
    template < index_t dimension >
    Ellipse< dimension >::Ellipse( const OwnerEllipse< dimension >& other )
        : Base( other.center(), other.axes() )
    {
    }
    template < index_t dimension >
    Ellipse< dimension >& Ellipse< dimension >::operator=(
        const Ellipse< dimension >& ) = default;
    template < index_t dimension >
    Ellipse< dimension >::Ellipse( Ellipse< dimension >&& ) noexcept = default;
    template < index_t dimension >
    Ellipse< dimension >& Ellipse< dimension >::operator=(
        Ellipse< dimension >&& ) noexcept = default;

    template class opengeode_geometry_api
        GenericEllipse< Point< 2 >, Frame< 2 >, 2 >;
    template class opengeode_geometry_api
        GenericEllipse< Point< 3 >, Frame< 3 >, 3 >;
    template class opengeode_geometry_api
        GenericEllipse< RefPoint< 2 >, RefFrame< 2 >, 2 >;
    template class opengeode_geometry_api
        GenericEllipse< RefPoint< 3 >, RefFrame< 3 >, 3 >;
    template class opengeode_geometry_api OwnerEllipse< 2 >;
    template class opengeode_geometry_api OwnerEllipse< 3 >;
    template class opengeode_geometry_api Ellipse< 2 >;
    template class opengeode_geometry_api Ellipse< 3 >;
} // namespace geode
