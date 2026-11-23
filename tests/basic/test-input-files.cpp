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
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <async++.h>

#include <absl/strings/match.h>
#include <absl/strings/str_cat.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/detail/file_content.hpp>
#include <geode/basic/detail/input_files.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/tests/common.hpp>

using Entries = std::vector< std::pair< std::string, std::string > >;

std::string as_string( geode::ByteSpan bytes )
{
    return { bytes.data(), bytes.size() };
}

void write_file( const std::filesystem::path& file, std::string_view content )
{
    std::ofstream stream{ file, std::ios::binary };
    stream.write(
        content.data(), static_cast< std::streamsize >( content.size() ) );
    geode::OpenGeodeBasicException::test(
        stream.good(), "Cannot write file ", file.u8string() );
}

void append_uint( std::string& data, uint32_t value, size_t size )
{
    for( const auto i : geode::Range{ size } )
    {
        data.push_back( static_cast< char >( ( value >> ( 8 * i ) ) & 0xFF ) );
    }
}

uint32_t crc32( std::string_view content )
{
    uint32_t crc{ 0xFFFFFFFF };
    for( const auto byte : content )
    {
        crc ^= static_cast< unsigned char >( byte );
        for( const auto bit : geode::Range{ 8 } )
        {
            geode_unused( bit );
            crc = ( crc >> 1 ) ^ ( 0xEDB88320 & ( 0 - ( crc & 1 ) ) );
        }
    }
    return ~crc;
}

/*!
 * Write a zip archive whose entries are stored (not compressed).
 */
void write_stored_zip( const std::string& zip, const Entries& entries )
{
    std::string archive;
    std::string central_directory;
    for( const auto& [name, content] : entries )
    {
        const auto offset = static_cast< uint32_t >( archive.size() );
        const auto crc = crc32( content );
        const auto size = static_cast< uint32_t >( content.size() );
        const auto name_size = static_cast< uint32_t >( name.size() );
        append_uint( archive, 0x04034b50, 4 ); // local header signature
        append_uint( archive, 20, 2 ); // version needed
        append_uint( archive, 0, 2 ); // flags
        append_uint( archive, 0, 2 ); // method: store
        append_uint( archive, 0, 2 ); // time
        append_uint( archive, 0x21, 2 ); // date: 1980-01-01
        append_uint( archive, crc, 4 );
        append_uint( archive, size, 4 ); // compressed size
        append_uint( archive, size, 4 ); // uncompressed size
        append_uint( archive, name_size, 2 );
        append_uint( archive, 0, 2 ); // extra field length
        absl::StrAppend( &archive, name, content );
        append_uint( central_directory, 0x02014b50, 4 );
        append_uint( central_directory, 20, 2 ); // version made by
        append_uint( central_directory, 20, 2 ); // version needed
        append_uint( central_directory, 0, 2 ); // flags
        append_uint( central_directory, 0, 2 ); // method: store
        append_uint( central_directory, 0, 2 ); // time
        append_uint( central_directory, 0x21, 2 ); // date: 1980-01-01
        append_uint( central_directory, crc, 4 );
        append_uint( central_directory, size, 4 ); // compressed size
        append_uint( central_directory, size, 4 ); // uncompressed size
        append_uint( central_directory, name_size, 2 );
        append_uint( central_directory, 0, 2 ); // extra field length
        append_uint( central_directory, 0, 2 ); // comment length
        append_uint( central_directory, 0, 2 ); // disk number
        append_uint( central_directory, 0, 2 ); // internal attributes
        append_uint( central_directory, 0, 4 ); // external attributes
        append_uint( central_directory, offset, 4 ); // local header offset
        absl::StrAppend( &central_directory, name );
    }
    const auto central_directory_offset =
        static_cast< uint32_t >( archive.size() );
    const auto nb_entries = static_cast< uint32_t >( entries.size() );
    absl::StrAppend( &archive, central_directory );
    append_uint( archive, 0x06054b50, 4 ); // end of central directory
    append_uint( archive, 0, 2 ); // disk number
    append_uint( archive, 0, 2 ); // disk of the central directory
    append_uint( archive, nb_entries, 2 ); // entries on this disk
    append_uint( archive, nb_entries, 2 ); // entries
    append_uint(
        archive, static_cast< uint32_t >( central_directory.size() ), 4 );
    append_uint( archive, central_directory_offset, 4 );
    append_uint( archive, 0, 2 ); // comment length
    write_file( zip, archive );
}

