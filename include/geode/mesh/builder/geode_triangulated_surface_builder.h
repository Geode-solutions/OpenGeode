/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( OpenGeodeTriangulatedSurface );
} // namespace geode

namespace geode
{
    /*!
     * Implementation class for TriangulatedSurfaceBuilder using OpenGeode data
     * structure
     */
    template < index_t dimension >
    class OpenGeodeTriangulatedSurfaceBuilder
        : public TriangulatedSurfaceBuilder< dimension >
    {
    public:
        OpenGeodeTriangulatedSurfaceBuilder(
            TriangulatedSurface< dimension >& triangulated_surface )
            : TriangulatedSurfaceBuilder< dimension >( triangulated_surface ),
              geode_triangulated_surface_(
                  dynamic_cast< OpenGeodeTriangulatedSurface< dimension >& >(
                      triangulated_surface ) )
        {
        }

    private:
        void do_set_point(
            index_t vertex_id, const Point< dimension >& point ) override;

        void do_create_vertex() override;

        void do_create_vertices( index_t nb ) override;

        void do_delete_surface_vertices(
            const std::vector< bool >& to_delete ) override;

        void do_set_polygon_vertex(
            const PolygonVertex& polygon_vertex, index_t vertex_id ) override;

        void do_create_triangle(
            const std::array< index_t, 3 >& vertices ) override;

        void do_delete_polygons(
            const std::vector< bool >& to_delete ) override;

        void do_set_polygon_adjacent(
            const PolygonEdge& polygon_edge, index_t adjacent_id ) override;

    private:
        OpenGeodeTriangulatedSurface< dimension >& geode_triangulated_surface_;
    };
    ALIAS_2D_AND_3D( OpenGeodeTriangulatedSurfaceBuilder );
} // namespace geode
