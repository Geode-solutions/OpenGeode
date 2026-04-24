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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/convert_to_mesh.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/section_input.hpp>

#include <geode/tests/common.hpp>

void run_test_brep()
{
    auto model = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "test_mesh3.og_brep" ) );
    const auto curve = std::get< 0 >( geode::convert_brep_into_curve( model ) );
    geode::OpenGeodeModelException::test(
        curve->nb_vertices() == 154, "BRep - Wrong number of curve vertices" );
    geode::OpenGeodeModelException::test(
        curve->nb_edges() == 162, "BRep - Wrong number of curve edges" );
    const auto surface =
        std::get< 0 >( geode::convert_brep_into_surface( model ) );
    geode::OpenGeodeModelException::test( surface->nb_vertices() == 840,
        "BRep - Wrong number of surface vertices" );
    geode::OpenGeodeModelException::test( surface->nb_polygons() == 1716,
        "BRep - Wrong number of surface polygons" );
    const auto solid = std::get< 0 >( geode::convert_brep_into_solid( model ) );
    geode::OpenGeodeModelException::test(
        solid->nb_vertices() == 1317, "BRep - Wrong number of solid vertices" );
    geode::OpenGeodeModelException::test( solid->nb_polyhedra() == 5709,
        "BRep - Wrong number of solid polyhedra" );
}

void run_test_section()
{
    const auto model =
        geode::load_section( absl::StrCat( geode::DATA_PATH, "quad.og_sctn" ) );

    const auto curve =
        std::get< 0 >( geode::convert_section_into_curve( model ) );
    geode::OpenGeodeModelException::test(
        curve->nb_vertices() == 0, "Section - Wrong number of curve vertices" );
    geode::OpenGeodeModelException::test(
        curve->nb_edges() == 0, "Section - Wrong number of curve edges" );
    const auto surface =
        std::get< 0 >( geode::convert_section_into_surface( model ) );
    geode::OpenGeodeModelException::test( surface->nb_vertices() == 4,
        "Section - Wrong number of surface vertices" );
    geode::OpenGeodeModelException::test( surface->nb_polygons() == 1,
        "Section - Wrong number of surface polygons" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    run_test_brep();
    run_test_section();
}

OPENGEODE_TEST( "convert-to-mesh" )