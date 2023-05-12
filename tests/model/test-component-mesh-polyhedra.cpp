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
#include <geode/basic/range.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/helpers/component_mesh_polyhedra.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>

#include <geode/tests/common.h>

void test_component_mesh_polyhedra()
{
    auto model = geode::load_brep(
        absl::StrCat( geode::data_path, "test_mesh3.og_brep" ) );

    for( const auto& block : model.blocks() )
    {
        for( const auto polyhedron_id :
            geode::Range{ block.mesh().nb_polyhedra() } )
        {
            const auto polyhedron_unique_vertices =
                geode::polyhedron_unique_vertices(
                    model, block, polyhedron_id );
            const auto result = geode::component_mesh_polyhedra(
                model, polyhedron_unique_vertices );
            OPENGEODE_EXCEPTION( result.size() == 1,
                "[TEST] Should have found a polyhedron corresponding "
                "to given unique vertices." );
            const geode::MeshElement result_cme{ block.id(), polyhedron_id };
            OPENGEODE_EXCEPTION( result[0] == result_cme,
                "[TEST] Result polyhedron is ", result[0].string(),
                ", but it should be ", result_cme.string() );
        }
    }
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    test_component_mesh_polyhedra();
}

OPENGEODE_TEST( "convert-mesh-polygons" )