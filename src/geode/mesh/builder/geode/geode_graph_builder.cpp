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

#include <geode/mesh/builder/geode/geode_graph_builder.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/geode/geode_graph.h>

namespace geode
{
    OpenGeodeGraphBuilder::OpenGeodeGraphBuilder(
        VertexSet& vertex_set, MeshBuilderFactoryKey )
        : OpenGeodeGraphBuilder( dynamic_cast< OpenGeodeGraph& >( vertex_set ) )
    {
    }

    OpenGeodeGraphBuilder::OpenGeodeGraphBuilder( OpenGeodeGraph& mesh )
        : GraphBuilder( mesh ), geode_graph_( mesh )
    {
    }

    void OpenGeodeGraphBuilder::do_create_vertex()
    {
        // Operation is directly handled by the AttributeManager
    }

    void OpenGeodeGraphBuilder::do_create_vertices( index_t /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    void OpenGeodeGraphBuilder::do_delete_graph_vertices(
        const std::vector< bool >& /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    void OpenGeodeGraphBuilder::do_permute_graph_vertices(
        absl::Span< const index_t > /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    void OpenGeodeGraphBuilder::do_set_edge_vertex(
        const EdgeVertex& edge_vertex, index_t vertex_id )
    {
        geode_graph_.set_edge_vertex( edge_vertex, vertex_id, {} );
    }

    void OpenGeodeGraphBuilder::do_create_edge()
    {
        // Operation is directly handled by the AttributeManager
    }

    void OpenGeodeGraphBuilder::do_create_edges( index_t /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    void OpenGeodeGraphBuilder::do_delete_edges(
        const std::vector< bool >& /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    void OpenGeodeGraphBuilder::do_permute_edges(
        absl::Span< const index_t > /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    void OpenGeodeGraphBuilder::do_copy_edges( const Graph& /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }
} // namespace geode
