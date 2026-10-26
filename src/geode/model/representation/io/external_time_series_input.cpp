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

#include <geode/model/representation/io/external_time_series_input.hpp>

#include <fstream>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/filename.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/timer.hpp>

#include <geode/model/representation/core/external_time_series.hpp>

namespace geode
{
    ExternalTimeSeries load_external_time_series( std::string_view filename )
    {
        const Timer timer;
        OpenGeodeModelException::check_exception(
            extension_from_filename( filename )
                == ExternalTimeSeries::native_extension_static(),
            nullptr, OpenGeodeException::TYPE::data,
            "[load_external_time_series] Wrong extension for file ", filename,
            ", only ", ExternalTimeSeries::native_extension_static(),
            " is supported" );
        std::ifstream file{ to_string( filename ), std::ifstream::binary };
        OpenGeodeModelException::check_exception( !file.fail(), nullptr,
            OpenGeodeException::TYPE::data,
            "[load_external_time_series] Failed to open file: ", filename );
        TContext context{};
        BitseryExtensions::register_deserialize_pcontext(
            std::get< 0 >( context ) );
        Deserializer archive{ context, file };
        ExternalTimeSeries time_series;
        archive.object( time_series );
        const auto& adapter = archive.adapter();
        OpenGeodeModelException::check_exception(
            adapter.error() == bitsery::ReaderError::NoError
                && adapter.isCompletedSuccessfully()
                && std::get< 1 >( context ).isValid(),
            nullptr, OpenGeodeException::TYPE::internal,
            "[load_external_time_series] Error while reading file: ",
            filename );
        Logger::info( "ExternalTimeSeries loaded from ", filename, " in ",
            timer.duration() );
        return time_series;
    }
} // namespace geode
