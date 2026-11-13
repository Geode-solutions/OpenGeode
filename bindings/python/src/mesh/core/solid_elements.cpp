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
        define_string_representation(
            pybind11::class_< PolyhedronVertex >( module, "PolyhedronVertex" ) )
            .def( pybind11::init<>() )
            .def( pybind11::init< index_t, index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "__hash__",
                []( const PolyhedronVertex& value ) {
                    return absl::HashOf( value.polyhedron_id, value.vertex_id );
                } )
            .def_readwrite( "polyhedron_id", &PolyhedronVertex::polyhedron_id )
            .def_readwrite( "vertex_id", &PolyhedronVertex::vertex_id );

        define_string_representation(
            pybind11::class_< PolyhedronFacet >( module, "PolyhedronFacet" ) )
            .def( pybind11::init<>() )
            .def( pybind11::init< index_t, index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "__hash__",
                []( const PolyhedronFacet& value ) {
                    return absl::HashOf( value.polyhedron_id, value.facet_id );
                } )
            .def_readwrite( "polyhedron_id", &PolyhedronFacet::polyhedron_id )
            .def_readwrite( "facet_id", &PolyhedronFacet::facet_id );

        define_string_representation( pybind11::class_< PolyhedronFacetVertex >(
                                          module, "PolyhedronFacetVertex" ) )
            .def( pybind11::init<>() )
            .def( pybind11::init< PolyhedronFacet, index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "__hash__",
                []( const PolyhedronFacetVertex& value ) {
                    return absl::HashOf( value.polyhedron_facet.polyhedron_id,
                        value.polyhedron_facet.facet_id, value.vertex_id );
                } )
            .def_readwrite(
                "polyhedron_facet", &PolyhedronFacetVertex::polyhedron_facet )
            .def_readwrite( "vertex_id", &PolyhedronFacetVertex::vertex_id );

        define_string_representation( pybind11::class_< PolyhedronFacetEdge >(
                                          module, "PolyhedronFacetEdge" ) )
            .def( pybind11::init<>() )
            .def( pybind11::init< PolyhedronFacet, local_index_t >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "__hash__",
                []( const PolyhedronFacetEdge& value ) {
                    return absl::HashOf( value.polyhedron_facet.polyhedron_id,
                        value.polyhedron_facet.facet_id, value.edge_id );
                } )
            .def_readwrite(
                "polyhedron_facet", &PolyhedronFacetEdge::polyhedron_facet )
            .def_readwrite( "edge_id", &PolyhedronFacetEdge::edge_id );
    }
} // namespace geode
