/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/bounding_box.hpp>

#include <geode/mesh/core/edged_curve.hpp>

#define PYTHON_EDGED_CURVE( dimension )                                        \
    const auto name##dimension =                                               \
        "EdgedCurve" + std::to_string( dimension ) + "D";                      \
    pybind11::class_< EdgedCurve##dimension##D, Graph,                         \
        CoordinateReferenceSystemManagers##dimension##D >(                     \
        module, name##dimension.c_str() )                                      \
        .def_static( "create",                                                 \
            static_cast<                                                       \
                std::unique_ptr< EdgedCurve##dimension##D > ( * )() >(         \
                &EdgedCurve##dimension##D::create ) )                          \
        .def( "clone", &EdgedCurve##dimension##D::clone )                      \
        .def( "edge_length", &EdgedCurve##dimension##D::edge_length )          \
        .def( "edge_barycenter", &EdgedCurve##dimension##D::edge_barycenter )  \
        .def( "bounding_box", &EdgedCurve##dimension##D::bounding_box )        \
        .def( "segment", &EdgedCurve##dimension##D::segment )                  \
        .def( "vertices_around_vertex",                                        \
            &EdgedCurve##dimension##D::vertices_around_vertex )                \
        .def( "texture_manager", &EdgedCurve##dimension##D::texture_manager,   \
            pybind11::return_value_policy::reference )

namespace geode
{
    void define_edged_curve( pybind11::module& module )
    {
        PYTHON_EDGED_CURVE( 2 );
        PYTHON_EDGED_CURVE( 3 );
    }
} // namespace geode
