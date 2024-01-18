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

#include "../../common.h"

#include <geode/geometry/point.h>

#include <geode/mesh/builder/surface_edges_builder.h>
#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/surface_mesh.h>

#define PYTHON_SURFACE_MESH_BUILDER( dimension )                               \
    const auto name##dimension =                                               \
        "SurfaceMeshBuilder" + std::to_string( dimension ) + "D";              \
    pybind11::class_< SurfaceMeshBuilder##dimension##D, VertexSetBuilder,      \
        CoordinateReferenceSystemManagersBuilder##dimension##D >(              \
        module, name##dimension.c_str() )                                      \
        .def_static( "create", &SurfaceMeshBuilder##dimension##D::create )     \
        .def(                                                                  \
            "create_point", &SurfaceMeshBuilder##dimension##D::create_point )  \
        .def( "create_polygon",                                                \
            &SurfaceMeshBuilder##dimension##D::create_polygon )                \
        .def( "set_polygon_vertex",                                            \
            &SurfaceMeshBuilder##dimension##D::set_polygon_vertex )            \
        .def( "set_polygon_adjacent",                                          \
            &SurfaceMeshBuilder##dimension##D::set_polygon_adjacent )          \
        .def( "compute_polygon_adjacencies",                                   \
            static_cast< void ( SurfaceMeshBuilder##dimension##D::* )() >(     \
                &SurfaceMeshBuilder##dimension##D::                            \
                    compute_polygon_adjacencies ) )                            \
        .def( "delete_polygons",                                               \
            &SurfaceMeshBuilder##dimension##D::delete_polygons )               \
        .def( "permute_polygons",                                              \
            &SurfaceMeshBuilder##dimension##D::permute_polygons )              \
        .def( "delete_isolated_vertices",                                      \
            &SurfaceMeshBuilder##dimension##D::delete_isolated_vertices )      \
        .def( "edges_builder",                                                 \
            &SurfaceMeshBuilder##dimension##D::edges_builder )

namespace geode
{
    void define_surface_mesh_builder( pybind11::module& module )
    {
        PYTHON_SURFACE_MESH_BUILDER( 2 );
        PYTHON_SURFACE_MESH_BUILDER( 3 );
    }
} // namespace geode
