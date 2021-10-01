/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/helpers/convert_to_mesh.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>
#include <geode/model/representation/io/brep_input.h>
#include <geode/model/representation/io/section_input.h>

#include <geode/tests/common.h>

void run_test_brep()
{
    auto model =
        geode::load_brep( absl::StrCat( geode::data_path, "layers.og_brep" ) );

    const auto output =
        geode::convert_brep_into_curve_and_surface_and_solid( model );
    const auto& curve = std::get< 0 >( output );
    OPENGEODE_EXCEPTION( curve->nb_vertices() == 16,
        "[Test] BRep - Wrong number of curve vertices" );
    OPENGEODE_EXCEPTION(
        curve->nb_edges() == 28, "[Test] BRep - Wrong number of curve edges" );

    const auto& surface = std::get< 1 >( output );
    OPENGEODE_EXCEPTION( surface->nb_vertices() == 16,
        "[Test] BRep - Wrong number of surface vertices" );
    OPENGEODE_EXCEPTION( surface->nb_polygons() == 16,
        "[Test] BRep - Wrong number of surface polygons" );

    const auto& solid = std::get< 2 >( output );
    OPENGEODE_EXCEPTION( solid->nb_vertices() == 16,
        "[Test] BRep - Wrong number of solid vertices" );
    OPENGEODE_EXCEPTION( solid->nb_polyhedra() == 0,
        "[Test] BRep - Wrong number of solid polyhedra" );
}

void run_test_section()
{
    auto model =
        geode::load_section( absl::StrCat( geode::data_path, "quad.og_sctn" ) );

    const auto output = geode::convert_section_into_curve_and_surface( model );
    const auto& curve = std::get< 0 >( output );
    OPENGEODE_EXCEPTION( curve->nb_vertices() == 0,
        "[Test] Section - Wrong number of curve vertices" );
    OPENGEODE_EXCEPTION( curve->nb_edges() == 0,
        "[Test] Section - Wrong number of curve edges" );

    const auto& surface = std::get< 1 >( output );
    OPENGEODE_EXCEPTION( surface->nb_vertices() == 1,
        "[Test] Section - Wrong number of surface vertices" );
    OPENGEODE_EXCEPTION( surface->nb_polygons() == 1,
        "[Test] Section - Wrong number of surface polygons" );
}

void test()
{
    run_test_brep();
    run_test_section();
}

OPENGEODE_TEST( "convert-to-mesh" )