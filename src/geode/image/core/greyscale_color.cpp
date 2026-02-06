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
#include <geode/image/core/greyscale_color.hpp>

#include <absl/hash/hash.h>

#include <geode/basic/attribute_utils.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/range.hpp>

namespace geode
{
    GreyscaleColor::GreyscaleColor()
    {
        value_ = 0;
    }

    GreyscaleColor::GreyscaleColor( local_index_t value ) : value_( value ) {}

    local_index_t GreyscaleColor::value() const
    {
        return value_;
    }

    void GreyscaleColor::set_value( local_index_t greyscale )
    {
        value_ = greyscale;
    }

    bool GreyscaleColor::operator==( const GreyscaleColor &other ) const
    {
        return value() == other.value();
    }

    bool GreyscaleColor::operator!=( const GreyscaleColor &other ) const
    {
        return value() != other.value();
    }

    GreyscaleColor GreyscaleColor::operator+(
        const GreyscaleColor &other ) const
    {
        return GreyscaleColor{ static_cast< geode::local_index_t >(
            value() / 2 + other.value() / 2 ) };
    }

    void GreyscaleColor::operator+=( const GreyscaleColor &other )
    {
        set_value( ( value() + other.value() ) / 2 );
    }

    std::string GreyscaleColor::string() const
    {
        return absl::StrCat( value_ );
    }

    template < typename Archive >
    void GreyscaleColor::serialize( Archive &archive )
    {
        archive.ext( *this, Growable< Archive, GreyscaleColor >{
                                { []( Archive &a, GreyscaleColor &color ) {
                                    a.value1b( color.value_ );
                                } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_image_api, GreyscaleColor );
} // namespace geode

namespace std
{
    size_t hash< geode::GreyscaleColor >::operator()(
        const geode::GreyscaleColor &color ) const
    {
        return absl::Hash< geode::local_index_t >()( color.value() );
    }
} // namespace std
