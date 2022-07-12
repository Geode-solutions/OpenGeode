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

#include <geode/mesh/builder/geode_regular_grid_surface_builder.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/geode_regular_grid_surface.h>

namespace geode
{
    OpenGeodeRegularGridBuilder< 2 >::OpenGeodeRegularGridBuilder(
        VertexSet& vertex_set, MeshBuilderFactoryKey )
        : OpenGeodeRegularGridBuilder< 2 >(
            dynamic_cast< OpenGeodeRegularGrid< 2 >& >( vertex_set ) )
    {
    }

    OpenGeodeRegularGridBuilder< 2 >::OpenGeodeRegularGridBuilder(
        OpenGeodeRegularGrid< 2 >& mesh )
        : RegularGridBuilder< 2 >( mesh ), geode_regular_grid_( mesh )
    {
    }

    void OpenGeodeRegularGridBuilder< 2 >::update_origin(
        const Point2D& origin )
    {
        geode_regular_grid_.update_origin( origin, {} );
    }

    void OpenGeodeRegularGridBuilder< 2 >::do_set_point(
        index_t vertex_id, Point2D point )
    {
        geode_regular_grid_.set_vertex( vertex_id, std::move( point ), {} );
    }

    void OpenGeodeRegularGridBuilder< 2 >::do_create_vertices(
        index_t /*unsued*/ )
    {
    }

        void OpenGeodeRegularGridBuilder< 2 >::do_copy_points(
        const SurfaceMesh< 2 >& /*unused*/ )
    {
    }

    void OpenGeodeRegularGridBuilder< 2 >::do_copy_polygons(
        const SurfaceMesh< 2 >& /*unused*/ )
    {
    }
} // namespace geode
