/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/core/solid_mesh.h>

#include <geode/model/helpers/component_mesh_queries.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>
#include <geode/model/representation/io/brep_output.h>

#include <geode/tests/common.h>

void run_test_brep()
{
    auto model = geode::load_brep(
        absl::StrCat( geode::data_path, "test_mesh3.og_brep" ) );

    for( const auto& block : model.blocks() )
    {
        for( const auto polyhedron_id :
            geode::Range{ block.mesh().nb_polyhedra() } )
        {
            for( const auto facet_id : geode::LRange{
                     block.mesh().nb_polyhedron_facets( polyhedron_id ) } )
            {
                const geode::PolyhedronFacet facet{ polyhedron_id, facet_id };
                geode::PolyhedronFacetVertices facet_unique_vertices;
                for( const auto facet_vertex_id : geode::LRange{
                         block.mesh().nb_polyhedron_facet_vertices( facet ) } )
                {
                    facet_unique_vertices.push_back(
                        model.unique_vertex( { block.component_id(),
                            block.mesh().polyhedron_facet_vertex(
                                { facet, facet_vertex_id } ) } ) );
                }
                const auto polyhedra_around_facet =
                    geode::block_mesh_polyhedra_from_unique_vertices_facet(
                        model, block, facet_unique_vertices );
                OPENGEODE_EXCEPTION(
                    absl::c_any_of( polyhedra_around_facet,
                        [polyhedron_id](
                            const geode::PolyhedronFacet& poly_around_id ) {
                            return polyhedron_id
                                   == poly_around_id.polyhedron_id;
                        } ),
                    "[Test] Facet ", facet_id, " of polyhedron ", polyhedron_id,
                    " could not be linked to the polyhedron through its unique "
                    "vertices." );
            }
        }
    }
}

void test()
{
    run_test_brep();
}

OPENGEODE_TEST( "convert-model-meshes" )