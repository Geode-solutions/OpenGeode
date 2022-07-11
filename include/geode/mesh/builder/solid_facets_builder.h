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

#include <geode/mesh/common.h>
#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMeshBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidEdgesBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a SolidFacets
     */
    template < index_t dimension >
    class SolidFacetsBuilder
    {
    public:
        SolidFacetsBuilder( SolidFacets< dimension >& facets );

        /*!
         * Delete all the isolated facets (not used as polyhedron facets)
         * @return the mapping between old facet indices to new ones.
         * Deleted facets new index is NO_ID
         */
        std::vector< index_t > delete_isolated_facets();

        index_t find_or_create_facet( PolyhedronFacetVertices facet_vertices );

        std::vector< index_t > delete_facets(
            const std::vector< bool >& to_delete );

        void remove_facet( PolyhedronFacetVertices facet_vertices );

        void update_facet_vertex( PolyhedronFacetVertices facet_vertices,
            index_t facet_vertex_id,
            index_t new_vertex_id );

        std::vector< index_t > update_facet_vertices(
            absl::Span< const index_t > old2new );

        void copy( const SolidFacets< dimension >& facets );

    private:
        SolidFacets< dimension >* facets_;
    };
    ALIAS_3D( SolidFacetsBuilder );
} // namespace geode
