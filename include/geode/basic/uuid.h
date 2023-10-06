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

/*
 * Modified and simplified version of https://github.com/r-lyeh-archived/sole
 * Only UUID version 4 is kept.
 */

/* Sole is a lightweight C++11 library to generate universally unique
 identificators.
 * Sole provides interface for UUID versions 0, 1 and 4.

 * https://github.com/r-lyeh/sole
 * Copyright (c) 2013,2014,2015 r-lyeh. zlib/libpng licensed.

 * Based on code by Dmitri Bouianov, Philip O'Toole, Poco C++ libraries and
 anonymous
 * code found on the net. Thanks guys!

 * Theory: (see Hoylen's answer at [1])
 * - UUID version 1 (48-bit MAC address + 60-bit clock with a resolution of
 100ns)
 *   Clock wraps in 3603 A.D.
 *   Up to 10000000 UUIDs per second.
 *   MAC address revealed.
 *
 * - UUID Version 4 (122-bits of randomness)
 *   See [2] or other analysis that describe how very unlikely a duplicate is.
 *
 * - Use v1 if you need to sort or classify UUIDs per machine.
 *   Use v1 if you are worried about leaving it up to probabilities (e.g. your
 are the
 *   type of person worried about the earth getting destroyed by a large
 asteroid in your
 *   lifetime). Just use a v1 and it is guaranteed to be unique till 3603 AD.
 *
 * - Use v4 if you are worried about security issues and determinism. That is
 because
 *   v1 UUIDs reveal the MAC address of the machine it was generated on and they
 can be
 *   predictable. Use v4 if you need more than 10 million uuids per second, or
 if your
 *   application wants to live past 3603 A.D.

 * Additionally a custom UUID v0 is provided:
 * - 16-bit PID + 48-bit MAC address + 60-bit clock with a resolution of 100ns
 since Unix epoch
 * - Format is EPOCH_LOW-EPOCH_MID-VERSION(0)|EPOCH_HI-PID-MAC
 * - Clock wraps in 3991 A.D.
 * - Up to 10000000 UUIDs per second.
 * - MAC address and PID revealed.

 * References:
 * - [1] http://stackoverflow.com/questions/1155008/how-unique-is-uuid
 * - [2]
 http://en.wikipedia.org/wiki/UUID#Random%5FUUID%5Fprobability%5Fof%5Fduplicates
 * - http://en.wikipedia.org/wiki/Universally_unique_identifier
 * - http://en.cppreference.com/w/cpp/numeric/random/random_device
 * - http://www.itu.int/ITU-T/asn1/uuid.html
 f81d4fae-7dec-11d0-a765-00a0c91e6bf6

 * - rlyeh ~~ listening to Hedon Cries / Until The Sun Goes up
 */

//////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

#include <absl/hash/hash.h>

#include <bitsery/bitsery.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/common.h>

namespace geode
{
    /*!
     * 128-bit basic UUID type that allows comparison and sorting.
     */
    struct opengeode_basic_api uuid
    {
    public:
        uuid();
        uuid( absl::string_view string );
        uuid( const uuid & ) = default;
        uuid( uuid && ) = default;
        uuid &operator=( uuid &&other ) = default;
        uuid &operator=( const uuid &other ) = default;
        ~uuid() = default;

        bool operator==( const uuid &other ) const;

        bool operator!=( const uuid &other ) const;

        bool operator<( const uuid &other ) const;

        std::string string() const;

        uint64_t ab;
        uint64_t cd;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive &archive )
        {
            archive.ext(
                *this, Growable< Archive, uuid >{ { []( Archive &a, uuid &id ) {
                    a.value8b( id.ab );
                    a.value8b( id.cd );
                } } } );
        }
    };
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::uuid >
    {
    public:
        size_t operator()( const geode::uuid &uuid ) const
        {
            return absl::Hash< uint64_t >()( uuid.ab )
                   ^ absl::Hash< uint64_t >()( uuid.cd );
        }
    };
} // namespace std
