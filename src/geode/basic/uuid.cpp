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

#include <geode/basic/uuid.h>

#include <iomanip>
#include <sstream>

#include <absl/random/random.h>

namespace
{
    size_t decode( char ch )
    {
        if( 'f' >= ch && ch >= 'a' )
        {
            return ch - 'a' + 10;
        }
        if( 'F' >= ch && ch >= 'A' )
        {
            return ch - 'A' + 10;
        }
        if( '9' >= ch && ch >= '0' )
        {
            return ch - '0';
        }
        return 0;
    }
} // namespace

namespace geode
{
    uuid::uuid()
    {
        static thread_local absl::BitGen gen;
        static thread_local absl::uniform_int_distribution< uint64_t > dist(
            0u, ~0u );

        ab = dist( gen );
        cd = dist( gen );

        ab = ( ab & 0xFFFFFFFFFFFF0FFFULL ) | 0x0000000000004000ULL;
        cd = ( cd & 0x3FFFFFFFFFFFFFFFULL ) | 0x8000000000000000ULL;
    }

    bool uuid::operator==( const uuid &other ) const
    {
        return ab == other.ab && cd == other.cd;
    }

    bool uuid::operator!=( const uuid &other ) const
    {
        return !operator==( other );
    }

    bool uuid::operator<( const uuid &other ) const
    {
        if( ab < other.ab )
        {
            return true;
        }
        if( ab > other.ab )
        {
            return false;
        }
        if( cd < other.cd )
        {
            return true;
        }
        return false;
    }

    std::string uuid::string() const
    {
        char string[] = "00000000-0000-0000-0000-000000000000";
        static constexpr char encode[] = "0123456789abcdef";

        index_t bit = 15;
        for( const auto i : Range{ 18 } )
        {
            if( i == 8 || i == 13 )
            {
                continue;
            }
            string[i] = encode[ab >> 4 * bit & 0x0f];
            bit--;
        }

        bit = 15;
        for( const auto i : Range{ 19, 36 } )
        {
            if( i == 23 )
            {
                continue;
            }
            string[i] = encode[cd >> 4 * bit & 0x0f];
            bit--;
        }

        return string;
    }

    uuid::uuid( absl::string_view string )
    {
        OPENGEODE_EXCEPTION( string.size() == 36, "[uuid] wrong string size" );
        OPENGEODE_EXCEPTION( string[8] == '-' && string[13] == '-'
                                 && string[18] == '-' && string[23] == '-',
            "[uuid] unknown string format" );

        for( const auto i : Range{ 18 } )
        {
            if( i == 8 || i == 13 )
            {
                continue;
            }
            ab = ab << 4 | decode( string[i] );
        }
        for( const auto i : Range{ 19, 36 } )
        {
            if( i == 23 )
            {
                continue;
            }
            cd = cd << 4 | decode( string[i] );
        }
    }
} // namespace geode
