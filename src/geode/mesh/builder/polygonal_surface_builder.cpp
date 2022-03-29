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

#include <geode/mesh/builder/polygonal_surface_builder.h>

#include <numeric>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/detail/mapping_after_deletion.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/polygonal_surface.h>

namespace geode
{
    template < index_t dimension >
    PolygonalSurfaceBuilder< dimension >::PolygonalSurfaceBuilder(
        PolygonalSurface< dimension >& mesh )
        : SurfaceMeshBuilder< dimension >( mesh ), polygonal_surface_( mesh )
    {
    }

    template < index_t dimension >
    std::unique_ptr< PolygonalSurfaceBuilder< dimension > >
        PolygonalSurfaceBuilder< dimension >::create(
            PolygonalSurface< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            PolygonalSurfaceBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    void PolygonalSurfaceBuilder< dimension >::copy(
        const PolygonalSurface< dimension >& polygonal_surface )
    {
        OPENGEODE_EXCEPTION( polygonal_surface_.nb_vertices() == 0
                                 && polygonal_surface_.nb_polygons() == 0,
            "[PolygonalSurfaceBuilder::copy] Cannot copy a mesh into an "
            "already initialized mesh." );
        SurfaceMeshBuilder< dimension >::copy( polygonal_surface );
    }

    template class opengeode_mesh_api PolygonalSurfaceBuilder< 2 >;
    template class opengeode_mesh_api PolygonalSurfaceBuilder< 3 >;
} // namespace geode
