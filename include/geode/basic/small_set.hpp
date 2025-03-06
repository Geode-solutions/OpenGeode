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

#include <absl/algorithm/container.h>
#include <absl/container/inlined_vector.h>

#include <geode/basic/common.hpp>
#include <geode/basic/range.hpp>

namespace geode
{
    template < typename Type, index_t capacity = 10 >
    class SmallSet
    {
    public:
        auto size() const
        {
            return container_.size();
        }

        auto empty() const
        {
            return container_.empty();
        }

        auto begin() const
        {
            return container_.begin();
        }

        auto end() const
        {
            return container_.end();
        }

        auto insert( const Type& element )
        {
            if( absl::c_contains( container_, element ) )
            {
                return false;
            }
            container_.push_back( element );
            return true;
        }

        auto at( index_t index ) const
        {
            return container_.at( index );
        }

        auto at( index_t index )
        {
            return container_.at( index );
        }

    private:
        absl::InlinedVector< Type, capacity > container_;
    };
} // namespace geode
