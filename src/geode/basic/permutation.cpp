/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/permutation.h>

#include <async++.h>

namespace geode
{
    std::vector< index_t > old2new_permutation(
        absl::Span< const index_t > permutation )
    {
        std::vector< index_t > old2new( permutation.size() );
        async::parallel_for( async::irange( size_t{ 0 }, permutation.size() ),
            [&old2new, &permutation]( size_t i ) {
                old2new[permutation[i]] = i;
            } );
        return old2new;
    }
} // namespace geode
