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

#include <sstream>
#include <stdexcept>
#include <string>

#include <absl/base/optimization.h>
#include <absl/debugging/stacktrace.h>
#include <absl/debugging/symbolize.h>
#include <absl/strings/str_cat.h>

#include <geode/basic/opengeode_basic_export.hpp>

namespace geode
{
    /*!
     * OpenGeode exception class.
     * Example:
     *       throw OpenGeodeException{ "Error while loading the BRep" };
     *
     *       try {
     *          ...
     *       }
     *       catch( ... )
     *       {
     *           return geode_lippincott();
     *       }
     */
    class OpenGeodeException : public std::runtime_error
    {
        static constexpr int MAX_STACK_DEPTH = 10;
        static constexpr int NB_SKIPPED_STACKS = 1;

    public:
        template < typename... Args >
        explicit OpenGeodeException( const Args&... message )
            : std::runtime_error{ absl::StrCat( message... ) },
              stack_size_{ absl::GetStackTrace(
                  stack_, MAX_STACK_DEPTH, NB_SKIPPED_STACKS ) }
        {
        }

        virtual ~OpenGeodeException() noexcept = default;

        std::string stack_trace() const
        {
            std::string stack_string;
            for( auto frame = 0; frame < stack_size_; ++frame )
            {
                absl::StrAppend( &stack_string, "  ", frame, ": " );
                if( char symbol[1024];
                    absl::Symbolize( stack_[frame], symbol, sizeof( symbol ) ) )
                {
                    absl::StrAppend( &stack_string, symbol, "\n" );
                }
                else
                {
                    absl::StrAppend( &stack_string, "Unknown \n" );
                }
            }
            return stack_string;
        }

    private:
        void* stack_[MAX_STACK_DEPTH];
        int stack_size_;
    };

    void opengeode_basic_api geode_assertion_failed( std::string_view condition,
        std::string_view message,
        std::string_view file,
        int line );

    /*!
     * Try to catch several exception types.
     * Always return 1.
     */
    int opengeode_basic_api geode_lippincott();
} // namespace geode

#ifdef OPENGEODE_DEBUG
#    define OPENGEODE_ASSERT( condition, ... )                                 \
        if( ABSL_PREDICT_FALSE( !( condition ) ) )                             \
        geode::geode_assertion_failed(                                         \
            #condition, absl::StrCat( __VA_ARGS__ ), __FILE__, __LINE__ )
#    define OPENGEODE_ASSERT_NOT_REACHED( ... )                                \
        geode::geode_assertion_failed( "should not have reached",              \
            absl::StrCat( __VA_ARGS__ ), __FILE__, __LINE__ )
#    define DEBUG_CONST const
#else
#    define OPENGEODE_ASSERT( x, ... )
#    define OPENGEODE_ASSERT_NOT_REACHED( ... )
#    define DEBUG_CONST
#endif

#define OPENGEODE_EXCEPTION( condition, ... )                                  \
    if( ABSL_PREDICT_FALSE( !( condition ) ) )                                 \
        throw geode::OpenGeodeException                                        \
        {                                                                      \
            __VA_ARGS__                                                        \
        }

#define OPENGEODE_RESEARCH( condition, ... )                                   \
    OPENGEODE_EXCEPTION( condition, __VA_ARGS__ )
