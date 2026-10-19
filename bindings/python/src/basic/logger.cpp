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

#include "../common.hpp"

#include <geode/basic/logger.hpp>

namespace geode
{
    void define_logger( pybind11::module& module )
    {
        pybind11::class_< Logger,
            std::unique_ptr< Logger, pybind11::nodelete > >
            logger( module, "Logger" );
        pybind11::enum_< Logger::LEVEL >( logger, "LEVEL" )
            .value( "trace", Logger::LEVEL::trace )
            .value( "debug", Logger::LEVEL::debug )
            .value( "info", Logger::LEVEL::info )
            .value( "warning", Logger::LEVEL::warning )
            .value( "error", Logger::LEVEL::error )
            .value( "critical", Logger::LEVEL::critical )
            .value( "off", Logger::LEVEL::off );
        logger.def_static( "level", &Logger::level, "Current logging level" )
            .def_static( "set_level", &Logger::set_level,
                pybind11::arg( "level" ),
                "Set the minimum level of the messages to display, e.g. "
                "Logger.set_level(Logger.LEVEL.warning) to hide info "
                "messages" )
            .def_static(
                "is_enabled", &Logger::is_enabled, pybind11::arg( "level" ) )
            .def_static(
                "trace",
                []( const std::string& message ) {
                    Logger::trace( message );
                },
                pybind11::arg( "message" ) )
            .def_static(
                "debug",
                []( const std::string& message ) {
                    Logger::debug( message );
                },
                pybind11::arg( "message" ) )
            .def_static(
                "info",
                []( const std::string& message ) {
                    Logger::info( message );
                },
                pybind11::arg( "message" ) )
            .def_static(
                "warning",
                []( const std::string& message ) {
                    Logger::warning( message );
                },
                pybind11::arg( "message" ) )
            .def_static(
                "error",
                []( const std::string& message ) {
                    Logger::error( message );
                },
                pybind11::arg( "message" ) )
            .def_static(
                "critical",
                []( const std::string& message ) {
                    Logger::critical( message );
                },
                pybind11::arg( "message" ) );
    }
} // namespace geode
