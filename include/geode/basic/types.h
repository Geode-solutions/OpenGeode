/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <absl/strings/string_view.h>

namespace geode
{
    static constexpr double global_epsilon{ 1E-8 };
    static constexpr double global_epsilon2{ global_epsilon * global_epsilon };
    static constexpr double global_epsilon3{ global_epsilon2 * global_epsilon };

    using index_t = unsigned int;
    using signed_index_t = int;
    using local_index_t = unsigned char;

    /// Value used for a invalid index
    static constexpr index_t NO_ID = index_t( -1 );
    static constexpr local_index_t NO_LID = local_index_t( -1 );

    inline constexpr unsigned char operator"" _uc(
        unsigned long long arg ) noexcept
    {
        return static_cast< unsigned char >( arg );
    }

    inline std::string to_string( absl::string_view view )
    {
        return static_cast< std::string >( view );
    }
} // namespace geode
