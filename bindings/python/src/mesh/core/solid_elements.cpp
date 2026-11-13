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

#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    void define_solid_elements( pybind11::module& module )
    {
        auto vertex = define_string_representation(
            pybind11::class_< PolyhedronVertex >( module, "PolyhedronVertex",
                "Local vertex of a polyhedron: the polyhedron index and the "
                "index of the vertex in the polyhedron" ),
            []( const PolyhedronVertex& value ) {
                return python_call_repr( "PolyhedronVertex",
                    { "polyhedron_id=" + python_repr( value.polyhedron_id ),
                        "vertex_id=" + python_repr( value.vertex_id ) } );
            } );
        auto facet = define_string_representation(
            pybind11::class_< PolyhedronFacet >( module, "PolyhedronFacet",
                "Local facet of a polyhedron: the polyhedron index and the "
                "index of the facet in the polyhedron" ),
            []( const PolyhedronFacet& value ) {
                return python_call_repr( "PolyhedronFacet",
                    { "polyhedron_id=" + python_repr( value.polyhedron_id ),
                        "facet_id=" + python_repr( value.facet_id ) } );
            } );
        auto facet_vertex = define_string_representation(
            pybind11::class_< PolyhedronFacetVertex >( module,
                "PolyhedronFacetVertex",
                "Local vertex of a polyhedron facet: the polyhedron facet and "
                "the index of the vertex in the facet" ),
            []( const PolyhedronFacetVertex& value ) {
                return python_call_repr( "PolyhedronFacetVertex",
                    { "facet=" + python_repr( value.polyhedron_facet ),
                        "vertex_id=" + python_repr( value.vertex_id ) } );
            } );
        auto facet_edge = define_string_representation(
            pybind11::class_< PolyhedronFacetEdge >( module,
                "PolyhedronFacetEdge",
                "Local edge of a polyhedron facet: the polyhedron facet and "
                "the index of the edge in the facet" ),
            []( const PolyhedronFacetEdge& value ) {
                return python_call_repr( "PolyhedronFacetEdge",
                    { "facet=" + python_repr( value.polyhedron_facet ),
                        "edge_id=" + python_repr( value.edge_id ) } );
            } );
        vertex
            .def( pybind11::init<>(),
                "Build a polyhedron vertex with invalid indices (NO_ID, "
                "NO_LID)" )
            .def( pybind11::init< index_t, index_t >(),
                pybind11::arg( "polyhedron_id" ), pybind11::arg( "vertex_id" ),
                "Build the local vertex vertex_id of the polyhedron "
                "polyhedron_id" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__", []( const PolyhedronVertex& value ) {
                return absl::HashOf( value.polyhedron_id, value.vertex_id );
            } );
        def_readwrite_named( vertex, "polyhedron_id",
            &PolyhedronVertex::polyhedron_id, "Index of the polyhedron" );
        def_readwrite_named( vertex, "vertex_id", &PolyhedronVertex::vertex_id,
            "Index of the vertex in the polyhedron" );
        facet
            .def( pybind11::init<>(),
                "Build a polyhedron facet with invalid indices (NO_ID, "
                "NO_LID)" )
            .def( pybind11::init< index_t, index_t >(),
                pybind11::arg( "polyhedron_id" ), pybind11::arg( "facet_id" ),
                "Build the local facet facet_id of the polyhedron "
                "polyhedron_id" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__", []( const PolyhedronFacet& value ) {
                return absl::HashOf( value.polyhedron_id, value.facet_id );
            } );
        def_readwrite_named( facet, "polyhedron_id",
            &PolyhedronFacet::polyhedron_id, "Index of the polyhedron" );
        def_readwrite_named( facet, "facet_id", &PolyhedronFacet::facet_id,
            "Index of the facet in the polyhedron" );
        facet_vertex
            .def( pybind11::init<>(),
                "Build a polyhedron facet vertex with invalid indices" )
            .def( pybind11::init< PolyhedronFacet, index_t >(),
                pybind11::arg( "facet" ), pybind11::arg( "vertex_id" ),
                "Build the local vertex vertex_id of the polyhedron facet" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__", []( const PolyhedronFacetVertex& value ) {
                return absl::HashOf( value.polyhedron_facet.polyhedron_id,
                    value.polyhedron_facet.facet_id, value.vertex_id );
            } );
        def_readwrite_named( facet_vertex, "polyhedron_facet",
            &PolyhedronFacetVertex::polyhedron_facet,
            "Polyhedron facet containing the vertex" );
        def_readwrite_named( facet_vertex, "vertex_id",
            &PolyhedronFacetVertex::vertex_id,
            "Index of the vertex in the facet" );
        facet_edge
            .def( pybind11::init<>(),
                "Build a polyhedron facet edge with invalid indices" )
            .def( pybind11::init< PolyhedronFacet, local_index_t >(),
                pybind11::arg( "facet" ), pybind11::arg( "edge_id" ),
                "Build the local edge edge_id of the polyhedron facet" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__", []( const PolyhedronFacetEdge& value ) {
                return absl::HashOf( value.polyhedron_facet.polyhedron_id,
                    value.polyhedron_facet.facet_id, value.edge_id );
            } );
        def_readwrite_named( facet_edge, "polyhedron_facet",
            &PolyhedronFacetEdge::polyhedron_facet,
            "Polyhedron facet containing the edge" );
        def_readwrite_named( facet_edge, "edge_id",
            &PolyhedronFacetEdge::edge_id, "Index of the edge in the facet" );
    }
} // namespace geode
