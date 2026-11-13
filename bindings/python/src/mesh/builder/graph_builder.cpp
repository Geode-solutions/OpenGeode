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

#include "../../common.hpp"
#include "../index_check.hpp"

#include <geode/mesh/builder/graph_builder.hpp>

namespace geode
{
    void define_graph_builder( pybind11::module& module )
    {
        pybind11::class_< GraphBuilder, VertexSetBuilder >(
            module, "GraphBuilder", "Builder to modify a graph" )
            .def_static( "create",
                geode::builder_creator( &GraphBuilder::create ),
                pybind11::arg( "mesh" ), pybind11::keep_alive< 0, 1 >(),
                "Return the builder to modify the graph, which keeps the mesh "
                "alive" )
            .def( "create_edge",
                static_cast< index_t ( GraphBuilder::* )() >(
                    &GraphBuilder::create_edge ),
                "Create an edge without vertices and return its index" )
            .def( "create_edge_with_vertices",
                static_cast< index_t ( GraphBuilder::* )( index_t, index_t ) >(
                    &GraphBuilder::create_edge ),
                "Create an edge between two vertices and return its index",
                pybind11::arg( "v0_id" ), pybind11::arg( "v1_id" ) )
            .def( "create_edges", &GraphBuilder::create_edges,
                "Create several edges and return the index of the first one",
                pybind11::arg( "nb" ) )
            .def( "set_edge_vertex",
                geode::checked_builder< GraphBuilder, Graph >(
                    "set_edge_vertex", &GraphBuilder::set_edge_vertex,
                    geode::EdgeVertexCheck{}, geode::VertexCheck{} ),
                "Set the vertex at an edge endpoint",
                pybind11::arg( "edge_vertex" ), pybind11::arg( "vertex_id" ) )
            .def( "delete_edges", &GraphBuilder::delete_edges,
                "Delete the edges whose flag is True and return the mapping "
                "from the old edge indices to the new ones, NO_ID for a "
                "deleted edge; to_delete has one flag per edge",
                pybind11::arg( "to_delete" ) )
            .def( "permute_edges", &GraphBuilder::permute_edges,
                "Reorder the edges so that the edge at position i is the old "
                "edge permutation[i], and return the mapping from the old edge "
                "indices to the new ones",
                pybind11::arg( "permutation" ) )
            .def( "delete_isolated_vertices",
                static_cast< std::vector< index_t > ( GraphBuilder::* )() >(
                    &GraphBuilder::delete_isolated_vertices ),
                "Delete the vertices that are not an edge endpoint and return "
                "the mapping from the old vertex indices to the new ones, "
                "NO_ID for a deleted vertex" );
    }
} // namespace geode
