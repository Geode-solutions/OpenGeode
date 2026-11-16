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

#include <geode/basic/attribute_manager.hpp>

#include <geode/mesh/core/graph.hpp>

namespace geode
{
    void define_graph_elements( pybind11::module& module )
    {
        auto edge_vertex = define_string_representation(
            pybind11::class_< EdgeVertex >( module, "EdgeVertex",
                "Endpoint of a graph edge: the edge index and the endpoint "
                "(0 or 1)" ),
            []( const EdgeVertex& value ) {
                return python_call_repr( "EdgeVertex",
                    { "edge_id=" + python_repr( value.edge_id ),
                        "vertex_id=" + python_repr( value.vertex_id ) } );
            } );
        edge_vertex
            .def( pybind11::init<>(),
                "Build an edge endpoint with invalid indices (NO_ID, NO_LID)" )
            .def( pybind11::init< index_t, index_t >(),
                pybind11::arg( "edge_id" ), pybind11::arg( "vertex_id" ),
                "Build the endpoint vertex_id (0 or 1) of the edge edge_id" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__", []( const EdgeVertex& value ) {
                return absl::HashOf( value.edge_id, value.vertex_id );
            } );
        def_readwrite_named(
            edge_vertex, "edge_id", &EdgeVertex::edge_id, "Index of the edge" );
        def_readwrite_named( edge_vertex, "vertex_id", &EdgeVertex::vertex_id,
            "Endpoint of the edge (0 or 1)" );
    }
} // namespace geode
