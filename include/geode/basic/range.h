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

#include <geode/basic/types.h>

namespace geode
{
    /*!
     * Base class for range-based iteration on custom range.
     * See derived classes for usage.
     */
    template < typename Type >
    class BaseRange
    {
    public:
        bool operator!=( const BaseRange & /*unused*/ ) const
        {
            return iter_ != last_;
        }

        void operator++()
        {
            ++iter_;
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
     *    for( auto i : Range{ n } )
     *    {
     *      // do something
     *    }
     */
    class Range : public BaseRange< index_t >
    {
    public:
        template < typename T1, typename T2 >
        Range( T1 begin, T2 end ) : BaseRange( begin, end )
        {
        }

        template < typename T >
        explicit Range( T end ) : Range( 0, end )
        {
        }

        const Range &begin() const
        {
            return *this;
        }

        const Range &end() const
        {
            return *this;
        }

        index_t operator*() const
        {
            return current();
        }
    };
} // namespace geode
