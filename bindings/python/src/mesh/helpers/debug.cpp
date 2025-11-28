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

#include <geode/mesh/helpers/detail/debug.hpp>

namespace geode
{
    void define_debug( pybind11::module& module )
    {
        module.def( "save_segment2D", &detail::save_segment< 2 > )
            .def( "save_segment3D", &detail::save_segment< 3 > )
            .def( "save_triangle2D", &detail::save_triangle< 2 > )
            .def( "save_triangle3D", &detail::save_triangle< 3 > )
            .def( "save_triangle2D", &detail::save_triangles< 2 > )
            .def( "save_triangle3D", &detail::save_triangles< 3 > )
            .def( "save_tetrahedron", &detail::save_tetrahedron )
            .def( "save_tetrahedra", &detail::save_tetrahedra );
    }
} // namespace geode
