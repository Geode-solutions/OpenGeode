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

#include <geode/tests/common.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/model/helpers/convert_brep_section.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>
#include <geode/model/representation/io/brep_input.h>

void test()
{
    geode::OpenGeodeModel::initialize();
    auto brep = geode::load_brep(
        absl::StrCat( geode::data_path, "random_dfn.og_brep" ) );
    const auto section =
        std::get< 0 >( geode::convert_brep_into_section( brep, 2 ) );

    OPENGEODE_EXCEPTION(
        section.nb_corners() == 172, "[Test] Section should have 172 corners" );
    OPENGEODE_EXCEPTION(
        section.nb_lines() == 288, "[Test] Section should have 288 lines" );
    OPENGEODE_EXCEPTION( section.nb_surfaces() == 117,
        "[Test] Section should have 117 surfaces" );

    const auto brep2 =
        std::get< 0 >( geode::convert_section_into_brep( section, 2, 10. ) );

    OPENGEODE_EXCEPTION(
        brep2.nb_corners() == 172, "[Test] BRep should have 172 corners" );
    OPENGEODE_EXCEPTION(
        brep2.nb_lines() == 288, "[Test] BRep should have 288 lines" );
    OPENGEODE_EXCEPTION(
        brep2.nb_surfaces() == 117, "[Test] BRep should have 117 surfaces" );
}

OPENGEODE_TEST( "convert-brep" )