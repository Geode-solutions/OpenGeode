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

#include <string>

#include <geode/basic/common.h>

namespace geode
{
    /* Absolute path to test source directory */
    static constexpr auto data_path = "@DATA_DIRECTORY@/";
} // namespace geode

/*!
 * The OPENGEODE_TEST macro takes a name as input and also use a function
 * named "test" that takes no argument. This function should be accessible
 * from the context in which OPENGEODE_TEST is called.
 */

#ifdef OPENGEODE_BENCHMARK
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_PREFIX_ALL
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <geode/tests/catch2.h>
#define OPENGEODE_TEST( name )                                                 \
    CATCH_TEST_CASE( name )                                                    \
    {                                                                          \
        CATCH_BENCHMARK( name )                                                \
        {                                                                      \
            return test();                                                     \
        };                                                                     \
    }
#else
#define OPENGEODE_TEST( name )                                                 \
    int main()                                                                 \
    {                                                                          \
        using namespace geode;                                                 \
                                                                               \
        try                                                                    \
        {                                                                      \
            test();                                                            \
                                                                               \
            Logger::info( "TEST SUCCESS" );                                    \
            return 0;                                                          \
        }                                                                      \
        catch( ... )                                                           \
        {                                                                      \
            return geode_lippincott();                                         \
        }                                                                      \
    }
#endif