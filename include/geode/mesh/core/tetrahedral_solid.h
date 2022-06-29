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

#include <geode/basic/bitsery_archive.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( TetrahedralSolidBuilder );
    class Tetrahedron;
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class TetrahedralSolid : public SolidMesh< dimension >
    {
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        using Builder = TetrahedralSolidBuilder< dimension >;

        /*!
         * Create a new TetrahedralSolid using default data structure.
         */
        static std::unique_ptr< TetrahedralSolid< dimension > > create();

        /*!
         * Create a new TetrahedralSolid using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        static std::unique_ptr< TetrahedralSolid< dimension > > create(
            const MeshImpl& impl );

        static MeshType type_name_static()
        {
            return MeshType{ absl::StrCat(
                "TetrahedralSolid", dimension, "D" ) };
        }

        std::unique_ptr< TetrahedralSolid< dimension > > clone() const;

        std::vector< std::array< index_t, 2 > > polyhedron_edges_vertices(
            index_t polyhedron ) const final;

        std::vector< PolyhedronFacetVertices > polyhedron_facets_vertices(
            index_t polyhedron ) const final;

        PolyhedraAroundEdge polyhedra_around_edge(
            const std::array< index_t, 2 >& vertices ) const final;

        PolyhedraAroundEdge polyhedra_around_edge(
            const std::array< index_t, 2 >& vertices,
            index_t first_polyhedron ) const final;

        /*!
         * Return the local indices of the two facets incident to the
         * edge in tetrahedron.
         * @param[in] tetrahedron_id Index of tetrahedron.
         * @param[in] edge_vertices Indices of the two vertices of an edge.
         * @warn throw an exception if edge_vertices are not tetrahedron
         * vertices.
         */
        std::array< PolyhedronFacet, 2 > edge_incident_facets(
            index_t tetrahedron_id,
            const std::array< index_t, 2 >& edge_vertices ) const;

        /*!
         * Return the local indices of the two facets incident to the
         * opposite edge in tetrahedron.
         * @param[in] tetrahedron_id Index of tetrahedron.
         * @param[in] edge_vertices Indices of the two vertices of an edge.
         * @warn throw an exception if edge_vertices are not tetrahedron
         * vertices.
         */
        std::array< PolyhedronFacet, 2 > opposite_edge_incident_facets(
            index_t tetrahedron_id,
            const std::array< index_t, 2 >& edge_vertices ) const;

        /*!
         * Return a geometrical Tetra from an element of the mesh.
         */
        Tetrahedron tetra( index_t tetrahedron_id ) const;

        /*!
         * Return a geometrical Triangle from a tetrahedron facet of the mesh.
         */
        Triangle< dimension > triangle( const PolyhedronFacet& facet ) const;

    protected:
        TetrahedralSolid() = default;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, TetrahedralSolid >{},
                []( Archive& a, TetrahedralSolid& tetrahedral_solid ) {
                    a.ext( tetrahedral_solid,
                        bitsery::ext::BaseClass< SolidMesh< dimension > >{} );
                } );
        }

        local_index_t get_nb_polyhedron_vertices(
            index_t /*unused*/ ) const final
        {
            return 4;
        }

        local_index_t get_nb_polyhedron_facets( index_t /*unused*/ ) const final
        {
            return 4;
        }

        local_index_t get_nb_polyhedron_facet_vertices(
            const PolyhedronFacet& /*unused*/ ) const final
        {
            return 3;
        }

        PolyhedronFacets polyhedron_vertex_facets(
            const PolyhedronVertex& polyhedron_vertex ) const final;
    };
    ALIAS_3D( TetrahedralSolid );
} // namespace geode
