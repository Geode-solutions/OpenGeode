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

#include <absl/container/inlined_vector.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/passkey.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidEdges );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidFacets );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMeshBuilder );

    ALIAS_2D_AND_3D( Vector );

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
        PolyhedronVertex( index_t polyhedron_id_in, local_index_t vertex_id_in )
            : polyhedron_id( polyhedron_id_in ), vertex_id( vertex_id_in )
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
        std::string string() const
        {
            return absl::StrCat( "(", polyhedron_id, ", ", vertex_id, ")" );
        }
        template < typename Archive >
        void serialize( Archive& archive );

        index_t polyhedron_id{ NO_ID };
        local_index_t vertex_id{ NO_LID };
    };

    /*!
     * This struct represents a facet in a polyhedron
     */
    struct opengeode_mesh_api PolyhedronFacet
    {
        PolyhedronFacet() = default;
        PolyhedronFacet( index_t polyhedron_id_in, local_index_t facet_id_in )
            : polyhedron_id( polyhedron_id_in ), facet_id( facet_id_in )
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
        std::string string() const
        {
            return absl::StrCat( "(", polyhedron_id, ", ", facet_id, ")" );
        }
        template < typename Archive >
        void serialize( Archive& archive );

        index_t polyhedron_id{ NO_ID };
        local_index_t facet_id{ NO_LID };
    };

    struct opengeode_mesh_api PolyhedronFacetVertex
    {
        PolyhedronFacetVertex() = default;
        PolyhedronFacetVertex(
            PolyhedronFacet facet, local_index_t vertex_id_in )
            : polyhedron_facet( std::move( facet ) ), vertex_id( vertex_id_in )
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
        std::string string() const
        {
            return absl::StrCat( "(", polyhedron_facet.polyhedron_id, ", ",
                polyhedron_facet.facet_id, ", ", vertex_id, ")" );
        }
        template < typename Archive >
        void serialize( Archive& archive );

        PolyhedronFacet polyhedron_facet;
        local_index_t vertex_id{ NO_LID };
    };

    struct opengeode_mesh_api PolyhedronFacetEdge
    {
        PolyhedronFacetEdge() = default;
        PolyhedronFacetEdge( PolyhedronFacet facet, local_index_t edge_id_in )
            : polyhedron_facet( std::move( facet ) ), edge_id( edge_id_in )
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
        std::string string() const
        {
            return absl::StrCat( "(", polyhedron_facet.polyhedron_id, ", ",
                polyhedron_facet.facet_id, ", ", edge_id, ")" );
        }
        template < typename Archive >
        void serialize( Archive& archive );

        PolyhedronFacet polyhedron_facet;
        local_index_t edge_id{ NO_LID };
    };

    using PolyhedronFacetVertices = absl::InlinedVector< index_t, 4 >;

    using PolyhedronVertices = absl::InlinedVector< index_t, 4 >;

    using PolyhedronFacets = absl::InlinedVector< PolyhedronFacet, 4 >;

    using PolyhedronFacetsOnBorder = PolyhedronFacets;

    using PolyhedraAroundVertex = absl::InlinedVector< PolyhedronVertex, 20 >;

    using PolyhedraAroundEdge = absl::InlinedVector< index_t, 10 >;

    using PolyhedraAroundFacet =
        absl::InlinedVector< index_t, 2 >; /* deprecated */

    using newPolyhedraAroundFacet = absl::InlinedVector< PolyhedronFacet, 2 >;

    /*!
     * This class represents a 3D Solid made up with polyhedra and provides mesh
     * functionnalities.
     */
    template < index_t dimension >
    class SolidMesh : public VertexSet
    {
        OPENGEODE_DISABLE_COPY( SolidMesh );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );
        PASSKEY( SolidMeshBuilder< dimension >, SolidMeshKey );

    public:
        using Builder = SolidMeshBuilder< dimension >;

        /*!
         * Create a new SolidMesh using default data structure.
         */
        static std::unique_ptr< SolidMesh< dimension > > create();

        /*!
         * Create a new SolidMesh using a specified data structure.
         * @param[in] impl Data structure implementation.
         */
        static std::unique_ptr< SolidMesh< dimension > > create(
            const MeshImpl& impl );

        std::unique_ptr< SolidMesh< dimension > > clone() const;

        ~SolidMesh();

        const Point< dimension >& point( index_t vertex_id ) const;

        index_t nb_polyhedra() const;

        bool is_vertex_isolated( index_t vertex_id ) const;

        /*!
         * Return the number of vertices in a polyhedron.
         */
        local_index_t nb_polyhedron_vertices( index_t polyhedron_id ) const;

        /*!
         * Return the number of facets in a polyhedron.
         */
        local_index_t nb_polyhedron_facets( index_t polyhedron_id ) const;

        /*!
         * Return the number of vertices in polyhedron facet.
         * @param[in] polyhedron_facet Local index of the facet in polyhedron.
         */
        local_index_t nb_polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const;

        /*!
         * Return the index in the mesh of a local vertex in a polyhedron.
         * @param[in] polyhedron_vertex Local index of vertex in polyhedron.
         */
        index_t polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const;

        /*!
         * Return all the indices in the mesh of polyhedron vertices.
         * @param[in] polyhedron_id Index of polyhedron.
         */
        PolyhedronVertices polyhedron_vertices( index_t polyhedron_id ) const;

        /*!
         * Return the local index in the tetrahedron of a vertex in the mesh.
         * @param[in] polyhedron_id Index of polyhedron.
         * @param[in] vertex_id Index of a vertex in the mesh.
         * @return Index in [0,nb_polyhedron_vertices()[ if polyhedron is around
         * the given vertex
         */
        absl::optional< local_index_t > vertex_in_polyhedron(
            index_t polyhedron_id, index_t vertex_id ) const;

        /*!
         * Return the index in the mesh of a given polyhedron facet vertex.
         * @param[in] polyhedron_facet_vertex Local index of the vertex in the
         * facet of a polyhedron.
         */
        index_t polyhedron_facet_vertex(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const;

        /*!
         * Return the indices in the mesh of the two polyhedron edge vertices.
         * @param[in] polyhedron_facet_edge Local index of edge in a polyhedron.
         */
        std::array< index_t, 2 > polyhedron_facet_edge_vertices(
            const PolyhedronFacetEdge& polyhedron_facet_edge ) const;

        absl::optional< PolyhedronFacetEdge >
            polyhedron_facet_edge_from_vertices(
                const std::array< index_t, 2 >& edge_vertices ) const;

        absl::optional< PolyhedronFacetEdge >
            polyhedron_facet_edge_from_vertices(
                const std::array< index_t, 2 >& edge_vertices,
                index_t polyhedron_id ) const;

        virtual std::vector< std::array< index_t, 2 > >
            polyhedron_edges_vertices( index_t polyhedron ) const;

        absl::optional< PolyhedronFacet > polyhedron_facet_from_vertices(
            PolyhedronFacetVertices polyhedron_facet_vertices ) const;

        PolyhedronFacetVertices polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const;

        virtual std::vector< PolyhedronFacetVertices >
            polyhedron_facets_vertices( index_t polyhedron ) const;

        virtual PolyhedronFacets polyhedron_vertex_facets(
            const PolyhedronVertex& polyhedron_vertex ) const;

        /*!
         * Return the index of the polyhedron adjacent through a facet.
         * @param[in] polyhedron_facet Local index of facet in polyhedron.
         * @return the index of the adjacent polyhedron if it exists.
         */
        absl::optional< index_t > polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const;

        /*!
         * Return the index of the facet of the adjacent polyhedron through
         * which polyhedra are adjacent.
         * @param[in] polyhedron_facet Local index of facet in polyhedron.
         * @return the index of the adjacent polyhedron facet if it exists.
         */
        absl::optional< PolyhedronFacet > polyhedron_adjacent_facet(
            const PolyhedronFacet& polyhedron_facet ) const;

        /*!
         * Return true if the facet is on border, i.e. if the polyhedron has no
         * adjacent through the specified facet.
         * @param[in] polyhedron_facet Local index of facet in polyhedron.
         */
        bool is_polyhedron_facet_on_border(
            const PolyhedronFacet& polyhedron_facet ) const;
        /*!
         * Return true if at least one polyhedron facet is on border

         * @param[in] polyhedron_id Index of a polyhedron
         */
        bool is_polyhedron_on_border( index_t polyhedron_id ) const;

        /*!
         * Return true if the edge belongs to the facet
         * @param[in] polyhedron_facet Local index of facet in polyhedron.
         * @param[in] edge_vertices Indices of edge vertices.
         */
        bool is_edge_in_polyhedron_facet(
            const PolyhedronFacet& polyhedron_facet,
            const std::array< index_t, 2 >& edge_vertices ) const;

        /*!
         * Return all the facets of a polyhedron that are on border
         * @param[in] polyhedron_id Index of a polyhedron
         */
        PolyhedronFacetsOnBorder polyhedron_facets_on_border(
            index_t polyhedron_id ) const;

        /*!
         * Return the length of a given edge.
         * @param[in] edge_vertices Indices of edge vertices.
         */
        double edge_length(
            const std::array< index_t, 2 >& edge_vertices ) const;

        /*!
         * Return the barycenter of a polyhedron
         * @param[in] polyhedron_id Index of a polyhedron
         */
        Point< dimension > polyhedron_barycenter( index_t polyhedron_id ) const;

        /*!
         * Return the barycenter coordinates of a given facet.
         * @param[in] facet_vertices Vertex indices of the facet.
         */
        Point< dimension > facet_barycenter(
            const PolyhedronFacetVertices& facet_vertices ) const;

        /*!
         * Return the coordinates of the barycenter of a given edge.
         * @param[in] edge_vertices Indices of edge vertices.
         */
        Point< dimension > edge_barycenter(
            const std::array< index_t, 2 >& edge_vertices ) const;

        /*!
         * Return the normal of a given PolyhedronFacet.
         * @param[in] polyhedron_facet Local index of facet in polyhedron.
         */
        template < index_t T = dimension >
        typename std::enable_if< T == 3, Vector3D >::type
            polyhedron_facet_normal(
                const PolyhedronFacet& polyhedron_facet ) const;

        /*!
         * Get all the polyhedra with one of the vertices matching given vertex.
         * @param[in] vertex_id Index of the vertex.
         * @pre This function needs that polyhedron adjacencies are computed
         */
        PolyhedraAroundVertex polyhedra_around_vertex(
            index_t vertex_id ) const;

        /*!
         * Get all the polyhedra with one of the vertices matching given
         * polyhedron vertex.
         * @param[in] polyhedron_vertex Local index of vertex in polyhedron.
         * @pre This function needs that polyhedron adjacencies are computed
         */
        PolyhedraAroundVertex polyhedra_around_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const;

        /*!
         * Return true if at least one of the polyhedron facets around the
         * vertex is on border.
         * @param[in] vertex_id Index of the vertex.
         * @pre This function needs that polyhedron adjacencies are computed
         */
        bool is_vertex_on_border( index_t vertex_id ) const;

        /*!
         * Return true if at least one of the polyhedron facets around the edge
         * is on border
         * @param[in] vertices Indices of edge vertices.
         */
        bool is_edge_on_border(
            const std::array< index_t, 2 >& vertices ) const;

        /*!
         * Return true if at least one of the polyhedron facets around the edges
         * is on border
         * @param[in] vertices Indices of edge vertices.
         * @param[in] first_polyhedron One polyhedron index to begin research
         */
        bool is_edge_on_border( const std::array< index_t, 2 >& vertices,
            index_t first_polyhedron ) const;

        /*!
         * Get all the polyhedra with both edge vertices.
         * @param[in] vertices Indices of edge vertices.
         * @pre This function needs that polyhedron adjacencies are computed
         */
        virtual PolyhedraAroundEdge polyhedra_around_edge(
            const std::array< index_t, 2 >& vertices ) const;

        /*!
         * Get all the polyhedra around the edge.
         * @param[in] edge Local index of an edge in a polyhedron.
         * @pre This function needs that polyhedron adjacencies are computed
         */
        virtual PolyhedraAroundEdge polyhedra_around_edge(
            const PolyhedronFacetEdge& edge ) const;

        /*!
         * Get all the polyhedra with both edge vertices.
         * @param[in] vertices Indices of edge vertices.
         * @param[in] first_polyhedron One polyhedron index to begin research.
         * @pre This function needs that polyhedron adjacencies are computed
         */
        virtual PolyhedraAroundEdge polyhedra_around_edge(
            const std::array< index_t, 2 >& vertices,
            index_t first_polyhedron ) const;

        /*!
         * Return all polyhedra made with the given facet.
         * @param[in] facet_vertices Vertex indices of the facet.
         */
        PolyhedraAroundFacet OPENGEODE_MESH_DEPRECATED polyhedra_from_facet(
            PolyhedronFacetVertices facet_vertices ) const;

        /*!
         * Return all polyhedra facets made with the given facet vertices.
         * @param[in] facet_vertices Vertex indices of the facet.
         */
        newPolyhedraAroundFacet polyhedra_from_facet_vertices(
            PolyhedronFacetVertices facet_vertices ) const;

        bool are_edges_enabled() const;

        void enable_edges() const;

        void disable_edges() const;

        const SolidEdges< dimension >& edges() const;

        bool are_facets_enabled() const;

        void enable_facets() const;

        void disable_facets() const;

        const SolidFacets< dimension >& facets() const;

        /*!
         * Access to the manager of attributes associated with polyhedra.
         */
        AttributeManager& polyhedron_attribute_manager() const;

        /*!
         * Compute the bounding box from mesh vertices
         */
        BoundingBox< dimension > bounding_box() const;

        /*!
         * Return one polyhedron with one of the vertices matching given vertex.
         * @param[in] vertex_id Index of the vertex.
         */
        absl::optional< PolyhedronVertex > polyhedron_around_vertex(
            index_t vertex_id ) const;

        void associate_polyhedron_vertex_to_vertex(
            const PolyhedronVertex& polyhedron_vertex,
            index_t vertex_id,
            SolidMeshKey );

        PolyhedronVertex polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex,
            SolidMeshKey ) const
        {
            return get_polyhedron_facet_vertex_id( polyhedron_facet_vertex );
        }

        SolidEdges< dimension >& edges( SolidMeshKey );

        void copy_edges( const SolidMesh< dimension >& solid, SolidMeshKey );

        SolidFacets< dimension >& facets( SolidMeshKey );

        void copy_facets( const SolidMesh< dimension >& solid, SolidMeshKey );

    protected:
        SolidMesh();

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        virtual const Point< dimension >& get_point(
            index_t vertex_id ) const = 0;

        virtual index_t get_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex ) const = 0;

        virtual local_index_t get_nb_polyhedron_vertices(
            index_t polyhedron_id ) const = 0;

        virtual local_index_t get_nb_polyhedron_facets(
            index_t polyhedron_id ) const = 0;

        virtual local_index_t get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& polyhedron_facet ) const = 0;

        virtual PolyhedronVertex get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const = 0;

        virtual absl::optional< index_t > get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const = 0;

        virtual absl::optional< PolyhedronVertex > get_polyhedron_around_vertex(
            index_t vertex_id ) const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( SolidMesh );
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::PolyhedronVertex >
    {
    public:
        size_t operator()(
            const geode::PolyhedronVertex& polyhedron_vertex ) const
        {
            return absl::Hash< geode::index_t >()(
                       polyhedron_vertex.polyhedron_id )
                   ^ absl::Hash< geode::index_t >()(
                       polyhedron_vertex.vertex_id );
        }
    };

    template <>
    struct hash< geode::PolyhedronFacet >
    {
    public:
        size_t operator()(
            const geode::PolyhedronFacet& polyhedron_facet ) const
        {
            return absl::Hash< geode::index_t >()(
                       polyhedron_facet.polyhedron_id )
                   ^ absl::Hash< geode::index_t >()(
                       polyhedron_facet.facet_id );
        }
    };

    template <>
    struct hash< geode::PolyhedronFacetVertex >
    {
    public:
        size_t operator()(
            const geode::PolyhedronFacetVertex& polyhedron_facet_vertex ) const
        {
            return absl::Hash< geode::PolyhedronFacet >()(
                       polyhedron_facet_vertex.polyhedron_facet )
                   ^ absl::Hash< geode::index_t >()(
                       polyhedron_facet_vertex.vertex_id );
        }
    };

    template <>
    struct hash< geode::PolyhedronFacetEdge >
    {
    public:
        size_t operator()(
            const geode::PolyhedronFacetEdge& polyhedron_facet_edge ) const
        {
            return absl::Hash< geode::PolyhedronFacet >()(
                       polyhedron_facet_edge.polyhedron_facet )
                   ^ absl::Hash< geode::index_t >()(
                       polyhedron_facet_edge.edge_id );
        }
    };
} // namespace std