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
    auto model =
        geode::load_brep( absl::StrCat( geode::data_path, "layers.og_brep" ) );

    geode::triangulate_surface_meshes( model );
    geode::convert_surface_meshes_into_triangulated_surfaces( model );

    const auto file_io =
        absl::StrCat( "test_triangulated_surfaces.", model.native_extension() );
    geode::save_brep( model, file_io );

    auto model2 = geode::load_brep( file_io );
}

void run_test_section()
{
    auto model =
        geode::load_section( absl::StrCat( geode::data_path, "quad.og_sctn" ) );

    geode::triangulate_surface_meshes( model );
    geode::convert_surface_meshes_into_triangulated_surfaces( model );

    const auto file_io =
        absl::StrCat( "test_triangulated_surfaces.", model.native_extension() );
    geode::save_section( model, file_io );

    auto model2 = geode::load_section( file_io );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    run_test_brep();
    run_test_section();
}

OPENGEODE_TEST( "convert-model-meshes" )