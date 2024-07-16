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

#include "../../common.hpp"

#include <geode/basic/attribute_manager.hpp>

#include <geode/mesh/core/graph.hpp>

namespace geode
{
    void define_graph( pybind11::module& module )
    {
        pybind11::class_< Graph, VertexSet >( module, "Graph" )
            .def_static(
                "create", static_cast< std::unique_ptr< Graph > ( * )() >(
                              &Graph::create ) )
            .def( "clone", &Graph::clone )
            .def( "edge_vertex", &Graph::edge_vertex )
            .def( "edge_vertices", &Graph::edge_vertices )
            .def( "nb_edges", &Graph::nb_edges )
            .def( "edge_attribute_manager", &Graph::edge_attribute_manager,
                pybind11::return_value_policy::reference )
            .def( "edges_around_vertex", &Graph::edges_around_vertex )
            .def( "is_vertex_isolated", &Graph::is_vertex_isolated );

        pybind11::class_< EdgeVertex >( module, "EdgeVertex" )
            .def( pybind11::init<>() )
            .def( pybind11::init< index_t, index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "string", &EdgeVertex::string )
            .def_readwrite( "edge_id", &EdgeVertex::edge_id )
            .def_readwrite( "vertex_id", &EdgeVertex::vertex_id );
    }
} // namespace geode
