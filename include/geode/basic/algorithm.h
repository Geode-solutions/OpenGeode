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

#include <geode/basic/common.h>
#include <geode/basic/range.h>

#include <algorithm>

namespace geode
{
    template < typename T, typename container >
    index_t find( const container& in, const T& value )
    {
        auto it = std::find( in.begin(), in.end(), value );
        if( it == in.end() )
        {
            return NO_ID;
        }
        else
        {
            return static_cast< index_t >( it - in.begin() );
        }
    }

    template < typename T, typename container >
    bool contain( const container& in, const T& value )
    {
        return find( in, value ) != NO_ID;
    }

    template < typename T >
    index_t delete_vector_elements( const std::vector< bool >& to_delete,
        std::vector< T >& values,
        bool resize = false )
    {
        OPENGEODE_EXCEPTION( to_delete.size() == values.size(),
            "Number of elements in the two vector should match" );
        index_t nb_removed_elements{ 0 };
        for( auto i : Range{ to_delete.size() } )
        {
            if( to_delete[i] )
            {
                nb_removed_elements++;
            }
            else
            {
                values[i - nb_removed_elements] = values[i];
            }
        }
        if( resize )
        {
            values.resize( to_delete.size() - nb_removed_elements );
        }
        return nb_removed_elements;
    }

} // namespace geode
