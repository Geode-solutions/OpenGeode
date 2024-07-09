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

#pragma once

#include <absl/types/span.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/vertex_identifier.h>

namespace geode
{
    /*!
     * Class managing modification of VertexIdentifier
     */
    class opengeode_model_api VertexIdentifierBuilder
    {
    public:
        VertexIdentifierBuilder( VertexIdentifier& vertex_identifier );

        /*!
         * Add a component in the VertexIdentifier
         */
        template < typename MeshComponent >
        void register_mesh_component( const MeshComponent& component )
        {
            vertex_identifier_.register_mesh_component( component, {} );
        }

        /*!
         * Remove a component from the VertexIdentifier and delete corresponding
         * information (i.e. the attribute on component mesh).
         */
        template < typename MeshComponent >
        void unregister_mesh_component( const MeshComponent& component )
        {
            vertex_identifier_.unregister_mesh_component( component, {} );
        }

        /*!
         * Create an empty unique vertex.
         * @return Index of the created unique vertex.
         */
        index_t create_unique_vertex();

        /*!
         * Create several empty unique vertices
         * @return Index of the first created unique vertex
         */
        index_t create_unique_vertices( index_t nb );

        /*!
         * Identify a component vertex to an existing unique vertex index.
         * @param[in] component_vertex_id Index of the vertex in the component.
         * @param[in] unique_vertex_id Unique vertex index.
         */
        void set_unique_vertex(
            ComponentMeshVertex component_vertex_id, index_t unique_vertex_id );

        /*!
         * Remove a component vertex to its unique vertex index.
         * @param[in] component_vertex_id Index of the vertex in the component.
         * @param[in] unique_vertex_id Unique vertex index.
         */
        void unset_unique_vertex(
            const ComponentMeshVertex& component_vertex_id,
            index_t unique_vertex_id );

        /*!
         * Remove a component vertex to its unique vertex index.
         * @param[in] component_id Component unique index used to filter
         * returned vertices.
         * @param[in] old2new Mapping between mesh component old vertex indices
         * to new ones. Deleted vertices new index is NO_ID.
         */
        void update_unique_vertices( const ComponentID& component_id,
            absl::Span< const index_t > old2new );

        /*!
         * Load the VertexIdentifier from a file.
         * @param[in] directory Folder containing the file that stores
         * VertexIdentifier information.
         */
        void load_unique_vertices( std::string_view directory );

        /*!
         * Delete all unique vertices not associated with any component
         */
        std::vector< index_t > delete_isolated_vertices();

    private:
        VertexIdentifier& vertex_identifier_;
    };
} // namespace geode