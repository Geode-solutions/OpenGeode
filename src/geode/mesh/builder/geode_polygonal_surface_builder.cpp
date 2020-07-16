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

#include <geode/mesh/builder/geode_polygonal_surface_builder.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/geode_polygonal_surface.h>

namespace geode
{
    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_set_mesh(
        VertexSet& mesh )
    {
        geode_polygonal_surface_ =
            &dynamic_cast< OpenGeodePolygonalSurface< dimension >& >( mesh );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_set_point(
        index_t vertex_id, Point< dimension > point )
    {
        geode_polygonal_surface_->set_vertex(
            vertex_id, std::move( point ), {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_create_vertex()
    {
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_create_vertices(
        index_t /*unused*/ )
    {
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::
        do_delete_surface_vertices( const std::vector< bool >& /*unused*/ )
    {
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_set_polygon_vertex(
        const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        geode_polygonal_surface_->set_polygon_vertex(
            polygon_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_create_polygon(
        absl::Span< const index_t > vertices )
    {
        geode_polygonal_surface_->add_polygon( vertices, {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_set_polygon_adjacent(
        const PolygonEdge& polygon_edge, index_t adjacent_id )
    {
        geode_polygonal_surface_->set_polygon_adjacent(
            polygon_edge, adjacent_id, {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::
        do_unset_polygon_adjacent( const PolygonEdge& polygon_edge )
    {
        geode_polygonal_surface_->set_polygon_adjacent(
            polygon_edge, NO_ID, {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_delete_polygons(
        const std::vector< bool >& to_delete )
    {
        geode_polygonal_surface_->remove_polygons( to_delete, {} );
    }

    template class opengeode_mesh_api OpenGeodePolygonalSurfaceBuilder< 2 >;
    template class opengeode_mesh_api OpenGeodePolygonalSurfaceBuilder< 3 >;
} // namespace geode
