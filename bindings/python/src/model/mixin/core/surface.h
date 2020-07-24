/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/core/polygonal_surface.h>

#include <geode/model/mixin/core/surface.h>

#define PYTHON_SURFACE( dimension )                                            \
    const auto name##dimension =                                               \
        "Surface" + std::to_string( dimension ) + "D";                         \
    pybind11::class_< Surface##dimension##D, Component##dimension##D >(        \
        module, name##dimension.c_str() )                                      \
        .def( "mesh",                                                          \
            &Surface##dimension##D::mesh< SurfaceMesh##dimension##D >,         \
            pybind11::return_value_policy::reference )                         \
        .def( "component_id", &Surface##dimension##D::component_id )           \
        .def_static( "component_type_static",                                  \
            &Surface##dimension##D::component_type_static )

namespace geode
{
    void define_surface( pybind11::module& module )
    {
        PYTHON_SURFACE( 2 );
        PYTHON_SURFACE( 3 );
    }
} // namespace geode
