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

#include <geode/mesh/builder/geode_graph_builder.h>

#include <geode/mesh/core/geode_graph.h>

namespace geode
{
    OpenGeodeGraphBuilder::OpenGeodeGraphBuilder( Graph& graph )
        : GraphBuilder( graph ),
          geode_graph_( dynamic_cast< OpenGeodeGraph& >( graph ) )
    {
    }

    void OpenGeodeGraphBuilder::do_create_vertex() {}

    void OpenGeodeGraphBuilder::do_create_vertices( index_t /*unused*/ ) {}

    void OpenGeodeGraphBuilder::do_delete_curve_vertices(
        const std::vector< bool >& /*unused*/ )
    {
    }

    void OpenGeodeGraphBuilder::do_set_edge_vertex(
        const EdgeVertex& edge_vertex, index_t vertex_id )
    {
        geode_graph_.set_edge_vertex( edge_vertex, vertex_id );
    }

    void OpenGeodeGraphBuilder::do_create_edge() {}

    void OpenGeodeGraphBuilder::do_create_edges( index_t /*unused*/ ) {}

    void OpenGeodeGraphBuilder::do_delete_edges(
        const std::vector< bool >& /*unused*/ )
    {
    }
} // namespace geode
