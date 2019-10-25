/*
 * Copyright (c) 2019 Geode-solutions
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

#include <bitsery/adapter/stream.h>
#include <bitsery/bitsery.h>
#include <bitsery/ext/inheritance.h>
#include <bitsery/ext/pointer.h>

#include <geode/basic/common.h>

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
        PContext& context );

    /*!
     * Register all the information needed by Bitsery to deserialize the objects
     * in the basic library.
     * @param[in] context The context where to register this information.
     * @warning The context can be used only once per archive.
     */
    void opengeode_basic_api register_basic_deserialize_pcontext(
        PContext& context );
} // namespace geode

#define SERIALIZE_BITSERY_ARCHIVE( EXPORT, TYPE )                              \
    template EXPORT void TYPE::serialize< geode::Serializer >(                 \
        geode::Serializer& );                                                  \
    template EXPORT void TYPE::serialize< geode::Deserializer >(               \
        geode::Deserializer& )
