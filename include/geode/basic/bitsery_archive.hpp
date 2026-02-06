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

#include <absl/container/fixed_array.h>
#include <absl/container/inlined_vector.h>

#include <bitsery/adapter/stream.h>
#include <bitsery/bitsery.h>
#include <bitsery/ext/inheritance.h>
#include <bitsery/ext/pointer.h>

#include <geode/basic/common.hpp>
#include <geode/basic/range.hpp>

namespace geode
{
    using PContext =
        bitsery::ext::PolymorphicContext< bitsery::ext::StandardRTTI >;
    using TContext = std::tuple< PContext,
        bitsery::ext::PointerLinkingContext,
        bitsery::ext::InheritanceContext >;
    using Serializer =
        bitsery::Serializer< bitsery::OutputBufferedStreamAdapter, TContext >;
    using Deserializer =
        bitsery::Deserializer< bitsery::InputStreamAdapter, TContext >;

    /*!
     * Register all the information needed by Bitsery to serialize the objects
     * in the basic library.
     * @param[in] context The context where to register this information.
     * @warning The context can be used only once per archive.
     */
    void opengeode_basic_api register_basic_serialize_pcontext(
        PContext &context );

    /*!
     * Register all the information needed by Bitsery to deserialize the objects
     * in the basic library.
     * @param[in] context The context where to register this information.
     * @warning The context can be used only once per archive.
     */
    void opengeode_basic_api register_basic_deserialize_pcontext(
        PContext &context );

    class opengeode_basic_api BitseryExtensions
    {
    public:
        using RegisterFunction = std::function< void( PContext & ) >;

        static void register_functions(
            RegisterFunction serializer, RegisterFunction deserializer );

        static void register_serialize_pcontext( PContext &context );

        static void register_deserialize_pcontext( PContext &context );
    };
} // namespace geode

namespace bitsery
{
    namespace traits
    {
        template < typename T, size_t N >
        struct ContainerTraits< absl::InlinedVector< T, N > >
            : public StdContainer< absl::InlinedVector< T, N >, true, true >
        {
        };
    } // namespace traits

    template < typename Serializer, typename T, size_t N >
    void serialize( Serializer &s, absl::InlinedVector< T, N > &obj )
    {
        s.container( obj, obj.max_size() );
    }

    namespace traits
    {
        template < typename T, size_t N >
        struct ContainerTraits< absl::FixedArray< T, N > >
            : public StdContainer< absl::FixedArray< T, N >, false, true >
        {
        };
    } // namespace traits

    template < typename Serializer, typename T, size_t N >
    void serialize( Serializer &s, absl::FixedArray< T, N > &obj )
    {
        s.container( obj, obj.max_size() );
    }
} // namespace bitsery

#define SERIALIZE_BITSERY_ARCHIVE( EXPORT, TYPE )                              \
    template EXPORT void TYPE::serialize< geode::Serializer >(                 \
        geode::Serializer & );                                                 \
    template EXPORT void TYPE::serialize< geode::Deserializer >(               \
        geode::Deserializer & )

#define BITSERY_CLASS_NAME( Type, Name )                                       \
    template <>                                                                \
    struct PolymorphicClassName< Type >                                        \
    {                                                                          \
        static constexpr auto name = Name;                                     \
    }
