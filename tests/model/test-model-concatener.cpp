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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/model/helpers/model_concatener.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>
#include <geode/model/representation/io/brep_output.h>

#include <geode/tests/common.h>

void check_concatenation(
    const geode::BRep& brep, absl::Span< const geode::index_t > nb_components )
{
    OPENGEODE_EXCEPTION( brep.nb_corners() == nb_components[0],
        "[Test] Concatenated model has ", brep.nb_corners(),
        " Corners, should have ", nb_components[0], " Corners" );
    OPENGEODE_EXCEPTION( brep.nb_lines() == nb_components[1],
        "[Test] Concatenated model has ", brep.nb_lines(),
        " Lines, should have ", nb_components[1], " Lines" );
    OPENGEODE_EXCEPTION( brep.nb_surfaces() == nb_components[2],
        "[Test] Concatenated model has ", brep.nb_surfaces(),
        " Surfaces, should have ", nb_components[2], " Surfaces" );
    OPENGEODE_EXCEPTION( brep.nb_blocks() == nb_components[3],
        "[Test] Concatenated model has ", brep.nb_blocks(),
        " Blocks, should have ", nb_components[3], " Blocks" );
    OPENGEODE_EXCEPTION( brep.nb_model_boundaries() == nb_components[4],
        "[Test] Concatenated model has ", brep.nb_model_boundaries(),
        " ModelBoundaries, should have ", nb_components[4],
        " ModelBoundaries" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    auto brep = geode::load_brep(
        absl::StrCat( geode::data_path, "prism_curve.og_brep" ) );
    const auto brep2 = geode::load_brep(
        absl::StrCat( geode::data_path, "dangling.og_brep" ) );
    std::array< geode::index_t, 5 > nb_components{ brep.nb_corners()
                                                       + brep2.nb_corners(),
        brep.nb_lines() + brep2.nb_lines(),
        brep.nb_surfaces() + brep2.nb_surfaces(),
        brep.nb_blocks() + brep2.nb_blocks(),
        brep.nb_model_boundaries() + brep2.nb_model_boundaries() };
    geode::BRepConcatener concatener{ brep };
    concatener.concatenate( brep2 );
    check_concatenation( brep, nb_components );
    geode::save_brep( brep, "concatenated_brep.og_brep" );
}

OPENGEODE_TEST( "model-concatener" )