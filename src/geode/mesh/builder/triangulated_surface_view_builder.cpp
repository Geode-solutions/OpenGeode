/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/mesh/builder/triangulated_surface_view_builder.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/triangulated_surface_view.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TriangulatedSurfaceViewBuilder< dimension > >
        TriangulatedSurfaceViewBuilder< dimension >::create(
            TriangulatedSurfaceView< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            TriangulatedSurfaceViewBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::do_set_mesh(
        VertexSet& mesh )
    {
        triangulated_surface_view_ =
            &dynamic_cast< TriangulatedSurfaceView< dimension >& >( mesh );
    }

    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::do_set_point(
        index_t /*unused*/, Point< dimension > /*unused*/ )
    {
        // triangulated_surface_view_->set_vertex( vertex_id, point, {} );
    }

    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::do_create_vertex()
    {
    }

    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::do_create_vertices(
        index_t /*unused*/ )
    {
    }

    template < index_t dimension >
    void
        TriangulatedSurfaceViewBuilder< dimension >::do_delete_surface_vertices(
            const std::vector< bool >& /*unused*/ )
    {
    }

    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::do_set_polygon_vertex(
        const PolygonVertex& /*unused*/, index_t /*unused*/ )
    {
        // triangulated_surface_view_->set_polygon_vertex(
        //     polygon_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::do_create_triangle(
        const std::array< index_t, 3 >& /*unused*/ )
    {
        // triangulated_surface_view_->add_triangle( vertices, {} );
    }

    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::do_create_triangles(
        index_t /*unused*/ )
    {
    }

    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::do_set_polygon_adjacent(
        const PolygonEdge& /*unused*/, index_t /*unused*/ )
    {
        // triangulated_surface_view_->set_polygon_adjacent(
        //     polygon_edge, adjacent_id, {} );
    }
    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::do_unset_polygon_adjacent(
        const PolygonEdge& /*unused*/ )
    {
        // polygonal_surface_view_->set_polygon_adjacent(
        //     polygon_edge, NO_ID, {} );
    }

    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::do_delete_polygons(
        const std::vector< bool >& /*unused*/ )
    {
    }

    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::add_viewed_vertex(
        index_t vertex_id )
    {
        triangulated_surface_view_->add_viewed_vertex( vertex_id, {} );
    }

    template < index_t dimension >
    void TriangulatedSurfaceViewBuilder< dimension >::add_viewed_triangle(
        index_t triangle_id )
    {
        triangulated_surface_view_->add_viewed_triangle( triangle_id, {} );
    }

    template class opengeode_mesh_api TriangulatedSurfaceViewBuilder< 2 >;
    template class opengeode_mesh_api TriangulatedSurfaceViewBuilder< 3 >;
} // namespace geode
