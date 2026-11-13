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

#include "../common.hpp"

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/quality.hpp>

#define PYTHON_TRIANGLE_QUALITY( dimension )                                   \
    const auto triangle_angle_based_quality##dimension =                       \
        "triangle_angle_based_quality" + std::to_string( dimension ) + "D";    \
    module.def( triangle_angle_based_quality##dimension.c_str(),               \
        &triangle_angle_based_quality< dimension >,                            \
        pybind11::arg( "triangle" ),                                           \
        "Return 4 sin(a0) sin(a1) sin(a2) / (sin(a0) + sin(a1) + sin(a2)) "    \
        "where ai are the angles of the triangle: 1 for an equilateral "       \
        "triangle, 0 for a degenerated one" )

namespace geode
{
    void define_quality( pybind11::module& module )
    {
        module.def( "tetrahedron_aspect_ratio", &tetrahedron_aspect_ratio,
            pybind11::arg( "tetra" ),
            "Return the aspect ratio of a tetrahedron: 1 for a regular "
            "tetrahedron, larger is worse, the largest double if it is "
            "degenerated" );
        module.def( "tetrahedron_volume_to_edge_ratio",
            &tetrahedron_volume_to_edge_ratio, pybind11::arg( "tetra" ),
            "Return 6 sqrt(2) V / l_rms^3 (V volume, l_rms root mean square of "
            "the edge lengths): 1 for a regular tetrahedron, 0 if it is "
            "degenerated or inverted" );
        module.def( "tetrahedron_volume_to_facet_ratio",
            &tetrahedron_volume_to_facet_ratio, pybind11::arg( "tetra" ),
            "Return 6 sqrt(6) V / (A l_max) (V volume, A total facet area, "
            "l_max longest edge): 1 for a regular tetrahedron, 0 if it is "
            "degenerated or inverted" );
        module.def( "tetrahedron_collapse_aspect_ratio",
            &tetrahedron_collapse_aspect_ratio, pybind11::arg( "tetra" ),
            "Return the collapse aspect ratio of a tetrahedron: the longest "
            "edge over its smallest height, normalized to 1 for a regular "
            "tetrahedron (larger is worse), the largest double if it is "
            "degenerated" );
        PYTHON_TRIANGLE_QUALITY( 2 );
        PYTHON_TRIANGLE_QUALITY( 3 );
    }
} // namespace geode
