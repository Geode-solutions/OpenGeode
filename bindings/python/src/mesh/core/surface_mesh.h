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

#include <pybind11/operators.h>

#include <geode/geometry/vector.h>

#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/surface_mesh.h>
#include <geode/mesh/core/triangulated_surface.h>

#define PYTHON_SURFACE_MESH( dimension )                                       \
    const auto name##dimension =                                               \
        "SurfaceMesh" + std::to_string( dimension ) + "D";                     \
    pybind11::class_< SurfaceMesh##dimension##D, VertexSet >(                  \
        module, name##dimension.c_str() )                                      \
        .def( "are_edges_enabled",                                             \
            &SurfaceMesh##dimension##D::are_edges_enabled )                    \
        .def( "enable_edges", &SurfaceMesh##dimension##D::enable_edges )       \
        .def( "disable_edges", &SurfaceMesh##dimension##D::disable_edges )     \
        .def( "edges",                                                         \
            ( const SurfaceEdges##dimension##D& (                              \
                SurfaceMesh##dimension##D::*) () const )                       \
                & SurfaceMesh##dimension##D::edges,                            \
            pybind11::return_value_policy::reference )                         \
        .def( "point", &SurfaceMesh##dimension##D::point )                     \
        .def( "nb_polygons", &SurfaceMesh##dimension##D::nb_polygons )         \
        .def( "nb_polygon_vertices",                                           \
            &SurfaceMesh##dimension##D::nb_polygon_vertices )                  \
        .def(                                                                  \
            "nb_polygon_edges", &SurfaceMesh##dimension##D::nb_polygon_edges ) \
        .def( "polygon_vertex", &SurfaceMesh##dimension##D::polygon_vertex )   \
        .def(                                                                  \
            "polygon_vertices", &SurfaceMesh##dimension##D::polygon_vertices ) \
        .def( "polygon_edge_vertex",                                           \
            &SurfaceMesh##dimension##D::polygon_edge_vertex )                  \
        .def( "next_polygon_vertex",                                           \
            &SurfaceMesh##dimension##D::next_polygon_vertex )                  \
        .def( "previous_polygon_vertex",                                       \
            &SurfaceMesh##dimension##D::previous_polygon_vertex )              \
        .def( "next_polygon_edge",                                             \
            &SurfaceMesh##dimension##D::next_polygon_edge )                    \
        .def(                                                                  \
            "polygon_adjacent", &SurfaceMesh##dimension##D::polygon_adjacent ) \
        .def( "polygon_adjacent_edge",                                         \
            &SurfaceMesh##dimension##D::polygon_adjacent_edge )                \
        .def( "is_edge_on_border",                                             \
            &SurfaceMesh##dimension##D::is_edge_on_border )                    \
        .def( "polygon_edges_on_border",                                       \
            &SurfaceMesh##dimension##D::polygon_edges_on_border )              \
        .def( "next_on_border", &SurfaceMesh##dimension##D::next_on_border )   \
        .def( "previous_on_border",                                            \
            &SurfaceMesh##dimension##D::previous_on_border )                   \
        .def( "polygon_edge_length",                                           \
            ( double( SurfaceMesh##dimension##D::* )( const PolygonEdge& )     \
                    const )                                                    \
                & SurfaceMesh##dimension##D::edge_length )                     \
        .def( "edge_length", ( double( SurfaceMesh##dimension##D::* )(         \
                                 const std::array< index_t, 2 >& ) const )     \
                                 & SurfaceMesh##dimension##D::edge_length )    \
        .def( "polygon_edge_barycenter",                                       \
            ( Point< dimension >( SurfaceMesh##dimension##D::* )(              \
                const PolygonEdge& ) const )                                   \
                & SurfaceMesh##dimension##D::edge_barycenter )                 \
        .def( "edge_barycenter",                                               \
            ( Point< dimension >( SurfaceMesh##dimension##D::* )(              \
                const std::array< index_t, 2 >& ) const )                      \
                & SurfaceMesh##dimension##D::edge_barycenter )                 \
        .def( "polygon_barycenter",                                            \
            &SurfaceMesh##dimension##D::polygon_barycenter )                   \
        .def( "polygon_area", &SurfaceMesh##dimension##D::polygon_area )       \
        .def( "polygon_around_vertex",                                         \
            &SurfaceMesh##dimension##D::polygon_around_vertex )                \
        .def( "polygons_around_vertex",                                        \
            ( const PolygonsAroundVertex& (                                    \
                SurfaceMesh##dimension##D::*) ( index_t ) const )              \
                & SurfaceMesh##dimension##D::polygons_around_vertex )          \
        .def( "polygons_around_polygon_vertex",                                \
            ( const PolygonsAroundVertex& (                                    \
                SurfaceMesh##dimension##D::*) ( const PolygonVertex& ) const ) \
                & SurfaceMesh##dimension##D::polygons_around_vertex )          \
        .def( "polygon_edge_from_vertices",                                    \
            &SurfaceMesh##dimension##D::polygon_edge_from_vertices )           \
        .def( "polygons_from_edge_vertices",                                   \
            &SurfaceMesh##dimension##D::polygons_from_edge_vertices )          \
        .def( "polygon_attribute_manager",                                     \
            &SurfaceMesh##dimension##D::polygon_attribute_manager,             \
            pybind11::return_value_policy::reference )                         \
        .def( "bounding_box", &SurfaceMesh##dimension##D::bounding_box )       \
        .def_static(                                                           \
            "create", ( std::unique_ptr< SurfaceMesh##dimension##D >( * )() )  \
                          & SurfaceMesh##dimension##D::create )                \
        .def( "clone", &SurfaceMesh##dimension##D::clone )                     \
        .def( "is_triangulated_type",                                          \
            []( const SurfaceMesh< dimension >& surface ) {                    \
                return surface.type_name()                                     \
                       == TriangulatedSurface<                                 \
                           dimension >::type_name_static();                    \
            } )                                                                \
        .def( "is_polygonal_type",                                             \
            []( const SurfaceMesh< dimension >& surface ) {                    \
                return surface.type_name()                                     \
                       == PolygonalSurface< dimension >::type_name_static();   \
            } )

namespace geode
{
    void define_surface_mesh( pybind11::module& module )
    {
        PYTHON_SURFACE_MESH( 2 );
        PYTHON_SURFACE_MESH( 3 )
            .def( "polygon_normal", &SurfaceMesh3D::polygon_normal< 3 > )
            .def( "polygon_vertex_normal",
                &SurfaceMesh3D::polygon_vertex_normal< 3 > );

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
