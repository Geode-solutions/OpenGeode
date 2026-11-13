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

#include <geode/mesh/builder/solid_edges_builder.hpp>
#include <geode/mesh/builder/solid_facets_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>

#define PYTHON_SOLID_MESH_BUILDER( dimension )                                 \
    const auto name##dimension =                                               \
        "SolidMeshBuilder" + std::to_string( dimension ) + "D";                \
    pybind11::class_< SolidMeshBuilder##dimension##D, VertexSetBuilder,        \
        CoordinateReferenceSystemManagersBuilder##dimension##D >(              \
        module, name##dimension.c_str(), "Builder to modify a solid mesh" )    \
        .def_static( "create",                                                 \
            geode::builder_creator( &SolidMeshBuilder##dimension##D::create ), \
            pybind11::arg( "mesh" ), pybind11::keep_alive< 0, 1 >(),           \
            "Return the builder to modify the solid mesh, which keeps the "    \
            "mesh alive" )                                                     \
        .def( "create_point", &SolidMeshBuilder##dimension##D::create_point,   \
            "Create a vertex with these coordinates and return its index",     \
            pybind11::arg( "point" ) )                                         \
        .def( "set_point",                                                     \
            geode::checked_builder< SolidMeshBuilder##dimension##D,            \
                VertexSet >( "set_point",                                      \
                &CoordinateReferenceSystemManagersBuilder<                     \
                    dimension >::set_point,                                    \
                geode::VertexCheck{} ),                                        \
            "Set the coordinates of an existing vertex in the active "         \
            "coordinate reference system",                                     \
            pybind11::arg( "vertex" ), pybind11::arg( "point" ) )              \
        .def( "create_polyhedron",                                             \
            &SolidMeshBuilder##dimension##D::create_polyhedron,                \
            "Create a polyhedron from its vertices and its facets, each "      \
            "facet being the list of the local indices, in vertices, of its "  \
            "vertices, and return the index of the polyhedron",                \
            pybind11::arg( "vertices" ), pybind11::arg( "facets" ) )           \
        .def( "set_polyhedron_vertex",                                         \
            geode::checked_builder< SolidMeshBuilder##dimension##D,            \
                SolidMesh< dimension > >( "set_polyhedron_vertex",             \
                &SolidMeshBuilder##dimension##D::set_polyhedron_vertex,        \
                geode::PolyhedronVertexCheck{}, geode::VertexCheck{} ),        \
            "Set the mesh vertex of a local vertex of a polyhedron",           \
            pybind11::arg( "polyhedron_vertex" ),                              \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "set_polyhedron_adjacent",                                       \
            geode::checked_builder< SolidMeshBuilder##dimension##D,            \
                SolidMesh< dimension > >( "set_polyhedron_adjacent",           \
                &SolidMeshBuilder##dimension##D::set_polyhedron_adjacent,      \
                geode::PolyhedronFacetCheck{}, geode::PolyhedronCheck{} ),     \
            "Set the polyhedron adjacent through a polyhedron facet",          \
            pybind11::arg( "polyhedron_facet" ),                               \
            pybind11::arg( "adjacent_id" ) )                                   \
        .def( "compute_polyhedron_adjacencies",                                \
            static_cast< void ( SolidMeshBuilder##dimension##D::* )() >(       \
                &SolidMeshBuilder##dimension##D::                              \
                    compute_polyhedron_adjacencies ),                          \
            pybind11::call_guard< pybind11::gil_scoped_release >(),            \
            "Compute the adjacencies between all the polyhedra" )              \
        .def( "delete_polyhedra",                                              \
            &SolidMeshBuilder##dimension##D::delete_polyhedra,                 \
            "Delete the polyhedra whose flag is True and return the mapping "  \
            "from the old polyhedron indices to the new ones, NO_ID for a "    \
            "deleted polyhedron; to_delete has one flag per polyhedron",       \
            pybind11::arg( "to_delete" ) )                                     \
        .def( "permute_polyhedra",                                             \
            &SolidMeshBuilder##dimension##D::permute_polyhedra,                \
            "Reorder the polyhedra so that the polyhedron at position i is "   \
            "the old polyhedron permutation[i], and return the mapping from "  \
            "the old polyhedron indices to the new ones",                      \
            pybind11::arg( "permutation" ) )                                   \
        .def( "delete_isolated_vertices",                                      \
            &SolidMeshBuilder##dimension##D::delete_isolated_vertices,         \
            "Delete the vertices that are not used by any polyhedron and "     \
            "return the mapping from the old vertex indices to the new ones, " \
            "NO_ID for a deleted vertex" )                                     \
        .def( "edges_builder", &SolidMeshBuilder##dimension##D::edges_builder, \
            pybind11::keep_alive< 0, 1 >(),                                    \
            "Return the builder of the edges of the solid, which keeps this "  \
            "builder alive" )                                                  \
        .def( "facets_builder",                                                \
            &SolidMeshBuilder##dimension##D::facets_builder,                   \
            pybind11::keep_alive< 0, 1 >(),                                    \
            "Return the builder of the facets of the solid, which keeps this " \
            "builder alive" )                                                  \
        .def( "associate_polyhedron_vertex_to_vertex",                         \
            &SolidMeshBuilder##dimension##D::                                  \
                associate_polyhedron_vertex_to_vertex,                         \
            "Record that the vertex is used by the polyhedron vertex, "        \
            "without modifying the polyhedron",                                \
            pybind11::arg( "polyhedron_vertex" ),                              \
            pybind11::arg( "vertex_id" ) )
namespace geode
{
    void define_solid_mesh_builder( pybind11::module& module )
    {
        PYTHON_SOLID_MESH_BUILDER( 3 );
    }
} // namespace geode