/*!
 * Archive written by Python zipfile with ZIP_DEFLATED, containing the
 * test_entries() in the same order.
 */
std::string deflate_zip()
{
    return absl::StrCat( geode::DATA_PATH, "input_files_deflate.zip" );
}

Entries test_entries()
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
    return { { "text.txt", text }, { "folder/binary.bin", binary },
        { "empty", "" }, { "small", "abc" } };
}

template < typename Function >
void check_data_error( Function&& function,
    std::string_view expected_message,
    std::string_view test_name )
{
    try
    {
        function();
    }
    catch( const geode::OpenGeodeException& exception )
    {
        geode::OpenGeodeBasicException::test(
            exception.type() == geode::OpenGeodeException::TYPE::data,
            test_name, ": expected a data error, got ", exception.what() );
        geode::OpenGeodeBasicException::test(
            absl::StrContains( exception.what(), expected_message ), test_name,
            ": message \"", exception.what(), "\" does not contain ",
            expected_message );
        return;
    }
    throw geode::OpenGeodeBasicException{ nullptr,
        geode::OpenGeodeException::TYPE::internal, test_name,
        ": no exception thrown" };
}

void check_files_content( const geode::detail::InputFiles& files,
    const Entries& entries,
    std::string_view test_name )
{
    for( const auto& [name, content] : entries )
    {
        geode::OpenGeodeBasicException::test(
            files.has( name ), test_name, ": missing entry ", name );
        const auto file = files.content( name );
        geode::OpenGeodeBasicException::test(
            as_string( file.span() ) == content, test_name,
            ": wrong content of ", name );
    }
    geode::OpenGeodeBasicException::test(
        !files.has( "missing" ), test_name, ": unexpected entry" );
    check_data_error(
        [&files] {
            geode_unused( files.content( "missing" ) );
        },
        "missing", test_name );
}

void test_mapped_file()
{
    const std::string filename{ "mapped_file.bin" };
    const auto entries = test_entries();
    const auto& content = entries[1].second;
    write_file( filename, content );
    {
        const auto mapping = geode::detail::MappedFile::map( filename );
        geode::OpenGeodeBasicException::test(
            mapping != nullptr, "[MappedFile] Cannot map a regular file" );
        geode::OpenGeodeBasicException::test(
            as_string( mapping->span() ) == content,
            "[MappedFile] Wrong mapped content" );
        // Released pages are mapped again when read
        mapping->release( mapping->span().subspan( 10, 50000 ) );
        geode::OpenGeodeBasicException::test(
            as_string( mapping->span() ) == content,
            "[MappedFile] Wrong content after release" );
        geode::detail::FileContent slice{ mapping,
            mapping->span().subspan( 100, 30000 ) };
        geode::OpenGeodeBasicException::test(
            as_string( slice.span() ) == content.substr( 100, 30000 ),
            "[MappedFile] Wrong slice" );
        const auto moved = std::move( slice );
        geode::OpenGeodeBasicException::test(
            slice.span().empty() && moved.span().size() == 30000,
            "[MappedFile] Wrong moved slice" );
    }
    const auto mapped = geode::detail::map_file_content( filename );
    geode::OpenGeodeBasicException::test(
        mapped.has_value() && as_string( mapped->span() ) == content,
        "[map_file_content] Wrong content" );
    geode::OpenGeodeBasicException::test(
        geode::detail::MappedFile::map( "missing_file.bin" ) == nullptr
            && !geode::detail::map_file_content( "missing_file.bin" ),
        "[MappedFile] Missing file should not be mapped" );
    const std::string empty{ "empty_file.bin" };
    write_file( empty, "" );
    const auto empty_content = geode::detail::map_file_content( empty );
    geode::OpenGeodeBasicException::test(
        empty_content.has_value() && empty_content->span().empty(),
        "[map_file_content] Wrong empty file content" );
    std::filesystem::remove( filename );
    std::filesystem::remove( empty );
}

