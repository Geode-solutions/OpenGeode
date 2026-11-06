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

#include <memory>
#include <vector>

#include <geode/basic/input.hpp>
#include <geode/basic/logger.hpp>

#include <geode/tests/common.hpp>

namespace
{
    bool load_one_throws( std::vector< std::unique_ptr< int > >&& objects )
    {
        try
        {
            geode_unused( geode::load_one( std::move( objects ), "file" ) );
        }
        catch( const geode::OpenGeodeException& exception )
        {
            geode::Logger::info( exception.what() );
            return true;
        }
        return false;
    }

    void test_single_object()
    {
        std::vector< std::unique_ptr< int > > objects;
        objects.emplace_back( std::make_unique< int >( 42 ) );
        const auto object = geode::load_one( std::move( objects ), "file" );
        geode::OpenGeodeBasicException::test(
            object && *object == 42, "[Test] Wrong single object" );
    }

    void test_no_object()
    {
        geode::OpenGeodeBasicException::test(
            load_one_throws( std::vector< std::unique_ptr< int > >{} ),
            "[Test] load_one should throw on an empty vector" );
    }

    void test_two_objects()
    {
        std::vector< std::unique_ptr< int > > objects;
        objects.emplace_back( std::make_unique< int >( 1 ) );
        objects.emplace_back( std::make_unique< int >( 2 ) );
        geode::OpenGeodeBasicException::test(
            load_one_throws( std::move( objects ) ),
            "[Test] load_one should throw on two objects" );
    }
} // namespace

void test()
{
    test_single_object();
    test_no_object();
    test_two_objects();
}

OPENGEODE_TEST( "input" )
