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

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/database.h>
#include <geode/basic/identifier.h>
#include <geode/basic/logger.h>
#include <geode/basic/uuid.h>

#include <geode/tests/common.h>

struct Foo : public geode::Identifier
{
    Foo() = default;
    Foo( double value ) : value_( value ) {}

    template < typename Archive >
    void serialize( Archive& archive )
    {
        archive.ext( *this, bitsery::ext::BaseClass< geode::Identifier >{} );
        archive.value8b( value_ );
    }

    double value_{ 0 };
};

void register_foo_serializer( geode::PContext& context )
{
    context
        .registerSingleBaseBranch< geode::Serializer, geode::Identifier, Foo >(
            "Foo" );
}

void register_foo_deserializer( geode::PContext& context )
{
    context.registerSingleBaseBranch< geode::Deserializer, geode::Identifier,
        Foo >( "Foo" );
}

void test_take_data( geode::Database& database, const geode::uuid& id )
{
    auto stolen_foo = database.take_data< Foo >( id );
    OPENGEODE_EXCEPTION(
        stolen_foo->value_ == 42, "[Test] Wrong value after take data" );
    auto foo_data = database.get_data( id );
    const auto& foo = foo_data.get< Foo >();
    OPENGEODE_EXCEPTION(
        foo.value_ == 42, "[Test] Wrong value after register data" );
    OPENGEODE_EXCEPTION( stolen_foo.get() != &foo,
        "[Test] Objects adresses should be different" );
}

void test_take_wrong_data( geode::Database& database, const geode::uuid& id )
{
    try
    {
        auto stolen_foo = database.take_data< geode::uuid >( id );
        throw geode::OpenGeodeException{
            "[Test] Should not be able to cast data into uuid"
        };
    }
    catch( ... )
    {
        return;
    }
}

geode::uuid test_register_data( geode::Database& database )
{
    auto foo = database.register_new_data( Foo{ 42 } );
    auto foo_data = database.get_data( foo );
    OPENGEODE_EXCEPTION( foo_data.get< Foo >().value_ == 42,
        "[Test] Wrong value after register data" );
    return foo;
}

geode::uuid test_register_unique_data( geode::Database& database )
{
    auto foo = database.register_new_data( absl::make_unique< Foo >( 22 ) );
    auto foo_data = database.get_data( foo );
    OPENGEODE_EXCEPTION( foo_data.get< Foo >().value_ == 22,
        "[Test] Wrong value after register unique data" );
    return foo;
}

void test_modify_data( geode::Database& database, const geode::uuid& id )
{
    auto foo_data = database.get_data( id );
    const auto& foo = foo_data.get< Foo >();
    auto taken_foo = database.take_data< Foo >( id );
    taken_foo->value_ = 12;
    OPENGEODE_EXCEPTION(
        taken_foo->value_ == 12, "[Test] Wrong value after modify taken data" );
    OPENGEODE_EXCEPTION(
        foo.value_ == 42, "[Test] Wrong value after register data" );
    database.update_data( id, std::move( taken_foo ) );
    auto foo_data2 = database.get_data( id );
    const auto& foo2 = foo_data2.get< Foo >();
    OPENGEODE_EXCEPTION(
        foo2.value_ == 12, "[Test] Wrong value after register data" );
    OPENGEODE_EXCEPTION(
        foo.value_ == 42, "[Test] Wrong value after register data" );
}

void test()
{
    geode::Database database( "temp" );
    database.register_serializer_functions(
        register_foo_serializer, register_foo_deserializer );
    auto foo0 = test_register_data( database );
    test_register_unique_data( database );
    test_take_data( database, foo0 );
    test_modify_data( database, foo0 );
    test_take_wrong_data( database, foo0 );
    OPENGEODE_EXCEPTION(
        database.nb_data() == 2, "[Test] Database incomplete" );
}

OPENGEODE_TEST( "filename" )