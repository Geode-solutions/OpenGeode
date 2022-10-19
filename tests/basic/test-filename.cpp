/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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
    OPENGEODE_EXCEPTION( geode::filename_with_extension( path1 ) == "file.ext",
        "[Test] Wrong result for path1 with extension" );
    OPENGEODE_EXCEPTION( geode::filename_without_extension( path1 ) == "file",
        "[Test] Wrong result for path1 without extension" );

    std::string path2 = "file.ext";
    OPENGEODE_EXCEPTION( geode::filename_with_extension( path2 ) == "file.ext",
        "[Test] Wrong result for path2 with extension" );
    OPENGEODE_EXCEPTION( geode::filename_without_extension( path2 ) == "file",
        "[Test] Wrong result for path2 without extension" );
    OPENGEODE_EXCEPTION( geode::filepath_without_extension( path2 ) == "file",
        "[Test] Wrong result for filepath2 without extension" );
    OPENGEODE_EXCEPTION( geode::filepath_without_filename( path2 ) == "",
        "[Test] Wrong result for filepath2 without file name" );

    std::string path3 = "C:/where/is/file.ext";
    OPENGEODE_EXCEPTION( geode::filename_with_extension( path3 ) == "file.ext",
        "[Test] Wrong result for path3 with extension" );
    OPENGEODE_EXCEPTION( geode::filename_without_extension( path3 ) == "file",
        "[Test] Wrong result for path3 without extension" );
}

OPENGEODE_TEST( "filename" )