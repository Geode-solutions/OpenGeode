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

#include <geode/basic/detail/input_files.hpp>

#include <filesystem>
#include <fstream>

#include <absl/algorithm/container.h>
#include <absl/strings/str_cat.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/uuid.hpp>

namespace
{
    std::filesystem::path to_path( std::string_view utf8_path )
    {
        return std::filesystem::u8path( utf8_path.begin(), utf8_path.end() );
    }
} // namespace

namespace geode
{
    namespace detail
    {
        InputFiles::~InputFiles() = default;

        DirectoryInputFiles::DirectoryInputFiles( std::string_view directory )
            : directory_{ to_string( directory ) }
        {
        }

        DirectoryInputFiles::~DirectoryInputFiles() = default;

        bool DirectoryInputFiles::has( std::string_view name ) const
        {
            std::error_code error;
            return std::filesystem::is_regular_file(
                to_path( location( name ) ), error );
        }

        std::vector< std::string > DirectoryInputFiles::names() const
        {
            std::vector< std::string > names;
            std::error_code error;
            for( const auto& file : std::filesystem::directory_iterator(
                     to_path( directory_ ), error ) )
            {
                if( file.is_regular_file( error ) )
                {
                    names.emplace_back( file.path().filename().u8string() );
                }
            }
            absl::c_sort( names );
            return names;
        }

        FileContent DirectoryInputFiles::content( std::string_view name ) const
        {
            const auto file = location( name );
            auto content = map_file_content( file );
            OpenGeodeBasicException::check_exception( content.has_value(),
                nullptr, OpenGeodeException::TYPE::data,
                "[DirectoryInputFiles] Cannot open file: ", file );
            return std::move( content.value() );
        }

        std::optional< std::string > DirectoryInputFiles::path(
            std::string_view name ) const
        {
            return location( name );
        }

        std::string DirectoryInputFiles::location( std::string_view name ) const
        {
            return absl::StrCat( directory_, "/", name );
        }

        TemporaryFile::TemporaryFile( std::string_view name, ByteSpan content )
        {
            std::error_code error;
            const auto temp_directory =
                std::filesystem::temp_directory_path( error );
            OpenGeodeBasicException::check_exception( !error, nullptr,
                OpenGeodeException::TYPE::internal,
                "[TemporaryFile] No temporary directory: ", error.message() );
            const auto directory = temp_directory / uuid{}.string();
            std::filesystem::create_directories( directory, error );
            OpenGeodeBasicException::check_exception( !error, nullptr,
                OpenGeodeException::TYPE::internal,
                "[TemporaryFile] Cannot create directory: ",
                directory.u8string(), " (", error.message(), ")" );
            directory_ = directory.u8string();
            const auto file = directory / to_path( name ).filename();
            path_ = file.u8string();
            std::ofstream stream{ file, std::ofstream::binary };
            stream.write( content.data(),
                static_cast< std::streamsize >( content.size() ) );
            stream.close();
            if( !stream )
            {
                std::filesystem::remove_all( directory, error );
                throw OpenGeodeBasicException{ nullptr,
                    OpenGeodeException::TYPE::internal,
                    "[TemporaryFile] Cannot write file: ", path_ };
            }
        }

        TemporaryFile::~TemporaryFile()
        {
            std::error_code error;
            std::filesystem::remove_all( to_path( directory_ ), error );
        }

        const std::string& TemporaryFile::path() const
        {
            return path_;
        }
    } // namespace detail
} // namespace geode
