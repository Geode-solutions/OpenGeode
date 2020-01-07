#pragma once

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/component_type.h>

namespace geode
{
    /*!
     * Identify a vertex in a geometric component
     */
    struct MeshComponentVertex
    {
        MeshComponentVertex( ComponentID component_id, index_t vertex_id )
            : component_id( std::move( component_id ) ), vertex( vertex_id )
        {
        }

        bool operator==( const MeshComponentVertex& other ) const
        {
            return component_id == other.component_id && vertex == other.vertex;
        }

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                DefaultGrowable< Archive, MeshComponentVertex >{},
                []( Archive& archive,
                    MeshComponentVertex& mesh_component_vertex ) {
                    archive.object( mesh_component_vertex.component_id );
                    archive.value4b( mesh_component_vertex.vertex );
                } );
        }

        ComponentID component_id;
        index_t vertex{ NO_ID };

    private:
        friend class bitsery::Access;
        MeshComponentVertex()
            : component_id( bitsery::Access::create< ComponentID >() )
        {
        }
    };

    /*!
     * This class identifies groups of geometric component vertices
     * as unique vertices.
     * This is a only topological information.
     */
    class opengeode_model_api VertexIdentifier
    {
        friend class VertexIdentifierBuilder;

    public:
        VertexIdentifier();
        ~VertexIdentifier();

        index_t nb_unique_vertices() const;

        /*!
         * Return the component vertices identified with an unique vertex.
         * @param[in] unique_vertex_id Indice of the unique vertex.
         */
        const std::vector< MeshComponentVertex >& mesh_component_vertices(
            index_t unique_vertex_id ) const;

        /*!
         * Return the component vertices identified with an unique vertex
         * only for vertices belonging to component of the specified type.
         * @param[in] unique_vertex_id Indice of the unique vertex.
         * @param[in] type Type of components used to filter returned vertices.
         */
        std::vector< MeshComponentVertex > mesh_component_vertices(
            index_t unique_vertex_id, const ComponentType& type ) const;

        /*!
         * Return the component vertices identified with an unique vertex
         * only for vertices belonging to one specified component.
         * @param[in] unique_vertex_id Indice of the unique vertex.
         * @param[in] component_id Component unique index used to filter
         * returned vertices.
         */
        std::vector< index_t > mesh_component_vertices(
            index_t unique_vertex_id, const uuid& component_id ) const;

        /*!
         * Return the unique vertex index of a given component vertex.
         * @param[in] component_vertex Vertex index in a geometric component.
         */
        index_t unique_vertex(
            const MeshComponentVertex& component_vertex ) const;

        /*!
         * Save the VertexIdentifier into a file.
         * @param[in] directory Folder in which create the file.
         */
        void save_unique_vertices( const std::string& directory ) const;

    private:
        /*!
         * Add a component in the VertexIdentifier
         */
        template < typename MeshComponent >
        void register_mesh_component( const MeshComponent& component );

        /*!
         * Remove a component from the VertexIdentifier and delete corresponding
         * information (i.e. the attribute on component mesh).
         */
        template < typename MeshComponent >
        void unregister_mesh_component( const MeshComponent& component );

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
            MeshComponentVertex component_vertex_id, index_t unique_vertex_id );

        /*!
         * Load the VertexIdentifier from a file.
         * @param[in] directory Folder containing the file that stores
         * VertexIdentifier information.
         */
        void load_unique_vertices( const std::string& directory );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
