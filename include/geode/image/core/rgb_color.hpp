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

#include <geode/basic/attribute_utils.hpp>
#include <geode/basic/range.hpp>

#include <geode/image/common.hpp>
#include <geode/image/core/detail/color_channel.hpp>

namespace geode
{
    class opengeode_image_api RGBColor
    {
    public:
        constexpr RGBColor() = default;

        constexpr RGBColor(
            local_index_t red, local_index_t green, local_index_t blue )
            : values_{ red, green, blue }
        {
        }

        [[nodiscard]] constexpr local_index_t red() const
        {
            return values_[0];
        }

        [[nodiscard]] constexpr local_index_t green() const
        {
            return values_[1];
        }

        [[nodiscard]] constexpr local_index_t blue() const
        {
            return values_[2];
        }

        constexpr void set_red( local_index_t red_scale )
        {
            values_[0] = red_scale;
        }

        constexpr void set_green( local_index_t green_scale )
        {
            values_[1] = green_scale;
        }

        constexpr void set_blue( local_index_t blue_scale )
        {
            values_[2] = blue_scale;
        }

        [[nodiscard]] constexpr bool operator==( const RGBColor &other ) const
        {
            return red() == other.red() && green() == other.green()
                   && blue() == other.blue();
        }

        [[nodiscard]] constexpr bool operator!=( const RGBColor &other ) const
        {
            return !( *this == other );
        }

        [[nodiscard]] constexpr RGBColor operator+(
            const RGBColor &other ) const
        {
            RGBColor result{ *this };
            result += other;
            return result;
        }

        constexpr void operator+=( const RGBColor &other )
        {
            set_red( ( red() + other.red() ) / 2 );
            set_green( ( green() + other.green() ) / 2 );
            set_blue( ( blue() + other.blue() ) / 2 );
        }

        [[nodiscard]] std::string string() const;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive &serializer );

    private:
        std::array< local_index_t, 3 > values_{ 0, 0, 0 };
    };

    template <>
    struct AttributeLinearInterpolationImpl< RGBColor >
    {
        template < template < typename > class Attribute >
        static RGBColor compute(
            const AttributeLinearInterpolation &interpolator,
            const Attribute< RGBColor > &attribute )
        {
            if( interpolator.indices_.empty() )
            {
                return attribute.default_values().default_value;
            }
            std::array< double, 3 > result{ 0, 0, 0 };
            bool is_same{ true };
            const auto first_value =
                attribute.value( interpolator.indices_[0] );
            for( const auto i : Indices{ interpolator.indices_ } )
            {
                const auto &i_value =
                    attribute.value( interpolator.indices_[i] );
                if( is_same )
                {
                    is_same = i_value == first_value;
                }
                const auto lambda = interpolator.lambdas_[i];
                result[0] += i_value.red() * lambda;
                result[1] += i_value.green() * lambda;
                result[2] += i_value.blue() * lambda;
            }
            if( is_same )
            {
                return first_value;
            }
            return RGBColor{ detail::color_channel( result[0] ),
                detail::color_channel( result[1] ),
                detail::color_channel( result[2] ) };
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
            OpenGeodeImageException::check_assertion( item < nb_items(),
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
    struct opengeode_image_api hash< geode::RGBColor >
    {
        size_t operator()( const geode::RGBColor &color ) const;
    };
} // namespace std
