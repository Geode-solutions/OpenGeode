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

#pragma once

#include <vector>

#include <geode/mesh/builder/vertex_set_builder.h>
#include <geode/mesh/common.h>
#include <geode/mesh/core/graph.h>

namespace geode
{
    class Graph;

    struct EdgeVertex;
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a Graph
     */
    class opengeode_mesh_api GraphBuilder : public VertexSetBuilder
    {
    public:
        /*!
         * Create the builder associated with a Graph.
         * @param[in] mesh The Graph to build/modify
         */
        static std::unique_ptr< GraphBuilder > create( Graph& mesh );

        /*!
         * Create a new edge.
         * @return the index of the created edge
         */
        index_t create_edge();

        /*!
         * Create a new edge between to vertex.
         * @param[in] v0_id Index of the edge initial vertex
         * @param[in] v1_id Index of the edge terminal vertex
         * @return the index of the created edge
         */
        index_t create_edge( index_t v0_id, index_t v1_id );

        /*!
         * Create several new edges.
         * @param[in] nb Number of edges to create
         * @return the index of the first created edge
         */
        index_t create_edges( index_t nb );

        /*!
         * Set a vertex as an edge endpoint
         * @param[in] edge_vertex Index of the edge endpoint
         * @param[in] vertex_id Index of the vertex to set as edge endpoint
         */
        void set_edge_vertex(
            const EdgeVertex& edge_vertex, index_t vertex_id );

        void associate_edge_vertex_to_vertex(
            const EdgeVertex& edge_vertex, index_t vertex_id );

        void disassociate_edge_vertex_to_vertex(
            const EdgeVertex& edge_vertex );

        void set_edges_around_vertex(
            index_t vertex_id, EdgesAroundVertex edges );

        /*!
         * Delete a set of edges
         * @param[in] to_delete Vector of size graph_.nb_edges(). If
         * to_delete[i] is true the edge of index i is deleted, else the edge is
         * kept.
         * @return the mapping between old edge indices to new ones.
         * Deleted edges new index is NO_ID
         */
        std::vector< index_t > delete_edges(
            const std::vector< bool >& to_delete );

        /*!
         * Permute edges to match the given order.
         * @param[in] permutation Vector of size graph_.nb_edges().
         * Each value corresponds to the destination position.
         * @return the mapping between old edge indices to new ones.
         */
        std::vector< index_t > permute_edges(
            absl::Span< const index_t > permutation );

        /*!
         * Delete all the isolated vertices (not used as edge endpoint)
         * @return the mapping between old vertex indices to new ones.
         * Deleted vertices new index is NO_ID
         */
        std::vector< index_t > delete_isolated_vertices();

        void copy( const Graph& graph );

    protected:
        GraphBuilder( Graph& mesh );

        using VertexSetBuilder::delete_vertices;

    private:
        void do_delete_vertices( const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) final;

        void do_permute_vertices( absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) final;

        virtual void do_permute_edges( absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_delete_graph_vertices(
            const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_permute_graph_vertices(
            absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_set_edge_vertex(
            const EdgeVertex& edge_vertex, index_t vertex_id ) = 0;

        virtual void do_create_edge() = 0;

        virtual void do_create_edges( index_t nb ) = 0;

        virtual void do_delete_edges( const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_copy_edges( const Graph& mesh ) = 0;

    private:
        Graph& graph_;
    };
} // namespace geode
