/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <vector>

#include <absl/base/casts.h>

#include <geode/basic/types.h>

namespace geode
{
    template < typename Type >
    class IncrementOperator
    {
    public:
        void operator()( Type& value ) const
        {
            value++;
        }
    };

    template < typename Type >
    class DecrementOperator
    {
    public:
        void operator()( Type& value ) const
        {
            value--;
        }
    };

    /*!
     * Base class for range-based iteration on custom range.
     * See derived classes for usage.
     */
    template < typename Type,
        template < typename > class NextOperator = IncrementOperator >
    class BaseRange
    {
    public:
        bool operator!=( const BaseRange& /*unused*/ ) const
        {
            return iter_ != last_;
        }

        void operator++()
        {
            next_operator_( iter_ );
        }

        Type current() const
        {
            return iter_;
        }

    protected:
        template < typename T1, typename T2 >
        BaseRange( T1 begin, T2 end )
            : iter_( static_cast< Type >( begin ) ),
              last_( static_cast< Type >( end ) )
        {
        }

    private:
        Type iter_;
        Type last_;
        NextOperator< Type > next_operator_;
    };

    /*!
     * Begin and end methods for range-based iteration on custom range.
     * See derived classes for usage.
     */
    template < typename Type >
    class BeginEnd
    {
    public:
        explicit BeginEnd( const Type& type ) : type_( type ) {}

        const Type& begin() const
        {
            return type_;
        }

        const Type& end() const
        {
            return type_;
        }

    private:
        const Type& type_;
    };

    /*!
     * This class can be used to iterate over integer loop.
     * Example:
     *              = C++98 loop =
     *    for( index_t i = 0; i < n; i++ )
     *    {
     *      // do something
     *    }
     *
     *            = C++11-like loop =
     *    for( const auto i : Range{ n } )
     *    {
     *      // do something
     *    }
     */
    template < typename Type >
    class TRange : public BaseRange< Type, IncrementOperator >,
                   public BeginEnd< TRange< Type > >
    {
    public:
        template < typename T1, typename T2 >
        TRange( T1 begin, T2 end )
            : BaseRange< Type, IncrementOperator >( begin, end ),
              BeginEnd< TRange< Type > >( *this )
        {
        }

        template < typename T >
        explicit TRange( T end ) : TRange( 0, end )
        {
        }

        Type operator*() const
        {
            return this->current();
        }
    };
    using Range = TRange< index_t >;
    using LRange = TRange< local_index_t >;

    template < typename Type >
    class TReverseRange : public BaseRange< Type, DecrementOperator >,
                          public BeginEnd< TReverseRange< Type > >
    {
    public:
        template < typename T1, typename T2 >
        TReverseRange( T1 begin, T2 end )
            : BaseRange< Type, DecrementOperator >( begin - 1, end - 1 ),
              BeginEnd< TReverseRange< Type > >( *this )
        {
        }

        template < typename T >
        explicit TReverseRange( T begin ) : TReverseRange( begin, 0 )
        {
        }

        Type operator*() const
        {
            return this->current();
        }
    };
    using ReverseRange = TReverseRange< index_t >;
    using LReverseRange = TReverseRange< local_index_t >;

    template < typename Type >
    class TIndices : public BaseRange< Type, IncrementOperator >,
                     public BeginEnd< TIndices< Type > >
    {
    public:
        template < typename Container >
        explicit TIndices( const Container& container )
            : BaseRange< Type, IncrementOperator >( 0, container.size() ),
              BeginEnd< TIndices< Type > >( *this )
        {
        }

        Type operator*() const
        {
            return this->current();
        }
    };
    using Indices = TIndices< index_t >;
    using LIndices = TIndices< local_index_t >;

    template < typename Type >
    class EraserRange : public BaseRange< index_t, DecrementOperator >,
                        public BeginEnd< EraserRange< Type > >
    {
    public:
        EraserRange( std::vector< Type >& values )
            : BaseRange< index_t, DecrementOperator >( values.size() - 1, -1 ),
              BeginEnd< EraserRange< Type > >( *this ),
              values_( values )
        {
        }

        Type&& operator*()
        {
            auto&& value = std::move( values_[this->current()] );
            values_.pop_back();
            return std::move( value );
        }

    private:
        std::vector< Type >& values_;
    };
} // namespace geode
