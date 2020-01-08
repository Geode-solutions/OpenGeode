/*
 * Copyright (c) 2019 Geode-solutions
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

        static void set_level( Level level );

        template < typename... Args >
        static void trace( const Args &... args )
        {
            log_trace( string_concatener( args... ) );
        }

        template < typename... Args >
        static void debug( const Args &... args )
        {
            log_debug( string_concatener( args... ) );
        }

        template < typename... Args >
        static void info( const Args &... args )
        {
            log_info( string_concatener( args... ) );
        }

        template < typename... Args >
        static void warn( const Args &... args )
        {
            log_warn( string_concatener( args... ) );
        }

        template < typename... Args >
        static void error( const Args &... args )
        {
            log_error( string_concatener( args... ) );
        }

        template < typename... Args >
        static void critical( const Args &... args )
        {
            log_critical( string_concatener( args... ) );
        }

    private:
        template < typename A0 >
        static std::string string_concatener( const A0 &a0 )
        {
            std::ostringstream out;
            out << a0;
            return out.str();
        }

        template < typename A0, typename... Args >
        static std::string string_concatener(
            const A0 &a0, const Args &... args )
        {
            return string_concatener( a0 ) + string_concatener( args... );
        }

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
