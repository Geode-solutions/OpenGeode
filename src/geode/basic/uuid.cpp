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

#include <geode/basic/uuid.h>

#include <iomanip>
#include <random>
#include <sstream>

namespace geode
{
    uuid::uuid()
    {
        static std::random_device rd;
        static std::uniform_int_distribution< uint64_t > dist( 0u, ~0u );

        ab = dist( rd );
        cd = dist( rd );

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
        std::stringstream ss;
        ss << std::hex << std::nouppercase << std::setfill( '0' );

        const uint32_t a = ( ab >> 32u );
        const uint32_t b = ( ab & 0xFFFFFFFFu );
        const uint32_t c = ( cd >> 32u );
        const uint32_t d = ( cd & 0xFFFFFFFFu );

        ss << std::setw( 8 ) << ( a ) << '-';
        ss << std::setw( 4 ) << ( b >> 16u ) << '-';
        ss << std::setw( 4 ) << ( b & 0xFFFFu ) << '-';
        ss << std::setw( 4 ) << ( c >> 16u ) << '-';
        ss << std::setw( 4 ) << ( c & 0xFFFFu );
        ss << std::setw( 8 ) << d;

        return ss.str();
    }
} // namespace geode
