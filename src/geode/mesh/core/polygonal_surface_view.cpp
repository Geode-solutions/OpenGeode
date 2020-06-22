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

#include <geode/mesh/core/polygonal_surface_view.h>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/detail/surface_mesh_view_impl.h>

namespace geode
{
    template < index_t dimension >
    class PolygonalSurfaceView< dimension >::Impl
        : public detail::SurfaceMeshViewImpl< dimension >
    {
    public:
        Impl( PolygonalSurfaceView< dimension >& surface_view,
            PolygonalSurface< dimension >& surface )
            : detail::SurfaceMeshViewImpl< dimension >( surface_view, surface )
        {
        }
    };

    template < index_t dimension >
    PolygonalSurfaceView< dimension >::PolygonalSurfaceView(
        PolygonalSurface< dimension >& surface )
        : impl_( *this, surface )
    {
    }

    template < index_t dimension >
    PolygonalSurfaceView< dimension >::~PolygonalSurfaceView() // NOLINT
    {
    }

    template < index_t dimension >
    index_t PolygonalSurfaceView< dimension >::viewed_vertex(
        index_t vertex_id ) const
    {
        return impl_->viewed_vertex( vertex_id );
    }

    template < index_t dimension >
    const Point< dimension >& PolygonalSurfaceView< dimension >::get_point(
        index_t vertex_id ) const
    {
        return impl_->get_point( vertex_id );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceView< dimension >::get_polygon_vertex(
        const PolygonVertex& polygon_vertex ) const
    {
        return impl_->get_polygon_vertex( polygon_vertex );
    }

    template < index_t dimension >
    absl::optional< index_t >
        PolygonalSurfaceView< dimension >::get_polygon_adjacent(
            const PolygonEdge& polygon_edge ) const
    {
        return impl_->get_polygon_adjacent( polygon_edge );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceView< dimension >::add_viewed_vertex(
        index_t vertex_id, PolygonalSurfaceViewKey )
    {
        return impl_->add_viewed_vertex( vertex_id );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceView< dimension >::viewed_polygon(
        index_t polygon_id ) const
    {
        return impl_->viewed_polygon( polygon_id );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceView< dimension >::add_viewed_polygon(
        index_t polygon_id, PolygonalSurfaceViewKey )
    {
        return impl_->add_viewed_polygon( polygon_id );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceView< dimension >::get_nb_polygon_vertices(
        index_t polygon_id ) const
    {
        return impl_->get_nb_polygon_vertices( polygon_id );
    }

    template < index_t dimension >
    index_t PolygonalSurfaceView< dimension >::get_polygon_edge(
        const PolygonEdge& polygon_edge ) const
    {
        return impl_->get_polygon_edge( polygon_edge );
    }

    template < index_t dimension >
    const std::array< index_t, 2 >&
        PolygonalSurfaceView< dimension >::get_edge_vertices(
            index_t edge_id ) const
    {
        return impl_->get_edge_vertices( edge_id );
    }

    template < index_t dimension >
    absl::optional< PolygonVertex >
        PolygonalSurfaceView< dimension >::get_polygon_around_vertex(
            index_t vertex_id ) const
    {
        return impl_->get_polygon_around_vertex( vertex_id );
    }

    template < index_t dimension >
    bool PolygonalSurfaceView< dimension >::get_isolated_edge(
        index_t edge_id ) const
    {
        return impl_->get_isolated_edge( edge_id );
    }

    template < index_t dimension >
    absl::optional< index_t >
        PolygonalSurfaceView< dimension >::get_edge_from_vertices(
            const std::array< index_t, 2 >& vertices ) const
    {
        return impl_->get_edge_from_vertices( vertices );
    }

    template class opengeode_mesh_api PolygonalSurfaceView< 2 >;
    template class opengeode_mesh_api PolygonalSurfaceView< 3 >;
} // namespace geode
