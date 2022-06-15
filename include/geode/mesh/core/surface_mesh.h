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

#include <absl/container/inlined_vector.h>
#include <absl/types/optional.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/passkey.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceEdges );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMeshBuilder );

    ALIAS_2D_AND_3D( Vector );

    class AttributeManager;

    struct PolygonEdge;
} // namespace geode

namespace geode
{
    /*!
     * This struct represents a local vertex in a polygon
     */
    struct opengeode_mesh_api PolygonVertex
    {
        PolygonVertex() = default;
        PolygonVertex( index_t polygon_id_in, local_index_t vertex_id_in )
            : polygon_id( polygon_id_in ), vertex_id( vertex_id_in )
        {
        }
        PolygonVertex( PolygonEdge polygon_edge );
        bool operator==( const PolygonVertex& other ) const
        {
            return polygon_id == other.polygon_id
                   && vertex_id == other.vertex_id;
        }
        bool operator!=( const PolygonVertex& other ) const
        {
            return !( *this == other );
        }
        std::string string() const
        {
            return absl::StrCat( "(", polygon_id, ", ", vertex_id, ")" );
        }
        template < typename Archive >
        void serialize( Archive& archive );

        index_t polygon_id{ NO_ID };
        local_index_t vertex_id{ NO_LID };
    };

    /*!
     * This struct represents an edge in a polygon
     */
    struct opengeode_mesh_api PolygonEdge
    {
        PolygonEdge() = default;
        PolygonEdge( index_t polygon_id_in, local_index_t edge_id_in )
            : polygon_id( polygon_id_in ), edge_id( edge_id_in )
        {
        }
        PolygonEdge( PolygonVertex polygon_vertex );
        bool operator==( const PolygonEdge& other ) const
        {
            return polygon_id == other.polygon_id && edge_id == other.edge_id;
        }
        bool operator!=( const PolygonEdge& other ) const
        {
            return !( *this == other );
        }
        std::string string() const
        {
            return absl::StrCat( "(", polygon_id, ", ", edge_id, ")" );
        }
        template < typename Archive >
        void serialize( Archive& archive );

        index_t polygon_id{ NO_ID };
        local_index_t edge_id{ NO_LID };
    };

    using PolygonVertices = absl::InlinedVector< index_t, 3 >;

    using PolygonEdgesOnBorder = absl::InlinedVector< PolygonEdge, 3 >;

    using PolygonsAroundVertex = absl::InlinedVector< PolygonVertex, 10 >;

    using PolygonsAroundEdge = absl::InlinedVector< PolygonEdge, 2 >;

    /*!
     * This class represents a Surface made up with polygons (triangles, quads,
     * ...) of arbitrary dimension and provides mesh functionnalities.
     */
    template < index_t dimension >
    class opengeode_mesh_api SurfaceMesh : public VertexSet
    {
        OPENGEODE_DISABLE_COPY( SurfaceMesh );
        PASSKEY( SurfaceMeshBuilder< dimension >, SurfaceMeshKey );

    public:
        using Builder = SurfaceMeshBuilder< dimension >;

        ~SurfaceMesh();

        /*!
         * Create a new SurfaceMesh using default data structure.
         */
        static std::unique_ptr< SurfaceMesh< dimension > > create();

        /*!
         * Create a new SurfaceMesh using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        static std::unique_ptr< SurfaceMesh< dimension > > create(
            const MeshImpl& impl );

        std::unique_ptr< SurfaceMesh< dimension > > clone() const;

        const Point< dimension >& point( index_t vertex_id ) const;

        index_t nb_polygons() const;

        bool is_vertex_isolated( index_t vertex_id ) const;

        /*!
         * Return the number of vertices in a polygon
         */
        local_index_t nb_polygon_vertices( index_t polygon_id ) const;

        /*!
         * Return the number of edges in a polygon
         */
        local_index_t nb_polygon_edges( index_t polygon_id ) const;

        /*!
         * Return the index in the mesh of a local vertex in a polygon
         * @param[in] polygon_vertex Local index of vertex in polygon
         */
        index_t polygon_vertex( const PolygonVertex& polygon_vertex ) const;

        /*!
         * Return all the indices in the mesh of polygon vertices.
         * @param[in] polygon_id Index of polygon.
         */
        PolygonVertices polygon_vertices( index_t polygon_id ) const;

