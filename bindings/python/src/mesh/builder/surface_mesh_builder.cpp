/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include "../../common.hpp"
#include "../index_check.hpp"

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/surface_edges_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#define PYTHON_SURFACE_MESH_BUILDER( dimension )                               \
    const auto name##dimension =                                               \
        "SurfaceMeshBuilder" + std::to_string( dimension ) + "D";              \
    pybind11::class_< SurfaceMeshBuilder##dimension##D, VertexSetBuilder,      \
        CoordinateReferenceSystemManagersBuilder##dimension##D >(              \
        module, name##dimension.c_str(), "Builder to modify a surface mesh" )  \
        .def_static( "create",                                                 \
            geode::builder_creator(                                            \
                &SurfaceMeshBuilder##dimension##D::create ),                   \
            pybind11::arg( "mesh" ), pybind11::keep_alive< 0, 1 >(),           \
            "Return the builder to modify the surface mesh, which keeps the "  \
            "mesh alive" )                                                     \
        .def( "create_point", &SurfaceMeshBuilder##dimension##D::create_point, \
            "Create a vertex with these coordinates and return its index",     \
            pybind11::arg( "point" ) )                                         \
        .def( "set_point",                                                     \
            geode::checked_builder< SurfaceMeshBuilder##dimension##D,          \
                VertexSet >( "set_point",                                      \
                &CoordinateReferenceSystemManagersBuilder<                     \
                    dimension >::set_point,                                    \
                geode::VertexCheck{} ),                                        \
            "Set the coordinates of an existing vertex in the active "         \
            "coordinate reference system",                                     \
            pybind11::arg( "vertex" ), pybind11::arg( "point" ) )              \
        .def( "create_polygon",                                                \
            &SurfaceMeshBuilder##dimension##D::create_polygon,                 \
            "Create a polygon from its ordered vertices and return its index", \
            pybind11::arg( "vertices" ) )                                      \
        .def( "set_polygon_vertex",                                            \
            geode::checked_builder< SurfaceMeshBuilder##dimension##D,          \
                SurfaceMesh< dimension > >( "set_polygon_vertex",              \
                &SurfaceMeshBuilder##dimension##D::set_polygon_vertex,         \
                geode::PolygonVertexCheck{}, geode::VertexCheck{} ),           \
            "Set the mesh vertex of a local vertex of a polygon",              \
            pybind11::arg( "polygon_vertex" ), pybind11::arg( "vertex_id" ) )  \
        .def( "set_polygon_adjacent",                                          \
            geode::checked_builder< SurfaceMeshBuilder##dimension##D,          \
                SurfaceMesh< dimension > >( "set_polygon_adjacent",            \
                &SurfaceMeshBuilder##dimension##D::set_polygon_adjacent,       \
                geode::PolygonEdgeCheck{}, geode::PolygonCheck{} ),            \
            "Set the polygon adjacent through a polygon edge",                 \
            pybind11::arg( "polygon_edge" ), pybind11::arg( "adjacent_id" ) )  \
        .def( "compute_polygon_adjacencies",                                   \
            static_cast< void ( SurfaceMeshBuilder##dimension##D::* )() >(     \
                &SurfaceMeshBuilder##dimension##D::                            \
                    compute_polygon_adjacencies ),                             \
            pybind11::call_guard< pybind11::gil_scoped_release >(),            \
            "Compute the adjacencies between all the polygons" )               \
        .def( "delete_polygons",                                               \
            &SurfaceMeshBuilder##dimension##D::delete_polygons,                \
            "Delete the polygons whose flag is True and return the mapping "   \
            "from the old polygon indices to the new ones, NO_ID for a "       \
            "deleted polygon; to_delete has one flag per polygon",             \
            pybind11::arg( "to_delete" ) )                                     \
        .def( "permute_polygons",                                              \
            &SurfaceMeshBuilder##dimension##D::permute_polygons,               \
            "Reorder the polygons so that the polygon at position i is the "   \
            "old polygon permutation[i], and return the mapping from the old " \
            "polygon indices to the new ones",                                 \
            pybind11::arg( "permutation" ) )                                   \
        .def( "delete_isolated_vertices",                                      \
            &SurfaceMeshBuilder##dimension##D::delete_isolated_vertices,       \
            "Delete the vertices that are not used by any polygon and return " \
            "the mapping from the old vertex indices to the new ones, NO_ID "  \
            "for a deleted vertex" )                                           \
        .def( "edges_builder",                                                 \
            &SurfaceMeshBuilder##dimension##D::edges_builder,                  \
            pybind11::keep_alive< 0, 1 >(),                                    \
            "Return the builder of the edges of the surface, which keeps "     \
            "this builder alive" )

namespace geode
{
    void define_surface_mesh_builder( pybind11::module& module )
    {
        PYTHON_SURFACE_MESH_BUILDER( 2 );
        PYTHON_SURFACE_MESH_BUILDER( 3 );
    }
} // namespace geode
