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

#include <bitsery/bitsery.h>
#include <bitsery/brief_syntax.h>

#include <geode/basic/growable.hpp>

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
        friend class bitsery::Access;

    public:
        NamedType() = default;

        explicit NamedType( Type value ) : value_( std::move( value ) ) {}

        [[nodiscard]] const Type& get() const
        {
            return value_;
        }

        bool operator==( const NamedType& other ) const
        {
            return value_ == other.value_;
        }

        bool operator!=( const NamedType& other ) const
        {
            return !( operator==( other ) );
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, NamedType >{
                                    { []( Archive& a, NamedType& named_type ) {
                                        a( named_type.value_ );
                                    } } } );
        }

    private:
        Type value_;
    };
} // namespace geode
