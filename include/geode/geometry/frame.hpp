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

#pragma once

#include <geode/geometry/common.hpp>
#include <geode/geometry/vector.hpp>

namespace geode
{
    template < index_t dimension >
    class Frame
    {
    public:
        Frame();
        explicit Frame(
            std::array< Vector< dimension >, dimension > directions );

        [[nodiscard]] const Vector< dimension >& direction(
            local_index_t index ) const;

        void set_direction(
            local_index_t index, Vector< dimension > direction );

        [[nodiscard]] Frame inverse() const;

        Frame operator+( const Frame& rhs ) const;

        Vector< dimension > operator*( const Vector< dimension >& rhs ) const;

        [[nodiscard]] Frame rescale( double length ) const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] local_index_t max_elongation_direction() const;

        [[nodiscard]] local_index_t min_elongation_direction() const;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        std::array< Vector< dimension >, dimension > frame_;
    };
    ALIAS_1D_AND_2D_AND_3D( Frame );
} // namespace geode