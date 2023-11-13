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

#pragma once

#include <absl/container/flat_hash_map.h>
#include <absl/container/inlined_vector.h>

#include <geode/basic/common.h>

namespace geode
{
    template < typename T1,
        typename T2,
        template < typename >
        class StorageType >
    class MappingBase
    {
        OPENGEODE_DISABLE_COPY( MappingBase );

    public:
        template < typename T >
        using Storage = typename StorageType< T >::Type;

        void reserve( index_t capacity )
        {
            in2out_.reserve( capacity );
            out2in_.reserve( capacity );
        }

        void clear()
        {
            in2out_.clear();
            out2in_.clear();
        }

        bool has_mapping_input( const T1& value ) const
        {
            return in2out_.contains( value );
        }

        bool has_mapping_output( const T2& value ) const
        {
            return out2in_.contains( value );
        }

        const Storage< T2 >& in2out( const T1& in ) const
        {
            return in2out_.at( in );
        }

        const Storage< T1 >& out2in( const T2& out ) const
        {
            return out2in_.at( out );
        }

        const absl::flat_hash_map< T1, Storage< T2 > >& in2out_map() const
        {
            return in2out_;
        }

        const absl::flat_hash_map< T2, Storage< T1 > >& out2in_map() const
        {
            return out2in_;
        }

    protected:
        MappingBase() = default;
        MappingBase( MappingBase&& other ) = default;
        MappingBase& operator=( MappingBase&& other ) = default;

        index_t size_input() const
        {
            return static_cast< index_t >( in2out_.size() );
        }

        index_t size_output() const
        {
            return static_cast< index_t >( out2in_.size() );
        }

        absl::flat_hash_map< T1, Storage< T2 > >& in2out_mapping()
        {
            return in2out_;
        }

        absl::flat_hash_map< T2, Storage< T1 > >& out2in_mapping()
        {
            return out2in_;
        }

    private:
        absl::flat_hash_map< T1, Storage< T2 > > in2out_;
        absl::flat_hash_map< T2, Storage< T1 > > out2in_;
    };

    template < typename T >
    struct OneValueStorage
    {
        using Type = T;
    };

    template < typename T1, typename T2 = T1 >
    class BijectiveMapping : public MappingBase< T1, T2, OneValueStorage >
    {
    public:
        void map( const T1& in, const T2& out )
        {
            if( this->has_mapping_input( in ) )
            {
                erase_in( in );
            }
            if( this->has_mapping_output( out ) )
            {
                erase_out( out );
            }
            emplace( in, out );
        }

        void erase_in( const T1& in )
        {
            if( !this->has_mapping_input( in ) )
            {
                return;
            }
            const auto out = this->in2out( in );
            this->in2out_mapping().erase( in );
            this->out2in_mapping().erase( out );
        }

        void erase_out( const T2& out )
        {
            if( !this->has_mapping_output( out ) )
            {
                return;
            }
            const auto in = this->out2in( out );
            this->in2out_mapping().erase( in );
            this->out2in_mapping().erase( out );
        }

        index_t size() const
        {
            return this->size_input();
        }

    private:
        void emplace( const T1& in, const T2& out )
        {
            this->in2out_mapping().emplace( in, out );
            this->out2in_mapping().emplace( out, in );
        }
    };

    template < typename T >
    struct MultipleValueStorage
    {
        using Type = absl::InlinedVector< T, 1 >;
    };

    template < typename T1, typename T2 = T1 >
    class GenericMapping : public MappingBase< T1, T2, MultipleValueStorage >
    {
    public:
        void map( const T1& in, const T2& out )
        {
            this->in2out_mapping()[in].push_back( out );
            this->out2in_mapping()[out].push_back( in );
        }

        void erase_in( const T1& in )
        {
            if( !this->has_mapping_input( in ) )
            {
                return;
            }
            for( const auto& out : this->in2out( in ) )
            {
                auto& out_map = this->out2in_mapping().at( out );
                const auto itr = absl::c_find( out_map, in );
                out_map.erase( itr );
                if( this->out2in( out ).empty() )
                {
                    this->out2in_mapping().erase( out );
                }
            }
            this->in2out_mapping().erase( in );
        }

        void erase_out( const T2& out )
        {
            if( !this->has_mapping_output( out ) )
            {
                return;
            }
            for( const auto& in : this->out2in( out ) )
            {
                auto& in_map = this->in2out_mapping().at( in );
                const auto itr = absl::c_find( in_map, out );
                in_map.erase( itr );
                if( this->in2out( in ).empty() )
                {
                    this->in2out_mapping().erase( in );
                }
            }
            this->out2in_mapping().erase( out );
        }

        index_t size_in() const
        {
            return this->size_input();
        }

        index_t size_out() const
        {
            return this->size_output();
        }
    };
} // namespace geode
