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

#include <algorithm>
#include <vector>

#include <absl/algorithm/container.h>

#include <geode/basic/common.h>
#include <geode/basic/range.h>

namespace geode
{
    /*
     * Delete some elements from a given vector.
     * @tparam T Type of the vector elements.
     * @param[in] to_delete Vector of the same size than values. If to_delete[i]
     * is true the i-th element is deleted, else the element is kept.
     * @param[in/out] values Vector in which perform deletions.
     * @pre T should have default constructor (needed for call resize method on
     * vector).
     * @return The number of deleted elements
     */
    template < typename T >
    index_t delete_vector_elements(
        const std::vector< bool >& to_delete, std::vector< T >& values )
    {
        OPENGEODE_ASSERT( to_delete.size() == values.size(),
            "[delete_vector_elements] Number of elements in the two vectors "
            "should match" );
        index_t nb_removed_elements{ 0 };
        if( absl::c_find( to_delete, true ) == to_delete.end() )
        {
            return 0;
        }
        for( const auto i : Indices{ to_delete } )
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
        values.erase( values.end() - nb_removed_elements, values.end() );
        return nb_removed_elements;
    }

    /*
     * Create a new vector containing only some elements from a given vector.
     * @tparam T Type of the vector elements.
     * @param[in] to_keep Vector of the same size than in_values. If to_keep[i]
     * is true the i-th element is kept.
     * @param[in] in_values Vector in which perform deletions.
     * @return A vector containing only kept elements of in_values
     */
    template < typename T >
    std::vector< T > extract_vector_elements(
        const std::vector< bool >& to_keep, const std::vector< T >& in_values )
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
        std::vector< T > out_values;
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

    /*
     * Modify the container by removing every duplicated values inside
     * @tparam Container Type of container.
     * @param[in] in container in which perform the search.
     */
    template < typename Container >
    void sort_unique( Container& in )
    {
        absl::c_sort( in );
        const auto last = std::unique( in.begin(), in.end() );
        in.erase( last, in.end() );
    }

} // namespace geode
