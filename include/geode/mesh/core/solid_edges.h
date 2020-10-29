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

#pragma once

#include <vector>

#include <absl/container/inlined_vector.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/passkey.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidEdgesBuilder );

    ALIAS_2D_AND_3D( Vector );

    class AttributeManager;
} // namespace geode

namespace geode
{
    /*!
     * This class represents a 3D Solid made up with polyhedra and provides mesh
     * functionnalities.
     */
    template < index_t dimension >
    class SolidEdges
    {
        OPENGEODE_DISABLE_COPY( SolidEdges );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );
        PASSKEY( SolidEdgesBuilder< dimension >, SolidEdgesKey );

    public:
        SolidEdges( const SolidMesh< dimension >& solid );
        ~SolidEdges();

        index_t nb_edges() const;

        bool isolated_edge( index_t edge_id ) const;

        /*!
         * Return the indices of edge vertices.
         * @param[in] edge_id Index of an edge.
         */
        const std::array< index_t, 2 >& edge_vertices( index_t edge_id ) const;

        /*!
         * Get the index of edge corresponding to given vertices
         * @param[in] vertices Ordered vertex indices
         */
        absl::optional< index_t > edge_from_vertices(
            const std::array< index_t, 2 >& vertices ) const;

        /*!
         * Get all the polyhedra with both edge vertices.
         * @param[in] edge_id Index of the edge
         */
        PolyhedraAroundEdge polyhedra_around_edge( index_t edge_id ) const;

        /*!
         * Access to the manager of attributes associated with edges.
         */
        AttributeManager& edge_attribute_manager() const;

        void update_edge_vertices(
            absl::Span< const index_t > old2new, SolidEdgesKey );

        void update_edge_vertex( std::array< index_t, 2 > edge_vertices,
            index_t edge_vertex_id,
            index_t new_vertex_id,
            SolidEdgesKey );

        void remove_edge(
            std::array< index_t, 2 > edge_vertices, SolidEdgesKey );

        std::vector< index_t > delete_edges(
            const std::vector< bool >& to_delete, SolidEdgesKey );

        std::vector< index_t > remove_isolated_edges( SolidEdgesKey );

        index_t find_or_create_edge(
            std::array< index_t, 2 > edge_vertices, SolidEdgesKey )
        {
            return find_or_create_edge( edge_vertices );
        }

        void overwrite_edges(
            const SolidEdges< dimension >& from, SolidEdgesKey );

    private:
        SolidEdges();

        index_t find_or_create_edge( std::array< index_t, 2 > edge_vertices );

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( SolidEdges );
} // namespace geode
