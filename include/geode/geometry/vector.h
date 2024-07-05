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

#include <geode/basic/range.h>

#include <geode/geometry/common.h>
#include <geode/geometry/point.h>

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

        explicit Vector( const Point< dimension > &vector )
            : Point< dimension >( vector )
        {
        }

        explicit Vector( std::array< double, dimension > values )
            : Point< dimension >( std::move( values ) )
        {
        }

        Vector( const Point< dimension > &from, const Point< dimension > &to )
            : Point< dimension >( to - from )
        {
        }

        double length2() const
        {
            return dot( *this );
        }

        double length() const
        {
            return std::sqrt( length2() );
        }

        Vector operator*( double multiplier ) const
        {
            return detail::coords_multiply( *this, multiplier );
        }

        Vector operator/( double divider ) const
        {
            return detail::coords_divide( *this, divider );
        }

        Vector operator+( const Vector &other ) const
        {
            return detail::coords_add( *this, other );
        }

        Vector operator-( const Vector &other ) const
        {
            return detail::coords_substract( *this, other );
        }

        void operator*=( double multiplier )
        {
            detail::coords_multiply_equal( *this, multiplier );
        }

        void operator/=( double divider )
        {
            detail::coords_divide_equal( *this, divider );
        }

        void operator+=( const Vector &other )
        {
            detail::coords_add_equal( *this, other );
        }

        void operator-=( const Vector &other )
        {
            detail::coords_substract_equal( *this, other );
        }

        Vector normalize() const
        {
            return *this / length();
        }

        double dot( const Vector &other ) const
        {
            double result{ 0 };
            for( const auto i : LRange{ dimension } )
            {
                result += this->value( i ) * other.value( i );
            }
            return result;
        }

        Vector cross( const Vector &other ) const
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
    ALIAS_1D_AND_2D_AND_3D( Vector );

    template < index_t dimension >
    struct AttributeLinearInterpolationImpl< Vector< dimension > >
    {
        template < template < typename > class Attribute >
        static Vector< dimension > compute(
            const AttributeLinearInterpolation &interpolator,
            const Attribute< Vector< dimension > > &attribute )
        {
            Vector< dimension > result;
            for( const auto i : Indices{ interpolator.indices_ } )
            {
                result = result
                         + attribute.value( interpolator.indices_[i] )
                               * interpolator.lambdas_[i];
            }
            return result;
        }
    };

    template < index_t dimension >
    struct GenericAttributeConversion< Vector< dimension > >
    {
        static float converted_value( const Vector< dimension > &vector )
        {
            return converted_item_value( vector, 0 );
        }

        static float converted_item_value(
            const Vector< dimension > &vector, local_index_t item )
        {
            OPENGEODE_ASSERT( item < nb_items(),
                "[GenericAttributeConversion] Accessing "
                "incorrect item value" );
            return static_cast< float >( vector.value( item ) );
        }

        static bool is_genericable()
        {
            return true;
        }

        static local_index_t nb_items()
        {
            return dimension;
        }
    };
} // namespace geode
