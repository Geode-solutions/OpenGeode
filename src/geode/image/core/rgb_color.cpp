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
    std::string RGBColor::string() const
    {
        return absl::StrCat( values_[0], " ", values_[1], " ", values_[2] );
    }

    template < typename Archive >
    void RGBColor::serialize( Archive &serializer )
    {
        serializer.ext( *this, Growable< Archive, RGBColor >{
                                   { []( Archive &archive, RGBColor &color ) {
                                       archive.container1b( color.values_ );
                                   } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_image_api, RGBColor );
} // namespace geode

namespace std
{
    size_t hash< geode::RGBColor >::operator()(
        const geode::RGBColor &color ) const
    {
        return absl::HashOf( color.red(), color.green(), color.blue() );
    }
} // namespace std
