/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/builder/solid_mesh_builder.h>

#define PYTHON_SOLID_MESH_BUILDER( dimension )                                 \
    const auto name##dimension =                                               \
        "SolidMeshBuilder" + std::to_string( dimension ) + "D";                \
    pybind11::class_< SolidMeshBuilder##dimension##D, VertexSetBuilder >(      \
        module, name##dimension.c_str() )                                      \
        .def_static( "create",                                                 \
            ( std::unique_ptr< SolidMeshBuilder##dimension##D >( * )(          \
                SolidMesh< dimension >& ) )                                    \
                & SolidMeshBuilder##dimension##D::create )                     \
        .def( "set_point", &SolidMeshBuilder##dimension##D::set_point )        \
        .def( "create_point", &SolidMeshBuilder##dimension##D::create_point )  \
        .def( "create_polyhedron",                                             \
            &SolidMeshBuilder##dimension##D::create_polyhedron )               \
        .def( "set_polyhedron_vertex",                                         \
            &SolidMeshBuilder##dimension##D::set_polyhedron_vertex )           \
        .def( "set_polyhedron_adjacent",                                       \
            &SolidMeshBuilder##dimension##D::set_polyhedron_adjacent )         \
        .def( "compute_polyhedron_adjacencies",                                \
            ( void ( SolidMeshBuilder##dimension##D::* )() )                   \
                & SolidMeshBuilder##dimension##D::                             \
                    compute_polyhedron_adjacencies )                           \
        .def( "delete_polyhedra",                                              \
            &SolidMeshBuilder##dimension##D::delete_polyhedra )                \
        .def( "delete_isolated_vertices",                                      \
            &SolidMeshBuilder##dimension##D::delete_isolated_vertices )        \
        .def( "delete_isolated_facets",                                        \
            &SolidMeshBuilder##dimension##D::delete_isolated_facets )          \
        .def( "delete_isolated_edges",                                         \
            &SolidMeshBuilder##dimension##D::delete_isolated_edges )           \
        .def( "associate_polyhedron_vertex_to_vertex",                         \
            &SolidMeshBuilder##dimension##D::                                  \
                associate_polyhedron_vertex_to_vertex )
namespace geode
{
    void define_solid_mesh_builder( pybind11::module& module )
    {
        PYTHON_SOLID_MESH_BUILDER( 3 );
    }
} // namespace geode
