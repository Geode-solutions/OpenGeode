/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/attribute.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/factory.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
    FORWARD_DECLARATION_DIMENSION_CLASS( PolygonalSurfaceBuilder );

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
        PolygonVertex( index_t polygon_id, index_t vertex_id )
            : polygon_id( polygon_id ), vertex_id( vertex_id )
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
        template < typename Archive >
        void serialize( Archive& archive )
        {
        archive.ext( *this, geode::Growable< Archive, PolygonVertex >{},
            []( Archive &archive, PolygonVertex &polygon_vertex ) {
            archive.value4b( polygon_vertex.polygon_id );
            archive.value4b( polygon_vertex.vertex_id );
            });
        }
        index_t polygon_id{ NO_ID };
        index_t vertex_id{ NO_ID };
    };

    /*!
     * This struct represents an edge in a polygon
     */
    struct opengeode_mesh_api PolygonEdge
    {
        PolygonEdge() = default;
        PolygonEdge( index_t polygon_id, index_t edge_id )
            : polygon_id( polygon_id ), edge_id( edge_id )
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
        template < typename Archive >
        void serialize( Archive& archive )
        {
        archive.ext( *this, geode::Growable< Archive, PolygonEdge >{},
            []( Archive &archive, PolygonEdge &polygon_edge ) {
            archive.value4b( polygon_edge.polygon_id );
            archive.value4b( polygon_edge.edge_id );
            });
        }
        index_t polygon_id{ NO_ID };
        index_t edge_id{ NO_ID };
    };

    /*!
     * This class represents a Surface made up with polygons (triangles, quads,
     * ...) of arbitrary dimension and provides mesh functionnalities.
     */
    template < index_t dimension >
    class opengeode_mesh_api PolygonalSurfaceBase : public VertexSet
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( PolygonalSurfaceBase );
        friend class PolygonalSurfaceBuilder< dimension >;

    public:
        ~PolygonalSurfaceBase();

        const Point< dimension >& point( index_t vertex_id ) const;

        index_t nb_polygons() const;

        /*!
         * Return the number of vertices in a polygon
         */
        index_t nb_polygon_vertices( index_t polygon_id ) const;

        /*!
         * Return the number of edges in a polygon
         */
        index_t nb_polygon_edges( index_t polygon_id ) const;

        /*!
         * Return the index in the mesh of a local vertex in a polygon
         * @param[in] polygon_vertex Local index of vertex in polygon
         */
        index_t polygon_vertex( const PolygonVertex& polygon_vertex ) const;

        /*!
         * Return the index in the mesh of a given polygon edge vertex.
         * @param[in] polygon_edge Local index of edge in a polygon.
         * @param[in] vertex_id Local index of vertex in the edge (0 or 1).
         */
        index_t polygon_edge_vertex(
            const PolygonEdge& polygon_edge, index_t vertex_id ) const;

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
         * @return NO_ID if the polygon edge is on border, else the index of the
         * adjacent polygon.
         */
        index_t polygon_adjacent( const PolygonEdge& polygon_edge ) const;

        /*!
         * Return the index of the edge of the adjacent polygon through which
         * polygons are adjacent.
         * @param[in] polygon_edge Local index of edge in polygon.
         * @return Undefined PolygonEdge if the polygon edge is on border, else
         * the index of the adjacent polygon edge.
         */
        PolygonEdge polygon_adjacent_edge(
            const PolygonEdge& polygon_edge ) const;

        /*!
         * Return true if the edge is on border, i.e. if the polygon has no
         * adjacent through the specified edge.
         * @param[in] polygon_edge Local index of edge in polygon.
         */
        bool is_edge_on_border( const PolygonEdge& polygon_edge ) const;

        /*!
         * Return all the edges of a polygon that are on border
         * @param[in] polygon_id Index of a polygon
         */
        std::vector< PolygonEdge > polygon_edges_on_border(
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
         * Return the length of a given polygon edge
         * @param[in] polygon_edge Local index of edge in a polygon
         */
        double polygon_edge_length( const PolygonEdge& polygon_edge ) const;

        /*!
         * Return the coordinates of the barycenter of a given polygon edge.
         * @param[in] polygon_edge Local index of edge in a polygon.
         */
        Point< dimension > polygon_edge_barycenter(
            const PolygonEdge& polygon_edge ) const;

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
         * Get all the polygons with one of the vertices matching given vertex.
         * @param[in] vertex_id Index of the vertex.
         * @pre This function needs that polygon adjacencies are computed
         */
        std::vector< PolygonVertex > polygons_around_vertex(
            index_t vertex_id ) const;

        /*!
         * Find the polygon edge corresponding to an ordered pair of vertex
         * indices.
         * @param[in] from_vertex_id Index of the vertex from which starts the
         * edge
         * @param[in] to_vertex_id Index of the vertex to which ends the edge
         * @return 1) true if such an edge exists, 2) Local index of the found
         * edge.
         */
        std::tuple< bool, PolygonEdge > polygon_edge_from_vertices(
            index_t from_vertex_id, index_t to_vertex_id ) const;

        /*!
         * Access to the manager of attributes associated with polygons.
         */
        AttributeManager& polygon_attribute_manager() const;

    protected:
        PolygonalSurfaceBase();

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        virtual const Point< dimension >& get_point(
            index_t vertex_id ) const = 0;

        const PolygonVertex& polygon_around_vertex( index_t vertex_id ) const;

        void associate_polygon_vertex_to_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id );

        virtual index_t get_polygon_vertex(
            const PolygonVertex& polygon_vertex ) const = 0;

        virtual index_t get_nb_polygon_vertices( index_t polygon_id ) const = 0;

        virtual index_t get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const = 0;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( PolygonalSurfaceBase );

    /*!
     * Interface class for Surface made up with polygons (triangles, quads, ...)
     * of arbitrary dimension.
     */
    template < index_t dimension >
    class PolygonalSurface : public PolygonalSurfaceBase< dimension >
    {
    public:
        /*!
         * Create a new PolygonalSurface using default data structure.
         */
        static std::unique_ptr< PolygonalSurface< dimension > > create();

        /*!
         * Create a new PolygonalSurface using a specified data structure.
         * @param[in] type Data structure type
         */
        static std::unique_ptr< PolygonalSurface< dimension > > create(
            const MeshType& type );

    protected:
        PolygonalSurface() = default;
    };

    /*!
     * Interface class for 3D Surface made up with polygons.
     */
    template <>
    class opengeode_mesh_api PolygonalSurface< 3 >
        : public PolygonalSurfaceBase< 3 >
    {
    public:
        static std::unique_ptr< PolygonalSurface< 3 > > create();

        static std::unique_ptr< PolygonalSurface< 3 > > create(
            const MeshType& type );

        /*!
         * Return the normal of a polygon
         */
        Vector3D polygon_normal( index_t polygon_id ) const;

        /*!
         * Return the normal at a polygon vertex
         */
        Vector3D polygon_vertex_normal( index_t vertex_id ) const;

    protected:
        PolygonalSurface() = default;
    };
    ALIAS_2D_AND_3D( PolygonalSurface );

    template < index_t dimension >
    using PolygonalSurfaceFactory =
        Factory< MeshType, PolygonalSurface< dimension > >;
    ALIAS_2D_AND_3D( PolygonalSurfaceFactory );
} // namespace geode
