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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/model/helpers/model_component_filter.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>

#include <geode/tests/common.h>

void check_brep_prism( const geode::BRep& brep )
{
    OPENGEODE_EXCEPTION( brep.nb_corners() == 7,
        "[Test] Filtered prism model has ", brep.nb_corners(),
        " Corners, should have 7 Corners" );
    OPENGEODE_EXCEPTION( brep.nb_lines() == 10,
        "[Test] Filtered prism model has ", brep.nb_lines(),
        " Lines, should have 10 Lines" );
    OPENGEODE_EXCEPTION( brep.nb_surfaces() == 5,
        "[Test] Filtered prism model has ", brep.nb_surfaces(),
        " Surfaces, should have 5 Surfaces" );
    OPENGEODE_EXCEPTION( brep.nb_blocks() == 1,
        "[Test] Filtered prism model has ", brep.nb_blocks(),
        " Blocks, should have 1 Block" );
}

void check_brep_dangling( const geode::BRep& brep )
{
    OPENGEODE_EXCEPTION( brep.nb_corners() == 8,
        "[Test] Filtered dangling model has ", brep.nb_corners(),
        " Corners, should have 8 Corners" );
    OPENGEODE_EXCEPTION( brep.nb_lines() == 12,
        "[Test] Filtered dangling model has ", brep.nb_lines(),
        " Lines, should have 12 Lines" );
    OPENGEODE_EXCEPTION( brep.nb_surfaces() == 6,
        "[Test] Filtered dangling model has ", brep.nb_surfaces(),
        " Surfaces, should have 6 Surfaces" );
    OPENGEODE_EXCEPTION( brep.nb_blocks() == 1,
        "[Test] Filtered dangling model has ", brep.nb_blocks(),
        " Blocks, should have 1 Block" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    auto brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "prism_curve.og_brep" ) );
    geode::filter_brep_components_with_regards_to_blocks( brep );
    check_brep_prism( brep );

    auto brep2 = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "dangling.og_brep" ) );
    geode::filter_brep_components_with_regards_to_blocks( brep2 );
    check_brep_dangling( brep2 );
}

OPENGEODE_TEST( "model-filter" )