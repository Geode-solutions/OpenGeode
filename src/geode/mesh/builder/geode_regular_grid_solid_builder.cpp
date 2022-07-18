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

#include <geode/mesh/builder/geode_regular_grid_solid_builder.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/geode_regular_grid_solid.h>

namespace geode
{
    OpenGeodeRegularGridBuilder< 3 >::OpenGeodeRegularGridBuilder(
        VertexSet& vertex_set, MeshBuilderFactoryKey )
        : OpenGeodeRegularGridBuilder< 3 >(
            dynamic_cast< OpenGeodeRegularGrid< 3 >& >( vertex_set ) )
    {
    }

    OpenGeodeRegularGridBuilder< 3 >::OpenGeodeRegularGridBuilder(
        OpenGeodeRegularGrid< 3 >& mesh )
        : RegularGridBuilder< 3 >( mesh ), geode_regular_grid_( mesh )
    {
    }

    void OpenGeodeRegularGridBuilder< 3 >::update_origin(
        const Point3D& origin )
    {
        geode_regular_grid_.update_origin( origin, {} );
    }

    void OpenGeodeRegularGridBuilder< 3 >::do_create_vertices(
        index_t /*unused*/ )
    {
        // Operation no needed for the RegularGrid
    }

    void OpenGeodeRegularGridBuilder< 3 >::do_copy_points(
        const SolidMesh< 3 >& /*unused*/ )
    {
        // Operation no needed for the RegularGrid
    }

    void OpenGeodeRegularGridBuilder< 3 >::do_copy_polyhedra(
        const SolidMesh< 3 >& /*unused*/ )
    {
        // Operation no needed for the RegularGrid
    }
} // namespace geode
