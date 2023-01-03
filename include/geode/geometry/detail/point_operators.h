/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/range.h>

namespace geode
{
    namespace detail
    {
        template < template < index_t > class Coords, index_t dimension >
        Coords< dimension > coords_multiply(
            const Coords< dimension > &input, double multiplier )
        {
            auto result = input;
            for( const auto i : LRange{ dimension } )
            {
                result.set_value( i, result.value( i ) * multiplier );
            }
            return result;
        }

        template < template < index_t > class Coords, index_t dimension >
        Coords< dimension > coords_divide(
            const Coords< dimension > &input, double divider )
        {
            OPENGEODE_EXCEPTION( std::fabs( divider ) > 1e-30,
                "[Point::operator/] Cannot divide Point by something close to "
                "zero" );
            auto result = input;
            for( const auto i : LRange{ dimension } )
            {
                result.set_value( i, result.value( i ) / divider );
            }
            return result;
        }

        template < template < index_t > class Coords, index_t dimension >
        Coords< dimension > coords_add(
            const Coords< dimension > &input, const Coords< dimension > &other )
        {
            auto result = input;
            for( const auto i : LRange{ dimension } )
            {
                result.set_value( i, result.value( i ) + other.value( i ) );
            }
            return result;
        }

        template < template < index_t > class Coords, index_t dimension >
        Coords< dimension > coords_substract(
            const Coords< dimension > &input, const Coords< dimension > &other )
        {
            auto result = input;
            for( const auto i : LRange{ dimension } )
            {
                result.set_value( i, result.value( i ) - other.value( i ) );
            }
            return result;
        }

        template < template < index_t > class Coords, index_t dimension >
        void coords_multiply_equal(
            Coords< dimension > &input, double multiplier )
        {
            for( const auto i : LRange{ dimension } )
            {
                input.set_value( i, input.value( i ) * multiplier );
            }
        }

        template < template < index_t > class Coords, index_t dimension >
        void coords_divide_equal( Coords< dimension > &input, double divider )
        {
            for( const auto i : LRange{ dimension } )
            {
                input.set_value( i, input.value( i ) / divider );
            }
        }

        template < template < index_t > class Coords, index_t dimension >
        void coords_add_equal(
            Coords< dimension > &input, const Coords< dimension > &other )
        {
            for( const auto i : LRange{ dimension } )
            {
                input.set_value( i, input.value( i ) + other.value( i ) );
            }
        }

        template < template < index_t > class Coords, index_t dimension >
        void coords_substract_equal(
            Coords< dimension > &input, const Coords< dimension > &other )
        {
            for( const auto i : LRange{ dimension } )
            {
                input.set_value( i, input.value( i ) - other.value( i ) );
            }
        }
    } // namespace detail
} // namespace geode
