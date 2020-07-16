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

#include <geode/mesh/core/polyhedral_solid_view.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/detail/solid_mesh_view_impl.h>

namespace geode
{
    template < index_t dimension >
    class PolyhedralSolidView< dimension >::Impl
        : public detail::SolidMeshViewImpl< dimension >
    {
    public:
        Impl( PolyhedralSolidView< dimension >& solid_view,
            const PolyhedralSolid< dimension >& solid )
            : detail::SolidMeshViewImpl< dimension >( solid_view, solid )
        {
        }
    };

    template < index_t dimension >
    PolyhedralSolidView< dimension >::PolyhedralSolidView(
        const PolyhedralSolid< dimension >& solid )
        : impl_( *this, solid )
    {
    }

    template < index_t dimension >
    PolyhedralSolidView< dimension >::PolyhedralSolidView(
        PolyhedralSolidView&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    PolyhedralSolidView< dimension >::~PolyhedralSolidView() // NOLINT
    {
    }

    template < index_t dimension >
    index_t PolyhedralSolidView< dimension >::viewed_vertex(
        index_t vertex_id ) const
    {
        return impl_->viewed_vertex( vertex_id );
    }

    template < index_t dimension >
    const Point< dimension >& PolyhedralSolidView< dimension >::get_point(
        index_t vertex_id ) const
    {
        return impl_->get_point( vertex_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolidView< dimension >::get_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        return impl_->get_polyhedron_vertex( polyhedron_vertex );
    }

    template < index_t dimension >
    absl::optional< index_t >
        PolyhedralSolidView< dimension >::get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        return impl_->get_polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    index_t PolyhedralSolidView< dimension >::add_viewed_vertex(
        index_t vertex_id, PolyhedralSolidViewKey )
    {
        return impl_->add_viewed_vertex( vertex_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolidView< dimension >::viewed_polyhedron(
        index_t polyhedron_id ) const
    {
        return impl_->viewed_polyhedron( polyhedron_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolidView< dimension >::add_viewed_polyhedron(
        index_t polyhedron_id, PolyhedralSolidViewKey )
    {
        return impl_->add_viewed_polyhedron( polyhedron_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolidView< dimension >::get_nb_polyhedron_vertices(
        index_t polyhedron_id ) const
    {
        return impl_->get_nb_polyhedron_vertices( polyhedron_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolidView< dimension >::get_polyhedron_facet(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        return impl_->get_polyhedron_facet( polyhedron_facet );
    }

    template < index_t dimension >
    const std::array< index_t, 2 >&
        PolyhedralSolidView< dimension >::get_edge_vertices(
            index_t edge_id ) const
    {
        return impl_->get_edge_vertices( edge_id );
    }

    template < index_t dimension >
    absl::optional< PolyhedronVertex >
        PolyhedralSolidView< dimension >::get_polyhedron_around_vertex(
            index_t vertex_id ) const
    {
        return impl_->get_polyhedron_around_vertex( vertex_id );
    }

    template < index_t dimension >
    bool PolyhedralSolidView< dimension >::get_isolated_edge(
        index_t edge_id ) const
    {
        return impl_->get_isolated_edge( edge_id );
    }

    template < index_t dimension >
    absl::optional< index_t >
        PolyhedralSolidView< dimension >::get_edge_from_vertices(
            const std::array< index_t, 2 >& vertices ) const
    {
        return impl_->get_edge_from_vertices( vertices );
    }

    template < index_t dimension >
    bool PolyhedralSolidView< dimension >::get_isolated_facet(
        index_t facet_id ) const
    {
        return impl_->get_isolated_facet( facet_id );
    }

    template < index_t dimension >
    const PolyhedronFacetVertices&
        PolyhedralSolidView< dimension >::get_facet_vertices(
            index_t facet_id ) const
    {
        return impl_->get_facet_vertices( facet_id );
    }

    template < index_t dimension >
    absl::optional< index_t >
        PolyhedralSolidView< dimension >::get_facet_from_vertices(
            const PolyhedronFacetVertices& vertices ) const
    {
        return impl_->get_facet_from_vertices( vertices );
    }

    template < index_t dimension >
    index_t PolyhedralSolidView< dimension >::get_nb_polyhedron_facets(
        index_t polyhedron_id ) const
    {
        return impl_->get_nb_polyhedron_facets( polyhedron_id );
    }

    template < index_t dimension >
    index_t PolyhedralSolidView< dimension >::get_nb_polyhedron_facet_vertices(
        const PolyhedronFacet& polyhedron_facet ) const
    {
        return impl_->get_nb_polyhedron_facet_vertices( polyhedron_facet );
    }

    template < index_t dimension >
    PolyhedronVertex
        PolyhedralSolidView< dimension >::get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
    {
        return impl_->get_polyhedron_facet_vertex_id( polyhedron_facet_vertex );
    }

    template class opengeode_mesh_api PolyhedralSolidView< 3 >;
} // namespace geode
