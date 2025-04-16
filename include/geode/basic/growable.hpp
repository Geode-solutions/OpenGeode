/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <absl/container/fixed_array.h>

#include <bitsery/bitsery.h>
#include <bitsery/ext/compact_value.h>

#include <geode/basic/common.hpp>

namespace geode
{
    template < typename Archive, typename T >
    class Growable
    {
    public:
        Growable( absl::FixedArray< std::function< void( Archive &, T & ) > >
                serializers )
            : version_( serializers.size() ),
              serializers_( std::move( serializers ) )
        {
        }

        template < typename Fnc >
        void serialize( Archive &ser, const T &obj, Fnc &&fnc ) const
        {
            geode_unused( fnc );
            ser.ext4b( version_, bitsery::ext::CompactValue{} );
            serializers_.back()( ser, const_cast< T & >( obj ) );
        }

        template < typename Fnc >
        void deserialize( Archive &des, T &obj, Fnc &&fnc ) const
        {
            geode_unused( fnc );
            index_t current_version;
            des.ext4b( current_version, bitsery::ext::CompactValue{} );
            serializers_.at( current_version - 1 )( des, obj );
        }

    private:
        index_t version_;
        absl::FixedArray< std::function< void( Archive &, T & ) > >
            serializers_;
    };
} // namespace geode

namespace bitsery
{
    namespace traits
    {
        template < typename Archive, typename T >
        struct ExtensionTraits< geode::Growable< Archive, T >, T >
        {
            using TValue = T;
            static constexpr bool SupportValueOverload = false;
            static constexpr bool SupportObjectOverload = true;
            static constexpr bool SupportLambdaOverload = true;
        };
    } // namespace traits
} // namespace bitsery
