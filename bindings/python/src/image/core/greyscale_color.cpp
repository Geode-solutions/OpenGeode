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

#include "../../common.hpp"

#include <geode/image/core/greyscale_color.hpp>

namespace geode
{
    void define_greyscale_color( pybind11::module& module )
    {
        define_string_representation(
            pybind11::class_< GreyscaleColor >( module, "GreyscaleColor",
                "Grey level of a pixel, between 0 and 255" ) )
            .def( pybind11::init<>(), "Build a black color" )
            .def( pybind11::init< local_index_t >(), pybind11::arg( "value" ),
                "Build a color from its grey level (0 to 255)" )
            .def( "value", &GreyscaleColor::value,
                "Return the grey level (0 to 255)" )
            .def( "set_value", &GreyscaleColor::set_value,
                pybind11::arg( "greyscale" ), "Set the grey level (0 to 255)" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__",
                []( const GreyscaleColor& value ) {
                    return absl::HashOf( value.value() );
                } )
            .def( pybind11::self + pybind11::self, pybind11::arg( "other" ),
                "Return the mix of the two colors: the grey level is the "
                "average of the grey levels of the two colors (rounded down)" )
            .def( pybind11::self += pybind11::self, pybind11::arg( "other" ),
                "Replace the color by the mix of the two colors: the grey "
                "level is the average of the grey levels of the two colors "
                "(rounded down)" );
    }
} // namespace geode
