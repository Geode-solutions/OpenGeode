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

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/component_type.hpp>

namespace geode
{
    struct MeshVertex;
    struct uuid;
    class VertexIdentifierBuilder;
} // namespace geode

namespace geode
{
    /*!
     * Identify a vertex in a geometric component
     */
    struct opengeode_model_api ComponentMeshVertex
    {
        ComponentMeshVertex(
            ComponentID component_id_in, index_t vertex_id_in );

        ~ComponentMeshVertex();

        [[nodiscard]] MeshVertex mesh_vertex() const;

        [[nodiscard]] bool operator==( const ComponentMeshVertex& other ) const;

        template < typename Archive >
        void serialize( Archive& archive );

        [[nodiscard]] std::string string() const
        {
            return absl::StrCat( component_id.string(), " ", vertex );
        }

        ComponentID component_id;
        index_t vertex{ NO_ID };

    private:
        friend class bitsery::Access;
        ComponentMeshVertex();
    };

    /*!
     * This class identifies groups of geometric component vertices
     * as unique vertices.
     * This is a only topological information.
     */
    class opengeode_model_api VertexIdentifier
    {
        PASSKEY( VertexIdentifierBuilder, BuilderKey );

    public:
        VertexIdentifier();
        ~VertexIdentifier();

        [[nodiscard]] index_t nb_unique_vertices() const;

        [[nodiscard]] bool is_unique_vertex_isolated(
            index_t unique_vertex_id ) const;

        /*!
         * Return the component vertices identified with an unique vertex.
         * @param[in] unique_vertex_id Indice of the unique vertex.
         */
        [[nodiscard]] const std::vector< ComponentMeshVertex >&
            component_mesh_vertices( index_t unique_vertex_id ) const;
        /*!
         * Return the unique vertex index of a given component vertex.
         * @param[in] component_vertex Vertex index in a geometric component.
         */
        [[nodiscard]] index_t unique_vertex(
            const ComponentMeshVertex& component_vertex ) const;

        /*!
         * Return true if given unique vertex has at least one mesh component
         * vertex of given component type
         */
        [[nodiscard]] bool has_component_mesh_vertices(
            index_t unique_vertex_id, const ComponentType& type ) const;

        /*!
         * Return true if given unique vertex has at least one mesh component
         * vertex of given component
         */
        [[nodiscard]] bool has_component_mesh_vertices(
            index_t unique_vertex_id, const uuid& component_id ) const;

        /*!
         * Save the VertexIdentifier into a file.
         * @param[in] directory Folder in which create the file.
         */
        void save_unique_vertices( std::string_view directory ) const;

    public:
        /*!
         * Add a component in the VertexIdentifier
         */
        template < typename MeshComponent >
        void register_mesh_component(
            const MeshComponent& component, BuilderKey );

        /*!
         * Remove a component from the VertexIdentifier and delete corresponding
         * information (i.e. the attribute on component mesh).
         */
        template < typename MeshComponent >
        void unregister_mesh_component(
            const MeshComponent& component, BuilderKey );

        /*!
         * Create an empty unique vertex.
         * @return Index of the created unique vertex.
         */
        index_t create_unique_vertex( BuilderKey );

        /*!
         * Create several empty unique vertices
         * @return Index of the first created unique vertex
         */
        index_t create_unique_vertices( index_t nb, BuilderKey );

        /*!
         * Identify a component vertex to an existing unique vertex index.
         * @param[in] component_vertex_id Index of the vertex in the component.
         * @param[in] unique_vertex_id Unique vertex index.
         */
        void set_unique_vertex( ComponentMeshVertex component_vertex_id,
            index_t unique_vertex_id,
            BuilderKey );

        /*!
         * Remove a component vertex to its unique vertex index.
         * @param[in] component_vertex_id Index of the vertex in the component.
         * @param[in] unique_vertex_id Unique vertex index.
         */
        void unset_unique_vertex(
            const ComponentMeshVertex& component_vertex_id,
            index_t unique_vertex_id,
            BuilderKey );

        /*!
         * Remove a component vertex to its unique vertex index.
         * @param[in] component_id Component unique index used to filter
         * returned vertices.
         * @param[in] old2new Mapping between mesh component old vertex indices
         * to new ones. Deleted vertices new index is NO_ID.
         */
        void update_unique_vertices( const ComponentID& component_id,
            absl::Span< const index_t > old2new,
            BuilderKey );

        /*!
         * Load the VertexIdentifier from a file.
         * @param[in] directory Folder containing the file that stores
         * VertexIdentifier information.
         */
        void load_unique_vertices( std::string_view directory, BuilderKey );

        /*!
         * Delete all unique vertices not associated with any component
         */
        std::vector< index_t > delete_isolated_vertices( BuilderKey );

    protected:
        VertexIdentifier( VertexIdentifier&& other ) noexcept;
        VertexIdentifier& operator=( VertexIdentifier&& other ) noexcept;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::ComponentMeshVertex >
    {
        size_t operator()( const geode::ComponentMeshVertex& cmv ) const
        {
            return absl::Hash< geode::uuid >()( cmv.component_id.id() )
                   ^ absl::Hash< geode::index_t >()( cmv.vertex );
        }
    };
} // namespace std
