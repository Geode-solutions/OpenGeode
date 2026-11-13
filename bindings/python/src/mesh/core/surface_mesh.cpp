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

#include <geode/basic/attribute_manager.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

#define PYTHON_SURFACE_MESH( dimension )                                       \
    const auto name##dimension =                                               \
        "SurfaceMesh" + std::to_string( dimension ) + "D";                     \
    pybind11::class_< SurfaceMesh##dimension##D, VertexSet,                    \
        CoordinateReferenceSystemManagers##dimension##D,                       \
        pybind11::smart_holder >( module, name##dimension.c_str(),             \
        "Surface made of polygons (triangles, quads, ...) whose vertices "     \
        "have coordinates" )                                                   \
        .def( "are_edges_enabled",                                             \
            &SurfaceMesh##dimension##D::are_edges_enabled,                     \
            "Return True if the edges of the surface are enabled" )            \
        .def( "enable_edges", &SurfaceMesh##dimension##D::enable_edges,        \
            "Compute and enable the edges of the surface, which are then "     \
            "available through edges()" )                                      \
        .def( "disable_edges", &SurfaceMesh##dimension##D::disable_edges,      \
            "Disable the edges of the surface and free them" )                 \
        .def( "edges",                                                         \
            static_cast< const SurfaceEdges##dimension##D& (                   \
                SurfaceMesh##dimension##D::*) () const >(                      \
                &SurfaceMesh##dimension##D::edges ),                           \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the edges of the surface; raise an error if they are not " \
            "enabled" )                                                        \
        .def( "nb_polygons", &SurfaceMesh##dimension##D::nb_polygons,          \
            "Return the number of polygons" )                                  \
        .def( "nb_polygon_vertices",                                           \
            geode::checked( "nb_polygon_vertices",                             \
                &SurfaceMesh##dimension##D::nb_polygon_vertices,               \
                geode::PolygonCheck{} ),                                       \
            "Return the number of vertices of the polygon",                    \
            pybind11::arg( "polygon_id" ) )                                    \
        .def( "nb_polygon_edges",                                              \
            geode::checked( "nb_polygon_edges",                                \
                &SurfaceMesh##dimension##D::nb_polygon_edges,                  \
                geode::PolygonCheck{} ),                                       \
            "Return the number of edges of the polygon",                       \
            pybind11::arg( "polygon_id" ) )                                    \
        .def( "polygon_vertex",                                                \
            geode::checked( "polygon_vertex",                                  \
                &SurfaceMesh##dimension##D::polygon_vertex,                    \
                geode::PolygonVertexCheck{} ),                                 \
            "Return the index in the mesh of a local vertex of a polygon",     \
            pybind11::arg( "polygon_vertex" ) )                                \
        .def( "polygon_vertices",                                              \
            geode::checked( "polygon_vertices",                                \
                &SurfaceMesh##dimension##D::polygon_vertices,                  \
                geode::PolygonCheck{} ),                                       \
            "Return the indices in the mesh of all the vertices of the "       \
            "polygon",                                                         \
            pybind11::arg( "polygon_id" ) )                                    \
        .def( "polygon_edge_vertex",                                           \
            geode::checked( "polygon_edge_vertex",                             \
                &SurfaceMesh##dimension##D::polygon_edge_vertex,               \
                geode::PolygonEdgeCheck{} ),                                   \
            "Return the index in the mesh of a vertex of a polygon edge, "     \
            "vertex_id being the local vertex of the edge (0 or 1)",           \
            pybind11::arg( "polygon_edge" ), pybind11::arg( "vertex_id" ) )    \
        .def( "next_polygon_vertex",                                           \
            geode::checked( "next_polygon_vertex",                             \
                &SurfaceMesh##dimension##D::next_polygon_vertex,               \
                geode::PolygonVertexCheck{} ),                                 \
            "Return the next vertex in the polygon (local indexation)",        \
            pybind11::arg( "polygon_vertex" ) )                                \
        .def( "previous_polygon_vertex",                                       \
            geode::checked( "previous_polygon_vertex",                         \
                &SurfaceMesh##dimension##D::previous_polygon_vertex,           \
                geode::PolygonVertexCheck{} ),                                 \
            "Return the previous vertex in the polygon (local indexation)",    \
            pybind11::arg( "polygon_vertex" ) )                                \
        .def( "next_polygon_edge",                                             \
            geode::checked( "next_polygon_edge",                               \
                &SurfaceMesh##dimension##D::next_polygon_edge,                 \
                geode::PolygonEdgeCheck{} ),                                   \
            "Return the next edge in the polygon (local indexation)",          \
            pybind11::arg( "polygon_edge" ) )                                  \
        .def( "polygon_adjacent",                                              \
            geode::checked( "polygon_adjacent",                                \
                &SurfaceMesh##dimension##D::polygon_adjacent,                  \
                geode::PolygonEdgeCheck{} ),                                   \
            "Return the index of the polygon adjacent through the edge, or "   \
            "None if there is none",                                           \
            pybind11::arg( "polygon_edge" ) )                                  \
        .def( "polygon_adjacent_edge",                                         \
            geode::checked( "polygon_adjacent_edge",                           \
                &SurfaceMesh##dimension##D::polygon_adjacent_edge,             \
                geode::PolygonEdgeCheck{} ),                                   \
            "Return the edge of the adjacent polygon through which the "       \
            "polygons are adjacent, or None if there is none",                 \
            pybind11::arg( "polygon_edge" ) )                                  \
        .def( "is_edge_on_border",                                             \
            geode::checked( "is_edge_on_border",                               \
                &SurfaceMesh##dimension##D::is_edge_on_border,                 \
                geode::PolygonEdgeCheck{} ),                                   \
            "Return True if the polygon has no adjacent polygon through the "  \
            "edge",                                                            \
            pybind11::arg( "polygon_edge" ) )                                  \
        .def( "polygon_edges_on_border",                                       \
            geode::checked( "polygon_edges_on_border",                         \
                &SurfaceMesh##dimension##D::polygon_edges_on_border,           \
                geode::PolygonCheck{} ),                                       \
            "Return the edges of the polygon that are on the border",          \
            pybind11::arg( "polygon_id" ) )                                    \
        .def( "next_on_border",                                                \
            geode::checked( "next_on_border",                                  \
                &SurfaceMesh##dimension##D::next_on_border,                    \
                geode::PolygonEdgeCheck{} ),                                   \
            "Return the next edge on the border; the given edge must be on "   \
            "the border",                                                      \
            pybind11::arg( "polygon_edge" ) )                                  \
        .def( "previous_on_border",                                            \
            geode::checked( "previous_on_border",                              \
                &SurfaceMesh##dimension##D::previous_on_border,                \
                geode::PolygonEdgeCheck{} ),                                   \
            "Return the previous edge on the border; the given edge must be "  \
            "on the border",                                                   \
            pybind11::arg( "polygon_edge" ) )                                  \
        .def( "polygon_edge_length",                                           \
            geode::checked( "polygon_edge_length",                             \
                static_cast< double ( SurfaceMesh##dimension##D::* )(          \
                    const PolygonEdge& ) const >(                              \
                    &SurfaceMesh##dimension##D::edge_length ),                 \
                geode::PolygonEdgeCheck{} ),                                   \
            "Return the length of the polygon edge",                           \
            pybind11::arg( "polygon_edge" ) )                                  \
        .def( "edge_length",                                                   \
            geode::checked( "edge_length",                                     \
                static_cast< double ( SurfaceMesh##dimension##D::* )(          \
                    const std::array< index_t, 2 >& ) const >(                 \
                    &SurfaceMesh##dimension##D::edge_length ),                 \
                geode::VertexCheck{} ),                                        \
            "Return the length of the edge between the two given vertices",    \
            pybind11::arg( "edge_vertices" ) )                                 \
        .def( "polygon_edge_barycenter",                                       \
            geode::checked( "polygon_edge_barycenter",                         \
                static_cast< Point< dimension > (                              \
                    SurfaceMesh##dimension##D::* )( const PolygonEdge& )       \
                        const >(                                               \
                    &SurfaceMesh##dimension##D::edge_barycenter ),             \
                geode::PolygonEdgeCheck{} ),                                   \
            "Return the barycenter of the polygon edge",                       \
            pybind11::arg( "polygon_edge" ) )                                  \
        .def( "edge_barycenter",                                               \
            geode::checked( "edge_barycenter",                                 \
                static_cast< Point< dimension > (                              \
                    SurfaceMesh##dimension##D::* )(                            \
                    const std::array< index_t, 2 >& ) const >(                 \
                    &SurfaceMesh##dimension##D::edge_barycenter ),             \
                geode::VertexCheck{} ),                                        \
            "Return the barycenter of the edge between the two given "         \
            "vertices",                                                        \
            pybind11::arg( "edge_vertices" ) )                                 \
        .def( "polygon_barycenter",                                            \
            geode::checked( "polygon_barycenter",                              \
                &SurfaceMesh##dimension##D::polygon_barycenter,                \
                geode::PolygonCheck{} ),                                       \
            "Return the barycenter of the polygon",                            \
            pybind11::arg( "polygon_id" ) )                                    \
        .def( "polygon_area",                                                  \
            geode::checked( "polygon_area",                                    \
                &SurfaceMesh##dimension##D::polygon_area,                      \
                geode::PolygonCheck{} ),                                       \
            "Return the area of the polygon, guaranteed only for a convex "    \
            "polygon",                                                         \
            pybind11::arg( "polygon_id" ) )                                    \
        .def( "vertices_around_vertex",                                        \
            geode::checked( "vertices_around_vertex",                          \
                &SurfaceMesh##dimension##D::vertices_around_vertex,            \
                geode::VertexCheck{} ),                                        \
            "Return the vertices linked to the vertex by a polygon edge; the " \
            "polygon adjacencies must be computed",                            \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "polygon_around_vertex",                                         \
            geode::checked( "polygon_around_vertex",                           \
                &SurfaceMesh##dimension##D::polygon_around_vertex,             \
                geode::VertexCheck{} ),                                        \
            "Return one polygon vertex located at the vertex, or None if no "  \
            "polygon uses it",                                                 \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "polygons_around_vertex",                                        \
            geode::checked( "polygons_around_vertex",                          \
                static_cast< const PolygonsAroundVertex& (                     \
                    SurfaceMesh##dimension##D::*) ( index_t ) const >(         \
                    &SurfaceMesh##dimension##D::polygons_around_vertex ),      \
                geode::VertexCheck{} ),                                        \
            "Return all the polygon vertices located at the vertex; the "      \
            "polygon adjacencies must be computed",                            \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "polygons_around_polygon_vertex",                                \
            geode::checked( "polygons_around_polygon_vertex",                  \
                static_cast< const PolygonsAroundVertex& (                     \
                    SurfaceMesh##dimension##D::*) ( const PolygonVertex& )     \
                        const >(                                               \
                    &SurfaceMesh##dimension##D::polygons_around_vertex ),      \
                geode::PolygonVertexCheck{} ),                                 \
            "Return all the polygon vertices located at the vertex of the "    \
            "given polygon vertex; the polygon adjacencies must be computed",  \
            pybind11::arg( "polygon_vertex" ) )                                \
        .def( "polygon_edge_from_vertices",                                    \
            geode::checked( "polygon_edge_from_vertices",                      \
                &SurfaceMesh##dimension##D::polygon_edge_from_vertices,        \
                geode::VertexCheck{}, geode::VertexCheck{} ),                  \
            "Return the polygon edge going from the first vertex to the "      \
            "second one, or None if there is none",                            \
            pybind11::arg( "from_vertex_id" ),                                 \
            pybind11::arg( "to_vertex_id" ) )                                  \
        .def( "polygon_edge_from_edge_vertices",                               \
            geode::checked( "polygon_edge_from_edge_vertices",                 \
                &SurfaceMesh##dimension##D::polygon_edge_from_edge_vertices,   \
                geode::VertexCheck{} ),                                        \
            "Return one polygon edge between the two vertices given in any "   \
            "order, or None if there is none",                                 \
            pybind11::arg( "edge_vertices" ) )                                 \
        .def( "polygons_from_edge_vertices",                                   \
            geode::checked( "polygons_from_edge_vertices",                     \
                &SurfaceMesh##dimension##D::polygons_from_edge_vertices,       \
                geode::VertexCheck{} ),                                        \
            "Return all the polygon edges between the two given vertices",     \
            pybind11::arg( "edge_vertices" ) )                                 \
        .def( "polygon_attribute_manager",                                     \
            &SurfaceMesh##dimension##D::polygon_attribute_manager,             \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the manager of the attributes associated with the "        \
            "polygons" )                                                       \
        .def( "is_vertex_on_border",                                           \
            geode::checked( "is_vertex_on_border",                             \
                &SurfaceMesh##dimension##D::is_vertex_on_border,               \
                geode::VertexCheck{} ),                                        \
            "Return True if some polygon around the vertex is on the border; " \
            "the polygon adjacencies must be computed",                        \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "texture_manager", &SurfaceMesh##dimension##D::texture_manager,  \
            pybind11::keep_alive< 0, 1 >(),                                    \
            "Return the manager of the textures of the surface mesh, which "   \
            "keeps the mesh alive" )                                           \
        .def( "bounding_box", &SurfaceMesh##dimension##D::bounding_box,        \
            "Return the bounding box of the vertices of the surface mesh" )    \
        .def_static( "create",                                                 \
            static_cast<                                                       \
                std::unique_ptr< SurfaceMesh##dimension##D > ( * )() >(        \
                &SurfaceMesh##dimension##D::create ),                          \
            "Create an empty surface mesh with the default data structure" )   \
        .def( "point",                                                         \
            geode::checked_as< SurfaceMesh##dimension##D >( "point",           \
                &CoordinateReferenceSystemManagers< dimension >::point,        \
                geode::VertexCheck{} ),                                        \
            "Return the coordinates of a vertex in the active coordinate "     \
            "reference system",                                                \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "clone", &SurfaceMesh##dimension##D::clone,                      \
            "Return a copy of the surface mesh, with the same data "           \
            "structure, name and identifier" )                                 \
        .def(                                                                  \
            "is_triangulated_type",                                            \
            []( const SurfaceMesh< dimension >& surface ) {                    \
                return surface.type_name()                                     \
                       == TriangulatedSurface<                                 \
                           dimension >::type_name_static();                    \
            },                                                                 \
            "Return True if the surface mesh is a triangulated surface" )      \
        .def(                                                                  \
            "is_polygonal_type",                                               \
            []( const SurfaceMesh< dimension >& surface ) {                    \
                return surface.type_name()                                     \
                       == PolygonalSurface< dimension >::type_name_static();   \
            },                                                                 \
            "Return True if the surface mesh is a polygonal surface" )

namespace geode
{
    void define_surface_mesh( pybind11::module& module )
    {
        PYTHON_SURFACE_MESH( 2 );
        PYTHON_SURFACE_MESH( 3 )
            .def( "polygon_normal",
                geode::checked( "polygon_normal",
                    &SurfaceMesh3D::polygon_normal< 3 >,
                    geode::PolygonCheck{} ),
                "Return the normal of the polygon, or None if the polygon is "
                "degenerated",
                pybind11::arg( "polygon_id" ) )
            .def( "polygon_vertex_normal",
                geode::checked( "polygon_vertex_normal",
                    &SurfaceMesh3D::polygon_vertex_normal< 3 >,
                    geode::VertexCheck{} ),
                "Return the normal at the vertex, the average of the normals "
                "of the polygons around it weighted by their angle at the "
                "vertex, or None if it cannot be computed; the polygon "
                "adjacencies must be computed",
                pybind11::arg( "vertex_id" ) );
    }
} // namespace geode
