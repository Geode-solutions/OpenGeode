/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geometry/projection.h>

#define PYTHON_PROJECTION( dimension )                                         \
    const auto point_segment_projection##dimension =                           \
        "point_segment_projection" + std::to_string( dimension ) + "D";        \
    module.def( point_segment_projection##dimension.c_str(),                   \
        &point_segment_projection< dimension > );                              \
    const auto point_line_projection##dimension =                              \
        "point_line_projection" + std::to_string( dimension ) + "D";           \
    module.def( point_line_projection##dimension.c_str(),                      \
        &point_line_projection< dimension > );                                 \
    const auto point_triangle_projection##dimension =                          \
        "point_triangle_projection" + std::to_string( dimension ) + "D";       \
    module.def( point_triangle_projection##dimension.c_str(),                  \
        &point_triangle_projection< dimension > )

namespace geode
{
    void define_projection( pybind11::module& module )
    {
        PYTHON_PROJECTION( 2 );
        PYTHON_PROJECTION( 3 );
        module.def( "point_plane_projection", &point_plane_projection );
    }
} // namespace geode
