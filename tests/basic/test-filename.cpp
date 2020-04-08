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

#include <memory>

#include <geode/basic/filename.h>
#include <geode/basic/logger.h>

#include <geode/tests/common.h>

void test()
{
    std::string path1 = "/where/is/file.ext";
    OPENGEODE_EXCEPTION( geode::filename_from_path( path1, true ) == "file.ext",
        "[Test] Wrong result for path1 with extension" );
    OPENGEODE_EXCEPTION( geode::filename_from_path( path1, false ) == "file",
        "[Test] Wrong result for path1 with extension" );

    std::string path2 = "file.ext";
    OPENGEODE_EXCEPTION( geode::filename_from_path( path2, true ) == "file.ext",
        "[Test] Wrong result for path2 with extension" );
    OPENGEODE_EXCEPTION( geode::filename_from_path( path2, false ) == "file",
        "[Test] Wrong result for path2 with extension" );

    std::string path3 = "C:/where/is/file.ext";
    OPENGEODE_EXCEPTION( geode::filename_from_path( path3, true ) == "file.ext",
        "[Test] Wrong result for path3 with extension" );
    OPENGEODE_EXCEPTION( geode::filename_from_path( path3, false ) == "file",
        "[Test] Wrong result for path3 with extension" );
}

OPENGEODE_TEST( "filename" )