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
        &triangle_angle_based_quality< dimension > )

namespace geode
{
    void define_quality( pybind11::module& module )
    {
        module.def( "tetrahedron_aspect_ratio", &tetrahedron_aspect_ratio );
        module.def( "tetrahedron_volume_to_edge_ratio",
            &tetrahedron_volume_to_edge_ratio );
        module.def( "tetrahedron_volume_to_facet_ratio",
            &tetrahedron_volume_to_facet_ratio );
        module.def( "tetrahedron_collapse_aspect_ratio",
            &tetrahedron_collapse_aspect_ratio );
        PYTHON_TRIANGLE_QUALITY( 2 );
        PYTHON_TRIANGLE_QUALITY( 3 );
    }
} // namespace geode
