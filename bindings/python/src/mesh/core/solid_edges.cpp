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

#include <geode/mesh/core/solid_edges.hpp>

#define PYTHON_SOLID_EDGES( dimension )                                        \
    const auto name##dimension =                                               \
        "SolidEdges" + std::to_string( dimension ) + "D";                      \
    pybind11::class_< SolidEdges##dimension##D >( module,                      \
        name##dimension.c_str(),                                               \
        "Edges of a solid mesh, available once enabled with enable_edges()" )  \
        .def( "nb_edges", &SolidEdges##dimension##D::nb_edges,                 \
            "Return the number of edges" )                                     \
        .def( "edge_vertices",                                                 \
            geode::checked( "edge_vertices",                                   \
                &SolidEdges##dimension##D::edge_vertices,                      \
                geode::EdgeCheck{} ),                                          \
            "Return the indices of the two vertices of the edge",              \
            pybind11::arg( "edge_id" ) )                                       \
        .def( "edge_from_vertices",                                            \
            &SolidEdges##dimension##D::edge_from_vertices,                     \
            "Return the index of the edge between the two given vertices, in " \
            "any order, or None if there is none",                             \
            pybind11::arg( "vertices" ) )                                      \
        .def( "edge_attribute_manager",                                        \
            &SolidEdges##dimension##D::edge_attribute_manager,                 \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the manager of the attributes associated with the edges" )

namespace geode
{
    void define_solid_edges( pybind11::module& module )
    {
        PYTHON_SOLID_EDGES( 3 );
    }
} // namespace geode
