/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/mesh/builder/solid_edges_builder.h>

#include <geode/basic/attribute_manager.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/solid_edges.h>
#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    template < index_t dimension >
    SolidEdgesBuilder< dimension >::SolidEdgesBuilder(
        SolidEdges< dimension >& edges )
        : edges_( &edges )
    {
    }

    template < index_t dimension >
    index_t SolidEdgesBuilder< dimension >::find_or_create_edge(
        std::array< index_t, 2 > edge_vertices )
    {
        return edges_->find_or_create_edge( std::move( edge_vertices ), {} );
    }

    template < index_t dimension >
    void SolidEdgesBuilder< dimension >::remove_edge(
        std::array< index_t, 2 > edge_vertices )
    {
        edges_->remove_edge( std::move( edge_vertices ), {} );
    }

    template < index_t dimension >
    std::vector< index_t >
        SolidEdgesBuilder< dimension >::delete_isolated_edges()
    {
        return edges_->remove_isolated_edges( {} );
    }

    template < index_t dimension >
    void SolidEdgesBuilder< dimension >::update_edge_vertex(
        std::array< index_t, 2 > edge_vertices,
        index_t edge_vertex_id,
        index_t new_vertex_id )
    {
        edges_->update_edge_vertex(
            std::move( edge_vertices ), edge_vertex_id, new_vertex_id, {} );
    }

    template < index_t dimension >
    void SolidEdgesBuilder< dimension >::update_edge_vertices(
        absl::Span< const index_t > old2new )
    {
        edges_->update_edge_vertices( old2new, {} );
    }

    template < index_t dimension >
    void SolidEdgesBuilder< dimension >::copy(
        const SolidEdges< dimension >& edges )
    {
        edges_->overwrite_edges( edges, {} );
    }

    template < index_t dimension >
    std::vector< index_t > SolidEdgesBuilder< dimension >::delete_edges(
        const std::vector< bool >& to_delete )
    {
        return edges_->delete_edges( to_delete, {} );
    }

    template class opengeode_mesh_api SolidEdgesBuilder< 3 >;
} // namespace geode
