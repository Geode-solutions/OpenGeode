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

#include <geode/mesh/core/surface_mesh.hpp>

namespace geode
{
    void define_surface_elements( pybind11::module& module )
    {
        auto vertex = define_string_representation(
            pybind11::class_< PolygonVertex >( module, "PolygonVertex",
                "Local vertex of a polygon: the polygon index and the index "
                "of the vertex in the polygon" ) );
        auto edge = define_string_representation(
            pybind11::class_< PolygonEdge >( module, "PolygonEdge",
                "Local edge of a polygon: the polygon index and the index of "
                "the edge in the polygon, edge i going from the vertex i to "
                "the vertex i + 1" ) );
        vertex
            .def( pybind11::init<>(),
                "Build a polygon vertex with invalid indices (NO_ID, NO_LID)" )
            .def( pybind11::init< index_t, index_t >(),
                pybind11::arg( "polygon_id" ), pybind11::arg( "vertex_id" ),
                "Build the local vertex vertex_id of the polygon polygon_id" )
            .def( pybind11::init< PolygonEdge >(),
                pybind11::arg( "polygon_edge" ),
                "Build the polygon vertex at the start of the polygon edge" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__", []( const PolygonVertex& value ) {
                return absl::HashOf( value.polygon_id, value.vertex_id );
            } );
        def_readwrite_named( vertex, "polygon_id", &PolygonVertex::polygon_id,
            "Index of the polygon" );
        def_readwrite_named( vertex, "vertex_id", &PolygonVertex::vertex_id,
            "Index of the vertex in the polygon" );
        edge.def( pybind11::init<>(),
                "Build a polygon edge with invalid indices (NO_ID, NO_LID)" )
            .def( pybind11::init< index_t, index_t >(),
                pybind11::arg( "polygon_id" ), pybind11::arg( "edge_id" ),
                "Build the local edge edge_id of the polygon polygon_id" )
            .def( pybind11::init< PolygonVertex >(),
                pybind11::arg( "polygon_vertex" ),
                "Build the polygon edge starting at the polygon vertex" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__", []( const PolygonEdge& value ) {
                return absl::HashOf( value.polygon_id, value.edge_id );
            } );
        def_readwrite_named( edge, "polygon_id", &PolygonEdge::polygon_id,
            "Index of the polygon" );
        def_readwrite_named( edge, "edge_id", &PolygonEdge::edge_id,
            "Index of the edge in the polygon" );
    }
} // namespace geode
