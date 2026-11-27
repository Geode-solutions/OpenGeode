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

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <absl/strings/str_cat.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/zip_file.hpp>

#include <geode/tests/common.hpp>

using ZipEntries = std::vector< std::pair< std::string, std::string > >;

std::string read_file( const std::filesystem::path& file )
{
    std::ifstream stream{ file, std::ios::binary };
    geode::OpenGeodeBasicException::test(
        stream.is_open(), "Cannot open file ", file.u8string() );
    return { std::istreambuf_iterator< char >{ stream },
        std::istreambuf_iterator< char >{} };
}

uint32_t read_uint( const std::string& data, size_t offset, size_t size )
{
    uint32_t value{ 0 };
    for( const auto i : geode::Range{ size } )
    {
        value |= static_cast< uint32_t >(
                     static_cast< unsigned char >( data[offset + i] ) )
                 << ( 8 * i );
    }
    return value;
}

void check_entries_are_deflated(
    const std::string& zip, const ZipEntries& entries )
{
    const auto archive = read_file( zip );
    // End of central directory record (no archive comment)
    constexpr size_t END_RECORD_SIZE{ 22 };
    const auto end_record = archive.size() - END_RECORD_SIZE;
    geode::OpenGeodeBasicException::test(
        archive.compare( end_record, 4, "PK\x05\x06" ) == 0,
        "End of central directory not found in ", zip );
    const auto nb_entries = read_uint( archive, end_record + 10, 2 );
    size_t header = read_uint( archive, end_record + 16, 4 );
    geode::index_t nb_checked{ 0 };
    for( const auto entry : geode::Range{ nb_entries } )
    {
        geode_unused( entry );
        geode::OpenGeodeBasicException::test(
            archive.compare( header, 4, "PK\x01\x02" ) == 0,
            "Invalid central directory header in ", zip );
        const auto name_length = read_uint( archive, header + 28, 2 );
        const auto name = archive.substr( header + 46, name_length );
        geode::OpenGeodeBasicException::test(
            read_uint( archive, header + 10, 2 ) == 8, "Zip entry ", name,
            " is not DEFLATE-compressed" );
        for( const auto& [entry_name, content] : entries )
        {
            if( entry_name == name )
            {
                geode::OpenGeodeBasicException::test(
                    read_uint( archive, header + 20, 4 )
                        < read_uint( archive, header + 24, 4 ),
                    "Zip entry ", name, " is not DEFLATE-compressed" );
                nb_checked++;
            }
        }
        header += 46 + name_length + read_uint( archive, header + 30, 2 )
                  + read_uint( archive, header + 32, 2 );
    }
    geode::OpenGeodeBasicException::test( nb_checked == entries.size(),
        "Missing DEFLATE-compressed entries in ", zip );
}

void test_unzip_deflate()
{
    std::string text;
    for( const auto i : geode::Range{ 10000 } )
    {
        absl::StrAppend( &text, "line ", i, " of a compressible file\n" );
    }
    std::string binary;
    for( const auto i : geode::Range{ 100000 } )
    {
        binary.push_back( static_cast< char >( ( i * i ) % 7 ) );
    }
    const ZipEntries entries{ { "text.txt", text },
        { "folder/binary.bin", binary } };
    // Written by Python zipfile with ZIP_DEFLATED
    const auto zip =
        absl::StrCat( geode::DATA_PATH, "input_files_deflate.zip" );
    check_entries_are_deflated( zip, entries );

    const geode::UnzipFile unzip{ zip, geode::uuid{}.string() };
    unzip.extract_all();
    const std::filesystem::path directory{ unzip.directory() };
    for( const auto& [name, content] : entries )
    {
        geode::OpenGeodeBasicException::test(
            read_file( directory / name ) == content, "Extracted entry ", name,
            " differs from the DEFLATE-compressed content" );
    }
}

void test()
{
    test_unzip_deflate();
    const auto is_not_a_zip = geode::is_zip_file(
        absl::StrCat( geode::DATA_PATH, "triange.og_tsf3d" ) );
    geode::OpenGeodeBasicException::test(
        !is_not_a_zip, "Not a zip file detection failed" );
    const auto is_a_zip = geode::is_zip_file(
        absl::StrCat( geode::DATA_PATH, "layers.og_brep" ) );
    geode::OpenGeodeBasicException::test(
        is_a_zip, "zip file detection failed" );
}

OPENGEODE_TEST( "zip-file" )