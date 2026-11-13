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

#include <geode/geometry/coordinate_system.hpp>

#define PYTHON_COORDINATE_SYSTEM( dimension )                                  \
    const auto name##dimension =                                               \
        "CoordinateSystem" + std::to_string( dimension ) + "D";                \
    define_string_representation(                                              \
        pybind11::class_< CoordinateSystem##dimension##D,                      \
            pybind11::smart_holder >( module, name##dimension.c_str(),         \
            "Coordinate system defined by an origin and one direction per "    \
            "axis" ) )                                                         \
        .def( pybind11::init<>(),                                              \
            "Build the canonical coordinate system, centered on the origin" )  \
        .def( pybind11::init< std::array< Vector< dimension >, dimension >,    \
                  Point< dimension > >(),                                      \
            pybind11::arg( "directions" ), pybind11::arg( "origin" ),          \
            "Build a coordinate system from the directions of its axes and "   \
            "its origin. Raise an error if the directions are colinear" )      \
        .def( pybind11::init< Point< dimension >,                              \
                  const std::array< Point< dimension >, dimension >& >(),      \
            pybind11::arg( "origin" ), pybind11::arg( "other_points" ),        \
            "Build a coordinate system from its origin and one point on each " \
            "axis. Raise an error if the axes are colinear" )                  \
        .def( "origin", &CoordinateSystem##dimension##D::origin,               \
            "Return the origin of the coordinate system" )                     \
        .def( "set_origin", &CoordinateSystem##dimension##D::set_origin,       \
            pybind11::arg( "origin" ),                                         \
            "Set the origin of the coordinate system" )                        \
        .def( "coordinates", &CoordinateSystem##dimension##D::coordinates,     \
            pybind11::arg( "global_coordinates" ),                             \
            "Return the coordinates in this coordinate system of a point "     \
            "given in the global one" )                                        \
        .def( "global_coordinates",                                            \
            &CoordinateSystem##dimension##D::global_coordinates,               \
            pybind11::arg( "coordinates" ),                                    \
            "Return the global coordinates of a point given in this "          \
            "coordinate system" )

namespace geode
{
    void define_coordinate_system( pybind11::module& module )
    {
        PYTHON_COORDINATE_SYSTEM( 1 );
        PYTHON_COORDINATE_SYSTEM( 2 );
        PYTHON_COORDINATE_SYSTEM( 3 );
    }
} // namespace geode
