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

#include "../common.hpp"

#include <geode/geometry/angle.hpp>

namespace geode
{
    void define_angle( pybind11::module& module )
    {
        pybind11::class_< Angle >( module, "Angle" )
            .def( "create_from_radians", &Angle::create_from_radians )
            .def( "create_from_degrees", &Angle::create_from_degrees )
            .def( "radians", &Angle::radians )
            .def( "degrees", &Angle::degrees )
            .def( "sin", &Angle::sin )
            .def( "cos", &Angle::cos )
            .def( "tan", &Angle::tan )
            .def( py::self == py::self )
            .def( py::self < py::self )
            .def( py::self > py::self )
            .def( py::self + py::self )
            .def( py::self - py::self )
            .def( py::self * double() )
            .def( py::self / double() )
            .def( "normalized_0_twopi", &Angle::normalized_0_twopi )
            .def( "normalized_minuspi_pi", &Angle::normalized_minuspi_pi )
            .def( "normalized_0_pi", &Angle::normalized_0_pi );
    }
} // namespace geode