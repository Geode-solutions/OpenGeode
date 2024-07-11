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

#include <absl/hash/hash.h>

#include <geode/basic/attribute_utils.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/range.h>

#include <geode/image/common.h>

namespace geode
{
    /*!
     * Description of a color in grey scale
     */
    class GreyscaleColor
    {
    public:
        GreyscaleColor()
        {
            value_ = 0;
        }

        explicit GreyscaleColor( local_index_t value ) : value_( value ) {}

        local_index_t value() const
        {
            return value_;
        }

        void set_value( local_index_t greyscale )
        {
            value_ = greyscale;
        }

        bool operator==( const GreyscaleColor &other ) const
        {
            return value() == other.value();
        }

        bool operator!=( const GreyscaleColor &other ) const
        {
            return value() != other.value();
        }

        GreyscaleColor operator+( const GreyscaleColor &other ) const
        {
            return GreyscaleColor{ static_cast< geode::local_index_t >(
                value() / 2 + other.value() / 2 ) };
        }

        void operator+=( const GreyscaleColor &other )
        {
            set_value( ( value() + other.value() ) / 2 );
        }

        std::string string() const
        {
            return absl::StrCat( value_ );
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive &archive )
        {
            archive.ext( *this, Growable< Archive, GreyscaleColor >{
                                    { []( Archive &a, GreyscaleColor &color ) {
                                        a.value1b( color.value_ );
                                    } } } );
        }

    private:
        local_index_t value_;
    };

    template <>
    struct AttributeLinearInterpolationImpl< GreyscaleColor >
    {
        template < template < typename > class Attribute >
        static GreyscaleColor compute(
            const AttributeLinearInterpolation &interpolator,
            const Attribute< GreyscaleColor > &attribute )
        {
            local_index_t result{ 0 };
            for( const auto i : Indices{ interpolator.indices_ } )
            {
                result += attribute.value( interpolator.indices_[i] ).value()
                          * interpolator.lambdas_[i];
            }
            return GreyscaleColor{ result };
        }
    };

    template <>
    struct GenericAttributeConversion< GreyscaleColor >
    {
        static float converted_value( const GreyscaleColor &color )
        {
            return color.value();
        }

        static float converted_item_value(
            const GreyscaleColor &color, local_index_t item )
        {
            geode_unused( item );
            OPENGEODE_ASSERT( item < nb_items(),
                "[GenericAttributeConversion] Accessing "
                "incorrect item value" );
            return static_cast< float >( color.value() );
        }

        static bool is_genericable()
        {
            return true;
        }
        static local_index_t nb_items()

        {
            return 1;
        }
    };
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::GreyscaleColor >
    {
    public:
        size_t operator()( const geode::GreyscaleColor &color ) const
        {
            return absl::Hash< geode::local_index_t >()( color.value() );
        }
    };
} // namespace std
