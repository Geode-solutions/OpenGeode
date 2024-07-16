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

#include <geode/basic/cached_value.hpp>
#include <geode/basic/logger.hpp>

#include <geode/tests/common.hpp>

geode::index_t nb_computed{ 0 };

struct Value
{
    double test() const
    {
        return d + i;
    }

    double d;
    int i;
};

Value compute_value( double d, int i )
{
    nb_computed++;
    Value value;
    value.d = d;
    value.i = i;
    return value;
}

void test()
{
    geode::CachedValue< Value > cache;
    OPENGEODE_EXCEPTION(
        nb_computed == 0, "[Test] Wrong number of computations" );
    OPENGEODE_EXCEPTION(
        cache( compute_value, 12., 15 ).test() == 27, "[Test] Wrong result" );
    OPENGEODE_EXCEPTION(
        nb_computed == 1, "[Test] Wrong number of computations" );
    OPENGEODE_EXCEPTION(
        cache( compute_value, 12., 15 ).test() == 27, "[Test] Wrong result" );
    OPENGEODE_EXCEPTION(
        cache( compute_value, 1., 5 ).test() == 27, "[Test] Wrong result" );
    OPENGEODE_EXCEPTION(
        nb_computed == 1, "[Test] Wrong number of computations" );
    cache.reset();
    OPENGEODE_EXCEPTION(
        cache( compute_value, 2., 5 ).test() == 7, "[Test] Wrong result" );
    OPENGEODE_EXCEPTION(
        nb_computed == 2, "[Test] Wrong number of computations" );
}

OPENGEODE_TEST( "cached-value" )