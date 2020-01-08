/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/factory.h>

#include <geode/tests/common.h>

class A
{
    OPENGEODE_DISABLE_COPY_AND_MOVE( A );

public:
    A() = default;
    ~A() = default;
};

class B
{
    OPENGEODE_DISABLE_COPY_AND_MOVE( B );

public:
    B() = default;
    ~B() = default;
};

class Base
{
    OPENGEODE_DISABLE_COPY_AND_MOVE( Base );

public:
    virtual ~Base() = default;

protected:
    Base( A &a, B &b ) : a_( a ), b_( b ) {}

protected:
    A &a_;
    B &b_;
};

class Derived : public Base
{
public:
    Derived( A &a, B &b ) : Base( a, b ) {}
};

void verdict( bool is_instantiated, const std::string &name )
{
    if( !is_instantiated )
    {
        throw geode::OpenGeodeException{ "TEST Failed to instantiate the ",
            name, " class" };
    }
}

void test()
{
    using factory = geode::Factory< std::string, Base, A &, B & >;
    factory::register_creator< Derived >( "Derived" );

    const auto creators = factory::list_creators();
    factory::register_creator< Derived >( "Derived" );
    OPENGEODE_EXCEPTION( factory::list_creators().size() == creators.size(),
        "[Test] Key registered twice" );

    A a;
    B b;
    OPENGEODE_EXCEPTION( factory::has_creator( "Derived" ),
        "[Test] Key has not been registered" );
    const auto d = factory::create( "Derived", a, b );
    verdict( d != nullptr, "Derived" );
}

OPENGEODE_TEST( "factory" )