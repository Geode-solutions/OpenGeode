/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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
#include <absl/strings/str_cat.h>

#include <geode/basic/opengeode_basic_export.h>

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
    public:
        template < typename... Args >
        explicit OpenGeodeException( const Args &... message )
            : std::runtime_error{ absl::StrCat( message... ) }
        {
        }
        virtual ~OpenGeodeException() noexcept {}
    };

    void opengeode_basic_api geode_assertion_failed(
        absl::string_view condition,
        absl::string_view message,
        absl::string_view file,
        int line );

    /*!
     * Try to catch several exception types.
     * Always return 1.
     */
    int opengeode_basic_api geode_lippincott();
} // namespace geode

#ifdef OPENGEODE_DEBUG
#define OPENGEODE_ASSERT( condition, message )                                 \
    if( ABSL_PREDICT_FALSE( !( condition ) ) )                                 \
    geode::geode_assertion_failed( #condition, message, __FILE__, __LINE__ )
#define OPENGEODE_ASSERT_NOT_REACHED( message )                                \
    geode::geode_assertion_failed(                                             \
        "should not reached", message, __FILE__, __LINE__ )
#else
#define OPENGEODE_ASSERT( x, message )
#define OPENGEODE_ASSERT_NOT_REACHED( message )
#endif
#define OPENGEODE_EXCEPTION( condition, ... )                                  \
    if( ABSL_PREDICT_FALSE( !( condition ) ) )                                 \
        throw geode::OpenGeodeException                                        \
        {                                                                      \
            __VA_ARGS__                                                        \
        }

