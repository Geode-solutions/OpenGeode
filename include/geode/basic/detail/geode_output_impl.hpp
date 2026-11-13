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

#pragma once

#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <absl/strings/ascii.h>
#include <absl/strings/str_join.h>

#include <geode/basic/filename.hpp>
#include <geode/basic/io.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/timer.hpp>

namespace geode::detail
{
    template < typename Factory >
    [[nodiscard]] std::unique_ptr< typename Factory::BaseClass >
        geode_object_output_writer( std::string_view& filename )
    {
        filename = absl::StripAsciiWhitespace( filename );
        const auto extension = lowercase_extension( filename );
        OpenGeodeBasicException::check_exception(
            Factory::has_creator( extension ), nullptr,
            OpenGeodeException::TYPE::data, "Unknown extension \"", extension,
            "\" for file \"", filename, "\". Available extensions: ",
            absl::StrJoin( Factory::list_creators(), ", " ),
            ". Other formats may be provided by IO libraries, which must be "
            "initialized (or imported in Python) first" );
        return Factory::create(
            extension, expand_predefined_folders( filename ) );
    }

    template < typename Factory, typename Object >
    std::vector< std::string > geode_object_output_impl(
        std::string_view type, const Object& object, std::string_view filename )
    {
        const Timer timer;
        auto output = geode_object_output_writer< Factory >( filename );
        const auto directories = filepath_without_filename(
            std::filesystem::u8path( expand_predefined_folders( filename ) ) );
        if( !directories.empty() )
        {
            std::filesystem::create_directories( directories );
        }
        auto output_filenames = output->write( object );
        Logger::info( type, " saved in ",
            absl::StrJoin( output_filenames, ", " ), " in ", timer.duration() );
        return output_filenames;
    }

    template < typename Exception,
        typename Factory,
        typename... ParentFactories,
        typename... ParentTypes >
    [[noreturn]] void throw_geode_object_output_exception(
        std::string_view type,
        std::string_view filename,
        const std::exception& exception,
        const ParentTypes&... parent_types )
    {
        Logger::error( exception.what() );
        print_available_extensions_if_unknown< Factory, ParentFactories... >(
            type, filename, parent_types... );
        // An unknown extension is an input error, like when loading
        const auto type_of_error =
            Factory::has_creator(
                lowercase_extension( absl::StripAsciiWhitespace( filename ) ) )
                ? OpenGeodeException::TYPE::internal
                : OpenGeodeException::TYPE::data;
        throw Exception{ nullptr, type_of_error, "Cannot save ", type,
            " in file: ", filename, ": ", exception.what() };
    }
} // namespace geode::detail
