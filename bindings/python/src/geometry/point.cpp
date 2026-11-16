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

#include <geode/geometry/point.hpp>

namespace
{
    template < geode::index_t dimension >
    size_t point_hash( const geode::Point< dimension >& point )
    {
        return absl::HashOf( point );
    }
} // namespace

#define PYTHON_POINT( dimension )                                              \
    const auto name##dimension = "Point" + std::to_string( dimension ) + "D";  \
    define_string_representation(                                              \
        pybind11::class_< Point##dimension##D >( module,                       \
            name##dimension.c_str(),                                           \
            "Point of the space, defined by its coordinates" ),                \
        []( const Point##dimension##D& point ) {                               \
            return python_call_repr(                                           \
                "Point" #dimension "D", { coordinates_repr( point ) } );       \
        } )                                                                    \
        .def( pybind11::init<>(), "Build a point at the origin" )              \
        .def( pybind11::init< std::array< double, dimension > >(),             \
            pybind11::arg( "values" ), "Build a point from its coordinates" )  \
        .def( "inexact_equal", &Point##dimension##D::inexact_equal,            \
            pybind11::arg( "other" ),                                          \
            "Return True if the two points are identical up to the global "    \
            "epsilon" )                                                        \
        .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )     \
        .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )     \
        .def( "__hash__", &point_hash< dimension > )                           \
        .def( pybind11::self* double(), pybind11::arg( "scalar" ) )            \
        .def( pybind11::self / double(), pybind11::arg( "scalar" ) )           \
        .def( pybind11::self + pybind11::self, pybind11::arg( "other" ) )      \
        .def( pybind11::self - pybind11::self, pybind11::arg( "other" ) )      \
        .def( pybind11::self *= double(), pybind11::arg( "scalar" ) )          \
        .def( pybind11::self /= double(), pybind11::arg( "scalar" ) )          \
        .def( pybind11::self += pybind11::self, pybind11::arg( "other" ) )     \
        .def( pybind11::self -= pybind11::self, pybind11::arg( "other" ) )     \
        .def(                                                                  \
            "value",                                                           \
            []( const Point##dimension##D& point, local_index_t index ) {      \
                check_index( "value", "axis", index, dimension );              \
                return point.value( index );                                   \
            },                                                                 \
            pybind11::arg( "index" ),                                          \
            "Return the coordinate of the given axis (0 for x, 1 for y...). "  \
            "Raise IndexError if the axis is not in [0, dimension)" )          \
        .def(                                                                  \
            "set_value",                                                       \
            []( Point##dimension##D& point, local_index_t index,               \
                double coordinate ) {                                          \
                check_index( "set_value", "axis", index, dimension );          \
                point.set_value( index, coordinate );                          \
            },                                                                 \
            pybind11::arg( "index" ), pybind11::arg( "coordinate" ),           \
            "Set the coordinate of the given axis (0 for x, 1 for y...). "     \
            "Raise IndexError if the axis is not in [0, dimension)" )

namespace geode
{
    void define_point( pybind11::module& module )
    {
        PYTHON_POINT( 1 );
        PYTHON_POINT( 2 );
        PYTHON_POINT( 3 );
    }
} // namespace geode