void test_directory_input_files()
{
    const std::filesystem::path directory{ "input_files_directory" };
    std::filesystem::remove_all( directory );
    std::filesystem::create_directories( directory / "folder" );
    const auto entries = test_entries();
    for( const auto& [name, content] : entries )
    {
        write_file( directory / name, content );
    }
    const geode::detail::DirectoryInputFiles files{ directory.u8string() };
    check_files_content( files, entries, "[DirectoryInputFiles]" );
    const std::vector< std::string > expected_names{ "empty", "small",
        "text.txt" };
    geode::OpenGeodeBasicException::test( files.names() == expected_names,
        "[DirectoryInputFiles] Wrong names (not recursive, sorted)" );
    geode::OpenGeodeBasicException::test(
        files.path( "small" ) == ( directory.u8string() + "/small" )
            && files.location( "small" ) == files.path( "small" ),
        "[DirectoryInputFiles] Wrong path" );
    geode::OpenGeodeBasicException::test( !files.has( "folder" ),
        "[DirectoryInputFiles] A directory is not a file" );
    geode::OpenGeodeBasicException::test(
        !geode::detail::map_file_content( ( directory / "folder" ).u8string() ),
        "[map_file_content] A directory has no content" );
    check_data_error(
        [&files] {
            geode_unused( files.content( "folder" ) );
        },
        "folder", "[DirectoryInputFiles] directory content" );
    std::filesystem::remove_all( directory );
}

void test_archive_input_files(
    const std::string& zip, std::string_view test_name )
{
    const auto entries = test_entries();
    const geode::detail::ArchiveInputFiles files{ zip };
    check_files_content( files, entries, test_name );
    std::vector< std::string > expected_names;
    for( const auto& entry : entries )
    {
        expected_names.push_back( entry.first );
    }
    geode::OpenGeodeBasicException::test(
        files.names() == expected_names, test_name, ": wrong names" );
    geode::OpenGeodeBasicException::test(
        !files.path( "small" ).has_value()
            && files.location( "small" ) == zip + "/small",
        test_name, ": wrong path" );
    // Several threads can read the entries
    std::vector< async::task< bool > > tasks;
    for( const auto& entry : entries )
    {
        tasks.push_back( async::spawn( [&files, &entry] {
            const auto& [name, content] = entry;
            return as_string( files.content( name ).span() ) == content;
        } ) );
    }
    for( auto& task : tasks )
    {
        geode::OpenGeodeBasicException::test(
            task.get(), test_name, ": wrong concurrent content" );
    }
}

void test_stored_archive_input_files()
{
    const std::string zip{ "input_files_stored.zip" };
    write_stored_zip( zip, test_entries() );
    test_archive_input_files( zip, "[ArchiveInputFiles] stored" );
    std::filesystem::remove( zip );
}

std::string read_file( const std::string& file )
{
    const auto content = geode::detail::map_file_content( file );
    geode::OpenGeodeBasicException::test(
        content.has_value(), "Cannot read ", file );
    return as_string( content->span() );
}

