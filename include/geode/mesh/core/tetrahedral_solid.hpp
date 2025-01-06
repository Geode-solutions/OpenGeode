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

#include <array>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( TetrahedralSolidBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
    class Tetrahedron;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class TetrahedralSolid : public SolidMesh< dimension >
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        using Builder = TetrahedralSolidBuilder< dimension >;
        static constexpr auto dim = dimension;

        /*!
         * Create a new TetrahedralSolid using default data structure.
         */
        [[nodiscard]] static std::unique_ptr< TetrahedralSolid< dimension > >
            create();

        /*!
         * Create a new TetrahedralSolid using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        [[nodiscard]] static std::unique_ptr< TetrahedralSolid< dimension > >
            create( const MeshImpl& impl );

        [[nodiscard]] static MeshType type_name_static();

        [[nodiscard]] std::unique_ptr< TetrahedralSolid< dimension > >
            clone() const;

        [[nodiscard]] PolyhedronEdgesVertices polyhedron_edges_vertices(
            index_t polyhedron ) const final;

        [[nodiscard]] std::array< PolyhedronFacetEdge, 6 > polyhedron_edges(
            index_t polyhedron ) const;

        [[nodiscard]] PolyhedronFacetsVertices polyhedron_facets_vertices(
            index_t polyhedron ) const final;

        [[nodiscard]] typename SolidMesh< dimension >::VerticesAroundVertex
            vertices_around_vertex( index_t vertex_id ) const final;

        [[nodiscard]] PolyhedraAroundEdge polyhedra_around_edge(
            const std::array< index_t, 2 >& vertices ) const final;

        [[nodiscard]] PolyhedraAroundEdge polyhedra_around_edge(
            const std::array< index_t, 2 >& vertices,
            index_t first_polyhedron ) const final;

        [[nodiscard]] PolyhedraAroundEdge polyhedra_around_edge(
            const PolyhedronFacetEdge& edge ) const final;

        [[nodiscard]] std::optional< PolyhedronFacet >
            polyhedron_adjacent_facet(
                const PolyhedronFacet& polyhedron_facet ) const final;

        /*
         * Return the two other tetrahedron vertices not given as input.
         * @param[in] tetrahedron_id Index of tetrahedron.
         * @param[in] edge_vertices Indices of the two vertices of an edge.
         * @warning do not check that the given edge_vertices are in tetrahedron
         * vertices.
         */
        [[nodiscard]] std::array< index_t, 2 > opposite_edge_vertices(
            index_t tetrahedron_id,
            const std::array< index_t, 2 >& edge_vertices ) const;

        /*!
         * Return the local indices of the two facets incident to the
         * edge in tetrahedron.
         * @param[in] tetrahedron_id Index of tetrahedron.
         * @param[in] edge_vertices Indices of the two vertices of an edge.
         * @warning throw an exception if edge_vertices are not tetrahedron
         * vertices.
         */
        [[nodiscard]] std::array< PolyhedronFacet, 2 > edge_incident_facets(
            index_t tetrahedron_id,
            const std::array< index_t, 2 >& edge_vertices ) const;

        /*!
         * Return the local indices of the two facets incident to the
         * opposite edge in tetrahedron.
         * @param[in] tetrahedron_id Index of tetrahedron.
         * @param[in] edge_vertices Indices of the two vertices of an edge.
         * @warning throw an exception if edge_vertices are not tetrahedron
         * vertices.
         */
        [[nodiscard]] std::array< PolyhedronFacet, 2 >
            opposite_edge_incident_facets( index_t tetrahedron_id,
                const std::array< index_t, 2 >& edge_vertices ) const;

        /*!
         * Return a geometrical Tetra from an element of the mesh.
         */
        [[nodiscard]] Tetrahedron tetrahedron( index_t tetrahedron_id ) const;

        /*!
         * Return a geometrical Triangle from a tetrahedron facet of the mesh.
         */
        [[nodiscard]] Triangle< dimension > triangle(
            const PolyhedronFacet& facet ) const;

    protected:
        TetrahedralSolid() = default;
        TetrahedralSolid( TetrahedralSolid&& other ) noexcept = default;
        TetrahedralSolid& operator=(
            TetrahedralSolid&& other ) noexcept = default;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        [[nodiscard]] local_index_t get_nb_polyhedron_vertices(
            index_t /*unused*/ ) const final
        {
            return 4;
        }

        [[nodiscard]] local_index_t get_nb_polyhedron_facets(
            index_t /*unused*/ ) const final
        {
            return 4;
        }

        [[nodiscard]] local_index_t get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& /*unused*/ ) const final
        {
            return 3;
        }

        [[nodiscard]] PolyhedronFacets polyhedron_vertex_facets(
            const PolyhedronVertex& polyhedron_vertex ) const final;
    };
    ALIAS_3D( TetrahedralSolid );
} // namespace geode
