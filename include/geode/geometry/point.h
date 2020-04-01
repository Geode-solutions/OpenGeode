/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <array>
#include <iostream>

#include <bitsery/bitsery.h>

#include <geode/basic/attribute_utils.h>
#include <geode/basic/range.h>

#include <geode/geometry/bitsery_archive.h>
#include <geode/geometry/common.h>

namespace geode
{
    /*!
     * Description of a point in the given dimension with double coordinates
     */
    template < index_t dimension >
    class Point
    {
    public:
        Point()
        {
            values_.fill( 0 );
        }

        Point( std::array< double, dimension > values )
            : values_( std::move( values ) )
        {
        }

        double value( index_t index ) const
        {
            return values_.at( index );
        }

        void set_value( index_t index, double value )
        {
            values_.at( index ) = value;
        }

        bool operator==( const Point &other ) const
        {
            for( const auto i : Range{ dimension } )
            {
                if( value( i ) != other.value( i ) )
                {
                    return false;
                }
            }
            return true;
        }

        bool operator!=( const Point &other ) const
        {
            return !( *this == other );
        }

        Point operator*( double multiplier ) const
        {
            Point result{ *this };
            for( const auto i : Range{ dimension } )
            {
                result.set_value( i, result.value( i ) * multiplier );
            }
            return result;
        }

        Point operator/( double divider ) const
        {
            OPENGEODE_EXCEPTION( std::fabs( divider ) > 1e-30,
                "[Point::operator/] Cannot divide Point by something close to "
                "zero" );
            Point result{ *this };
            for( const auto i : Range{ dimension } )
            {
                result.set_value( i, result.value( i ) / divider );
            }
            return result;
        }

        Point operator+( const Point &other ) const
        {
            Point result{ *this };
            for( const auto i : Range{ dimension } )
            {
                result.set_value( i, result.value( i ) + other.value( i ) );
            }
            return result;
        }

        Point operator-( const Point &other ) const
        {
            Point result{ *this };
            for( const auto i : Range{ dimension } )
            {
                result.set_value( i, result.value( i ) - other.value( i ) );
            }
            return result;
        }

        bool inexact_equal( const Point &other, double epsilon ) const
        {
            double square_length{ 0 };
            for( const auto i : Range{ dimension } )
            {
                const double diff{ other.value( i ) - this->value( i ) };
                square_length += diff * diff;
            }
            return square_length < epsilon * epsilon;
        }

        std::string string() const
        {
            std::string result;
            const auto *sep = "";
            for( const auto i : Range{ dimension } )
            {
                absl::StrAppend( &result, sep, value( i ) );
                sep = " ";
            }
            return result;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive &archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Point >{},
                []( Archive &archive, Point &point ) {
                    archive.container8b( point.values_ );
                } );
        }

    private:
        std::array< double, dimension > values_;
    };
    ALIAS_2D_AND_3D( Point );

    template < index_t dimension >
    struct AttributeLinearInterpolationImpl< Point< dimension > >
    {
        template < template < typename > class Attribute >
        static Point< dimension > compute(
            const AttributeLinearInterpolation &interpolator,
            const Attribute< Point< dimension > > &attribute )
        {
            Point< dimension > result;
            for( auto i : Range{ interpolator.indices_.size() } )
            {
                result = result
                         + attribute.value( interpolator.indices_[i] )
                               * interpolator.lambdas_[i];
            }
            return result;
        }
    };
} // namespace geode
