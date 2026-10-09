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

#include <geode/basic/file.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>

#include <absl/strings/match.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/string.hpp>

namespace
{
    bool read_line( std::ifstream& file, std::string& line )
    {
        if( !std::getline( file, line ) )
        {
            return false;
        }
        if( !line.empty() && line.back() == '\r' )
        {
            line.pop_back();
        }
        return true;
    }
} // namespace

namespace geode
{
    bool file_exists( std::string_view file_path )
    {
        return std::filesystem::exists( to_string( file_path ) );
    }

    bool line_starts_with( std::ifstream& file, std::string_view check )
    {
        std::string line;
        if( !read_line( file, line ) )
        {
            return false;
        }
        return string_starts_with( line, check );
    }

    void check_keyword( std::ifstream& file, std::string_view keyword )
    {
        std::string line;
        const auto has_line = read_line( file, line );
        OpenGeodeBasicException::check_exception(
            has_line && string_starts_with( line, keyword ), nullptr,
            OpenGeodeException::TYPE::data, "Line should starts with \"",
            keyword, "\", got \"", has_line ? line : "end of file", "\"" );
    }

    std::string goto_keyword( std::ifstream& file, std::string_view word )
    {
        std::string line;
        while( read_line( file, line ) )
        {
            if( string_starts_with( line, word ) )
            {
                return line;
            }
        }
        throw geode::OpenGeodeBasicException{ nullptr,
            OpenGeodeException::TYPE::data,
            "[goto_keyword] Cannot find the requested keyword: ", word };
    }

    std::string goto_keywords(
        std::ifstream& file, absl::Span< const std::string_view > words )
    {
        std::string line;
        while( read_line( file, line ) )
        {
            for( const auto word : words )
            {
                if( string_starts_with( line, word ) )
                {
                    return line;
                }
            }
        }
        throw geode::OpenGeodeBasicException{ nullptr,
            OpenGeodeException::TYPE::data,
            "[goto_keywords] Cannot find one of the requested keywords" };
    }

    std::optional< std::string > goto_keyword_if_it_exists(
        std::ifstream& file, std::string_view word )
    {
        std::optional< std::string > line{ std::in_place };
        while( read_line( file, line.value() ) )
        {
            if( string_starts_with( line.value(), word ) )
            {
                return line;
            }
        }
        geode::Logger::debug( "[goto_keyword_if_it_exists] Couldn't find word ",
            word, " in the file, returning to file begin." );
        file.clear();
        file.seekg( 0, std::ios::beg );
        return std::nullopt;
    }

    std::optional< std::string > next_keyword_if_it_exists(
        std::ifstream& file, std::string_view word )
    {
        std::optional< std::string > line{ std::in_place };
        const auto previous_position = file.tellg();
        if( read_line( file, line.value() )
            && string_starts_with( line.value(), word ) )
        {
            return line;
        }
        file.clear();
        file.seekg( previous_position );
        return std::nullopt;
    }
} // namespace geode
