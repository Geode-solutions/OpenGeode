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

#include <geode/mesh/builder/polyhedral_solid_view_builder.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/polyhedral_solid_view.h>

namespace geode
{
    template < index_t dimension >
    PolyhedralSolidViewBuilder< dimension >::PolyhedralSolidViewBuilder(
        VertexSet& vertex_set, MeshBuilderFactoryKey )
        : PolyhedralSolidViewBuilder< dimension >(
            dynamic_cast< PolyhedralSolidView< dimension >& >( vertex_set ) )
    {
    }

    template < index_t dimension >
    PolyhedralSolidViewBuilder< dimension >::PolyhedralSolidViewBuilder(
        PolyhedralSolidView< dimension >& mesh )
        : PolyhedralSolidBuilder< dimension >( mesh ),
          polyhedral_solid_view_( mesh )
    {
    }

    template < index_t dimension >
    std::unique_ptr< PolyhedralSolidViewBuilder< dimension > >
        PolyhedralSolidViewBuilder< dimension >::create(
            PolyhedralSolidView< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            PolyhedralSolidViewBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::do_set_point(
        index_t /*unused*/, Point< dimension > /*unused*/ )
    {
        // polyhedral_solid_view_.set_vertex( vertex_id, point, {} );
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::do_create_vertex()
    {
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::do_create_vertices(
        index_t /*unused*/ )
    {
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::do_delete_solid_vertices(
        const std::vector< bool >& /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::do_permute_solid_vertices(
        absl::Span< const index_t > /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::do_set_polyhedron_vertex(
        const PolyhedronVertex& /*unused*/, index_t /*unused*/ )
    {
        // polyhedral_solid_view_.set_polyhedron_vertex(
        //     polyhedron_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::do_create_polyhedron(
        absl::Span< const index_t > /*unused*/,
        absl::Span< const std::vector< local_index_t > > /*unused*/ )
    {
        // polyhedral_solid_view_.add_polyhedron( vertices, facets, {} );
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::do_set_polyhedron_adjacent(
        const PolyhedronFacet& /*unused*/, index_t /*unused*/ )
    {
        // polyhedral_solid_view_.set_polyhedron_adjacent(
        //     polyhedron_facet, adjacent_id, {} );
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::do_unset_polyhedron_adjacent(
        const PolyhedronFacet& /*unused*/ )
    {
        // polyhedral_solid_view_.set_polyhedron_adjacent(
        //     polyhedron_facet, NO_ID, {} );
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::do_delete_polyhedra(
        const std::vector< bool >& /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // polyhedral_solid_view_.remove_polyhedra( to_delete, {} );
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::do_permute_polyhedra(
        absl::Span< const index_t > /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::add_viewed_vertex(
        index_t vertex_id )
    {
        polyhedral_solid_view_.add_viewed_vertex( vertex_id, {} );
    }

    template < index_t dimension >
    void PolyhedralSolidViewBuilder< dimension >::add_viewed_polyhedron(
        index_t polyhedron_id )
    {
        polyhedral_solid_view_.add_viewed_polyhedron( polyhedron_id, {} );
    }

    template class opengeode_mesh_api PolyhedralSolidViewBuilder< 3 >;
} // namespace geode
