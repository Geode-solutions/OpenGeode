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

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <map>
#include <random>
#include <string>
#include <string_view>

#include <absl/strings/str_cat.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/point.hpp>

/*
 * Shared tools of the brute force tests: portable random numbers and report
 * of the failures with their exact (hexadecimal) inputs.
 */
namespace
{
    /*
     * The generator output is portable, unlike the standard distributions
     * (implementation-defined): the same inputs are tested on all platforms.
     */
    std::mt19937_64 generator{ 42 };

    /*
     * Uniform double in [min, max) from the 53 high bits of the generator
     */
    inline double uniform( double min, double max )
    {
        const auto unit =
            std::ldexp( static_cast< double >( generator() >> 11 ), -53 );
        return min + ( max - min ) * unit;
    }

    /*
     * Uniform integer in [min, max] (the modulo bias is negligible for the
     * tested ranges)
     */
    inline std::int64_t random_int( std::int64_t min, std::int64_t max )
    {
        const auto range = static_cast< std::uint64_t >( max - min ) + 1;
        return min + static_cast< std::int64_t >( generator() % range );
    }

    std::map< std::string, geode::index_t > issues;
    std::string current_mode;

    inline std::string hex( double value )
    {
        char buffer[32];
        std::snprintf( buffer, sizeof( buffer ), "%a", value );
        return buffer;
    }

    template < geode::index_t dimension >
    std::string hex( const geode::Point< dimension >& point )
    {
        std::string result;
        for( const auto d : geode::LRange{ dimension } )
        {
            absl::StrAppend( &result, hex( point.value( d ) ), " " );
        }
        return result;
    }

    /*
     * Counts the failure in its category and current mode, logs the first
     * two of each
     */
    inline void record(
        const std::string& category, const std::string& details )
    {
        if( issues[category + current_mode]++ < 2 )
        {
            geode::Logger::warning( category, current_mode, " | ", details );
        }
    }

    template < typename... Args >
    void check(
        bool condition, const std::string& category, const Args&... details )
    {
        if( !condition )
        {
            record( category, absl::StrCat( details... ) );
        }
    }

    /*
     * Logs the failure counts, then fails if any
     */
    inline void report_issues( std::string_view name )
    {
        for( const auto& [category, count] : issues )
        {
            geode::Logger::info( category, ": ", count );
        }
        geode::OpenGeodeGeometryException::test(
            issues.empty(), name, " issues found" );
    }
} // namespace
