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

#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/bounding_box.hpp>

#include <geode/mesh/core/edged_curve.hpp>

#define PYTHON_EDGED_CURVE( dimension )                                        \
    const auto name##dimension =                                               \
        "EdgedCurve" + std::to_string( dimension ) + "D";                      \
    pybind11::class_< EdgedCurve##dimension##D, Graph,                         \
        CoordinateReferenceSystemManagers##dimension##D,                       \
        pybind11::smart_holder >( module, name##dimension.c_str(),             \
        "Curve made of edges linking vertices that have coordinates" )         \
        .def_static( "create",                                                 \
            static_cast<                                                       \
                std::unique_ptr< EdgedCurve##dimension##D > ( * )() >(         \
                &EdgedCurve##dimension##D::create ),                           \
            "Create an empty edged curve with the default data structure" )    \
        .def( "point",                                                         \
            geode::checked_as< EdgedCurve##dimension##D >( "point",            \
                &CoordinateReferenceSystemManagers< dimension >::point,        \
                geode::VertexCheck{} ),                                        \
            "Return the coordinates of a vertex in the active coordinate "     \
            "reference system",                                                \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "clone", &EdgedCurve##dimension##D::clone,                       \
            "Return a copy of the edged curve, with the same data structure, " \
            "name and identifier" )                                            \
        .def( "edge_length",                                                   \
            geode::checked( "edge_length",                                     \
                &EdgedCurve##dimension##D::edge_length, geode::EdgeCheck{} ),  \
            "Return the length of the edge", pybind11::arg( "edge_id" ) )      \
        .def( "edge_barycenter",                                               \
            geode::checked( "edge_barycenter",                                 \
                &EdgedCurve##dimension##D::edge_barycenter,                    \
                geode::EdgeCheck{} ),                                          \
            "Return the barycenter of the edge", pybind11::arg( "edge_id" ) )  \
        .def( "bounding_box", &EdgedCurve##dimension##D::bounding_box,         \
            "Return the bounding box of the vertices of the edged curve" )     \
        .def( "segment",                                                       \
            geode::checked( "segment", &EdgedCurve##dimension##D::segment,     \
                geode::EdgeCheck{} ),                                          \
            pybind11::arg( "edge_id" ), pybind11::keep_alive< 0, 1 >(),        \
            "Return the segment geometry of the edge" )                        \
        .def( "vertices_around_vertex",                                        \
            geode::checked( "vertices_around_vertex",                          \
                &EdgedCurve##dimension##D::vertices_around_vertex,             \
                geode::VertexCheck{} ),                                        \
            "Return the vertices linked to the vertex by an edge",             \
            pybind11::arg( "vertex_id" ) )                                     \
        .def( "texture_manager", &EdgedCurve##dimension##D::texture_manager,   \
            pybind11::keep_alive< 0, 1 >(),                                    \
            "Return the manager of the textures of the edged curve, which "    \
            "keeps the mesh alive" )

namespace geode
{
    void define_edged_curve( pybind11::module& module )
    {
        PYTHON_EDGED_CURVE( 2 );
        PYTHON_EDGED_CURVE( 3 );
    }
} // namespace geode
