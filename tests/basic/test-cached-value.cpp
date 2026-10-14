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

#include <atomic>

#include <async++.h>

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

std::atomic< geode::index_t > nb_concurrent_computed{ 0 };

Value compute_value_concurrent( double d, int i )
{
    nb_concurrent_computed++;
    Value value;
    value.d = d;
    value.i = i;
    return value;
}

void test_recompute_if_invalid()
{
    geode::CachedValue< Value > cache;
    const auto accept_all = []( const Value& /*unused*/ ) {
        return true;
    };
    const auto accept_positive = []( const Value& value ) {
        return value.i > 0;
    };
    async::parallel_for( async::irange( 0, 1000 ), [&]( int /*unused*/ ) {
        geode::OpenGeodeBasicException::test(
            cache.recompute_if_invalid(
                     accept_all, compute_value_concurrent, 12., 15 )
                    .test()
                == 27,
            "Wrong concurrent result" );
    } );
    geode::OpenGeodeBasicException::test( nb_concurrent_computed == 1,
        "Concurrent callers on a valid cache should compute once, not ",
        nb_concurrent_computed.load() );
    geode::OpenGeodeBasicException::test(
        cache.recompute_if_invalid(
                 accept_positive, compute_value_concurrent, 1., -5 )
                .test()
            == 27,
        "Valid value should be kept" );
    geode::OpenGeodeBasicException::test(
        nb_concurrent_computed == 1, "Valid value should not be recomputed" );
    geode::OpenGeodeBasicException::test(
        cache.recompute_if_invalid(
                 []( const Value& /*unused*/ ) {
                     return false;
                 },
                 compute_value_concurrent, 1., -5 )
                .test()
            == -4,
        "Invalid value should be recomputed" );
    geode::OpenGeodeBasicException::test( nb_concurrent_computed == 2,
        "Invalid value should be recomputed once" );
}

void test()
{
    test_recompute_if_invalid();
    geode::CachedValue< Value > cache;
    geode::OpenGeodeBasicException::test(
        nb_computed == 0, "Wrong number of computations" );
    geode::OpenGeodeBasicException::test(
        cache( compute_value, 12., 15 ).test() == 27, "Wrong result" );
    geode::OpenGeodeBasicException::test(
        nb_computed == 1, "Wrong number of computations" );
    geode::OpenGeodeBasicException::test(
        cache( compute_value, 12., 15 ).test() == 27, "Wrong result" );
    geode::OpenGeodeBasicException::test(
        cache( compute_value, 1., 5 ).test() == 27, "Wrong result" );
    geode::OpenGeodeBasicException::test(
        nb_computed == 1, "Wrong number of computations" );
    cache.reset();
    geode::OpenGeodeBasicException::test(
        cache( compute_value, 2., 5 ).test() == 7, "Wrong result" );
    geode::OpenGeodeBasicException::test(
        nb_computed == 2, "Wrong number of computations" );
}

OPENGEODE_TEST( "cached-value" )