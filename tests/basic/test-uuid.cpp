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

#include <sstream>

#include <absl/container/flat_hash_map.h>

#include <bitsery/ext/std_map.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/tests/common.hpp>

void test_generation()
{
    for( const auto i : geode::Range{ 100 } )
    {
        geode_unused( i );
        const geode::uuid id;
        SDEBUG( id );
        const geode::uuid id2;
        geode::OpenGeodeBasicException::test(
            id2 != id, "UUIDs should be different" );
        geode::OpenGeodeBasicException::test(
            id2 < id || id < id2, "UUIDs should be different" );
        const geode::uuid same{ id.string() };
        geode::OpenGeodeBasicException::test(
            id == same, "UUIDs should be equal" );
    }
    const geode::uuid upper{ "0198D5E1-A2B3-7C4D-8E5F-60718293A4B5" };
    geode::OpenGeodeBasicException::test(
        upper.string() == "0198d5e1-a2b3-7c4d-8e5f-60718293a4b5",
        "Upper case UUID should be parsed" );
}

void test_invalid_string()
{
    for( const auto* invalid : { "zzzzzzzz-zzzz-zzzz-zzzz-zzzzzzzzzzzz",
             " 198d5e1-a2b3-7c4d-8e5f-60718293a4b5",
             "0198d5e1-a2b3-7c4d-8e5f-60718293a4b+" } )
    {
        bool thrown{ false };
        try
        {
            const geode::uuid id{ invalid };
            geode_unused( id );
        }
        catch( const geode::OpenGeodeException& )
        {
            thrown = true;
        }
        geode::OpenGeodeBasicException::test(
            thrown, "Invalid UUID string should be rejected: ", invalid );
    }
}

void test_deserialization()
{
    const geode::uuid nil{ geode::BITSERY::constructor };
    for( const auto byte : nil.bytes_ )
    {
        geode::OpenGeodeBasicException::test(
            byte == 0, "UUID built for deserialization should be nil" );
    }
    geode::OpenGeodeBasicException::test(
        bitsery::Access::create< geode::uuid >() == nil,
        "UUID created by bitsery should be nil (not generated)" );

    using Map = absl::flat_hash_map< geode::uuid, geode::index_t >;
    const geode::uuid id;
    Map map;
    for( const auto i : geode::Range{ 10 } )
    {
        map.emplace( geode::uuid{}, i );
    }
    std::stringstream stream;
    {
        geode::TContext context{};
        geode::Serializer archive{ context, stream };
        archive.object( id );
        archive.ext( map, bitsery::ext::StdMap{ map.size() },
            []( geode::Serializer& local_archive, geode::uuid& key,
                geode::index_t& value ) {
                local_archive.object( key );
                local_archive.value4b( value );
            } );
        archive.adapter().flush();
    }
    geode::uuid loaded_id{ geode::BITSERY::constructor };
    Map loaded_map;
    {
        geode::TContext context{};
        const auto bytes = stream.str();
        geode::Deserializer archive{ context, bytes.data(), bytes.size() };
        archive.object( loaded_id );
        archive.ext( loaded_map, bitsery::ext::StdMap{ map.size() },
            []( geode::Deserializer& local_archive, geode::uuid& key,
                geode::index_t& value ) {
                local_archive.object( key );
                local_archive.value4b( value );
            } );
        geode::OpenGeodeBasicException::test(
            archive.adapter().error() == bitsery::ReaderError::NoError
                && archive.adapter().isCompletedSuccessfully(),
            "UUID deserialization should succeed" );
    }
    geode::OpenGeodeBasicException::test(
        loaded_id == id, "Deserialized UUID should be the saved one" );
    geode::OpenGeodeBasicException::test(
        loaded_map == map, "Deserialized UUID keys should be the saved ones" );
}

void test()
{
    test_generation();
    test_invalid_string();
    test_deserialization();
}

OPENGEODE_TEST( "uuid" )