/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/basic/bitsery_archive.h>

#include <absl/types/span.h>

namespace
{
    class BitseryExtensionsStorage
    {
        using RegisterFunction = geode::BitseryExtensions::RegisterFunction;

    public:
        void register_functions(
            RegisterFunction serializer, RegisterFunction deserializer )
        {
            serializers_.emplace_back( std::move( serializer ) );
            deserializers_.emplace_back( std::move( deserializer ) );
        }

        absl::Span< const RegisterFunction > serializers()
        {
            return serializers_;
        }

        absl::Span< const RegisterFunction > deserializers()
        {
            return deserializers_;
        }

    private:
        std::vector< RegisterFunction > serializers_;
        std::vector< RegisterFunction > deserializers_;
    };

    BitseryExtensionsStorage &storage()
    {
        static BitseryExtensionsStorage storage;
        return storage;
    }
} // namespace

namespace geode
{
    void BitseryExtensions::register_functions(
        RegisterFunction serializer, RegisterFunction deserializer )
    {
        storage().register_functions(
            std::move( serializer ), std::move( deserializer ) );
    }

    void BitseryExtensions::register_serialize_pcontext( PContext &context )
    {
        for( const auto &function : storage().serializers() )
        {
            function( context );
        }
    }

    void BitseryExtensions::register_deserialize_pcontext( PContext &context )
    {
        for( const auto &function : storage().deserializers() )
        {
            function( context );
        }
    }
} // namespace geode
