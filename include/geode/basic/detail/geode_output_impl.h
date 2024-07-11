/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/logger.h>
#include <geode/basic/timer.h>

namespace geode
{
    namespace detail
    {
        template < typename Factory >
        std::unique_ptr< typename Factory::BaseClass >
            geode_object_output_writer( std::string_view& filename )
        {
            filename = absl::StripAsciiWhitespace( filename );
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            OPENGEODE_EXCEPTION( Factory::has_creator( extension ),
                "Unknown extension: ", extension );
            return Factory::create(
                extension, expand_predefined_folders( filename ) );
        }

        template < typename Factory, typename Object >
        std::vector< std::string > geode_object_output_impl(
            std::string_view type,
            const Object& object,
            std::string_view filename )
        {
            const Timer timer;
            auto output = geode_object_output_writer< Factory >( filename );
            std::filesystem::create_directories(
                filepath_without_filename( filename ) );
            auto result = output->write( object );
            Logger::info(
                type, " saved in ", filename, " in ", timer.duration() );
            return result;
        }
    } // namespace detail
} // namespace geode
