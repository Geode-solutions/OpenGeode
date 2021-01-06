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

#include <geode/basic/logger.h>
#include <geode/basic/permutation.h>

#include <geode/tests/common.h>

void test()
{
    std::vector< double > data{ 0.1, 0.2, 0.3, 0.4, 0.5 };
    std::vector< geode::index_t > permutation{ 2, 4, 3, 0, 1 };

    geode::permute( data, permutation );
    std::vector< double > data_answer{ 0.3, 0.5, 0.4, 0.1, 0.2 };
    OPENGEODE_EXCEPTION( data == data_answer, "[Test] Wrong permutation data" );

    const auto old2new = geode::old2new_permutation( permutation );
    absl::FixedArray< geode::index_t > answer_permutation{ 3, 4, 0, 2, 1 };
    OPENGEODE_EXCEPTION(
        old2new == answer_permutation, "[Test] Wrong permutation old2new" );
}

OPENGEODE_TEST( "permutation" )
