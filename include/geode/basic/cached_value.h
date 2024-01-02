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

#pragma once

#include <type_traits>

#include <bitsery/brief_syntax.h>

#include <geode/basic/bitsery_archive.h>

namespace geode
{
    template < typename ReturnType >
    class CachedValue
    {
    public:
        template < typename... Args >
        using CachedFunction =
            typename std::add_pointer< ReturnType( Args... ) >::type;

        template < typename... Args >
        const ReturnType& operator()(
            CachedFunction< Args... > computer, Args&&... args ) const
        {
            if( !computed_ )
            {
                value_ = computer( std::forward< Args >( args )... );
                computed_ = true;
            }
            return value_;
        }

        bool operator!=( const CachedValue& other ) const
        {
            if( computed() && other.computed() )
            {
                return value() != other.value();
            }
            return false;
        }

        void reset()
        {
            computed_ = false;
        }

        bool computed() const
        {
            return computed_;
        }

        const ReturnType& value() const
        {
            return value_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, CachedValue >{
                                    { []( Archive& a, CachedValue& value ) {
                                        a.value1b( value.computed_ );
                                        a( value.value_ );
                                    } } } );
        }

    private:
        mutable bool computed_{ false };
        mutable ReturnType value_;
    };
} // namespace geode