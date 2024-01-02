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

#include <async++.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>
#include <geode/basic/range.h>

#include <geode/tests/common.h>

void test_when_all()
{
    try
    {
        std::vector< async::task< void > > tasks;
        for( const auto t : geode::Range{ 10 } )
        {
            geode_unused( t );
            tasks.emplace_back( async::spawn( [] {
                throw std::runtime_error( "Some list error" );
            } ) );
        }
        for( auto& task : async::when_all( tasks ).get() )
        {
            task.get();
        }
        exit( 1 );
    }
    catch( ... )
    {
        geode::geode_lippincott();
    }

    try
    {
        std::vector< async::task< int > > tasks;
        for( const auto t : geode::Range{ 10 } )
        {
            geode_unused( t );
            tasks.emplace_back( async::spawn( [] {
                throw std::runtime_error( "Some list error 2" );
                return 42;
            } ) );
        }
        async::when_all( tasks )
            .then( []( std::vector< async::task< int > > all_tasks ) {
                for( auto& task : all_tasks )
                {
                    DEBUG( "toto" );
                    task.get();
                }
            } )
            .get();
        exit( 1 );
    }
    catch( ... )
    {
        geode::geode_lippincott();
    }
}

void test_parallel()
{
    try
    {
        async::parallel_invoke(
            [] {
                throw std::runtime_error( "Some // error 0" );
            },
            [] {
                throw std::runtime_error( "Some // error 1" );
            },
            [] {
                throw std::runtime_error( "Some // error 2" );
            },
            [] {
                throw std::runtime_error( "Some // error 3" );
            } );
        exit( 1 );
    }
    catch( ... )
    {
        geode::geode_lippincott();
    }

    try
    {
        async::parallel_for( async::irange( 0, 10 ), []( int i ) {
            geode_unused( i );
            throw std::runtime_error( "Some for error" );
        } );
        exit( 1 );
    }
    catch( ... )
    {
        geode::geode_lippincott();
    }
}

void test_async()
{
    try
    {
        async::spawn( [] {
            throw std::runtime_error( "Some async error" );
            return 42;
        } )
            .then( []( int t ) {
                DEBUG( t );
            } )
            .get();
        exit( 1 );
    }
    catch( ... )
    {
        geode::geode_lippincott();
    }
}

void test_exception()
{
    try
    {
        throw geode::OpenGeodeException{ "try ", "some ", "concatenation" };
    }
    catch( ... )
    {
        geode::geode_lippincott();
    }

    try
    {
        throw std::runtime_error{ "try std exception" };
    }
    catch( ... )
    {
        geode::geode_lippincott();
    }

    try
    {
        throw std::runtime_error{ "test" };
    }
    catch( ... )
    {
        geode::geode_lippincott();
    }

    try
    {
        geode::geode_assertion_failed(
            "0 == 1", "0 is never equal to 1.", "this file", 109 );
    }
    catch( ... )
    {
        geode::geode_lippincott();
    }
}

void test()
{
    test_exception();
    test_async();
    test_parallel();
    test_when_all();
}

OPENGEODE_TEST( "assert" )