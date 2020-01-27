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

#include <pybind11/operators.h>

#include <geode/geometry/vector.h>

#include <geode/mesh/core/polygonal_surface.h>

#define PYTHON_POLYGONAL_SURFACE( dimension )                                  \
    const auto namebase##dimension =                                           \
        "PolygonalSurfaceBase" + std::to_string( dimension ) + "D";            \
    pybind11::class_< PolygonalSurfaceBase##dimension##D, VertexSet >(         \
        module, namebase##dimension.c_str() )                                  \
        .def( "point", &PolygonalSurfaceBase##dimension##D::point )            \
        .def( "nb_edges", &PolygonalSurfaceBase##dimension##D::nb_edges )      \
        .def(                                                                  \
            "nb_polygons", &PolygonalSurfaceBase##dimension##D::nb_polygons )  \
        .def( "nb_polygon_vertices",                                           \
            &PolygonalSurfaceBase##dimension##D::nb_polygon_vertices )         \
        .def( "nb_polygon_edges",                                              \
            &PolygonalSurfaceBase##dimension##D::nb_polygon_edges )            \
        .def( "polygon_vertex",                                                \
            &PolygonalSurfaceBase##dimension##D::polygon_vertex )              \
        .def( "polygon_edge",                                                  \
            &PolygonalSurfaceBase##dimension##D::polygon_edge )                \
        .def( "polygon_edge_vertex",                                           \
            &PolygonalSurfaceBase##dimension##D::polygon_edge_vertex )         \
        .def( "edge_vertices",                                                 \
            &PolygonalSurfaceBase##dimension##D::edge_vertices )               \
        .def( "next_polygon_vertex",                                           \
            &PolygonalSurfaceBase##dimension##D::next_polygon_vertex )         \
        .def( "previous_polygon_vertex",                                       \
            &PolygonalSurfaceBase##dimension##D::previous_polygon_vertex )     \
        .def( "next_polygon_edge",                                             \
            &PolygonalSurfaceBase##dimension##D::next_polygon_edge )           \
        .def( "polygon_adjacent",                                              \
            &PolygonalSurfaceBase##dimension##D::polygon_adjacent )            \
        .def( "polygon_adjacent_edge",                                         \
            &PolygonalSurfaceBase##dimension##D::polygon_adjacent_edge )       \
        .def( "is_edge_on_border",                                             \
            &PolygonalSurfaceBase##dimension##D::is_edge_on_border )           \
        .def( "polygon_edges_on_border",                                       \
            &PolygonalSurfaceBase##dimension##D::polygon_edges_on_border )     \
        .def( "next_on_border",                                                \
            &PolygonalSurfaceBase##dimension##D::next_on_border )              \
        .def( "previous_on_border",                                            \
            &PolygonalSurfaceBase##dimension##D::previous_on_border )          \
        .def(                                                                  \
            "edge_length", &PolygonalSurfaceBase##dimension##D::edge_length )  \
        .def( "edge_barycenter",                                               \
            &PolygonalSurfaceBase##dimension##D::edge_barycenter )             \
        .def( "polygon_barycenter",                                            \
            &PolygonalSurfaceBase##dimension##D::polygon_barycenter )          \
        .def( "polygon_area",                                                  \
            &PolygonalSurfaceBase##dimension##D::polygon_area )                \
        .def( "polygons_around_vertex",                                        \
            &PolygonalSurfaceBase##dimension##D::polygons_around_vertex )      \
        .def( "polygon_edge_from_vertices",                                    \
            &PolygonalSurfaceBase##dimension##D::polygon_edge_from_vertices )  \
        .def( "edge_from_vertices",                                            \
            &PolygonalSurfaceBase##dimension##D::edge_from_vertices )          \
        .def( "edge_attribute_manager",                                        \
            &PolygonalSurfaceBase##dimension##D::edge_attribute_manager,       \
            pybind11::return_value_policy::reference )                         \
        .def( "polygon_attribute_manager",                                     \
            &PolygonalSurfaceBase##dimension##D::polygon_attribute_manager,    \
            pybind11::return_value_policy::reference )                         \
        .def( "bounding_box",                                                  \
            &PolygonalSurfaceBase##dimension##D::bounding_box );               \
                                                                               \
    const auto name##dimension =                                               \
        "PolygonalSurface" + std::to_string( dimension ) + "D";                \
    pybind11::class_< PolygonalSurface##dimension##D,                          \
        PolygonalSurfaceBase##dimension##D >(                                  \
        module, name##dimension.c_str() )                                      \
        .def( "create",                                                        \
            ( std::unique_ptr< PolygonalSurface##dimension##D >( * )() )       \
                & PolygonalSurface##dimension##D::create )                     \
        .def( "clone", &PolygonalSurface##dimension##D::clone )

namespace geode
{
    void define_polygonal_surface( pybind11::module& module )
    {
        PYTHON_POLYGONAL_SURFACE( 2 );
        PYTHON_POLYGONAL_SURFACE( 3 )
            .def( "polygon_normal", &PolygonalSurface3D::polygon_normal )
            .def( "polygon_vertex_normal",
                &PolygonalSurface3D::polygon_vertex_normal );

        pybind11::class_< PolygonVertex >( module, "PolygonVertex" )
            .def( pybind11::init<>() )
            .def( pybind11::init< index_t, index_t >() )
            .def( pybind11::init< PolygonEdge >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def_readwrite( "polygon_id", &PolygonVertex::polygon_id )
            .def_readwrite( "vertex_id", &PolygonVertex::vertex_id );

        pybind11::class_< PolygonEdge >( module, "PolygonEdge" )
            .def( pybind11::init<>() )
            .def( pybind11::init< index_t, index_t >() )
            .def( pybind11::init< PolygonVertex >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def_readwrite( "polygon_id", &PolygonEdge::polygon_id )
            .def_readwrite( "edge_id", &PolygonEdge::edge_id );
    }
} // namespace geode
