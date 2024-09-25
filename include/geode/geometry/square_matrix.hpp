/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/basic/range.hpp>

#include <geode/geometry/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
}

namespace geode
{
    /*!
     * Description of a vector in the given dimension with double coordinates
     */
    template < index_t dimension >
    class SquareMatrix
    {
    public:
        SquareMatrix() = default;
        explicit SquareMatrix( double default_value );
        explicit SquareMatrix(
            std::array< Vector< dimension >, dimension > matrix_rows );

        [[nodiscard]] double value(
            local_index_t row, local_index_t column ) const;

        [[nodiscard]] const Vector< dimension >& row( local_index_t row ) const;

        void set_value( local_index_t row, local_index_t column, double value );

        [[nodiscard]] Vector< dimension > operator*(
            const Vector< dimension >& vector ) const;

        [[nodiscard]] SquareMatrix< dimension > operator*(
            const SquareMatrix< dimension >& matrix ) const;

        void operator+=( const SquareMatrix< dimension >& other );

        void operator-=( const SquareMatrix< dimension >& other );

        void operator*=( double multiplier );

        [[nodiscard]] double determinant() const;

        [[nodiscard]] SquareMatrix< dimension > transpose() const;

        [[nodiscard]] SquareMatrix< dimension > inverse() const;

    private:
        std::array< Vector< dimension >, dimension > matrix_rows_;
    };
    ALIAS_2D_AND_3D( SquareMatrix );
} // namespace geode
