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
        auto vertex =
            pybind11::class_< PolygonVertex >( module, "PolygonVertex" );
        auto edge = pybind11::class_< PolygonEdge >( module, "PolygonEdge" );

        vertex.def( pybind11::init<>() )
            .def( pybind11::init< index_t, index_t >() )
            .def( pybind11::init< PolygonEdge >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "string", &PolygonVertex::string )
            .def_readwrite( "polygon_id", &PolygonVertex::polygon_id )
            .def_readwrite( "vertex_id", &PolygonVertex::vertex_id );

        edge.def( pybind11::init<>() )
            .def( pybind11::init< index_t, index_t >() )
            .def( pybind11::init< PolygonVertex >() )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "string", &PolygonEdge::string )
            .def_readwrite( "polygon_id", &PolygonEdge::polygon_id )
            .def_readwrite( "edge_id", &PolygonEdge::edge_id );
    }
} // namespace geode
