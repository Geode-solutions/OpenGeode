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

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>

#define PYTHON_REGULAR_GRID( Base, dimension )                                 \
    const auto name##dimension =                                               \
        "RegularGrid" + std::to_string( dimension ) + "D";                     \
    pybind11::class_< RegularGrid##dimension##D, Base##dimension##D,           \
        Grid##dimension##D >( module, name##dimension.c_str() )                \
        .def_static( "create",                                                 \
            static_cast<                                                       \
                std::unique_ptr< RegularGrid##dimension##D > ( * )() >(        \
                &RegularGrid##dimension##D::create ) )                         \
        .def( "clone", &RegularGrid##dimension##D::clone )                     \
        .def( "bounding_box", &RegularGrid##dimension##D::bounding_box )

namespace geode
{
    void define_regular_grid( pybind11::module& module )
    {
        PYTHON_REGULAR_GRID( SurfaceMesh, 2 );
        PYTHON_REGULAR_GRID( SolidMesh, 3 );
    }
} // namespace geode
