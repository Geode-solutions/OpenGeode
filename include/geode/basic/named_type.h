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

#include <bitsery/bitsery.h>

namespace geode
{
    /*!
     * This class enforces a strong typing that can be used in API signature.
     * For example:
     *       struct CustomTag
     *       {
     *       };
     *       using CustomType = NamedType< std::string, CustomTag >;
     *
     *       CustomType is a new type with a std::string as underlying storage.
     */
    template < typename Type, typename Tag >
    class NamedType
    {
    public:
        explicit NamedType( Type value ) : value_( std::move( value ) ) {}

        const Type& get() const
        {
            return value_;
        }

        bool operator==( const NamedType& other ) const
        {
            return value_ == other.value_;
        }

    private:
        friend class bitsery::Access;
        NamedType() = default;

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.archive( value_ );
        }

    private:
        Type value_;
    };
} // namespace geode
