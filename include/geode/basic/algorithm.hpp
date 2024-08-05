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

#include <algorithm>
#include <vector>

#include <absl/algorithm/container.h>

#include <geode/basic/common.hpp>
#include <geode/basic/range.hpp>

namespace geode
{
    /*!
     * Delete some elements from a given vector.
     * @param[in] to_delete Vector of the same size than values. If to_delete[i]
     * is true the i-th element is deleted, else the element is kept.
     * @param[in/out] values Vector in which perform deletions.
     * @return The number of deleted elements
     */
    template < typename DeleteContainer, typename ValueContainer >
    index_t delete_vector_elements(
        const DeleteContainer& to_delete, ValueContainer& values )
    {
        OPENGEODE_ASSERT( to_delete.size() == values.size(),
            "[delete_vector_elements] Number of elements in the two vectors "
            "should match" );
        const auto it = absl::c_find( to_delete, true );
        if( it == to_delete.end() )
        {
            return 0;
        }
        index_t nb_removed_elements{ 0 };
        for( const auto i : Range{ it - to_delete.begin(), to_delete.size() } )
        {
            if( to_delete[i] )
            {
                nb_removed_elements++;
            }
            else
            {
                values[i - nb_removed_elements] = std::move( values[i] );
            }
        }
        values.erase( values.end() - nb_removed_elements, values.end() );
        return nb_removed_elements;
    }

    /*!
     * Create a new vector containing only some elements from a given vector.
     * @param[in] to_keep Vector of the same size than in_values. If to_keep[i]
     * is true the i-th element is kept.
     * @param[in] in_values Vector in which perform deletions.
     * @return A vector containing only kept elements of in_values
     */
    template < typename DeleteContainer, typename ValueContainer >
    ValueContainer extract_vector_elements(
        const DeleteContainer& to_keep, const ValueContainer& in_values )
    {
        OPENGEODE_ASSERT( to_keep.size() == in_values.size(),
            "[extract_vector_elements] Number of elements in the two vectors "
            "should match" );
        const auto nb =
            static_cast< index_t >( absl::c_count( to_keep, true ) );
        if( nb == in_values.size() )
        {
            return in_values;
        }
        ValueContainer out_values;
        out_values.reserve( nb );
        for( const auto i : Indices{ to_keep } )
        {
            if( to_keep[i] )
            {
                out_values.push_back( in_values[i] );
            }
        }
        return out_values;
    }

    /*!
     * Modify the container by removing every duplicated values inside
     * @tparam Container Type of container.
     * @param[in] container container in which perform the search.
     */
    template < typename Container >
    void sort_unique( Container& container )
    {
        absl::c_sort( container );
        const auto last = std::unique( container.begin(), container.end() );
        container.erase( last, container.end() );
    }

    template < typename Container, typename Comparison >
    void sort_unique( Container& container, Comparison comp )
    {
        absl::c_sort( container, comp );
        const auto last = std::unique( container.begin(), container.end() );
        container.erase( last, container.end() );
    }

    /*!
     * Concatenate tuples into a single tuple.
     */
    template < typename... tuples >
    using tuple_cat = decltype( std::tuple_cat( std::declval< tuples >()... ) );
} // namespace geode
