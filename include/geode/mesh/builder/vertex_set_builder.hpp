/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <absl/types/span.h>

#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/passkey.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/mesh_id.hpp>

namespace geode
{
    class MeshBuilderFactoryKey;
    class VertexSet;
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a VertexSet
     */
    class opengeode_mesh_api VertexSetBuilder : public IdentifierBuilder
    {
        OPENGEODE_DISABLE_COPY( VertexSetBuilder );
        PASSKEY( VertexSet, VertexSetKey );

    public:
        VertexSetBuilder( VertexSetBuilder&& ) noexcept = default;

        virtual ~VertexSetBuilder() = default;

        /*!
         * Create the builder associated with a VertexSet.
         * @param[in] mesh The VertexSet to build/modify
         */
        [[nodiscard]] static std::unique_ptr< VertexSetBuilder > create(
            VertexSet& mesh );

        /*!
         * Create a new vertex.
         * @return the index of the created vertex
         */
        index_t create_vertex();

        /*!
         * Create a set of vertices.
         * @param[in] nb Number of vertices to create.
         * @return the index of the first created vertex
         */
        index_t create_vertices( index_t nb );

        /*!
         * Delete a set of vertices.
         * @param[in] to_delete Vector of size vertex_set_.nb_vertices(). If
         * to_delete[i] is true the vertex of index i is deleted, else it is
         * kept.
         * @return the mapping between old vertex indices to new ones.
         * Deleted vertices new index is NO_ID
         */
        std::vector< index_t > delete_vertices(
            const std::vector< bool >& to_delete );

        /*!
         * Permute vertices to match the given order.
         * @param[in] permutation Vector of size vertex_set_.nb_vertices().
         * Each value corresponds to the destination position.
         * @return  the mapping between old vertex indices to new ones.
         */
        std::vector< index_t > permute_vertices(
            absl::Span< const index_t > permutation );

    public:
        void copy( const VertexSet& vertex_set, VertexSetKey key );

    protected:
        explicit VertexSetBuilder( VertexSet& vertex_set );

        void copy( const VertexSet& vertex_set );

        virtual void do_create_vertex() = 0;

    private:
        virtual void do_create_vertices( index_t nb ) = 0;

        virtual void do_delete_vertices( const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_permute_vertices(
            absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) = 0;

    private:
        VertexSet& vertex_set_;
    };
} // namespace geode
