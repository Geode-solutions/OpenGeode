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

#include <geode/model/helpers/component_mesh_queries.h>

#include <absl/container/inlined_vector.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/core/brep.h>

namespace
{
    absl::FixedArray< std::vector< geode::index_t > >
        block_facet_from_model_unique_vertices( const geode::BRep& model,
            const geode::Block3D& block,
            absl::Span< const geode::index_t > facet_unique_vertices )
    {
        absl::FixedArray< std::vector< geode::index_t > >
            block_facet_from_unique_vertices( facet_unique_vertices.size() );
        for( const auto polygon_vertex_id :
            geode::LIndices{ facet_unique_vertices } )
        {
            block_facet_from_unique_vertices[polygon_vertex_id] =
                model.mesh_component_vertices(
                    facet_unique_vertices[polygon_vertex_id], block.id() );
        }
        return block_facet_from_unique_vertices;
    }

    geode::newPolyhedraAroundFacet block_mesh_polyhedra_around_triangle(
        const geode::SolidMesh3D& mesh,
        absl::Span< const std::vector< geode::index_t > >
            block_facet_from_unique_vertices )
    {
        geode::newPolyhedraAroundFacet polyhedra_around_polygon_facet;
        for( const auto first_vertex_id : block_facet_from_unique_vertices[0] )
        {
            for( const auto second_vertex_id :
                block_facet_from_unique_vertices[1] )
            {
                for( const auto third_vertex_id :
                    block_facet_from_unique_vertices[2] )
                {
                    const auto polyhedra_from_facet_vertices =
                        mesh.polyhedra_from_facet_vertices( { { first_vertex_id,
                            second_vertex_id, third_vertex_id } } );
                    if( !polyhedra_from_facet_vertices.empty() )
                    {
                        for( const auto& polyhedron :
                            polyhedra_from_facet_vertices )
                        {
                            polyhedra_around_polygon_facet.emplace_back(
                                polyhedron );
                        }
                    }
                }
            }
        }
        return polyhedra_around_polygon_facet;
    }

    geode::newPolyhedraAroundFacet block_mesh_polyhedra_around_quadrangle(
        const geode::SolidMesh3D& mesh,
        absl::Span< const std::vector< geode::index_t > >
            block_facet_from_unique_vertices )
    {
        for( const auto first_vertex_id : block_facet_from_unique_vertices[0] )
        {
            for( const auto second_vertex_id :
                block_facet_from_unique_vertices[1] )
            {
                for( const auto third_vertex_id :
                    block_facet_from_unique_vertices[2] )
                {
                    for( const auto fourth_vertex_id :
                        block_facet_from_unique_vertices[3] )
                    {
                        const auto polyhedra_around_polygon_facet =
                            mesh.polyhedra_from_facet_vertices(
                                { { first_vertex_id, second_vertex_id,
                                    third_vertex_id, fourth_vertex_id } } );
                        if( !polyhedra_around_polygon_facet.empty() )
                        {
                            return polyhedra_around_polygon_facet;
                        }
                    }
                }
            }
        }
        return {};
    }
} // namespace

namespace geode
{
    newPolyhedraAroundFacet block_mesh_polyhedra_from_unique_vertices_facet(
        const BRep& model,
        const Block3D& block,
        absl::Span< const index_t > facet_unique_vertices )
    {
        const auto block_facet_from_unique_vertices =
            block_facet_from_model_unique_vertices(
                model, block, facet_unique_vertices );
        if( facet_unique_vertices.size() == 3 )
        {
            return block_mesh_polyhedra_around_triangle(
                block.mesh(), block_facet_from_unique_vertices );
        }
        if( facet_unique_vertices.size() == 4 )
        {
            return block_mesh_polyhedra_around_quadrangle(
                block.mesh(), block_facet_from_unique_vertices );
        }
        OPENGEODE_ASSERT_NOT_REACHED(
            "[helpers::block_mesh_polyhedra_from_unique_vertices_facet] "
            "Function is not yet implemented for facets other than "
            "triangles and quadrangles." );
        return {};
    }
} // namespace geode