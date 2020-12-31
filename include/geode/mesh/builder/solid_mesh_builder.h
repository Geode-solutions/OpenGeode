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

#include <geode/basic/passkey.h>

#include <geode/mesh/builder/vertex_set_builder.h>
#include <geode/mesh/common.h>
#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidFacetsBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidEdgesBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a SolidMesh
     */
    template < index_t dimension >
    class SolidMeshBuilder : public VertexSetBuilder
    {
        PASSKEY( SolidMesh< dimension >, BuilderKey );

    public:
        /*!
         * Create the builder associated with a SolidMesh.
         * @param[in] mesh The SolidMesh to build/modify
         */
        static std::unique_ptr< SolidMeshBuilder< dimension > > create(
            SolidMesh< dimension >& mesh );

        SolidEdgesBuilder< dimension > edges_builder();

        SolidFacetsBuilder< dimension > facets_builder();

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
         * Create a new polyhedron from vertices and facets.
         * @param[in] vertices The vertices defining the polyhedron to create
         * @param[in] facets The list of ordered vertices defining all the
         * facets of the polyhedron
         * @return the index of the created polyhedron
         */
        index_t create_polyhedron( absl::Span< const index_t > vertices,
            absl::Span< const std::vector< local_index_t > > facets );

        /*!
         * Modify a polyhedron vertex.
         * @param[in] polyhedron_vertex The index of the polyhedron vertex to
         * modify
         * @param[in] vertex_id Index of the mesh vertex to set as polyhedron
         * vertex
         */
        void set_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex, index_t vertex_id );

        /*!
         * Set a polyhedron adgjacent through a facet.
         * @param[in] polygon_facet The index of the polygon facet
         * @param[in] adjacent_id Index of the adjacent polyhedron
         */
        void set_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet, index_t adjacent_id );

        /*!
         * Unset a polyhedron adjacency through a facet.

         * @param[in] polygon_facet The index of the polygon facet
         */
        void unset_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet );

        /*!
         * Compute all the adjacencies between the solid polyhedra
         */
        void compute_polyhedron_adjacencies();

        /*!
         * Compute the adjacencies between the given solid polyhedra
         * @param[in] polyhedra_to_connect Set of polyhedra for which compute
         * adjacencies
         */
        void compute_polyhedron_adjacencies(
            absl::Span< const index_t > polyhedra_to_connect );

        /*!
         * Delete a set of solid polyhedra
         * @param[in] to_delete Vector of size solid_mesh_.nb_polyhedra().
         * If to_delete[i] is true the polyhedra of index i is deleted, else it
         * is kept.
         * @return the mapping between old polyhedron indices to new ones.
         * Deleted polyhedra new index is NO_ID
         */
        std::vector< index_t > delete_polyhedra(
            const std::vector< bool >& to_delete );

        /*!
         * Permute polyhedra to match the given order.
         * @param[in] permutation Vector of size solid_mesh_.nb_polyhedra().
         * Each value corresponds to the destination position.
         * @return the mapping between old polyhedron indices to new ones.
         */
        absl::FixedArray< index_t > permute_polyhedra(
            absl::Span< const index_t > permutation );

        /*!
         * Delete all the isolated vertices (not used as polyhedron vertices)
         * @return the mapping between old vertex indices to new ones.
         * Deleted vertices new index is NO_ID
         */
        std::vector< index_t > delete_isolated_vertices();

        /*!
         * Set a polyhedron vertex to a given vertex.
         * @param[in] polyhedron_vertex PolyhedronVertex corresponding to the
         * vertex.
         * @param[in] vertex_id Index of the vertex.
         */
        void associate_polyhedron_vertex_to_vertex(
            const PolyhedronVertex& polyhedron_vertex, index_t vertex_id );

        /*!
         * Unset polyhedron vertex information to a given vertex.
         * @param[in] vertex_id Index of the vertex.
         */
        void disassociate_polyhedron_vertex_to_vertex( index_t vertex_id );

        void copy( const SolidMesh< dimension >& solid_mesh, BuilderKey )
        {
            copy( solid_mesh );
        }

        void set_mesh(
            SolidMesh< dimension >& mesh, MeshBuilderFactoryKey key );

    protected:
        SolidMeshBuilder() = default;

        void copy( const SolidMesh< dimension >& solid_mesh );

        void update_polyhedron_info(
            index_t polyhedron_id, absl::Span< const index_t > vertices );

    private:
        /*!
         * @brief Sets a point.
         * @param[in] vertex_id the vertex, in 0.. @function nb_vetices()-1.
         * @param[in] vertex the vertex coordinates
         * @return reference to the point that corresponds to the vertex.
         */
        virtual void do_set_point(
            index_t vertex_id, Point< dimension > point ) = 0;

        void do_delete_vertices( const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) final;

        void do_permute_vertices( absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) final;

        virtual void do_delete_solid_vertices(
            const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_permute_solid_vertices(
            absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_set_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex, index_t vertex_id ) = 0;

        virtual void do_create_polyhedron( absl::Span< const index_t > vertices,
            absl::Span< const std::vector< local_index_t > > facets ) = 0;

        virtual void do_delete_polyhedra(
            const std::vector< bool >& to_delete ) = 0;

        virtual void do_permute_polyhedra(
            absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) = 0;

        virtual void do_set_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet, index_t adjacent_id ) = 0;

        virtual void do_unset_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) = 0;

        void update_polyhedron_vertex(
            const PolyhedronVertex& polyhedron_vertex, index_t vertex_id );

        void update_polyhedron_vertices( absl::Span< const index_t > old2new );

        void remove_polyhedra_facets( const std::vector< bool >& to_delete );

        void remove_polyhedra_edges( const std::vector< bool >& to_delete );

        virtual std::vector< PolyhedronFacetVertices >
            get_polyhedron_facet_vertices( absl::Span< const index_t > vertices,
                absl::Span< const std::vector< index_t > > facets ) const;

    private:
        SolidMesh< dimension >* solid_mesh_;
    };
    ALIAS_3D( SolidMeshBuilder );
} // namespace geode
