/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <sstream>
#include <string>

#include <geode/basic/common.h>
#include <geode/basic/pimpl.h>

namespace geode
{
    /*!
     * Custom OpenGeode logger. Can be used with several levels:
     *    Logger::info( "My information is ", 42 );
     *    Logger::warn( "My warning is ", 42, " or more" );
     */
    class opengeode_basic_api Logger
    {
    public:
        enum struct Level
        {
            trace,
            debug,
            info,
            warn,
            err,
            critical,
            off
        };

        static Level level();

        static void set_level( Level level );

        template < typename... Args >
        static void trace( const Args &... args )
        {
            log_trace( absl::StrCat( args... ) );
        }

        template < typename... Args >
        static void debug( const Args &... args )
        {
            log_debug( absl::StrCat( args... ) );
        }

        template < typename... Args >
        static void info( const Args &... args )
        {
            log_info( absl::StrCat( args... ) );
        }

        template < typename... Args >
        static void warn( const Args &... args )
        {
            log_warn( absl::StrCat( args... ) );
        }

        template < typename... Args >
        static void error( const Args &... args )
        {
            log_error( absl::StrCat( args... ) );
        }

        template < typename... Args >
        static void critical( const Args &... args )
        {
            log_critical( absl::StrCat( args... ) );
        }

    private:
        Logger();
        ~Logger();

        static Logger &instance();

        static void log_trace( const std::string &message );
        static void log_debug( const std::string &message );
        static void log_info( const std::string &message );
        static void log_warn( const std::string &message );
        static void log_error( const std::string &message );
        static void log_critical( const std::string &message );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode

#define DEBUG( a ) geode::Logger::debug( #a, " = ", a )
#define SDEBUG( a ) geode::Logger::debug( #a, " = ", a.string() )
