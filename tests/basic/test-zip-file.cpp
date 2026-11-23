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
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <absl/strings/str_cat.h>

#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/zip_file.hpp>

#include <geode/tests/common.hpp>

using ZipEntries = std::vector< std::pair< std::string, std::string > >;

void write_deflate_zip( const std::string& zip, const ZipEntries& entries )
{
    void* writer = mz_zip_writer_create();
    mz_zip_writer_set_compress_method( writer, MZ_COMPRESS_METHOD_DEFLATE );
    auto status = mz_zip_writer_open_file( writer, zip.c_str(), 0, 0 );
    for( const auto& [name, content] : entries )
    {
        if( status != MZ_OK )
        {
            break;
        }
        mz_zip_file info{};
        info.filename = name.c_str();
        info.compression_method = MZ_COMPRESS_METHOD_DEFLATE;
        info.flag = MZ_ZIP_FLAG_UTF8;
        info.modified_date = std::time( nullptr );
        status = mz_zip_writer_add_buffer( writer, content.data(),
            static_cast< int32_t >( content.size() ), &info );
    }
    const auto close_status = mz_zip_writer_close( writer );
    mz_zip_writer_delete( &writer );
    geode::OpenGeodeBasicException::test(
        status == MZ_OK && close_status == MZ_OK, "Cannot write DEFLATE zip (",
        status, ", ", close_status, ")" );
}

void check_entries_are_deflated( const std::string& zip )
{
    void* reader = mz_zip_reader_create();
    geode::OpenGeodeBasicException::test(
        mz_zip_reader_open_file( reader, zip.c_str() ) == MZ_OK,
        "Cannot open DEFLATE zip" );
    auto status = mz_zip_reader_goto_first_entry( reader );
    while( status == MZ_OK )
    {
        mz_zip_file* info = nullptr;
        geode::OpenGeodeBasicException::test(
            mz_zip_reader_entry_get_info( reader, &info ) == MZ_OK,
            "Cannot read DEFLATE zip entry" );
        geode::OpenGeodeBasicException::test(
            info->compression_method == MZ_COMPRESS_METHOD_DEFLATE
                && info->compressed_size < info->uncompressed_size,
            "Zip entry ", info->filename, " is not DEFLATE-compressed" );
        status = mz_zip_reader_goto_next_entry( reader );
    }
    mz_zip_reader_close( reader );
    mz_zip_reader_delete( &reader );
    geode::OpenGeodeBasicException::test(
        status == MZ_END_OF_LIST, "Error while reading DEFLATE zip entries" );
}

std::string read_file( const std::filesystem::path& file )
{
    std::ifstream stream{ file, std::ios::binary };
    geode::OpenGeodeBasicException::test(
        stream.is_open(), "Cannot open extracted file ", file.u8string() );
    return { std::istreambuf_iterator< char >{ stream },
        std::istreambuf_iterator< char >{} };
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
    const std::string zip{ "deflate.zip" };
    write_deflate_zip( zip, entries );
    check_entries_are_deflated( zip );

    const geode::UnzipFile unzip{ zip, geode::uuid{}.string() };
    unzip.extract_all();
    const std::filesystem::path directory{ unzip.directory() };
    for( const auto& [name, content] : entries )
    {
        geode::OpenGeodeBasicException::test(
            read_file( directory / name ) == content, "Extracted entry ", name,
            " differs from the DEFLATE-compressed content" );
    }
    std::filesystem::remove( zip );
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