/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include "../../common.h"

#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

#define PYTHON_TRIANGULATED_SURFACE( dimension )                               \
    const auto name##dimension =                                               \
        "TriangulatedSurface" + std::to_string( dimension ) + "D";             \
    pybind11::class_< TriangulatedSurface##dimension##D,                       \
        SurfaceMesh##dimension##D >( module, name##dimension.c_str() )         \
        .def_static(                                                           \
            "create", static_cast< std::unique_ptr<                            \
                          TriangulatedSurface##dimension##D > ( * )() >(       \
                          &TriangulatedSurface##dimension##D::create ) )       \
        .def( "clone", &TriangulatedSurface##dimension##D::clone )             \
        .def( "triangle", &TriangulatedSurface##dimension##D::triangle )

namespace geode
{
    void define_triangulated_surface( pybind11::module& module )
    {
        PYTHON_TRIANGULATED_SURFACE( 2 );
        PYTHON_TRIANGULATED_SURFACE( 3 );
    }
} // namespace geode
