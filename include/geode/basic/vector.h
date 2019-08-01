/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/common.h>
#include <geode/basic/point.h>
#include <geode/basic/range.h>

namespace geode
{
    /*!
     * Description of a vector in the given dimension with double coordinates
     */
    template < index_t dimension >
    class Vector : public Point< dimension >
    {
    public:
        Vector() = default;

        Vector( const Point< dimension > &vector )
            : Point< dimension >( vector )
        {
        }

        Vector( std::array< double, dimension > values )
            : Point< dimension >( std::move( values ) )
        {
        }

        Vector( const Point< dimension > &from, const Point< dimension > &to )
            : Point< dimension >( to - from )
        {
        }

        double length() const
        {
            double result2{ 0 };
            for( auto i : Range{ dimension } )
            {
                result2 += this->value( i ) * this->value( i );
            }
            return std::sqrt( result2 );
        }

        Vector normalize() const
        {
            return *this / length();
        }

        double dot( const Vector &other )
        {
            double result{ 0 };
            for( auto i : Range{ dimension } )
            {
                result += this->value( i ) * other.value( i );
            }
            return result;
        }

        Vector cross( const Vector &other )
        {
            static_assert(
                dimension == 3, "Cross product only possible in 3D" );
            return Vector{ { this->value( 1 ) * other.value( 2 )
                                 - this->value( 2 ) * other.value( 1 ),
                this->value( 2 ) * other.value( 0 )
                    - this->value( 0 ) * other.value( 2 ),
                this->value( 0 ) * other.value( 1 )
                    - this->value( 1 ) * other.value( 0 ) } };
        }
    };
    ALIAS_2D_AND_3D( Vector );
} // namespace geode
