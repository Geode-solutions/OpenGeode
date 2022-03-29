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

#include <absl/types/span.h>

#include <geode/basic/passkey.h>

#include <geode/mesh/builder/vertex_set_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMeshBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceEdges );
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a SurfaceEdges
     */
    template < index_t dimension >
    class SurfaceEdgesBuilder
    {
    public:
        SurfaceEdgesBuilder( SurfaceEdges< dimension >& edges );

        /*!
         * Delete all the isolated edges (not used as polygon edges)
         * @return the mapping between old edge indices to new ones.
         * Deleted edges new index is NO_ID
         */
        std::vector< index_t > delete_isolated_edges();

        index_t find_or_create_edge( std::array< index_t, 2 > edge_vertices );

        std::vector< index_t > delete_edges(
            const std::vector< bool >& to_delete );

        void remove_edge( std::array< index_t, 2 > edge_vertices );

        void update_edge_vertices( absl::Span< const index_t > old2new );

        void update_edge_vertex( std::array< index_t, 2 > edge_vertices,
            index_t edge_vertex_id,
            index_t new_vertex_id );

        void copy( const SurfaceEdges< dimension >& edges );

    private:
        SurfaceEdges< dimension >* edges_;
    };
    ALIAS_2D_AND_3D( SurfaceEdgesBuilder );
} // namespace geode
