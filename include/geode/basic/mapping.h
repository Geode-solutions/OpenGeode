/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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
    template < typename T1, typename T2 = T1 >
    class BijectiveMapping
    {
    public:
        void map( const T1& in, const T2& out )
        {
            in2out_.emplace( in, out );
            out2in_.emplace( out, in );
        }

        void reserve( index_t capacity )
        {
            in2out_.reserve( capacity );
            out2in_.reserve( capacity );
        }

        bool has_mapping_input( const T1& value ) const
        {
            return in2out_.contains( value );
        }

        bool has_mapping_output( const T2& value ) const
        {
            return out2in_.contains( value );
        }

        const T2& in2out( const T1& in ) const
        {
            return in2out_.at( in );
        }
        const T1& out2in( const T2& out ) const
        {
            return out2in_.at( out );
        }

    private:
        absl::flat_hash_map< T1, T2 > in2out_;
        absl::flat_hash_map< T2, T1 > out2in_;
    };

    template < typename T1, typename T2 = T1, index_t Capacity = 1 >
    class GenericMapping
    {
    public:
        void map( const T1& in, const T2& out )
        {
            in2out_[in].push_back( out );
            out2in_[out].push_back( in );
        }

        void reserve( index_t capacity )
        {
            in2out_.reserve( capacity );
            out2in_.reserve( capacity );
        }

        const absl::InlinedVector< T2, Capacity >& in2out( const T1& in ) const
        {
            return in2out_.at( in );
        }
        const absl::InlinedVector< T1, Capacity >& out2in( const T2& out ) const
        {
            return out2in_.at( out );
        }

    private:
        absl::flat_hash_map< T1, absl::InlinedVector< T2, Capacity > > in2out_;
        absl::flat_hash_map< T2, absl::InlinedVector< T1, Capacity > > out2in_;
    };
} // namespace geode
