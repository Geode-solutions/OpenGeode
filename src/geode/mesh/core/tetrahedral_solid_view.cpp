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

#include <geode/mesh/core/tetrahedral_solid_view.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/detail/solid_mesh_view_impl.h>

namespace geode
{
    template < index_t dimension >
    class TetrahedralSolidView< dimension >::Impl
        : public detail::SolidMeshViewImpl< dimension >
    {
    public:
        Impl( TetrahedralSolidView< dimension >& solid_view,
            const TetrahedralSolid< dimension >& solid )
            : detail::SolidMeshViewImpl< dimension >( solid_view, solid )
        {
        }
    };

    template < index_t dimension >
    TetrahedralSolidView< dimension >::TetrahedralSolidView(
        const TetrahedralSolid< dimension >& solid )
        : impl_( *this, solid )
    {
    }

    template < index_t dimension >
    TetrahedralSolidView< dimension >::TetrahedralSolidView(
        TetrahedralSolidView&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    TetrahedralSolidView< dimension >::~TetrahedralSolidView() // NOLINT
    {
    }

    template < index_t dimension >
    index_t TetrahedralSolidView< dimension >::viewed_vertex(
        index_t vertex_id ) const
    {
        return impl_->viewed_vertex( vertex_id );
    }

    template < index_t dimension >
    const Point< dimension >& TetrahedralSolidView< dimension >::get_point(
        index_t vertex_id ) const
    {
        return impl_->get_point( vertex_id );
    }

    template < index_t dimension >
    index_t TetrahedralSolidView< dimension >::get_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        return impl_->get_polyhedron_vertex( polyhedron_vertex );
    }

    template < index_t dimension >
    absl::optional< index_t >
        TetrahedralSolidView< dimension >::get_polyhedron_adjacent(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        return impl_->get_polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    index_t TetrahedralSolidView< dimension >::add_viewed_vertex(
        index_t vertex_id, TetrahedralSolidViewKey )
    {
        return impl_->add_viewed_vertex( vertex_id );
    }

    template < index_t dimension >
    index_t TetrahedralSolidView< dimension >::viewed_tetrahedron(
        index_t polyhedron_id ) const
    {
        return impl_->viewed_polyhedron( polyhedron_id );
    }

    template < index_t dimension >
    index_t TetrahedralSolidView< dimension >::add_viewed_tetrahedron(
        index_t polyhedron_id, TetrahedralSolidViewKey )
    {
        return impl_->add_viewed_polyhedron( polyhedron_id );
    }

    template < index_t dimension >
    absl::optional< PolyhedronVertex >
        TetrahedralSolidView< dimension >::get_polyhedron_around_vertex(
            index_t vertex_id ) const
    {
        return impl_->get_polyhedron_around_vertex( vertex_id );
    }

    template < index_t dimension >
    PolyhedronVertex
        TetrahedralSolidView< dimension >::get_polyhedron_facet_vertex_id(
            const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
    {
        return impl_->get_polyhedron_facet_vertex_id( polyhedron_facet_vertex );
    }

    template class opengeode_mesh_api TetrahedralSolidView< 3 >;
} // namespace geode
