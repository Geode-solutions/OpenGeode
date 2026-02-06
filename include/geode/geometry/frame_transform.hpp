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

#include <geode/basic/pimpl.hpp>

#include <geode/geometry/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Frame );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class FrameTransform
    {
    public:
        FrameTransform(
            const Frame< dimension >& from, const Frame< dimension >& to );
        virtual ~FrameTransform();

        [[nodiscard]] virtual local_index_t direction(
            local_index_t index ) const;

        [[nodiscard]] virtual signed_index_t orientation(
            local_index_t index ) const;

        [[nodiscard]] virtual Frame< dimension > apply(
            const Frame< dimension >& frame ) const;

        [[nodiscard]] virtual Vector< dimension > apply(
            const Vector< dimension >& vector ) const;

        [[nodiscard]] virtual Point< dimension > apply(
            const Point< dimension >& point ) const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( FrameTransform );
} // namespace geode