/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/string.h>

#include <absl/strings/str_split.h>

namespace geode
{
    std::vector< absl::string_view > string_split( absl::string_view string )
    {
        return absl::StrSplit( absl::StripAsciiWhitespace( string ),
            absl::ByAnyChar( " 	" ) /* space + tab */, absl::SkipWhitespace() );
    }

    bool string_starts_with( absl::string_view string, absl::string_view check )
    {
        return absl::StartsWith( string, check );
    }

    index_t string_to_index( absl::string_view string )
    {
        index_t value{ 0 };
        const auto status = absl::SimpleAtoi( string, &value );
        OPENGEODE_EXCEPTION( status,
            "[string_to_index] Cannot convert string \"", string,
            "\" to index" );
        return value;
    }

    int string_to_int( absl::string_view string )
    {
        int value{ 0 };
        const auto status = absl::SimpleAtoi( string, &value );
        OPENGEODE_EXCEPTION( status, "[string_to_int] Cannot convert string \"",
            string, "\" to int" );
        return value;
    }

    float string_to_float( absl::string_view string )
    {
        float value{ 0 };
        const auto status = absl::SimpleAtof( string, &value );
        OPENGEODE_EXCEPTION( status,
            "[string_to_float] Cannot convert string \"", string,
            "\" to float" );
        return value;
    }

    double string_to_double( absl::string_view string )
    {
        double value{ 0 };
        const auto status = absl::SimpleAtod( string, &value );
        OPENGEODE_EXCEPTION( status,
            "[string_to_double] Cannot convert string \"", string,
            "\" to double" );
        return value;
    }
} // namespace geode