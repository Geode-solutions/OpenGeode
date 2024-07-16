/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/mesh/builder/geode/geode_triangulated_surface_builder.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/mesh_builder_factory.hpp>
#include <geode/mesh/core/geode/geode_triangulated_surface.hpp>

namespace geode
{
    template < index_t dimension >
    OpenGeodeTriangulatedSurfaceBuilder<
        dimension >::OpenGeodeTriangulatedSurfaceBuilder( VertexSet& vertex_set,
        MeshBuilderFactoryKey )
        : OpenGeodeTriangulatedSurfaceBuilder< dimension >(
            dynamic_cast< OpenGeodeTriangulatedSurface< dimension >& >(
                vertex_set ) )
    {
    }

    template < index_t dimension >
    OpenGeodeTriangulatedSurfaceBuilder< dimension >::
        OpenGeodeTriangulatedSurfaceBuilder(
            OpenGeodeTriangulatedSurface< dimension >& mesh )
        : TriangulatedSurfaceBuilder< dimension >( mesh ),
          geode_triangulated_surface_( mesh )
    {
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder< dimension >::do_create_vertex()
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder< dimension >::do_create_vertices(
        index_t /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder< dimension >::
        do_delete_surface_vertices( const std::vector< bool >& /*unused*/,
            absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder< dimension >::
        do_permute_surface_vertices( absl::Span< const index_t > /*unused*/,
            absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void
        OpenGeodeTriangulatedSurfaceBuilder< dimension >::do_set_polygon_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        geode_triangulated_surface_.set_polygon_vertex(
            polygon_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder< dimension >::do_create_triangle(
        const std::array< index_t, 3 >& vertices )
    {
        geode_triangulated_surface_.add_triangle( vertices, {} );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder< dimension >::do_create_triangles(
        index_t /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder<
        dimension >::do_set_polygon_adjacent( const PolygonEdge& polygon_edge,
        index_t adjacent_id )
    {
        geode_triangulated_surface_.set_polygon_adjacent(
            polygon_edge, adjacent_id, {} );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder< dimension >::
        do_unset_polygon_adjacent( const PolygonEdge& polygon_edge )
    {
        geode_triangulated_surface_.set_polygon_adjacent(
            polygon_edge, NO_ID, {} );
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder< dimension >::do_delete_polygons(
        const std::vector< bool >& /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder< dimension >::do_permute_polygons(
        absl::Span< const index_t > /*unused*/,
        absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder< dimension >::do_copy_points(
        const SurfaceMesh< dimension >& /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodeTriangulatedSurfaceBuilder< dimension >::do_copy_polygons(
        const SurfaceMesh< dimension >& /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template class opengeode_mesh_api OpenGeodeTriangulatedSurfaceBuilder< 2 >;
    template class opengeode_mesh_api OpenGeodeTriangulatedSurfaceBuilder< 3 >;
} // namespace geode
