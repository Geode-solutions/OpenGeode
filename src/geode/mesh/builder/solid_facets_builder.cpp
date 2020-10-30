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

#include <geode/mesh/builder/solid_facets_builder.h>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/solid_facets.h>
#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    template < index_t dimension >
    SolidFacetsBuilder< dimension >::SolidFacetsBuilder(
        SolidFacets< dimension >& facets )
        : facets_( &facets )
    {
    }

    template < index_t dimension >
    index_t SolidFacetsBuilder< dimension >::find_or_create_facet(
        PolyhedronFacetVertices facet_vertices )
    {
        return facets_->find_or_create_facet( std::move( facet_vertices ), {} );
    }

    template < index_t dimension >
    std::vector< index_t >
        SolidFacetsBuilder< dimension >::delete_isolated_facets()
    {
        return facets_->remove_isolated_facets( {} );
    }

    template < index_t dimension >
    void SolidFacetsBuilder< dimension >::update_facet_vertices(
        absl::Span< const index_t > old2new )
    {
        facets_->update_facet_vertices( old2new, {} );
    }

    template < index_t dimension >
    void SolidFacetsBuilder< dimension >::update_facet_vertex(
        PolyhedronFacetVertices facet_vertices,
        index_t facet_vertex_id,
        index_t new_vertex_id )
    {
        OPENGEODE_ASSERT( facet_vertex_id < facet_vertices.size(),
            "[SolidFacetsBuilder::update_facet_vertex] "
            "Accessing an invalid vertex in facet" );
        facets_->update_facet_vertex(
            std::move( facet_vertices ), facet_vertex_id, new_vertex_id, {} );
    }

    template < index_t dimension >
    void SolidFacetsBuilder< dimension >::remove_facet(
        PolyhedronFacetVertices facet_vertices )
    {
        facets_->remove_facet( std::move( facet_vertices ), {} );
    }

    template < index_t dimension >
    std::vector< index_t > SolidFacetsBuilder< dimension >::delete_facets(
        const std::vector< bool >& to_delete )
    {
        return facets_->delete_facets( to_delete, {} );
    }

    template < index_t dimension >
    void SolidFacetsBuilder< dimension >::copy(
        const SolidFacets< dimension >& facets )
    {
        facets_->overwrite_facets( facets, {} );
    }

    template class opengeode_mesh_api SolidFacetsBuilder< 3 >;
} // namespace geode
