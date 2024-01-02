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

#include <array>

#include <absl/hash/hash.h>

#include <geode/basic/attribute_utils.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/range.h>

#include <geode/image/common.h>

namespace geode
{
    /*!
     * Description of a color in RGB
     */
    class RGBColor
    {
    public:
        RGBColor()
        {
            values_.fill( 0 );
        }

        RGBColor( local_index_t red, local_index_t green, local_index_t blue )
            : values_( { red, green, blue } )
        {
        }

        local_index_t red() const
        {
            return values_[0];
        }

        local_index_t green() const
        {
            return values_[1];
        }

        local_index_t blue() const
        {
            return values_[2];
        }

        void set_red( local_index_t red_scale )
        {
            values_[0] = red_scale;
        }

        void set_green( local_index_t green_scale )
        {
            values_[1] = green_scale;
        }

        void set_blue( local_index_t blue_scale )
        {
            values_[2] = blue_scale;
        }

        bool operator==( const RGBColor &other ) const
        {
            if( red() != other.red() )
            {
                return false;
            }
            if( green() != other.green() )
            {
                return false;
            }
            if( blue() != other.blue() )
            {
                return false;
            }
            return true;
        }

        bool operator!=( const RGBColor &other ) const
        {
            return !( *this == other );
        }

        RGBColor operator+( const RGBColor &other ) const
        {
            RGBColor result;
            result.set_red( ( result.red() + other.red() ) / 2 );
            result.set_green( ( result.green() + other.green() ) / 2 );
            result.set_blue( ( result.blue() + other.blue() ) / 2 );
            return result;
        }

        void operator+=( const RGBColor &other )
        {
            set_red( ( red() + other.red() ) / 2 );
            set_green( ( green() + other.green() ) / 2 );
            set_blue( ( blue() + other.blue() ) / 2 );
        }

        std::string string() const
        {
            return absl::StrCat( values_[0], " ", values_[1], " ", values_[2] );
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive &archive )
        {
            archive.ext( *this, Growable< Archive, RGBColor >{
                                    { []( Archive &a, RGBColor &color ) {
                                        a.container1b( color.values_ );
                                    } } } );
        }

    private:
        std::array< local_index_t, 3 > values_;
    };

    template <>
    struct AttributeLinearInterpolationImpl< RGBColor >
    {
        template < template < typename > class Attribute >
        static RGBColor compute(
            const AttributeLinearInterpolation &interpolator,
            const Attribute< RGBColor > &attribute )
        {
            RGBColor result;
            for( const auto i : Indices{ interpolator.indices_ } )
            {
                const auto &att_value =
                    attribute.value( interpolator.indices_[i] );
                result.set_red(
                    result.red() + att_value.red() * interpolator.lambdas_[i] );
                result.set_green(
                    result.green()
                    + att_value.green() * interpolator.lambdas_[i] );
                result.set_blue(
                    result.blue()
                    + att_value.blue() * interpolator.lambdas_[i] );
            }
            return result;
        }
    };

    template <>
    struct GenericAttributeConversion< RGBColor >
    {
        static float converted_value( const RGBColor &color )
        {
            return converted_item_value( color, 0 );
        }

        static float converted_item_value(
            const RGBColor &color, local_index_t item )
        {
            OPENGEODE_ASSERT( item < nb_items(),
                "[GenericAttributeConversion] Accessing "
                "incorrect item value" );
            if( item == 0 )
            {
                return static_cast< float >( color.red() );
            }
            if( item == 1 )
            {
                return static_cast< float >( color.green() );
            }
            return static_cast< float >( color.blue() );
        }

        static bool is_genericable()
        {
            return true;
        }
        static local_index_t nb_items()

        {
            return 3;
        }
    };
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::RGBColor >
    {
    public:
        size_t operator()( const geode::RGBColor &color ) const
        {
            return absl::Hash< geode::local_index_t >()( color.red() )
                   ^ absl::Hash< geode::local_index_t >()( color.green() )
                   ^ absl::Hash< geode::local_index_t >()( color.blue() );
        }
    };
} // namespace std