void test_corrupted_entry()
{
    const std::string zip{ "corrupted_entry.zip" };
    const auto entries = test_entries();
    write_stored_zip( zip, entries );
    auto archive = read_file( zip );
    // Flip one byte in the middle of the stored text entry
    const auto position = archive.find( "line 5000 " );
    geode::OpenGeodeBasicException::test(
        position != std::string::npos, "Entry not found in the archive" );
    archive[position] = 'X';
    write_file( zip, archive );
    const geode::detail::ArchiveInputFiles files{ zip };
    check_data_error(
        [&files] {
            geode_unused( files.content( "text.txt" ) );
        },
        "text.txt", "[ArchiveInputFiles] corrupted entry" );
    // The other entries are still readable
    geode::OpenGeodeBasicException::test(
        as_string( files.content( "small" ).span() ) == "abc",
        "[ArchiveInputFiles] Wrong entry next to a corrupted one" );
    std::filesystem::remove( zip );
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

void write_uint( std::string& data, size_t offset, size_t size, uint32_t value )
{
    for( const auto i : geode::Range{ size } )
    {
        data[offset + i] = static_cast< char >( ( value >> ( 8 * i ) ) & 0xFF );
    }
}

/*!
 * Return the offset of the central directory header of the given entry.
 */
size_t central_header( const std::string& archive, std::string_view name )
{
    const std::string signature{ "PK\x01\x02" };
    for( auto position = archive.find( signature );
        position != std::string::npos;
        position = archive.find( signature, position + 1 ) )
    {
        const auto name_length = read_uint( archive, position + 28, 2 );
        if( archive.compare( position + 46, name_length, name ) == 0
            && name_length == name.size() )
        {
            return position;
        }
    }
    throw geode::OpenGeodeBasicException{ nullptr,
        geode::OpenGeodeException::TYPE::internal, "Entry ", name,
        " not found in the central directory" };
}

void test_corrupted_deflate_stream()
{
    const std::string zip{ "corrupted_deflate.zip" };
    auto archive = read_file( deflate_zip() );
    // First entry: its local header is at the start of the archive
    geode::OpenGeodeBasicException::test(
        archive.compare( 30, 8, "text.txt" ) == 0,
        "[ArchiveInputFiles] text.txt should be the first entry" );
    const auto data =
        30 + read_uint( archive, 26, 2 ) + read_uint( archive, 28, 2 );
    // Final block of the reserved (invalid) block type 3
    archive[data] = static_cast< char >( 0x07 );
    write_file( zip, archive );
    const geode::detail::ArchiveInputFiles files{ zip };
    check_data_error(
        [&files] {
            geode_unused( files.content( "text.txt" ) );
        },
        "Cannot decompress zip entry corrupted_deflate.zip/text.txt",
        "[ArchiveInputFiles] corrupted deflate stream" );
    geode::OpenGeodeBasicException::test(
        as_string( files.content( "small" ).span() ) == "abc",
        "[ArchiveInputFiles] Wrong entry next to a corrupted deflate stream" );
    std::filesystem::remove( zip );
}

void test_truncated_entry()
{
    const std::string zip{ "truncated_entry.zip" };
    auto archive = read_file( deflate_zip() );
    const auto archive_size = static_cast< uint32_t >( archive.size() );
    // Local header of "small" beyond the end of the archive
    write_uint( archive, central_header( archive, "small" ) + 42, 4,
        archive_size + 1000 );
    // Local header of "empty" not on a local header signature
    write_uint( archive, central_header( archive, "empty" ) + 42, 4, 1 );
    // Data of "text.txt" larger than the archive
    write_uint(
        archive, central_header( archive, "text.txt" ) + 20, 4, archive_size );
    write_file( zip, archive );
    const geode::detail::ArchiveInputFiles files{ zip };
    check_data_error(
        [&files] {
            geode_unused( files.content( "small" ) );
        },
        "Truncated zip entry truncated_entry.zip/small",
        "[ArchiveInputFiles] local header out of range" );
    check_data_error(
        [&files] {
            geode_unused( files.content( "empty" ) );
        },
        "Invalid local header of zip entry truncated_entry.zip/empty",
        "[ArchiveInputFiles] invalid local header" );
    check_data_error(
        [&files] {
            geode_unused( files.content( "text.txt" ) );
        },
        "Truncated zip entry truncated_entry.zip/text.txt",
        "[ArchiveInputFiles] data out of range" );
    geode::OpenGeodeBasicException::test(
        files.content( "folder/binary.bin" ).span().size() == 100000,
        "[ArchiveInputFiles] Wrong entry next to truncated ones" );
    std::filesystem::remove( zip );
}

void test_unsafe_entry()
{
    const std::string zip{ "unsafe_entry.zip" };
    write_stored_zip( zip, { { "../outside", "abc" } } );
    check_data_error(
        [&zip] {
            const geode::detail::ArchiveInputFiles files{ zip };
        },
        "../outside", "[ArchiveInputFiles] unsafe entry" );
    std::filesystem::remove( zip );
}

void test_not_an_archive()
{
    check_data_error(
        [] {
            const geode::detail::ArchiveInputFiles files{ absl::StrCat(
                geode::DATA_PATH, "triangle.og_tsf3d" ) };
        },
        "triangle.og_tsf3d", "[ArchiveInputFiles] not an archive" );
    check_data_error(
        [] {
            const geode::detail::ArchiveInputFiles files{ "missing.zip" };
        },
        "missing.zip", "[ArchiveInputFiles] missing archive" );
}

void test_temporary_file()
{
    std::string directory;
    {
        const geode::detail::TemporaryFile file{ "entry.txt", { "abc", 3 } };
        const std::filesystem::path path{ file.path() };
        geode::OpenGeodeBasicException::test(
            path.filename() == "entry.txt" && read_file( file.path() ) == "abc",
            "[TemporaryFile] Wrong file" );
        directory = path.parent_path().u8string();
    }
    geode::OpenGeodeBasicException::test( !std::filesystem::exists( directory ),
        "[TemporaryFile] Directory not removed" );
}

void set_environment(
    const char* name, const std::optional< std::string >& value )
{
#ifdef _WIN32
    _putenv_s( name, value ? value->c_str() : "" );
#else
    if( value )
    {
        setenv( name, value->c_str(), 1 );
    }
    else
    {
        unsetenv( name );
    }
#endif
}

std::optional< std::string > get_environment( const char* name )
{
    const auto* value = std::getenv( name );
    if( value == nullptr )
    {
        return std::nullopt;
    }
    return std::string{ value };
}

void test_temporary_file_without_temporary_directory()
{
    // Not a directory: the temporary directory cannot be used
    const std::string not_a_directory{ "temporary_file_not_a_directory" };
    write_file( not_a_directory, "abc" );
    const auto absolute =
        std::filesystem::absolute( not_a_directory ).u8string();
    std::vector< std::pair< const char*, std::optional< std::string > > > saved;
    for( const auto* name : { "TMPDIR", "TMP", "TEMP", "TEMPDIR" } )
    {
        saved.emplace_back( name, get_environment( name ) );
        set_environment( name, absolute );
    }
    bool internal_error{ false };
    try
    {
        const geode::detail::TemporaryFile file{ "entry.txt", { "abc", 3 } };
    }
    catch( const geode::OpenGeodeException& exception )
    {
        internal_error =
            exception.type() == geode::OpenGeodeException::TYPE::internal;
    }
    catch( ... )
    {
    }
    for( const auto& [name, value] : saved )
    {
        set_environment( name, value );
    }
    std::filesystem::remove( not_a_directory );
    geode::OpenGeodeBasicException::test( internal_error,
        "[TemporaryFile] Unusable temporary directory should raise an "
        "internal OpenGeodeException" );
}

void test()
{
    test_mapped_file();
    test_directory_input_files();
    test_stored_archive_input_files();
    test_archive_input_files( deflate_zip(), "[ArchiveInputFiles] deflated" );
    test_corrupted_entry();
    test_corrupted_deflate_stream();
    test_truncated_entry();
    test_unsafe_entry();
    test_not_an_archive();
    test_temporary_file();
    test_temporary_file_without_temporary_directory();
}

OPENGEODE_TEST( "input-files" )
