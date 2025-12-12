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

#include <atomic>
#include <type_traits>

#include <bitsery/brief_syntax.h>

#include <absl/synchronization/mutex.h>

#include <geode/basic/growable.hpp>

namespace geode
{
    template < typename ReturnType >
    class CachedValue
    {
    public:
        template < typename... Args >
        using CachedFunction =
            typename std::add_pointer< ReturnType( Args... ) >::type;

        CachedValue() = default;
        CachedValue( const CachedValue& other )
        {
            value_ = other.value_;
            computed_ = other.computed_.load();
        }
        CachedValue( CachedValue&& other ) noexcept
        {
            value_ = std::move( other.value_ );
            computed_ = other.computed_.load();
        }

        CachedValue& operator=( const CachedValue& other )
        {
            value_ = other.value_;
            computed_ = other.computed_.load();
            return *this;
        }

        CachedValue& operator=( CachedValue&& other ) noexcept
        {
            value_ = std::move( other.value_ );
            computed_ = other.computed_.load();
            return *this;
        }

        template < typename... Args >
        const ReturnType& operator()(
            CachedFunction< Args... > computer, Args&&... args ) const
        {
            if( !computed_ )
            {
                absl::MutexLock lock{ &mutex_ };
                if( !computed_ )
                {
                    value_ = computer( std::forward< Args >( args )... );
                    computed_ = true;
                }
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

        [[nodiscard]] bool computed() const
        {
            return computed_;
        }

        [[nodiscard]] const ReturnType& value() const
        {
            return value_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext(
                *this, Growable< Archive, CachedValue >{
                           { []( Archive& a, CachedValue& value ) {
                                bool computed;
                                a.value1b( computed );
                                value.computed_ = computed;
                                a( value.value_ );
                            },
                               []( Archive& /*a*/, CachedValue& /*value*/ ) {
                                   // skip serialize
                               } } } );
        }

    private:
        mutable std::atomic< bool > computed_{ false };
        mutable ReturnType value_;
        mutable absl::Mutex mutex_;
    };
} // namespace geode