        /*!
         * Return the index in the mesh of a given polygon edge vertex.
         * @param[in] polygon_edge Local index of edge in a polygon.
         * @param[in] vertex_id Local index of vertex in the edge (0 or 1).
         */
        index_t polygon_edge_vertex(
            const PolygonEdge& polygon_edge, local_index_t vertex_id ) const;

        /*!
         * Return the indices in the mesh of the two polygon edge vertices.
         * @param[in] polygon_edge Local index of edge in a polygon.
         */
        std::array< index_t, 2 > polygon_edge_vertices(
            const PolygonEdge& polygon_edge ) const;

        /*!
         * Return the next vertex in a polygon (local indexation)
         * @param[in] polygon_vertex Local index of vertex in polygon
         */
        PolygonVertex next_polygon_vertex(
            const PolygonVertex& polygon_vertex ) const;

        /*!
         * Return the previous vertex in a polygon (local indexation)
         * @param[in] polygon_vertex Local index of vertex in polygon
         */
        PolygonVertex previous_polygon_vertex(
            const PolygonVertex& polygon_vertex ) const;

        /*!
         * Return the next edge in a polygon (local indexation)
         * @param[in] polygon_edge Local index of edge in polygon
         */
        PolygonEdge next_polygon_edge( const PolygonEdge& polygon_edge ) const;

        /*!
         * Return the previous edge in a polygon (local indexation)
         * @param[in] polygon_edge Local index of edge in polygon
         */
        PolygonEdge previous_polygon_edge(
            const PolygonEdge& polygon_edge ) const;

        /*!
         * Return the index of the polygon adjacent through an edge.
         * @param[in] polygon_edge Local index of edge in polygon.
         * @return the index of the adjacent polygon if it exists.
         */
        absl::optional< index_t > polygon_adjacent(
            const PolygonEdge& polygon_edge ) const;

        /*!
         * Return the index of the edge of the adjacent polygon through which
         * polygons are adjacent.
         * @param[in] polygon_edge Local index of edge in polygon.
         * @return the index of the adjacent polygon edge if it exists.
         */
        absl::optional< PolygonEdge > polygon_adjacent_edge(
            const PolygonEdge& polygon_edge ) const;

        /*!
         * Return true if the vertex is on border, i.e. if at there are polygons
         * around this vertex on border.
         * @param[in] vertex_id Index the vertex.
         */
        bool is_vertex_on_border( index_t vertex_id ) const;

        /*!
         * Return true if the edge is on border, i.e. if the polygon has no
         * adjacent through the specified edge.
         * @param[in] polygon_edge Local index of edge in polygon.
         */
        bool is_edge_on_border( const PolygonEdge& polygon_edge ) const;

        /*!
         * Return true if at least one of the polygon edges is on border
         * @param[in] polygon_id Index of a polygon
         */
        bool is_polygon_on_border( index_t polygon_id ) const;

        /*!
         * Return all the edges of a polygon that are on border
         * @param[in] polygon_id Index of a polygon
         */
        PolygonEdgesOnBorder polygon_edges_on_border(
            index_t polygon_id ) const;

        /*!
         * Return the next edge on the border (local indexation).
         * @param[in] polygon_edge Local index of edge in a polygon.
         * @pre The given polygon edge should be on border.
         */
        PolygonEdge next_on_border( const PolygonEdge& polygon_edge ) const;

        /*!
         * Return the previous edge on the border (local indexation).
         * @param[in] polygon_edge Local index of edge in a polygon.
         * @pre The given polygon edge should be on border.
         */
        PolygonEdge previous_on_border( const PolygonEdge& polygon_edge ) const;

        /*!
         * Return the length of a given edge.
         * @param[in] polygon_edge Local index of edge in a polygon.
         */
        double edge_length( const PolygonEdge& polygon_edge ) const;
        double edge_length(
            const std::array< index_t, 2 >& polygon_edge_vertices ) const;

        /*!
         * Return the coordinates of the barycenter of a given edge.
         * @param[in] polygon_edge Local index of edge in a polygon.
         */
        Point< dimension > edge_barycenter(
            const PolygonEdge& polygon_edge ) const;
        Point< dimension > edge_barycenter(
            const std::array< index_t, 2 >& polygon_edge_vertices ) const;

        /*!
         * Return the barycenter of a polygon
         * @param[in] polygon_id Index of a polygon
         */
        Point< dimension > polygon_barycenter( index_t polygon_id ) const;

        /*!
         * Return the area of a polygon.
         * @param[in] polygon_id Index of a polygon.
         * @warning Result guaranteed only for convex polygon.
         */
        double polygon_area( index_t polygon_id ) const;

