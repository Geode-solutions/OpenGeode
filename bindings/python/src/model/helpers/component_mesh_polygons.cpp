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

#include <geode/model/helpers/component_mesh_polygons.hpp>

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace geode
{
    void define_component_mesh_polygons( pybind11::module& module )
    {
        auto cls_BlockPolyhedronFacet =
            pybind11::class_< BlockPolyhedronFacet >( module,
                "BlockPolyhedronFacet",
                "A polyhedron facet of a Block mesh together with the indices "
                "of its vertices in the Block mesh" )
                .def(
                    pybind11::init<>(), "Build an empty BlockPolyhedronFacet" )
                .def( pybind11::init< PolyhedronFacet >(),
                    pybind11::arg( "facet" ),
                    "Build a BlockPolyhedronFacet of the given facet, without "
                    "vertices yet" )

            ;
        def_readwrite_named( cls_BlockPolyhedronFacet, "facet",
            &BlockPolyhedronFacet::facet,
            "Polyhedron facet: polyhedron index and facet index in the Block "
            "mesh" );
        def_readwrite_named( cls_BlockPolyhedronFacet, "vertices",
            &BlockPolyhedronFacet::vertices,
            "Block mesh vertices of the facet; in the results of the helpers, "
            "they follow the order of the vertices of the Surface polygon" );

        auto cls_BlockPolyhedraFacetVertices =
            pybind11::class_< BlockPolyhedraFacetVertices >( module,
                "BlockPolyhedraFacetVertices",
                "The Block polyhedron facets coincident with a Surface "
                "polygon: the one with the same orientation and the one with "
                "the opposite orientation, each possibly None" )
                .def( pybind11::init<>(), "Build an empty result" )
                .def( "nb_facets", &BlockPolyhedraFacetVertices::nb_facets,
                    "Return the number of facets found, between 0 and 2" )

            ;
        def_readwrite_named( cls_BlockPolyhedraFacetVertices,
            "oriented_polyhedron_facet",
            &BlockPolyhedraFacetVertices::oriented_polyhedron_facet,
            "Block facet with the same orientation as the Surface polygon, or "
            "None" );
        def_readwrite_named( cls_BlockPolyhedraFacetVertices,
            "opposite_polyhedron_facet",
            &BlockPolyhedraFacetVertices::opposite_polyhedron_facet,
            "Block facet with the opposite orientation to the Surface polygon, "
            "or None" );

        auto cls_SurfacePolygonEdge =
            pybind11::class_< SurfacePolygonEdge >( module,
                "SurfacePolygonEdge",
                "A polygon edge of a Surface mesh (field facet) together with "
                "the indices of its two vertices in the Surface mesh (field "
                "vertices)" )
                .def( pybind11::init<>(), "Build an empty SurfacePolygonEdge" )
                .def( pybind11::init< PolygonEdge >(), pybind11::arg( "edge" ),
                    "Build a SurfacePolygonEdge of the given polygon edge, "
                    "without vertices yet" )

            ;
        def_readwrite_named( cls_SurfacePolygonEdge, "facet",
            &SurfacePolygonEdge::edge,
            "Polygon edge: polygon index and edge index in the Surface mesh" );
        def_readwrite_named( cls_SurfacePolygonEdge, "vertices",
            &SurfacePolygonEdge::vertices,
            "Surface mesh vertices of the two ends of the edge; in the results "
            "of the helpers, they follow the order of the vertices of the Line "
            "edge" );

        auto cls_SurfacePolygonsEdgeVertices =
            pybind11::class_< SurfacePolygonsEdgeVertices >( module,
                "SurfacePolygonsEdgeVertices",
                "The Surface polygon edges coincident with a Line edge: the "
                "one with the same orientation and the one with the opposite "
                "orientation, each possibly None" )
                .def( pybind11::init<>(), "Build an empty result" )
                .def( "nb_edges", &SurfacePolygonsEdgeVertices::nb_edges,
                    "Return the number of edges found, between 0 and 2" )

            ;
        def_readwrite_named( cls_SurfacePolygonsEdgeVertices, "oriented_edge",
            &SurfacePolygonsEdgeVertices::oriented_edge,
            "Surface edge with the same orientation as the Line edge, or "
            "None" );
        def_readwrite_named( cls_SurfacePolygonsEdgeVertices, "opposite_edge",
            &SurfacePolygonsEdgeVertices::opposite_edge,
            "Surface edge with the opposite orientation to the Line edge, or "
            "None" );

        module
            .def( "block_polygon_unique_vertices",
                static_cast< PolygonVertices ( * )( const BRep&, const Block3D&,
                    const PolyhedronFacet& ) >( &polygon_unique_vertices ),
                pybind11::arg( "model" ), pybind11::arg( "block" ),
                pybind11::arg( "facet" ),
                "Return the unique vertex of each vertex of the polyhedron "
                "facet of the Block mesh, NO_ID for a vertex without unique "
                "vertex" )
            .def( "surface_polygon_unique_vertices",
                static_cast< PolygonVertices ( * )( const BRep&,
                    const Surface3D&, index_t ) >( &polygon_unique_vertices ),
                pybind11::arg( "model" ), pybind11::arg( "surface" ),
                pybind11::arg( "polygon_id" ),
                "Return the unique vertex of each vertex of the polygon of the "
                "Surface mesh, NO_ID for a vertex without unique vertex" )
            .def( "block_mesh_polyhedra_from_surface_polygon",
                &block_mesh_polyhedra_from_surface_polygon,
                pybind11::arg( "model" ), pybind11::arg( "block" ),
                pybind11::arg( "surface" ), pybind11::arg( "polygon_id" ),
                "Return the polyhedron facets of the Block mesh whose vertices "
                "are identified with those of the Surface polygon (0 to 2 "
                "facets); only triangles and quadrangles are supported, other "
                "polygons raise an error" )
            .def( "block_vertices_from_surface_polygon",
                &block_vertices_from_surface_polygon, pybind11::arg( "model" ),
                pybind11::arg( "block" ), pybind11::arg( "surface" ),
                pybind11::arg( "polygon_id" ),
                "Return the BlockPolyhedronFacet coincident with the Surface "
                "polygon (0 to 2): each gives the polyhedron facet and, in "
                "'vertices', the Block mesh vertices matching the polygon "
                "vertices in the polygon order; only triangles and quadrangles "
                "are supported, other polygons raise an error" )
            .def( "oriented_block_vertices_from_surface_polygon",
                &oriented_block_vertices_from_surface_polygon,
                pybind11::arg( "model" ), pybind11::arg( "block" ),
                pybind11::arg( "surface" ), pybind11::arg( "polygon_id" ),
                "Same as block_vertices_from_surface_polygon, the facets being "
                "sorted: oriented_polyhedron_facet is the one with the same "
                "orientation as the Surface polygon, opposite_polyhedron_facet "
                "the one with the opposite orientation (None if absent)" )
            .def( "brep_surface_vertices_from_line_edge",
                static_cast< absl::InlinedVector< SurfacePolygonEdge, 2 > ( * )(
                    const BRep&, const Surface3D&, const Line3D&, index_t ) >(
                    &surface_vertices_from_line_edge ),
                pybind11::arg( "model" ), pybind11::arg( "surface" ),
                pybind11::arg( "line" ), pybind11::arg( "edge_id" ),
                "Return the SurfacePolygonEdge of the Surface coincident with "
                "the edge of the Line (0 to 2): each gives the polygon edge "
                "(in 'facet') and, in 'vertices', the Surface mesh vertices "
                "matching the Line edge vertices in the edge order; the edge "
                "vertices must be identified with Surface vertices (unique "
                "vertices)" )
            .def( "brep_oriented_surface_vertices_from_line_edge",
                static_cast< SurfacePolygonsEdgeVertices ( * )(
                    const BRep&, const Surface3D&, const Line3D&, index_t ) >(
                    &oriented_surface_vertices_from_line_edge ),
                pybind11::arg( "model" ), pybind11::arg( "surface" ),
                pybind11::arg( "line" ), pybind11::arg( "edge_id" ),
                "Same as brep_surface_vertices_from_line_edge, the edges being "
                "sorted: oriented_edge is the one with the same orientation as "
                "the Line edge, opposite_edge the one with the opposite "
                "orientation (None if absent)" )
            .def( "section_surface_vertices_from_line_edge",
                static_cast< absl::InlinedVector< SurfacePolygonEdge, 2 > ( * )(
                    const Section&, const Surface2D&, const Line2D&,
                    index_t ) >( &surface_vertices_from_line_edge ),
                pybind11::arg( "model" ), pybind11::arg( "surface" ),
                pybind11::arg( "line" ), pybind11::arg( "edge_id" ),
                "Return the SurfacePolygonEdge of the Surface coincident with "
                "the edge of the Line (0 to 2): each gives the polygon edge "
                "(in 'facet') and, in 'vertices', the Surface mesh vertices "
                "matching the Line edge vertices in the edge order; the edge "
                "vertices must be identified with Surface vertices (unique "
                "vertices)" )
            .def( "section_oriented_surface_vertices_from_line_edge",
                static_cast< SurfacePolygonsEdgeVertices ( * )( const Section&,
                    const Surface2D&, const Line2D&, index_t ) >(
                    &oriented_surface_vertices_from_line_edge ),
                pybind11::arg( "model" ), pybind11::arg( "surface" ),
                pybind11::arg( "line" ), pybind11::arg( "edge_id" ),
                "Same as section_surface_vertices_from_line_edge, the edges "
                "being sorted: oriented_edge is the one with the same "
                "orientation as the Line edge, opposite_edge the one with the "
                "opposite orientation (None if absent)" );
    }
} // namespace geode