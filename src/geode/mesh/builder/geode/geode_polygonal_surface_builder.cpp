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

#include <geode/mesh/builder/geode/geode_polygonal_surface_builder.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/geode/geode_polygonal_surface.h>

namespace geode
{
    template < index_t dimension >
    OpenGeodePolygonalSurfaceBuilder<
        dimension >::OpenGeodePolygonalSurfaceBuilder( VertexSet& vertex_set,
        MeshBuilderFactoryKey )
        : OpenGeodePolygonalSurfaceBuilder< dimension >(
            dynamic_cast< OpenGeodePolygonalSurface< dimension >& >(
                vertex_set ) )
    {
    }

    template < index_t dimension >
    OpenGeodePolygonalSurfaceBuilder< dimension >::
        OpenGeodePolygonalSurfaceBuilder(
            OpenGeodePolygonalSurface< dimension >& mesh )
        : PolygonalSurfaceBuilder< dimension >( mesh ),
          geode_polygonal_surface_( mesh )
    {
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_set_point(
        index_t vertex_id, Point< dimension > point )
    {
        geode_polygonal_surface_.set_vertex(
            vertex_id, std::move( point ), {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_create_vertex()
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_create_vertices(
        index_t /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::
        do_delete_surface_vertices( const std::vector< bool >& /*unused*/,
            absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::
        do_permute_surface_vertices( absl::Span< const index_t > /*unused*/,
            absl::Span< const index_t > /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_set_polygon_vertex(
        const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        geode_polygonal_surface_.set_polygon_vertex(
            polygon_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_create_polygon(
        absl::Span< const index_t > vertices )
    {
        geode_polygonal_surface_.add_polygon( vertices, {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_set_polygon_adjacent(
        const PolygonEdge& polygon_edge, index_t adjacent_id )
    {
        geode_polygonal_surface_.set_polygon_adjacent(
            polygon_edge, adjacent_id, {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::
        do_unset_polygon_adjacent( const PolygonEdge& polygon_edge )
    {
        geode_polygonal_surface_.set_polygon_adjacent(
            polygon_edge, NO_ID, {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_delete_polygons(
        const std::vector< bool >& to_delete,
        absl::Span< const index_t > /*unused*/ )
    {
        geode_polygonal_surface_.remove_polygons( to_delete, {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_permute_polygons(
        absl::Span< const index_t > permutation,
        absl::Span< const index_t > /*unused*/ )
    {
        geode_polygonal_surface_.permute_polygons( permutation, {} );
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_copy_points(
        const SurfaceMesh< dimension >& /*unused*/ )
    {
        // Operation is directly handled by the AttributeManager
    }

    template < index_t dimension >
    void OpenGeodePolygonalSurfaceBuilder< dimension >::do_copy_polygons(
        const SurfaceMesh< dimension >& surface_mesh )
    {
        geode_polygonal_surface_.copy_polygons(
            dynamic_cast< const OpenGeodePolygonalSurface< dimension >& >(
                surface_mesh ),
            {} );
    }

    template class opengeode_mesh_api OpenGeodePolygonalSurfaceBuilder< 2 >;
    template class opengeode_mesh_api OpenGeodePolygonalSurfaceBuilder< 3 >;
} // namespace geode