        /*!
         * Return the normal of a polygon
         */
        template < index_t T = dimension >
        typename std::enable_if< T == 3, Vector3D >::type polygon_normal(
            index_t polygon_id ) const;

        /*!
         * Return the normal at a polygon vertex
         */
        template < index_t T = dimension >
        typename std::enable_if< T == 3, Vector3D >::type polygon_vertex_normal(
            index_t vertex_id ) const;

        /*!
         * Return the normal of a polygon
         */
        template < index_t T = dimension >
        typename std::enable_if< T == 3, absl::optional< Vector3D > >::type
            new_polygon_normal( index_t polygon_id ) const;

        /*!
         * Return the normal at a polygon vertex
         */
        template < index_t T = dimension >
        typename std::enable_if< T == 3, absl::optional< Vector3D > >::type
            new_polygon_vertex_normal( index_t vertex_id ) const;

        /*!
         * Get all the polygons with one of the vertices matching given vertex.
         * @param[in] vertex_id Index of the vertex.
         * @pre This function needs that polygon adjacencies are computed
         */
        PolygonsAroundVertex polygons_around_vertex( index_t vertex_id ) const;

        /*!
         * Get all the polygons with one of the vertices matching given vertex.
         * @param[in] polygon_vertex Local index of vertex in polygon.
         * @pre This function needs that polygon adjacencies are computed
         */
        PolygonsAroundVertex polygons_around_vertex(
            const PolygonVertex& vertex ) const;

        /*!
         * Find the polygon edge corresponding to an ordered pair of vertex
         * indices.
         * @param[in] from_vertex_id Index of the vertex from which starts the
         * edge
         * @param[in] to_vertex_id Index of the vertex to which ends the edge
         * @return Local index if the edge is found.
         */
        absl::optional< PolygonEdge > polygon_edge_from_vertices(
            index_t from_vertex_id, index_t to_vertex_id ) const;

        /*!
         * Find the polygon edges corresponding to a pair of vertex indices.
         * @return Local indices of the edges found
         */
        PolygonsAroundEdge polygons_from_edge_vertices(
            std::array< index_t, 2 > edge_vertices ) const;

        bool are_edges_enabled() const;

        void enable_edges() const;

        void disable_edges() const;

        const SurfaceEdges< dimension >& edges() const;

        /*!
         * Access to the manager of attributes associated with polygons.
         */
        AttributeManager& polygon_attribute_manager() const;

        /*!
         * Compute the bounding box from mesh vertices
         */
        BoundingBox< dimension > bounding_box() const;

        /*!
         * Return one polygon with one of the vertices matching given vertex.
         * @param[in] vertex_id Index of the vertex.
         */
        absl::optional< PolygonVertex > polygon_around_vertex(
            index_t vertex_id ) const;

        void associate_polygon_vertex_to_vertex(
            const PolygonVertex& polygon_vertex,
            index_t vertex_id,
            SurfaceMeshKey );

        SurfaceEdges< dimension >& edges( SurfaceMeshKey );

        void copy_edges(
            const SurfaceMesh< dimension >& surface_mesh, SurfaceMeshKey );

    protected:
        SurfaceMesh();

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        virtual const Point< dimension >& get_point(
            index_t vertex_id ) const = 0;

        virtual index_t get_polygon_vertex(
            const PolygonVertex& polygon_vertex ) const = 0;

        virtual local_index_t get_nb_polygon_vertices(
            index_t polygon_id ) const = 0;

        virtual absl::optional< index_t > get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const = 0;

        virtual absl::optional< PolygonVertex > get_polygon_around_vertex(
            index_t vertex_id ) const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( SurfaceMesh );
} // namespace geode

namespace std
{
    template <>
    struct hash< geode::PolygonVertex >
    {
    public:
        size_t operator()( const geode::PolygonVertex& polygon_vertex ) const
        {
            return absl::Hash< geode::index_t >()( polygon_vertex.polygon_id )
                   ^ absl::Hash< geode::index_t >()( polygon_vertex.vertex_id );
        }
    };

    template <>
    struct hash< geode::PolygonEdge >
    {
    public:
        size_t operator()( const geode::PolygonEdge& polygon_edge ) const
        {
            return absl::Hash< geode::index_t >()( polygon_edge.polygon_id )
                   ^ absl::Hash< geode::index_t >()( polygon_edge.edge_id );
        }
    };
} // namespace std