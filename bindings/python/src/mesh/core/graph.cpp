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

#include <geode/basic/attribute_manager.hpp>

#include <geode/mesh/core/graph.hpp>

namespace geode
{
    void define_graph( pybind11::module& module )
    {
        pybind11::class_< Graph, VertexSet, pybind11::smart_holder >(
            module, "Graph", "Graph: a set of vertices linked by edges" )
            .def_static( "create",
                static_cast< std::unique_ptr< Graph > ( * )() >(
                    &Graph::create ),
                "Create an empty graph with the default data structure" )
            .def( "clone", &Graph::clone,
                "Return a copy of the graph, with the same data structure, "
                "name and identifier" )
            .def( "edge_vertex",
                geode::checked( "edge_vertex", &Graph::edge_vertex,
                    geode::EdgeVertexCheck{} ),
                "Return the index of the vertex at this edge endpoint",
                pybind11::arg( "edge_vertex" ) )
            .def( "edge_vertices",
                geode::checked( "edge_vertices", &Graph::edge_vertices,
                    geode::EdgeCheck{} ),
                "Return the indices of the two vertices of the edge",
                pybind11::arg( "edge_id" ) )
            .def( "nb_edges", &Graph::nb_edges, "Return the number of edges" )
            .def( "edge_attribute_manager", &Graph::edge_attribute_manager,
                pybind11::return_value_policy::reference_internal,
                "Return the manager of the attributes associated with the "
                "edges" )
            .def( "edges_around_vertex",
                geode::checked( "edges_around_vertex",
                    &Graph::edges_around_vertex, geode::VertexCheck{} ),
                "Return all the edge endpoints located at the vertex",
                pybind11::arg( "vertex_id" ) )
            .def( "is_vertex_isolated",
                geode::checked( "is_vertex_isolated",
                    &Graph::is_vertex_isolated, geode::VertexCheck{} ),
                "Return True if the vertex is not an endpoint of any edge",
                pybind11::arg( "vertex_id" ) );
    }
} // namespace geode
