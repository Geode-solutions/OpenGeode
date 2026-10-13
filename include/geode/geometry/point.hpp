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

#include <array>

#include <absl/hash/hash.h>

#include <geode/basic/attribute_utils.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/common.hpp>
#include <geode/geometry/detail/point_operators.hpp>

namespace geode
{
    /*!
     * Description of a point in the given dimension with double coordinates
     */
    template < index_t dimension >
    class Point
    {
    public:
        static constexpr auto dim = dimension;

        Point()
        {
            values_.fill( 0 );
        }

        explicit Point( std::array< double, dimension > values )
            : values_( std::move( values ) )
        {
        }

        [[nodiscard]] double value( local_index_t index ) const
        {
            return values_[index];
        }

        void set_value( local_index_t index, double coordinate )
        {
            values_[index] = coordinate;
        }

        [[nodiscard]] bool operator==( const Point &other ) const
        {
            for( const auto i : LRange{ dimension } )
            {
                if( values_[i] != other.values_[i] )
                {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] bool operator!=( const Point &other ) const
        {
            return !( *this == other );
        }

        [[nodiscard]] bool operator<( const Point &other ) const
        {
            for( const auto i : LRange{ dimension } )
            {
                if( values_[i] < other.values_[i] )
                {
                    return true;
                }
                if( values_[i] > other.values_[i] )
                {
                    return false;
                }
            }
            return false;
        }

        [[nodiscard]] bool operator<=( const Point &other ) const
        {
            return operator<( other ) || operator==( other );
        }

        [[nodiscard]] Point operator*( double multiplier ) const
        {
            return detail::coords_multiply( *this, multiplier );
        }

        [[nodiscard]] Point operator/( double divider ) const
        {
            return detail::coords_divide( *this, divider );
        }

        [[nodiscard]] Point operator+( const Point &other ) const
        {
            return detail::coords_add( *this, other );
        }

        [[nodiscard]] Point operator-( const Point &other ) const
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

        [[nodiscard]] bool inexact_equal( const Point &other ) const
        {
            double square_length{ 0 };
            static constexpr auto SQR_EPSILON = GLOBAL_EPSILON * GLOBAL_EPSILON;
            for( const auto i : LRange{ dimension } )
            {
                const double diff{ other.values_[i] - values_[i] };
                square_length += diff * diff;
                if( square_length > SQR_EPSILON )
                {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] std::string string() const;

        [[nodiscard]] Point< dimension - 1 > project_point(
            local_index_t axis_to_remove ) const;

        template < typename H >
        friend H AbslHashValue( H h, const Point &point )
        {
            return H::combine( std::move( h ), point.values_ );
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive &serializer );

    private:
        std::array< double, dimension > values_;
    };
    ALIAS_1D_AND_2D_AND_3D( Point );

    template < index_t dimension >
    struct AttributeLinearInterpolationImpl< Point< dimension > >
    {
        template < template < typename > class Attribute >
        [[nodiscard]] static Point< dimension > compute(
            const AttributeLinearInterpolation &interpolator,
            const Attribute< Point< dimension > > &attribute )
        {
            Point< dimension > result;
            bool is_same{ true };
            const auto &first_value =
                attribute.value( interpolator.indices_[0] );
            for( const auto i : Indices{ interpolator.indices_ } )
            {
                const auto &i_value =
                    attribute.value( interpolator.indices_[i] );
                if( is_same )
                {
                    is_same = i_value == first_value;
                }
                result = result + i_value * interpolator.lambdas_[i];
            }
            if( is_same )
            {
                return first_value;
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
            OpenGeodeGeometryException::check_assertion( item < nb_items(),
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
} // namespace geode

namespace std
{
    template <>
    struct opengeode_geometry_api hash< geode::Point1D >
    {
        size_t operator()( const geode::Point1D &point ) const;
    };

    template <>
    struct opengeode_geometry_api hash< geode::Point2D >
    {
        size_t operator()( const geode::Point2D &point ) const;
    };

    template <>
    struct opengeode_geometry_api hash< geode::Point3D >
    {
        size_t operator()( const geode::Point3D &point ) const;
    };
} // namespace std
