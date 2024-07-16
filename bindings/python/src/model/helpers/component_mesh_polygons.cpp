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
        pybind11::class_< BlockPolyhedronFacet >(
            module, "BlockPolyhedronFacet" )
            .def( pybind11::init<>() )
            .def( pybind11::init< PolyhedronFacet >() )
            .def_readwrite( "facet", &BlockPolyhedronFacet::facet )
            .def_readwrite( "vertices", &BlockPolyhedronFacet::vertices );

        pybind11::class_< BlockPolyhedraFacetVertices >(
            module, "BlockPolyhedraFacetVertices" )
            .def( pybind11::init<>() )
            .def( "nb_facets", &BlockPolyhedraFacetVertices::nb_facets )
            .def_readwrite( "oriented_polyhedron_facet",
                &BlockPolyhedraFacetVertices::oriented_polyhedron_facet )
            .def_readwrite( "opposite_polyhedron_facet",
                &BlockPolyhedraFacetVertices::opposite_polyhedron_facet );

        pybind11::class_< SurfacePolygonEdge >( module, "SurfacePolygonEdge" )
            .def( pybind11::init<>() )
            .def( pybind11::init< PolygonEdge >() )
            .def_readwrite( "facet", &SurfacePolygonEdge::edge )
            .def_readwrite( "vertices", &SurfacePolygonEdge::vertices );

        pybind11::class_< SurfacePolygonsEdgeVertices >(
            module, "SurfacePolygonsEdgeVertices" )
            .def( pybind11::init<>() )
            .def( "nb_edges", &SurfacePolygonsEdgeVertices::nb_edges )
            .def_readwrite(
                "oriented_edge", &SurfacePolygonsEdgeVertices::oriented_edge )
            .def_readwrite(
                "opposite_edge", &SurfacePolygonsEdgeVertices::opposite_edge );

        module
            .def( "block_polygon_unique_vertices",
                static_cast< PolygonVertices ( * )( const BRep&, const Block3D&,
                    const PolyhedronFacet& ) >( &polygon_unique_vertices ) )
            .def( "surface_polygon_unique_vertices",
                static_cast< PolygonVertices ( * )( const BRep&,
                    const Surface3D&, index_t ) >( &polygon_unique_vertices ) )
            .def( "block_mesh_polyhedra_from_surface_polygon",
                &block_mesh_polyhedra_from_surface_polygon )
            .def( "block_vertices_from_surface_polygon",
                &block_vertices_from_surface_polygon )
            .def( "oriented_block_vertices_from_surface_polygon",
                &oriented_block_vertices_from_surface_polygon )
            .def( "brep_surface_vertices_from_line_edge",
                static_cast< absl::InlinedVector< SurfacePolygonEdge, 2 > ( * )(
                    const BRep&, const Surface3D&, const Line3D&, index_t ) >(
                    &surface_vertices_from_line_edge ) )
            .def( "brep_oriented_surface_vertices_from_line_edge",
                static_cast< SurfacePolygonsEdgeVertices ( * )(
                    const BRep&, const Surface3D&, const Line3D&, index_t ) >(
                    &oriented_surface_vertices_from_line_edge ) )
            .def( "section_surface_vertices_from_line_edge",
                static_cast< absl::InlinedVector< SurfacePolygonEdge, 2 > ( * )(
                    const Section&, const Surface2D&, const Line2D&,
                    index_t ) >( &surface_vertices_from_line_edge ) )
            .def( "section_oriented_surface_vertices_from_line_edge",
                static_cast< SurfacePolygonsEdgeVertices ( * )( const Section&,
                    const Surface2D&, const Line2D&, index_t ) >(
                    &oriented_surface_vertices_from_line_edge ) );
    }
} // namespace geode