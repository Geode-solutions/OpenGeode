/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

/*
 * Modified from Geogram
 * http://alice.loria.fr/index.php/software/4-library/75-geogram.html
 * Copyright (c) 2012-2014, Bruno Levy
 */

#pragma once

#include <absl/algorithm/container.h>
#include <absl/container/fixed_array.h>
#include <absl/types/span.h>

#include <geode/basic/common.hpp>
#include <geode/basic/range.hpp>

namespace geode
{
    template < index_t nb_elements, typename Container >
    void multiple_permute(
        Container& data, absl::Span< const index_t > permutation )
    {
        OpenGeodeBasicException::check_exception(
            permutation.size() == data.size() / nb_elements, nullptr,
            OpenGeodeException::TYPE::data,
            "[multiple_permute] Data and permutation should have the same "
            "size" );
        const auto index = []( const auto i ) {
            return nb_elements * i;
        };
        std::vector< bool > visited( permutation.size(), false );
        for( const auto p : Indices{ permutation } )
        {
            if( visited[p] )
            {
                continue;
            }
            visited[p] = true;
            auto i = p;
            absl::FixedArray< typename Container::value_type, nb_elements >
                temp( std::make_move_iterator( data.begin() + index( i ) ),
                    std::make_move_iterator(
                        data.begin() + index( i ) + nb_elements ) );
            auto j = permutation[p];
            while( j != p )
            {
                const auto from = index( j );
                const auto to = index( i );
                std::move( data.begin() + from,
                    data.begin() + from + nb_elements, data.begin() + to );
                visited[j] = true;
                i = j;
                j = permutation[i];
            }
            const auto to = index( i );
            absl::c_move( temp, data.begin() + to );
        }
    }

    template < typename Container >
    void permute( Container& data, absl::Span< const index_t > permutation )
    {
        multiple_permute< 1 >( data, permutation );
    }

    [[nodiscard]] std::vector< index_t > opengeode_basic_api
        old2new_permutation( absl::Span< const index_t > permutation );
} // namespace geode
