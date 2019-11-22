/*
 * Copyright (c) 2019 Geode-solutions
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

#include <fstream>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/logger.h>

#include <geode/tests/common.h>

#define CHECK( arg, value )                                                    \
    OPENGEODE_EXCEPTION( arg == value, "[Test] Wrong value for " #arg );

struct Foo
{
    template < typename Archive >
    void serialize( Archive &archive )
    {
        archive.ext( *this, geode::DefaultGrowable< Archive, Foo >{},
            []( Archive &archive, Foo &foo ) {
                archive.value8b( foo.double_ );
                archive.value4b( foo.unsigned_int_ );
            } );
    }

    double double_{ 10 };
    unsigned int unsigned_int_{ 10 };
};

struct Foo2
{
    template < typename Archive >
    void serialize( Archive &archive )
    {
        archive.ext( *this, geode::Growable< Archive, Foo2 >{
                                { []( Archive &archive, Foo2 &foo ) {
                                     archive.value8b( foo.double_ );
                                     archive.value4b( foo.unsigned_int_ );
                                 },
                                    []( Archive &archive, Foo2 &foo ) {
                                        archive.value1b( foo.bool_ );
                                    } } } );
    }
    double double_{ 10 };
    unsigned int unsigned_int_{ 10 };
    bool bool_{ false };
};

struct Foo3
{
    template < typename Archive >
    void serialize( Archive &archive )
    {
        archive.ext( *this, geode::Growable< Archive, Foo3 >{
                                { []( Archive &archive, Foo3 &foo ) {
                                     archive.value8b( foo.double_ );
                                     archive.value4b( foo.unsigned_int_ );
                                 },
                                    []( Archive &archive, Foo3 &foo ) {
                                        archive.value1b( foo.bool_ );
                                    },
                                    []( Archive &archive, Foo3 &foo ) {
                                        archive.value4b( foo.int_ );
                                    } },
                                { []( Foo3 &foo ) { foo.bool_ = true; },
                                    []( Foo3 &foo ) { foo.int_ = -52; } } } );
    }
    double double_{ 10 };
    unsigned int unsigned_int_{ 10 };
    bool bool_{ false };
    int int_{ 10 };
};

template < typename Out, typename T >
Out test_growable( const T &foo )
{
    std::ofstream output{ "name", std::ofstream::binary };
    geode::TContext context_output{};
    geode::Serializer serializer{ context_output, output };
    serializer.object( foo );
    serializer.adapter().flush();

    std::ifstream input{ "name", std::ifstream::binary };
    geode::TContext context_input{};
    geode::Deserializer deserializer{ context_input, input };
    Out new_foo;
    deserializer.object( new_foo );
    const auto &adapter = deserializer.adapter();
    OPENGEODE_EXCEPTION( adapter.error() == bitsery::ReaderError::NoError
                             && adapter.isCompletedSuccessfully(),
        "[Test] Error while reading file" );
    return new_foo;
}

void test()
{
    Foo foo;
    foo.double_ = 42.5;
    foo.unsigned_int_ = 42;
    auto foo2 = test_growable< Foo2 >( foo );
    CHECK( foo2.double_, 42.5 );
    CHECK( foo2.unsigned_int_, 42 );
    CHECK( foo2.bool_, false );

    foo2.bool_ = true;
    auto foo3 = test_growable< Foo2 >( foo2 );
    CHECK( foo3.double_, 42.5 );
    CHECK( foo3.unsigned_int_, 42 );
    CHECK( foo3.bool_, true );

    auto foo4 = test_growable< Foo3 >( foo );
    CHECK( foo4.double_, 42.5 );
    CHECK( foo4.unsigned_int_, 42 );
    CHECK( foo4.bool_, true );
    CHECK( foo4.int_, -52 );
}

OPENGEODE_TEST( "growable" )