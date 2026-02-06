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

#include <geode/image/core/rgb_color.hpp>

#include <array>

#include <absl/hash/hash.h>

#include <geode/basic/attribute_utils.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/range.hpp>

namespace geode
{
    RGBColor::RGBColor()
    {
        values_.fill( 0 );
    }

    RGBColor::RGBColor(
        local_index_t red, local_index_t green, local_index_t blue )
        : values_( { red, green, blue } )
    {
    }

    local_index_t RGBColor::red() const
    {
        return values_[0];
    }

    local_index_t RGBColor::green() const
    {
        return values_[1];
    }

    local_index_t RGBColor::blue() const
    {
        return values_[2];
    }

    void RGBColor::set_red( local_index_t red_scale )
    {
        values_[0] = red_scale;
    }

    void RGBColor::set_green( local_index_t green_scale )
    {
        values_[1] = green_scale;
    }

    void RGBColor::set_blue( local_index_t blue_scale )
    {
        values_[2] = blue_scale;
    }

    bool RGBColor::operator==( const RGBColor &other ) const
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

    bool RGBColor::operator!=( const RGBColor &other ) const
    {
        return !( *this == other );
    }

    RGBColor RGBColor::operator+( const RGBColor &other ) const
    {
        RGBColor result;
        result.set_red( ( result.red() + other.red() ) / 2 );
        result.set_green( ( result.green() + other.green() ) / 2 );
        result.set_blue( ( result.blue() + other.blue() ) / 2 );
        return result;
    }

    void RGBColor::operator+=( const RGBColor &other )
    {
        set_red( ( red() + other.red() ) / 2 );
        set_green( ( green() + other.green() ) / 2 );
        set_blue( ( blue() + other.blue() ) / 2 );
    }

    std::string RGBColor::string() const
    {
        return absl::StrCat( values_[0], " ", values_[1], " ", values_[2] );
    }

    template < typename Archive >
    void RGBColor::serialize( Archive &archive )
    {
        archive.ext( *this,
            Growable< Archive, RGBColor >{ { []( Archive &a, RGBColor &color ) {
                a.container1b( color.values_ );
            } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_image_api, RGBColor );
} // namespace geode

namespace std
{
    size_t hash< geode::RGBColor >::operator()(
        const geode::RGBColor &color ) const
    {
        return absl::Hash< geode::local_index_t >()( color.red() )
               ^ absl::Hash< geode::local_index_t >()( color.green() )
               ^ absl::Hash< geode::local_index_t >()( color.blue() );
    }
} // namespace std
