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

#include <geode/model/representation/io/external_time_series_helpers_output.hpp>

#include <filesystem>
#include <fstream>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/filename.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/timer.hpp>

#include <geode/model/helpers/external_time_series_helpers.hpp>

namespace
{
    void write_external_time_series_helpers(
        const geode::ExternalTimeSeriesHelpers& helpers,
        std::string_view filename )
    {
        std::ofstream file{ geode::to_string( filename ),
            std::ofstream::binary };
        geode::OpenGeodeModelException::check_exception( !file.fail(), nullptr,
            geode::OpenGeodeException::TYPE::data,
            "[save_external_time_series_helpers] Failed to open file: ",
            filename );
        geode::TContext context{};
        geode::BitseryExtensions::register_serialize_pcontext(
            std::get< 0 >( context ) );
        geode::Serializer archive{ context, file };
        archive.object( helpers );
        archive.adapter().flush();
        geode::OpenGeodeModelException::check_exception(
            std::get< 1 >( context ).isValid(), nullptr,
            geode::OpenGeodeException::TYPE::internal,
            "[save_external_time_series_helpers] Error while writing file: ",
            filename );
    }
} // namespace

namespace geode
{
    void save_external_time_series_helpers(
        const ExternalTimeSeriesHelpers& helpers, std::string_view filename )
    {
        const Timer timer;
        OpenGeodeModelException::check_exception(
            extension_from_filename( filename )
                == ExternalTimeSeriesHelpers::native_extension_static(),
            nullptr, OpenGeodeException::TYPE::data,
            "[save_external_time_series_helpers] Wrong extension for file ",
            filename, ", only ",
            ExternalTimeSeriesHelpers::native_extension_static(),
            " is supported" );
        try
        {
            write_external_time_series_helpers( helpers, filename );
        }
        catch( ... )
        {
            std::filesystem::remove( to_string( filename ) );
            throw;
        }
        Logger::info( "ExternalTimeSeriesHelpers saved in ", filename, " in ",
            timer.duration() );
    }
} // namespace geode
