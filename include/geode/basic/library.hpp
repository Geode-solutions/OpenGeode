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

#include <absl/base/optimization.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/opengeode_basic_export.hpp>
#include <geode/basic/pimpl.hpp>
#include <geode/basic/singleton.hpp>

namespace geode
{
    class opengeode_basic_api Library : public Singleton
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( Library );

    public:
        ~Library() override;

    protected:
        Library();

        void call_initialize( const char* library_name );

    private:
        virtual void do_initialize() = 0;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode

/*!
 * Use this macro to declare an OpenGeode library in a header file (usually
 * common.h)
 */
#define OPENGEODE_LIBRARY( export_api, project_name, library_name )            \
    class export_api project_name##library_name##Library                       \
        : public geode::Library                                                \
    {                                                                          \
    public:                                                                    \
        static void initialize()                                               \
        {                                                                      \
            project_name##library_name##Library& library =                     \
                Singleton::instance< project_name##library_name##Library >();  \
            library.call_initialize( #library_name );                          \
        }                                                                      \
                                                                               \
    private:                                                                   \
        void do_initialize() override;                                         \
    };                                                                         \
                                                                               \
    class export_api project_name##library_name##Exception                     \
        : public project_name##Exception                                       \
    {                                                                          \
    public:                                                                    \
        template < typename... Args >                                          \
        project_name##library_name##Exception(                                 \
            std::any data, TYPE type, const Args&... message )                 \
            : project_name##Exception{ #project_name, #library_name,           \
                  std::move( data ), type, message... }                        \
        {                                                                      \
        }                                                                      \
                                                                               \
        template < typename DataProvider,                                      \
            typename MessageProvider,                                          \
            typename ExceptionClass = project_name##library_name##Exception >  \
        static void exception( bool condition,                                 \
            DataProvider&& data_provider,                                      \
            TYPE type,                                                         \
            MessageProvider&& message_provider )                               \
        {                                                                      \
            if( ABSL_PREDICT_FALSE( !( condition ) ) )                         \
            {                                                                  \
                throw ExceptionClass{ data_provider(), type,                   \
                    message_provider() };                                      \
            }                                                                  \
        }                                                                      \
                                                                               \
        template < typename Condition, typename MessageProvider >              \
        static void assertion(                                                 \
            Condition&& condition, MessageProvider&& message_provider )        \
        {                                                                      \
            if constexpr( is_debug_build )                                     \
            {                                                                  \
                if( ABSL_PREDICT_FALSE( !( condition() ) ) )                   \
                {                                                              \
                    geode::Logger::critical( #project_name, " ",               \
                        #library_name, " assertion failed" );                  \
                    geode::Logger::critical(                                   \
                        "Message: ", message_provider() );                     \
                    exit( 1 );                                                 \
                }                                                              \
            }                                                                  \
        }                                                                      \
                                                                               \
        template < typename... Args >                                          \
        static void test( bool condition, const Args&... message )             \
        {                                                                      \
            if( ABSL_PREDICT_FALSE( !( condition ) ) )                         \
            {                                                                  \
                geode::Logger::critical( "Test failed: ", message... );        \
                exit( 1 );                                                     \
            }                                                                  \
        }                                                                      \
    }

#define check_assertion( condition, ... )                                      \
    assertion(                                                                 \
        [&] {                                                                  \
            return ( condition );                                              \
        },                                                                     \
        [&] {                                                                  \
            return absl::StrCat( __VA_ARGS__ );                                \
        } )

#define check_exception( condition, data, type, ... )                          \
    exception( ( condition ),                                                  \
        [&]() -> std::any {                                                    \
            return ( data );                                                   \
        },                                                                     \
        type,                                                                  \
        [&] {                                                                  \
            return absl::StrCat( __VA_ARGS__ );                                \
        } )

/*!
 * Use this macro to implement an OpenGeode library in a cpp file (usually
 * common.cpp) and call all functions that need to be initialized.
 */
#define OPENGEODE_LIBRARY_IMPLEMENTATION( project_name, library_name )         \
    void project_name##library_name##Library::do_initialize()

namespace geode
{
    OPENGEODE_LIBRARY( opengeode_basic_api, OpenGeode, Basic );
} // namespace geode