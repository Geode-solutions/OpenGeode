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

#include <geode/geometry/vector.h>

#include <geode/mesh/builder/polygonal_surface_builder.h>

#define PYTHON_POLYGONAL_SURFACE_BUILDER( dimension )                          \
    const auto name##dimension =                                               \
        "PolygonalSurfaceBuilder" + std::to_string( dimension ) + "D";         \
    pybind11::class_< PolygonalSurfaceBuilder##dimension##D,                   \
        VertexSetBuilder >( module, name##dimension.c_str() )                  \
        .def( "create",                                                        \
            ( std::unique_ptr< PolygonalSurfaceBuilder##dimension##D >( * )(   \
                PolygonalSurface< dimension >& ) )                             \
                & PolygonalSurfaceBuilder##dimension##D::create )              \
        .def( "set_point", &PolygonalSurfaceBuilder##dimension##D::set_point ) \
        .def( "create_point",                                                  \
            &PolygonalSurfaceBuilder##dimension##D::create_point )             \
        .def( "create_polygon",                                                \
            &PolygonalSurfaceBuilder##dimension##D::create_polygon )           \
        .def( "set_polygon_vertex",                                            \
            &PolygonalSurfaceBuilder##dimension##D::set_polygon_vertex )       \
        .def( "set_polygon_adjacent",                                          \
            &PolygonalSurfaceBuilder##dimension##D::set_polygon_adjacent )     \
        .def( "compute_polygon_adjacencies",                                   \
            ( void ( PolygonalSurfaceBuilder##dimension##D::* )() )            \
                & PolygonalSurfaceBuilder##dimension##D::                      \
                    compute_polygon_adjacencies )                              \
        .def( "delete_polygons",                                               \
            &PolygonalSurfaceBuilder##dimension##D::delete_polygons )          \
        .def( "delete_isolated_vertices",                                      \
            &PolygonalSurfaceBuilder##dimension##D::delete_isolated_vertices ) \
        .def( "delete_isolated_edges",                                         \
            &PolygonalSurfaceBuilder##dimension##D::delete_isolated_edges )    \
        .def( "polygon_around_vertex",                                         \
            &PolygonalSurfaceBuilder##dimension##D::polygon_around_vertex )    \
        .def( "associate_polygon_vertex_to_vertex",                            \
            &PolygonalSurfaceBuilder##dimension##D::                           \
                associate_polygon_vertex_to_vertex )

namespace geode
{
    void define_polygonal_surface_builder( pybind11::module& module )
    {
        PYTHON_POLYGONAL_SURFACE_BUILDER( 2 );
        PYTHON_POLYGONAL_SURFACE_BUILDER( 3 );
    }
} // namespace geode
