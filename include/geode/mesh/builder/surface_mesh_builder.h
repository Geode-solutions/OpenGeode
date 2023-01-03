/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/mesh/builder/vertex_set_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceEdgesBuilder );

    struct PolygonEdge;
    struct PolygonVertex;
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a SurfaceMesh
     */
    template < index_t dimension >
    class SurfaceMeshBuilder : public VertexSetBuilder
    {
    public:
        virtual ~SurfaceMeshBuilder();

        /*!
         * Create the builder associated with a SurfaceMesh.
         * @param[in] mesh The SurfaceMesh to build/modify
         */
        static std::unique_ptr< SurfaceMeshBuilder< dimension > > create(
            SurfaceMesh< dimension >& mesh );

        SurfaceEdgesBuilder< dimension > edges_builder();

        /*!
         * Set coordinates to a vertex. This vertex should be created before.
         * @param[in] vertex_id The vertex, in [0, nb_vertices()-1].
         * @param[in] point The vertex coordinates
         */
        void set_point( index_t vertex_id, Point< dimension > point );

        /*!
         * Create a new point with associated coordinates.
         * @param[in] point The point to create
         * @return the index of the created point
         */
        index_t create_point( Point< dimension > point );

        /*!
         * Create a new polygon from vertices.
         * @param[in] vertices The ordered vertices defining the polygon to
         * create
         * @return the index of the created polygon
         */
        index_t create_polygon( absl::Span< const index_t > vertices );

        /*!
         * Modify a polygon vertex.
         * @param[in] polygon_vertex The index of the polygon vertex to modify
         * @param[in] vertex_id Index of the mesh vertex to set as polygon
         * vertex
         */
        void set_polygon_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id );

        /*!
         * Replace old polygon vertices from a given vertex to another.
         * @param[in] old_vertex_id Index of the initial mesh vertex to modify
         * @param[in] new_vertex_id Index of the target mesh vertex to set as
         * polygon vertex
         */
        void replace_vertex( index_t old_vertex_id, index_t new_vertex_id );

        /*!
         * Set a polygon adjacent through an edge.
         * @param[in] polygon_edge The index of the polygon edge
         * @param[in] adjacent_id Index of the adjacent polygon
         */
        void set_polygon_adjacent(
            const PolygonEdge& polygon_edge, index_t adjacent_id );

        /*!
         * Unset a polygon adjacency through an edge.
         * @param[in] polygon_edge The index of the polygon edge
         */
        void unset_polygon_adjacent( const PolygonEdge& polygon_edge );

        /*!
         * Compute all the adjacencies between the surface polygons
         */
        void compute_polygon_adjacencies();

        /*!
         * Compute the adjacencies between the given surface polygons
         * @param[in] polygons_to_connect Set of polygons for which compute
         * adjacencies
         */
        void compute_polygon_adjacencies(
            absl::Span< const index_t > polygons_to_connect );

        /*!
         * Delete a set of surface polygons
         * @param[in] to_delete Vector of size surface_mesh_.nb_polygons().
         * If to_delete[i] is true the polygon of index i is deleted, else it is
         * kept.
         * @return the mapping between old polygon indices to new ones.
         * Deleted polygons new index is NO_ID
         */
        std::vector< index_t > delete_polygons(
            const std::vector< bool >& to_delete );
        /*!
         * Permute polygons to match the given order.
         * @param[in] permutation Vector of size surface_mesh_.nb_polygons().
         * Each value corresponds to the destination position.
         * @return the mapping between old polygon indices to new ones.
         */
        std::vector< index_t > permute_polygons(
            absl::Span< const index_t > permutation );

        /*!
         * Delete all the isolated vertices (not used as polygon vertices)
         * @return the mapping between old vertex indices to new ones.
         * Deleted vertices new index is NO_ID
         */
        std::vector< index_t > delete_isolated_vertices();

        /*!
         * Set a polygon vertex to a given vertex.
         * @param[in] polygon_vertex PolygonVertex corresponding to the vertex.
         * @param[in] vertex_id Index of the vertex.
         */
        void associate_polygon_vertex_to_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id );

        /*!
         *Unset polygon vertex information to a given vertex.
         * @param[in] vertex_id Index of the vertex.
         */
        void disassociate_polygon_vertex_to_vertex( index_t vertex_id );

        void reset_polygons_around_vertex( index_t vertex_id );

        void copy( const SurfaceMesh< dimension >& surface_mesh );

    protected:
        SurfaceMeshBuilder( SurfaceMesh< dimension >& mesh );

        using VertexSetBuilder::delete_vertices;

    private:
        void update_polygon_adjacencies( absl::Span< const index_t > old2new );

        virtual void do_set_point(
            index_t vertex_id, Point< dimension > point ) = 0;

        void do_delete_vertices( const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) final;

        void do_permute_vertices( absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) final;

        virtual void do_permute_polygons(
            absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_delete_surface_vertices(
            const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_permute_surface_vertices(
            absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_set_polygon_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id ) = 0;

        virtual void do_create_polygon(
            absl::Span< const index_t > vertices ) = 0;

        virtual void do_delete_polygons( const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_set_polygon_adjacent(
            const PolygonEdge& polygon_edge, index_t adjacent_id ) = 0;

        virtual void do_unset_polygon_adjacent(
            const PolygonEdge& polygon_edge ) = 0;

        void update_polygon_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id );

        void update_polygon_vertices( absl::Span< const index_t > old2new );

        virtual void do_copy_points(
            const SurfaceMesh< dimension >& surface_mesh ) = 0;

        virtual void do_copy_polygons(
            const SurfaceMesh< dimension >& surface_mesh ) = 0;

    private:
        SurfaceMesh< dimension >& surface_mesh_;
    };
    ALIAS_2D_AND_3D( SurfaceMeshBuilder );
} // namespace geode
