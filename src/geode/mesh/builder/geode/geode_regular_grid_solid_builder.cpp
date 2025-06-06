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

#include <geode/mesh/builder/geode/geode_regular_grid_solid_builder.hpp>

#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/mesh_builder_factory.hpp>
#include <geode/mesh/core/geode/geode_regular_grid_solid.hpp>

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

    OpenGeodeRegularGridBuilder< 3 >::OpenGeodeRegularGridBuilder(
        OpenGeodeRegularGridBuilder&& ) noexcept = default;

    void OpenGeodeRegularGridBuilder< 3 >::update_origin(
        const Point3D& origin )
    {
        geode_regular_grid_.update_origin( origin, {} );
        this->set_grid_origin( origin );
    }

    void OpenGeodeRegularGridBuilder< 3 >::update_origin_and_directions(
        Point3D origin, std::array< Vector3D, 3 > directions )
    {
        geode_regular_grid_.update_origin_and_directions(
            origin, directions, {} );
        this->set_grid_origin( std::move( origin ) );
        this->set_grid_directions( std::move( directions ) );
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
