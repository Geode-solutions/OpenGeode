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
#include <geode/basic/range.h>

#include <geode/tests/common.h>

void test()
{
    std::vector< geode::index_t > range1;
    for( const auto i : geode::Range{ 10 } )
    {
        range1.push_back( i );
    }
    OPENGEODE_EXCEPTION(
        range1.front() == 0, "[Test] Wrong Range first item (1)" );
    OPENGEODE_EXCEPTION(
        range1.back() == 9, "[Test] Wrong Range last item (1)" );

    std::vector< geode::index_t > range2;
    for( const auto i : geode::Range{ 3, 10 } )
    {
        range2.push_back( i );
    }
    OPENGEODE_EXCEPTION(
        range2.front() == 3, "[Test] Wrong Range first item (2)" );
    OPENGEODE_EXCEPTION(
        range2.back() == 9, "[Test] Wrong Range last item (2)" );

    std::vector< geode::index_t > reverse_range1;
    for( const auto i : geode::ReverseRange{ 10 } )
    {
        reverse_range1.push_back( i );
    }
    OPENGEODE_EXCEPTION( reverse_range1.front() == 9,
        "[Test] Wrong ReverseRange first item (1)" );
    OPENGEODE_EXCEPTION(
        reverse_range1.back() == 0, "[Test] Wrong ReverseRange last item (2)" );

    std::vector< geode::index_t > reverse_range2;
    for( const auto i : geode::ReverseRange{ 10, 3 } )
    {
        reverse_range2.push_back( i );
    }
    OPENGEODE_EXCEPTION( reverse_range2.front() == 9,
        "[Test] Wrong ReverseRange first item (1)" );
    OPENGEODE_EXCEPTION(
        reverse_range2.back() == 3, "[Test] Wrong ReverseRange last item (2)" );
}

OPENGEODE_TEST( "range" )