/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <vector>

#include <geode/mesh/builder/polygonal_surface_builder.hpp>
#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodePolygonalSurface );
    class VertexSet;
} // namespace geode

namespace geode
{
    /*!
     * Implementation class for PolygonalSurfaceBuilder using OpenGeode data
     * structure
     */
    template < index_t dimension >
    class OpenGeodePolygonalSurfaceBuilder
        : public PolygonalSurfaceBuilder< dimension >
    {
    public:
        static constexpr auto dim = dimension;

        OpenGeodePolygonalSurfaceBuilder(
            VertexSet& vertex_set, MeshBuilderFactoryKey );

        explicit OpenGeodePolygonalSurfaceBuilder(
            OpenGeodePolygonalSurface< dimension >& mesh );

        OpenGeodePolygonalSurfaceBuilder(
            OpenGeodePolygonalSurfaceBuilder&& ) noexcept;

    private:
        void do_create_vertex() final;

        void do_create_vertices( index_t nb ) final;

        void do_delete_surface_vertices( const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) final;

        void do_permute_surface_vertices(
            absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) final;

        void do_set_polygon_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id ) final;

        void do_create_polygon( absl::Span< const index_t > vertices ) final;

        void do_delete_polygons( const std::vector< bool >& to_delete,
            absl::Span< const index_t > old2new ) final;

        void do_permute_polygons( absl::Span< const index_t > permutation,
            absl::Span< const index_t > old2new ) final;

        void do_set_polygon_adjacent(
            const PolygonEdge& polygon_edge, index_t adjacent_id ) final;

        void do_unset_polygon_adjacent( const PolygonEdge& polygon_edge ) final;

        void do_copy_points(
            const SurfaceMesh< dimension >& surface_mesh ) final;

        void do_copy_polygons(
            const SurfaceMesh< dimension >& surface_mesh ) final;

    private:
        OpenGeodePolygonalSurface< dimension >& geode_polygonal_surface_;
    };
    ALIAS_2D_AND_3D( OpenGeodePolygonalSurfaceBuilder );
} // namespace geode
