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

#include <geode/geometry/angle.hpp>

namespace geode
{
    void define_angle( pybind11::module& module )
    {
        pybind11::class_< Angle >( module, "Angle", "Angle stored in radians" )
            .def_static( "create_from_radians", &Angle::create_from_radians,
                pybind11::arg( "radians" ),
                "Build an angle from a value in radians" )
            .def_static( "create_from_degrees", &Angle::create_from_degrees,
                pybind11::arg( "degrees" ),
                "Build an angle from a value in degrees" )
            .def( "radians", &Angle::radians, "Return the angle in radians" )
            .def( "degrees", &Angle::degrees, "Return the angle in degrees" )
            .def( "sin", &Angle::sin, "Return the sine of the angle" )
            .def( "cos", &Angle::cos, "Return the cosine of the angle" )
            .def( "tan", &Angle::tan, "Return the tangent of the angle" )
            .def( "inexact_equal", &Angle::inexact_equal,
                pybind11::arg( "other" ),
                "Return True if the two angles are equal up to the global "
                "epsilon" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__",
                []( const Angle& value ) {
                    return absl::HashOf( value.radians() );
                } )
            .def( pybind11::self < pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self > pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self + pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self - pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self * double(), pybind11::arg( "scalar" ) )
            .def( pybind11::self / double(), pybind11::arg( "scalar" ) )
            .def( "normalized_between_0_and_2pi",
                &Angle::normalized_between_0_and_2pi,
                "Return the equivalent angle in [0, 2 pi[" )
            .def( "normalized_between_minuspi_and_pi",
                &Angle::normalized_between_minuspi_and_pi,
                "Return the equivalent angle in ]-pi, pi]" )
            .def( "normalized_between_0_and_pi",
                &Angle::normalized_between_0_and_pi,
                "Return the equivalent angle in [0, pi]" );
    }
} // namespace geode