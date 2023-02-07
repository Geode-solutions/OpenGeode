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

#include <geode/basic/passkey.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidFacetsBuilder );

    class AttributeManager;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class SolidFacets
    {
        OPENGEODE_DISABLE_COPY( SolidFacets );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );
        PASSKEY( SolidFacetsBuilder< dimension >, SolidFacetsKey );

    public:
        using Builder = SolidFacetsBuilder< dimension >;
        static constexpr auto dim = dimension;

        SolidFacets();
        SolidFacets( const SolidMesh< dimension >& solid );
        ~SolidFacets();

        index_t nb_facets() const;

        bool is_facet_isolated( index_t facet_id ) const;

        /*!
         * Return the indices of facet vertices.
         * @param[in] edge_id Index of an edge.
         */
        const PolyhedronFacetVertices& facet_vertices( index_t facet_id ) const;

        /*!
         * Get the index of facet corresponding to given vertices
         * @param[in] vertices Ordered vertex indices
         */
        absl::optional< index_t > facet_from_vertices(
            const PolyhedronFacetVertices& vertices ) const;

        /*!
         * Access to the manager of attributes associated with facets.
         */
        AttributeManager& facet_attribute_manager() const;

    public:
        std::vector< index_t > update_facet_vertices(
            absl::Span< const index_t > old2new, SolidFacetsKey );

        void update_facet_vertex( PolyhedronFacetVertices facet_vertices,
            index_t facet_vertex_id,
            index_t new_vertex_id,
            SolidFacetsKey );

        void remove_facet(
            PolyhedronFacetVertices facet_vertices, SolidFacetsKey );

        std::vector< index_t > delete_facets(
            const std::vector< bool >& to_delete, SolidFacetsKey );

        std::vector< index_t > remove_isolated_facets( SolidFacetsKey );

        index_t find_or_create_facet(
            PolyhedronFacetVertices facet_vertices, SolidFacetsKey )
        {
            return find_or_create_facet( std::move( facet_vertices ) );
        }

        void overwrite_facets(
            const SolidFacets< dimension >& from, SolidFacetsKey );

    private:
        index_t find_or_create_facet( PolyhedronFacetVertices facet_vertices );

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( SolidFacets );
} // namespace geode
