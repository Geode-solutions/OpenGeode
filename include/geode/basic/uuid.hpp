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

#include <string>

#include <absl/hash/hash.h>

#include <bitsery/bitsery.h>

#include <geode/basic/common.hpp>
#include <geode/basic/growable.hpp>

namespace geode
{
    struct opengeode_basic_api uuid
    {
    public:
        uuid();
        explicit uuid( std::string_view string );
        uuid( const uuid & ) = default;
        uuid( uuid && ) = default;
        uuid &operator=( uuid &&other ) = default;
        uuid &operator=( const uuid &other ) = default;
        ~uuid() = default;

        [[nodiscard]] bool operator==( const uuid &other ) const;

        [[nodiscard]] bool operator!=( const uuid &other ) const;

        [[nodiscard]] bool operator<( const uuid &other ) const;

        [[nodiscard]] std::string string() const;

        template < typename H >
        friend H AbslHashValue( H h, const uuid &value )
        {
            return H::combine( std::move( h ), value.bytes_ );
        }

        std::array< std::uint8_t, 16 > bytes_;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive &archive )
        {
            archive.ext( *this, Growable< Archive, uuid >{
                                    { []( Archive &a, uuid &id ) {
                                         uint64_t ab;
                                         uint64_t cd;
                                         a.value8b( ab );
                                         a.value8b( cd );
                                         id.bytes_[0] = ( ab >> 56 ) & 0xFF;
                                         id.bytes_[1] = ( ab >> 48 ) & 0xFF;
                                         id.bytes_[2] = ( ab >> 40 ) & 0xFF;
                                         id.bytes_[3] = ( ab >> 32 ) & 0xFF;
                                         id.bytes_[4] = ( ab >> 24 ) & 0xFF;
                                         id.bytes_[5] = ( ab >> 16 ) & 0xFF;
                                         id.bytes_[6] = ( ab >> 8 ) & 0xFF;
                                         id.bytes_[7] = ( ab >> 0 ) & 0xFF;
                                         id.bytes_[8] = ( cd >> 56 ) & 0xFF;
                                         id.bytes_[9] = ( cd >> 48 ) & 0xFF;
                                         id.bytes_[10] = ( cd >> 40 ) & 0xFF;
                                         id.bytes_[11] = ( cd >> 32 ) & 0xFF;
                                         id.bytes_[12] = ( cd >> 24 ) & 0xFF;
                                         id.bytes_[13] = ( cd >> 16 ) & 0xFF;
                                         id.bytes_[14] = ( cd >> 8 ) & 0xFF;
                                         id.bytes_[15] = ( cd >> 0 ) & 0xFF;
                                     },
                                        []( Archive &a, uuid &id ) {
                                            a.container1b( id.bytes_ );
                                        } } } );
        }
    };
} // namespace geode

namespace std
{
    template <>
    struct opengeode_basic_api hash< geode::uuid >
    {
        size_t operator()( const geode::uuid &uuid ) const;
    };
} // namespace std
