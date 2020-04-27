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

#pragma once

#include <absl/container/flat_hash_map.h>
#include <absl/container/inlined_vector.h>

#include <geode/basic/common.h>

namespace geode
{
    template < typename T >
    class BijectiveMapping
    {
    public:
        void map( const T& in, const T& out )
        {
            in2out_.emplace( in, out );
            out2in_.emplace( out, in );
        }

        void reserve( index_t capacity )
        {
            in2out_.reserve( capacity );
            out2in_.reserve( capacity );
        }

        const T& in2out( const T& in ) const
        {
            return in2out_.at( in );
        }
        const T& out2in( const T& out ) const
        {
            return out2in_.at( out );
        }

        const absl::flat_hash_map< T, T >& get() const
        {
            return in2out_;
        }

        const absl::flat_hash_map< T, T >& get_inverse() const
        {
            return out2in_;
        }

    private:
        absl::flat_hash_map< T, T > in2out_;
        absl::flat_hash_map< T, T > out2in_;
    };

    template < typename T, index_t Capacity >
    class Mapping
    {
    public:
        void map( const T& in, const T& out )
        {
            in2out_[in].push_back( out );
            out2in_[out].push_back( in );
        }

        // void reserve( index_t capacity )
        // {
        //     in2out_.reserve( capacity );
        //     out2in_.reserve( capacity );
        // }

        const absl::InlinedVector< T, Capacity >& in2out( const T& in ) const
        {
            return in2out_.at( in );
        }
        const absl::InlinedVector< T, Capacity >& out2in( const T& out ) const
        {
            return out2in_.at( out );
        }

    private:
        absl::flat_hash_map< T, absl::InlinedVector< T, Capacity > > in2out_;
        absl::flat_hash_map< T, absl::InlinedVector< T, Capacity > > out2in_;
    };
} // namespace geode
