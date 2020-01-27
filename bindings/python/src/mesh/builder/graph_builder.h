/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/builder/graph_builder.h>

namespace geode
{
    void define_graph_builder( pybind11::module& module )
    {
        pybind11::class_< GraphBuilder, VertexSetBuilder >(
            module, "GraphBuilder" )
            .def( "create", ( std::unique_ptr< GraphBuilder >( * )( Graph& ) )
                                & GraphBuilder::create )
            .def( "create_edge",
                ( index_t( GraphBuilder::* )() ) & GraphBuilder::create_edge )
            .def( "create_edge_with_vertices",
                ( index_t( GraphBuilder::* )( index_t, index_t ) )
                    & GraphBuilder::create_edge )
            .def( "create_edges", &GraphBuilder::create_edges )
            .def( "set_edge_vertex", &GraphBuilder::set_edge_vertex )
            .def( "delete_edges", &GraphBuilder::delete_edges )
            .def( "delete_isolated_vertices",
                &GraphBuilder::delete_isolated_vertices );
    }
} // namespace geode
