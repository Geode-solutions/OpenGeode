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

#include <async++.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>

#include <geode/tests/common.hpp>

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
        throw geode::OpenGeodeBasicException{ nullptr,
            geode::OpenGeodeException::TYPE::internal, "try ", "some ",
            "concatenation" };
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
}

void test_exception_string()
{
    try
    {
        try
        {
            try
            {
                throw std::runtime_error{ "root cause" };
            }
            catch( ... )
            {
                geode::throw_lippincott< geode::OpenGeodeBasicException >(
                    geode::OpenGeodeException::TYPE::internal, "middle" );
            }
        }
        catch( ... )
        {
            geode::throw_lippincott< geode::OpenGeodeBasicException >(
                geode::OpenGeodeException::TYPE::data, "top" );
        }
    }
    catch( const geode::OpenGeodeException& exception )
    {
        const auto expected = absl::StrCat( exception.string(),
            "\nFrom: ", exception.parent().string(),
            "\nFrom: ", exception.parent().parent().string() );
        geode::OpenGeodeBasicException::test(
            exception.full_string() == expected,
            "[Test] Wrong full_string: ", exception.full_string() );
        geode::OpenGeodeBasicException::test(
            geode::exception_string( exception ) == expected,
            "[Test] Wrong exception_string of an OpenGeodeException" );
        geode::OpenGeodeBasicException::test(
            exception.parent().parent().string().find(
                "std::exception: root cause" )
                != std::string::npos,
            "[Test] Root cause not in the chain" );
    }

    const geode::OpenGeodeBasicException single{ nullptr,
        geode::OpenGeodeException::TYPE::data, "alone" };
    geode::OpenGeodeBasicException::test(
        single.full_string() == single.string(),
        "[Test] full_string without parent should be string()" );
    geode::OpenGeodeBasicException::test(
        geode::exception_string( std::runtime_error{ "plain" } )
            == "std::exception: plain",
        "[Test] Wrong exception_string of a std::exception" );
}

void test()
{
    test_exception_string();
    test_exception();
    test_async();
    test_parallel();
    test_when_all();
}

OPENGEODE_TEST( "assert" )