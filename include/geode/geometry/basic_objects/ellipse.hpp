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

#pragma once

#include <geode/geometry/common.hpp>
#include <geode/geometry/frame.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( Frame );
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerEllipse );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );

    template < index_t dimension >
    using RefPoint = std::reference_wrapper< const Point< dimension > >;
    ALIAS_2D_AND_3D( RefPoint );
    template < index_t dimension >
    using RefFrame = std::reference_wrapper< const Frame< dimension > >;
    ALIAS_2D_AND_3D( RefFrame );
} // namespace geode

namespace geode
{
    template < typename PointType, typename FrameType, index_t dimension >
    class GenericEllipse
    {
    public:
        static constexpr auto dim = dimension;

        GenericEllipse( PointType center, FrameType axis );

        GenericEllipse(
            const GenericEllipse< PointType, FrameType, dimension >& other );
        GenericEllipse< PointType, FrameType, dimension >& operator=(
            const GenericEllipse< PointType, FrameType, dimension >& other );
        GenericEllipse( GenericEllipse< PointType, FrameType, dimension >&&
                other ) noexcept;
        GenericEllipse< PointType, FrameType, dimension >& operator=(
            GenericEllipse< PointType, FrameType, dimension >&&
                other ) noexcept;

        [[nodiscard]] const Point< dimension >& center() const;
        [[nodiscard]] const Frame< dimension >& axes() const;
        [[nodiscard]] BoundingBox< dimension > bounding_box() const;

    private:
        PointType center_;
        FrameType axes_;
    };

    template < index_t dimension >
    class OwnerEllipse : public GenericEllipse< Point< dimension >,
                             Frame< dimension >,
                             dimension >
    {
        using Base =
            GenericEllipse< Point< dimension >, Frame< dimension >, dimension >;

    public:
        explicit OwnerEllipse(
            Point< dimension > center, Frame< dimension > axes );

        OwnerEllipse( const OwnerEllipse< dimension >& other );
        OwnerEllipse< dimension >& operator=(
            const OwnerEllipse< dimension >& other );
        OwnerEllipse( OwnerEllipse< dimension >&& other ) noexcept;
        OwnerEllipse< dimension >& operator=(
            OwnerEllipse< dimension >&& other ) noexcept;
    };
    ALIAS_2D_AND_3D( OwnerEllipse );

    template < index_t dimension >
    class Ellipse : public GenericEllipse< RefPoint< dimension >,
                        RefFrame< dimension >,
                        dimension >
    {
        using Base = GenericEllipse< RefPoint< dimension >,
            RefFrame< dimension >,
            dimension >;

    public:
        Ellipse(
            const Point< dimension >& center, const Frame< dimension >& axes );
        Ellipse( const Ellipse< dimension >& other );
        Ellipse( const OwnerEllipse< dimension >& other );
        Ellipse< dimension >& operator=( const Ellipse< dimension >& other );
        Ellipse( Ellipse< dimension >&& other ) noexcept;
        Ellipse< dimension >& operator=(
            Ellipse< dimension >&& other ) noexcept;
    };
    ALIAS_2D_AND_3D( Ellipse );
} // namespace geode
