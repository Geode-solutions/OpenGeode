/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include "../common.h"

#include <geode/geometry/coordinate_system.h>

#define PYTHON_COORDINATE_SYSTEM( dimension )                                  \
    const auto name##dimension =                                               \
        "CoordinateSystem" + std::to_string( dimension ) + "D";                \
    pybind11::class_< CoordinateSystem##dimension##D >(                        \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init<>() )                                             \
        .def( pybind11::init< std::array< Vector< dimension >, dimension >,    \
            Point< dimension > >() )                                           \
        .def( pybind11::init< Point< dimension >,                              \
            const std::array< Point< dimension >, dimension >& >() )           \
        .def( "origin", &CoordinateSystem##dimension##D::origin )              \
        .def( "set_origin", &CoordinateSystem##dimension##D::set_origin )      \
        .def( "coordinates", &CoordinateSystem##dimension##D::coordinates )    \
        .def( "string", &CoordinateSystem##dimension##D::string )              \
        .def( "global_coordinates",                                            \
            &CoordinateSystem##dimension##D::global_coordinates )

namespace geode
{
    void define_coordinate_system( pybind11::module& module )
    {
        PYTHON_COORDINATE_SYSTEM( 2 );
        PYTHON_COORDINATE_SYSTEM( 3 );
    }
} // namespace geode
