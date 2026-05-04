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

#include <geode/tests/common.hpp>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/model/helpers/compute_unique_vertices.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>

void test_brep()
{
    auto brep = geode::load_brep( absl::StrCat(
        geode::DATA_PATH, "rectangular_cuboid_without_uv.og_brep" ) );
    geode::BRepBuilder brep_builder{ brep };
    geode::compute_model_unique_vertices( brep, brep_builder );
    geode::OpenGeodeModelException::test(
        brep.nb_unique_vertices() == 8, "Wrong number of unique vertices" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_brep();
}

OPENGEODE_TEST( "convert-brep" )