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

#include <geode/geometry/barycentric_coordinates.h>

#define PYTHON_BARYCENTRIC( dimension )                                        \
    const auto triangle_barycentric_coordinates##dimension =                   \
        "triangle_barycentric_coordinates" + std::to_string( dimension )       \
        + "D";                                                                 \
    module.def( triangle_barycentric_coordinates##dimension.c_str(),           \
        &triangle_barycentric_coordinates< dimension > );                      \
    const auto segment_barycentric_coordinates##dimension =                    \
        "segment_barycentric_coordinates" + std::to_string( dimension ) + "D"; \
    module.def( segment_barycentric_coordinates##dimension.c_str(),            \
        &segment_barycentric_coordinates< dimension > )

namespace geode
{
    void define_barycentric( pybind11::module& module )
    {
        PYTHON_BARYCENTRIC( 2 );
        PYTHON_BARYCENTRIC( 3 );
        module.def(
            "tetrahedron_barycentric_coordinates", &tetrahedron_barycentric_coordinates );
    }
} // namespace geode
