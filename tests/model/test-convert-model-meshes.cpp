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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>
#include <geode/basic/range.h>
#include <geode/basic/uuid.h>

#include <geode/model/helpers/convert_model_meshes.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>
#include <geode/model/representation/io/brep_input.h>
#include <geode/model/representation/io/brep_output.h>
#include <geode/model/representation/io/section_input.h>
#include <geode/model/representation/io/section_output.h>

#include <geode/tests/common.h>

void run_test_brep()
{
    geode::BRep model;
    geode::load_brep(
        model, absl::StrCat( geode::data_path, "test_v5.og_brep" ) );

    geode::convert_surface_meshes_into_triangulated_surfaces( model );
    geode::convert_block_meshes_into_tetrahedral_solids( model );

    const auto file_io =
        absl::StrCat( "test_triangulated_surfaces.", model.native_extension() );
    geode::save_brep( model, file_io );

    geode::BRep model2;
    geode::load_brep( model2, file_io );
}

void run_test_section()
{
    geode::Section model;
    geode::load_section(
        model, absl::StrCat( geode::data_path, "test_v5.og_sctn" ) );

    geode::convert_surface_meshes_into_triangulated_surfaces( model );

    const auto file_io =
        absl::StrCat( "test_triangulated_surfaces.", model.native_extension() );
    geode::save_section( model, file_io );

    geode::Section model2;
    geode::load_section( model2, file_io );
}

void test()
{
    run_test_brep();
    run_test_section();
}

OPENGEODE_TEST( "convert-model-meshes" )