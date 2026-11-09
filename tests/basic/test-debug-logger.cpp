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

#include <string>

#include <geode/basic/library.hpp>
#include <geode/basic/logger.hpp>

#include <geode/basic/detail/disable_debug_logger.hpp>

#include <geode/tests/common.hpp>

namespace
{
    int& value_counter()
    {
        static int counter{ 0 };
        return counter;
    }

    int& string_counter()
    {
        static int counter{ 0 };
        return counter;
    }

    int expensive_value()
    {
        return ++value_counter();
    }

    struct Stringable
    {
        std::string string() const
        {
            ++string_counter();
            return "stringable";
        }
    };

    void test_disabled_debug_does_not_evaluate()
    {
        // Variable only used in debug macros: must not trigger a warning.
        const auto only_for_debug = 3;
        const Stringable object;

        DEBUG( expensive_value() );
        DEBUG( only_for_debug )
        SDEBUG( object );
        SDEBUG( object )

        geode::OpenGeodeBasicException::test( value_counter() == 0,
            "[Test] DEBUG argument evaluated while debug logs are disabled" );
        geode::OpenGeodeBasicException::test( string_counter() == 0,
            "[Test] SDEBUG argument evaluated while debug logs are disabled" );
    }

    void test()
    {
        test_disabled_debug_does_not_evaluate();
    }
} // namespace

OPENGEODE_TEST( "debug-logger" )
