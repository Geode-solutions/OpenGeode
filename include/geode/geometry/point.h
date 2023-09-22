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

#include <array>
#include <limits>
#include <sstream>

#include <absl/hash/hash.h>

#include <bitsery/bitsery.h>

#include <geode/basic/attribute_utils.h>
#include <geode/basic/range.h>

#include <geode/geometry/bitsery_archive.h>
#include <geode/geometry/common.h>
#include <geode/geometry/detail/point_operators.h>

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

        double value( local_index_t index ) const
        {
            return values_[index];
        }

        void set_value( local_index_t index, double coordinate )
        {
            values_[index] = coordinate;
        }

        bool operator==( const Point &other ) const
        {
            for( const auto i : LRange{ dimension } )
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

        bool operator<( const Point &other ) const
        {
            for( const auto i : LRange{ dimension } )
            {
                if( value( i ) < other.value( i ) )
                {
                    return true;
                }
                if( value( i ) > other.value( i ) )
                {
                    return false;
                }
            }
            return false;
        }

        bool operator<=( const Point &other ) const
        {
            return operator<( other ) || operator==( other );
        }

        Point operator*( double multiplier ) const
        {
            return detail::coords_multiply( *this, multiplier );
        }

        Point operator/( double divider ) const
        {
            return detail::coords_divide( *this, divider );
        }

        Point operator+( const Point &other ) const
        {
            return detail::coords_add( *this, other );
        }

        Point operator-( const Point &other ) const
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

        void operator+=( const Point &other )
        {
            detail::coords_add_equal( *this, other );
        }

        void operator-=( const Point &other )
        {
            detail::coords_substract_equal( *this, other );
        }

        bool inexact_equal( const Point &other ) const
        {
            double square_length{ 0 };
            static constexpr auto sqr_epsilon = global_epsilon * global_epsilon;
            for( const auto i : LRange{ dimension } )
            {
                const double diff{ other.value( i ) - this->value( i ) };
                square_length += diff * diff;
                if( square_length > sqr_epsilon )
                {
                    return false;
                }
            }
            return true;
        }

        std::string string() const
        {
            std::ostringstream oss;
            oss.precision( std::numeric_limits< double >::digits10 );
            const auto *sep = "";
            for( const auto i : LRange{ dimension } )
            {
                oss << sep << value( i );
                sep = " ";
            }
            return oss.str();
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive &archive )
        {
            archive.ext( *this,
                Growable< Archive, Point >{ { []( Archive &a, Point &point ) {
                    a.container8b( point.values_ );
                } } } );
        }

    private:
        std::array< double, dimension > values_;
    };
    ALIAS_1D_AND_2D_AND_3D( Point );

    template < index_t dimension >
    struct AttributeLinearInterpolationImpl< Point< dimension > >
    {
        template < template < typename > class Attribute >
        static Point< dimension > compute(
            const AttributeLinearInterpolation &interpolator,
            const Attribute< Point< dimension > > &attribute )
        {
            Point< dimension > result;
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
    struct GenericAttributeConversion< Point< dimension > >
    {
        static float converted_value( const Point< dimension > &point )
        {
            return converted_item_value( point, 0 );
        }

        static float converted_item_value(
            const Point< dimension > &point, local_index_t item )
        {
            OPENGEODE_ASSERT( item < nb_items(),
                "[GenericAttributeConversion] Accessing "
                "incorrect item value" );
            return static_cast< float >( point.value( item ) );
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

    template < index_t dimension >
    class OpenGeodePointException : public OpenGeodeException
    {
    public:
        template < typename... Args >
        explicit OpenGeodePointException(
            Point< dimension > point_in, const Args &...message )
            : OpenGeodeException{ absl::StrCat(
                message..., " at ", point_in.string() ) },
              point{ std::move( point_in ) }
        {
        }

        Point< dimension > point;
    };
    ALIAS_1D_AND_2D_AND_3D( OpenGeodePointException );
} // namespace geode

// NOLINTNEXTLINE
#define OPENGEODE_POINT_EXCEPTION( dimension, condition, point, ... )          \
    if( ABSL_PREDICT_FALSE( !( condition ) ) )                                 \
        throw geode::OpenGeodePointException< dimension >                      \
        {                                                                      \
            point, __VA_ARGS__                                                 \
        }

// NOLINTNEXTLINE
#define OPENGEODE_POINT_EXCEPTION1D( condition, point, ... )                   \
    OPENGEODE_POINT_EXCEPTION( 1, condition, point, __VA_ARGS__ )

// NOLINTNEXTLINE
#define OPENGEODE_POINT_EXCEPTION2D( condition, point, ... )                   \
    OPENGEODE_POINT_EXCEPTION( 2, condition, point, __VA_ARGS__ )

// NOLINTNEXTLINE
#define OPENGEODE_POINT_EXCEPTION3D( condition, point, ... )                   \
    OPENGEODE_POINT_EXCEPTION( 3, condition, point, __VA_ARGS__ )

namespace std
{
    template <>
    struct hash< geode::Point1D >
    {
    public:
        size_t operator()( const geode::Point1D &point ) const
        {
            return absl::Hash< double >()( point.value( 0 ) );
        }
    };

    template <>
    struct hash< geode::Point2D >
    {
    public:
        size_t operator()( const geode::Point2D &point ) const
        {
            return absl::Hash< double >()( point.value( 0 ) )
                   ^ absl::Hash< double >()( point.value( 1 ) );
        }
    };

    template <>
    struct hash< geode::Point3D >
    {
    public:
        size_t operator()( const geode::Point3D &point ) const
        {
            return absl::Hash< double >()( point.value( 0 ) )
                   ^ absl::Hash< double >()( point.value( 1 ) )
                   ^ absl::Hash< double >()( point.value( 2 ) );
        }
    };
} // namespace std
