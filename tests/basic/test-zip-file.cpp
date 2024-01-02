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

#include <absl/strings/str_cat.h>

#include <geode/basic/assert.h>
#include <geode/basic/zip_file.h>

#include <geode/tests/common.h>

void test()
{
    const auto is_not_a_zip = geode::is_zip_file(
        absl::StrCat( geode::data_path, "triange.og_tsf3d" ) );
    OPENGEODE_EXCEPTION(
        !is_not_a_zip, "[Test] Not a zip file detection failed" );
    const auto is_a_zip = geode::is_zip_file(
        absl::StrCat( geode::data_path, "layers.og_brep" ) );
    OPENGEODE_EXCEPTION( is_a_zip, "[Test] zip file detection failed" );
}

OPENGEODE_TEST( "zip-file" )