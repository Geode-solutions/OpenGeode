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

#include <geode/basic/attribute.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/factory.h>
#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( PolyhedralSolidBuilder );

    class AttributeManager;
} // namespace geode

namespace geode
{
    /*!
     * This struct represents a local vertex in a polyhedron
     */
    struct opengeode_mesh_api PolyhedronVertex
    {
        PolyhedronVertex() = default;
        PolyhedronVertex( index_t polyhedron_id, index_t vertex_id )
            : polyhedron_id( polyhedron_id ), vertex_id( vertex_id )
        {
        }
        bool operator==( const PolyhedronVertex& other ) const
        {
            return polyhedron_id == other.polyhedron_id
                   && vertex_id == other.vertex_id;
        }
        bool operator!=( const PolyhedronVertex& other ) const
        {
            return !( *this == other );
        }
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, PolyhedronVertex >{},
                []( Archive& archive, PolyhedronVertex& polyhedron_vertex ) {
                    archive.value4b( polyhedron_vertex.polyhedron_id );
                    archive.value4b( polyhedron_vertex.vertex_id );
                } );
        }
        index_t polyhedron_id{ NO_ID };
        index_t vertex_id{ NO_ID };
    };

    /*!
     * This struct represents a facet in a polyhedron
     */
    struct opengeode_mesh_api PolyhedronFacet
    {
        PolyhedronFacet() = default;
        PolyhedronFacet( index_t polyhedron_id, index_t facet_id )
            : polyhedron_id( polyhedron_id ), facet_id( facet_id )
        {
        }
        bool operator==( const PolyhedronFacet& other ) const
        {
            return polyhedron_id == other.polyhedron_id
                   && facet_id == other.facet_id;
        }
        bool operator!=( const PolyhedronFacet& other ) const
        {
            return !( *this == other );
        }
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, PolyhedronFacet >{},
                []( Archive& archive, PolyhedronFacet& polyhedron_facet ) {
                    archive.value4b( polyhedron_facet.polyhedron_id );
                    archive.value4b( polyhedron_facet.facet_id );
                } );
        }
        index_t polyhedron_id{ NO_ID };
        index_t facet_id{ NO_ID };
    };

    struct opengeode_mesh_api PolyhedronFacetVertex
    {
        PolyhedronFacetVertex() = default;
        PolyhedronFacetVertex( PolyhedronFacet facet, index_t vertex_id )
            : polyhedron_facet( std::move( facet ) ), vertex_id( vertex_id )
        {
        }
        bool operator==( const PolyhedronFacetVertex& other ) const
        {
            return polyhedron_facet == other.polyhedron_facet
                   && vertex_id == other.vertex_id;
        }
        bool operator!=( const PolyhedronFacetVertex& other ) const
        {
            return !( *this == other );
        }
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                DefaultGrowable< Archive, PolyhedronFacetVertex >{},
                []( Archive& archive,
                    PolyhedronFacetVertex& polyhedron_facet_vertex ) {
                    archive.object( polyhedron_facet_vertex.polyhedron_facet );
                    archive.value4b( polyhedron_facet_vertex.vertex_id );
                } );
        }
        PolyhedronFacet polyhedron_facet;
        index_t vertex_id{ NO_ID };
    };

    struct opengeode_mesh_api PolyhedronFacetEdge
    {
        PolyhedronFacetEdge() = default;
        PolyhedronFacetEdge( PolyhedronFacet facet, index_t edge_id )
            : polyhedron_facet( std::move( facet ) ), edge_id( edge_id )
        {
        }
        bool operator==( const PolyhedronFacetEdge& other ) const
        {
            return polyhedron_facet == other.polyhedron_facet
                   && edge_id == other.edge_id;
        }
        bool operator!=( const PolyhedronFacetEdge& other ) const
        {
            return !( *this == other );
        }
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                DefaultGrowable< Archive, PolyhedronFacetEdge >{},
                []( Archive& archive,
                    PolyhedronFacetEdge& polyhedron_facet_edge ) {
                    archive.object( polyhedron_facet_edge.polyhedron_facet );
                    archive.value4b( polyhedron_facet_edge.edge_id );
                } );
        }
        PolyhedronFacet polyhedron_facet;
        index_t edge_id{ NO_ID };
    };

    using PolyhedronFacetVertices = absl::InlinedVector< index_t, 4 >;

    using PolyhedronFacetsOnBorder = absl::InlinedVector< PolyhedronFacet, 4 >;

    using PolyhedraAroundVertex = absl::InlinedVector< PolyhedronVertex, 20 >;

    using PolyhedraAroundEdge = absl::InlinedVector< index_t, 10 >;

    using PolyhedraAroundFacet = absl::InlinedVector< index_t, 2 >;

    /*!
     * This class represents a 3D Solid made up with polyhedra and provides mesh
     * functionnalities.
     */
    template < index_t dimension >
    class PolyhedralSolid : public VertexSet
    {
        OPENGEODE_DISABLE_COPY( PolyhedralSolid );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );
        PASSKEY( PolyhedralSolidBuilder< dimension >, PolyhedralSolidKey );

    public:
        /*!
         * Create a new PolyhedralSolid using default data structure.
         */
        static std::unique_ptr< PolyhedralSolid< dimension > > create();

        /*!
         * Create a new PolyhedralSolid using a specified data structure.
         * @param[in] type Data structure type.
         */
        static std::unique_ptr< PolyhedralSolid< dimension > > create(
            const MeshType& type );

        std::unique_ptr< PolyhedralSolid< dimension > > clone() const;

        ~PolyhedralSolid();

        const Point< dimension >& point( index_t vertex_id ) const;

        index_t nb_polyhedra() const;

        index_t nb_facets() const;

        index_t nb_edges() const;

        bool isolated_vertex( index_t vertex_id ) const;

        bool isolated_edge( index_t edge_id ) const;

        bool isolated_facet( index_t facet_id ) const;

        /*!
         * Return the number of vertices in a polyhedron.
         */
        index_t nb_polyhedron_vertices( index_t polyhedron_id ) const;

        /*!
         * Return the number of facets in a polyhedron.
         */
        index_t nb_polyhedron_facets( index_t polyhedron_id ) const;

        /*!
         * Return the number of vertices in polyhedron facet.
         * @param[in] polyhedron_facet Local index of the facet in polyhedron.
         */
        index_t nb_polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const;

        /*!
         * Return the index in the mesh of a local vertex in a polyhedron.
         * @param[in] polyhedron_vertex Local index of vertex in polyhedron.
         */
        index_t polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const;

        /*!
         * Return the local index in the tetrahedron of a vertex in the mesh.
         * @param[in] polyhedron_id Index of polyhedron.
         * @param[in] vertex_id Index of a vertex in the mesh.
         * @return Index in [0,nb_polyhedron_vertices()[ if polyhedron is around
         * the given vertex, else NO_ID
         */
        index_t vertex_in_polyhedron(
            index_t polyhedron_id, index_t vertex_id ) const;

        /*!
         * Return the index in the mesh of a local facet in a polyhedron.
         * @param[in] polyhedron_facet Local index of facet in polyhedron.
         */
        index_t polyhedron_facet(
            const PolyhedronFacet& polyhedron_facet ) const;

        /*!
         * Return the index in the mesh of a given polyhedron facet vertex.
         * @param[in] polyhedron_facet_vertex Local index of the vertex in the
         * facet of a polyhedron.
         */
        index_t polyhedron_facet_vertex(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const;

        /*!
         * Return the index in the mesh of a given polyhedron facet edge.
         * @param[in] polyhedron_facet_edge Local index of the edge in the
         * facet of a polyhedron.
         */
        index_t polyhedron_facet_edge(
            const PolyhedronFacetEdge& polyhedron_facet_edge ) const;

        /*!
         * Return the indices of facet vertices.
         * @param[in] edge_id Index of an edge.
         */
        const PolyhedronFacetVertices& facet_vertices( index_t facet_id ) const;

        /*!
         * Return the indices of edge vertices.
         * @param[in] edge_id Index of an edge.
         */
        const std::array< index_t, 2 >& edge_vertices( index_t edge_id ) const;

        /*!
         * Get the index of facet corresponding to given vertices
         * @param[in] vertices Ordered vertex indices
         */
        index_t facet_from_vertices(
            const PolyhedronFacetVertices& vertices ) const;

        /*!
         * Get the index of edge corresponding to given vertices
         * @param[in] vertices Ordered vertex indices
         */
        index_t edge_from_vertices(
            const std::array< index_t, 2 >& vertices ) const;

        /*!
         * Return the index of the polyhedron adjacent through a facet.
         * @param[in] polyhedron_facet Local index of facet in polyhedron.
         * @return NO_ID if the polyhedron facet is on border, else the index of
         * the adjacent polyhedron.
         */
        index_t polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const;

        /*!
         * Return the index of the facet of the adjacent polyhedron through
         * which polyhedra are adjacent.
         * @param[in] polyhedron_facet Local index of facet in polyhedron.
         * @return Undefined PolyhedronFacet if the polyhedron facet is on
         * border, else the index of the adjacent polyhedron facet.
         */
        PolyhedronFacet polyhedron_adjacent_facet(
            const PolyhedronFacet& polyhedron_facet ) const;

        /*!
         * Return true if the facet is on border, i.e. if the polyhedron has no
         * adjacent through the specified facet.
         * @param[in] polyhedron_facet Local index of facet in polyhedron.
         */
        bool is_polyhedron_facet_on_border(
            const PolyhedronFacet& polyhedron_facet ) const;

        /*!
         * Return all the facets of a polyhedron that are on border
         * @param[in] polyhedron_id Index of a polyhedron
         */
        PolyhedronFacetsOnBorder polyhedron_facets_on_border(
            index_t polyhedron_id ) const;

        /*!
         * Return the volume of a polyhedron.
         * @param[in] polyhedron_id Index of a polyhedron.
         * @warning Not implemented yet.
         */
        double polyhedron_volume( index_t polyhedron_id ) const;

        /*!
         * Return the area of a facet.
         * @param[in] facet_id Index of facet.
         * @warning Not implemented yet.
         */
        double facet_area( index_t facet_id ) const;

        /*!
         * Return the length of a given edge.
         * @param[in] edge_id Index of edge.
         */
        double edge_length( index_t edge_id ) const;

        /*!
         * Return the barycenter of a polyhedron
         * @param[in] polyhedron_id Index of a polyhedron
         */
        Point< dimension > polyhedron_barycenter( index_t polyhedron_id ) const;

        /*!
         * Return the barycenter coordinates of a given facet.
         * @param[in] facet_id Index of facet.
         */
        Point< dimension > facet_barycenter( index_t facet_id ) const;

        /*!
         * Return the coordinates of the barycenter of a given edge.
         * @param[in] edge_id Index of edge.
         */
        Point< dimension > edge_barycenter( index_t edge_id ) const;

        /*!
         * Get all the polyhedra with one of the vertices matching given vertex.
         * @param[in] vertex_id Index of the vertex
         */
        PolyhedraAroundVertex polyhedra_around_vertex(
            index_t vertex_id ) const;

        /*!
         * Get all the polyhedra with both edge vertices.
         * @param[in] edge_id Index of the edge
         */
        PolyhedraAroundEdge polyhedra_around_edge( index_t edge_id ) const;

        /*!
         * Return all polyhedra made with the given facet.
         * @param[in] facet_id Index of the facet
         */
        PolyhedraAroundFacet polyhedra_from_facet( index_t facet_id ) const;

        /*!
         * Access to the manager of attributes associated with polyhedra.
         */
        AttributeManager& polyhedron_attribute_manager() const;

        /*!
         * Access to the manager of attributes associated with facets.
         */
        AttributeManager& facet_attribute_manager() const;

        /*!
         * Access to the manager of attributes associated with edges.
         */
        AttributeManager& edge_attribute_manager() const;

        /*!
         * Compute the bounding box from mesh vertices
         */
        BoundingBox< dimension > bounding_box() const;
        const PolyhedronVertex& polyhedron_around_vertex(
            index_t vertex_id ) const;

        void associate_polyhedron_vertex_to_vertex(
            const PolyhedronVertex& polyhedron_vertex,
            index_t vertex_id,
            PolyhedralSolidKey );

        void update_facet_vertices(
            absl::Span< const index_t > old2new, PolyhedralSolidKey );

        void update_edge_vertices(
            absl::Span< const index_t > old2new, PolyhedralSolidKey );

        void update_facet_vertex( PolyhedronFacetVertices facet_vertices,
            index_t facet_vertex_id,
            index_t new_vertex_id,
            PolyhedralSolidKey );

        void update_edge_vertex( std::array< index_t, 2 > edge_vertices,
            index_t edge_vertex_id,
            index_t new_vertex_id,
            PolyhedralSolidKey );

        void remove_facet(
            PolyhedronFacetVertices facet_vertices, PolyhedralSolidKey );

        void remove_edge(
            std::array< index_t, 2 > edge_vertices, PolyhedralSolidKey );

        std::vector< index_t > delete_facets(
            const std::vector< bool >& to_delete, PolyhedralSolidKey );

        std::vector< index_t > delete_edges(
            const std::vector< bool >& to_delete, PolyhedralSolidKey );

        std::vector< index_t > remove_isolated_facets( PolyhedralSolidKey );

        std::vector< index_t > remove_isolated_edges( PolyhedralSolidKey );

        index_t find_or_create_facet(
            PolyhedronFacetVertices facet_vertices, PolyhedralSolidKey )
        {
            return find_or_create_facet( facet_vertices );
        }

        index_t find_or_create_edge(
            std::array< index_t, 2 > edge_vertices, PolyhedralSolidKey )
        {
            return find_or_create_edge( edge_vertices );
        }

        PolyhedronVertex polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex,
            PolyhedralSolidKey ) const
        {
            return get_polyhedron_facet_vertex_id( polyhedron_facet_vertex );
        }

    protected:
        PolyhedralSolid();

        index_t find_or_create_facet( PolyhedronFacetVertices facet_vertices );

        index_t find_or_create_edge( std::array< index_t, 2 > edge_vertices );

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        virtual const Point< dimension >& get_point(
            index_t vertex_id ) const = 0;

        virtual index_t get_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const = 0;

        virtual index_t get_nb_polyhedron_vertices(
            index_t polyhedron_id ) const = 0;

        virtual index_t get_nb_polyhedron_facets(
            index_t polyhedron_id ) const = 0;

        virtual index_t get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const = 0;

        virtual index_t get_polyhedron_facet_vertex(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const = 0;

        virtual PolyhedronVertex get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const = 0;

        virtual index_t get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const = 0;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( PolyhedralSolid );

    template < index_t dimension >
    using PolyhedralSolidFactory =
        Factory< MeshType, PolyhedralSolid< dimension > >;
    ALIAS_3D( PolyhedralSolidFactory );
} // namespace geode
