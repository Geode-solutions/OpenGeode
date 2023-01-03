/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <absl/container/fixed_array.h>
#include <absl/types/span.h>

#include <geode/basic/common.h>
#include <geode/basic/range.h>

namespace geode
{
    template < typename Container >
    void permute( Container& data, absl::Span< const index_t > permutation )
    {
        std::vector< bool > visited( permutation.size(), false );
        for( const auto p : Indices{ permutation } )
        {
            if( visited[p] )
            {
                continue;
            }
            visited[p] = true;
            auto i = p;
            auto temp = data[i];
            auto j = permutation[p];
            while( j != p )
            {
                data[i] = std::move( data[j] );
                visited[j] = true;
                i = j;
                j = permutation[i];
            }
            data[i] = std::move( temp );
        }
    }

    std::vector< index_t > opengeode_basic_api old2new_permutation(
        absl::Span< const index_t > permutation );
} // namespace geode
