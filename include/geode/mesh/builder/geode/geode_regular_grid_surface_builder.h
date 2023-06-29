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

#pragma once

#include <geode/geometry/point.h>

#include <geode/mesh/builder/regular_grid_surface_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodeRegularGrid );
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodeRegularGridBuilder );
    class VertexSet;
} // namespace geode

namespace geode
{
    /*!
     * Implementation class for RegularGridBuilder using OpenGeode data
     * structure
     */
    template <>
    class opengeode_mesh_api OpenGeodeRegularGridBuilder< 2 >
        : public RegularGridBuilder< 2 >
    {
    public:
        static constexpr index_t dim{ 2 };

        OpenGeodeRegularGridBuilder(
            VertexSet& vertex_set, MeshBuilderFactoryKey );

        OpenGeodeRegularGridBuilder( OpenGeodeRegularGrid< 2 >& mesh );

    private:
        void update_origin( const Point2D& origin ) final;

        void update_origin_and_directions(
            Point2D origin, std::array< Vector2D, 2 > directions ) final;

        void do_create_vertices( index_t /*unused*/ ) final;

        void do_create_vertex() final
        {
            throw OpenGeodeException{
                "[RegularGridBuilder] Cannot use create_vertex in RegularGrid"
            };
        }

        void do_permute_polygons( absl::Span< const index_t > /*unused*/,
            absl::Span< const index_t > /*unused*/ ) final
        {
            throw OpenGeodeException{ "[RegularGridBuilder] Cannot use "
                                      "permute_polygons in RegularGrid" };
        }

        void do_delete_surface_vertices( const std::vector< bool >& /*unused*/,
            absl::Span< const index_t > /*unused*/ ) final
        {
            throw OpenGeodeException{
                "[RegularGridBuilder] Cannot use delete_surface_vertices in "
                "RegularGrid"
            };
        }

        void do_permute_surface_vertices(
            absl::Span< const index_t > /*unused*/,
            absl::Span< const index_t > /*unused*/ ) final
        {
            throw OpenGeodeException{
                "[RegularGridBuilder] Cannot use permute_surface_vertices in "
                "RegularGrid"
            };
        }

        void do_set_polygon_vertex(
            const PolygonVertex& /*unused*/, index_t /*unused*/ ) final
        {
            throw OpenGeodeException{ "[RegularGridBuilder] Cannot use "
                                      "set_polygon_vertex in RegularGrid" };
        }

        void do_create_polygon( absl::Span< const index_t > /*unused*/ ) final
        {
            throw OpenGeodeException{
                "[RegularGridBuilder] Cannot use create_polygon in RegularGrid"
            };
        }

        void do_delete_polygons( const std::vector< bool >& /*unused*/,
            absl::Span< const index_t > /*unused*/ ) final
        {
            throw OpenGeodeException{
                "[RegularGridBuilder] Cannot use delete_polygons in RegularGrid"
            };
        }

        void do_set_polygon_adjacent(
            const PolygonEdge& /*unused*/, index_t /*unused*/ ) final
        {
            throw OpenGeodeException{ "[RegularGridBuilder] Cannot use "
                                      "set_polygon_adjacent in RegularGrid" };
        }

        void do_unset_polygon_adjacent( const PolygonEdge& /*unused*/ ) final
        {
            throw OpenGeodeException{ "[RegularGridBuilder] Cannot use "
                                      "unset_polygon_adjacent in RegularGrid" };
        }

        void do_copy_points( const SurfaceMesh< 2 >& surface_mesh ) final;

        void do_copy_polygons( const SurfaceMesh< 2 >& surface_mesh ) final;

    private:
        OpenGeodeRegularGrid< 2 >& geode_regular_grid_;
    };
    ALIAS_2D( OpenGeodeRegularGridBuilder );
} // namespace geode
