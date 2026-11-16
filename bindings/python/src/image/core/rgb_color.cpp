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

#include <geode/image/core/rgb_color.hpp>

namespace geode
{
    void define_rgb_color( pybind11::module& module )
    {
        define_string_representation(
            pybind11::class_< RGBColor >( module, "RGBColor",
                "Color of a pixel given by its red, green and blue levels, "
                "between 0 and 255" ),
            []( const RGBColor& color ) {
                return python_call_repr( "RGBColor",
                    { python_repr( color.red() ), python_repr( color.green() ),
                        python_repr( color.blue() ) } );
            } )
            .def( pybind11::init<>(), "Build a black color" )
            .def(
                pybind11::init< local_index_t, local_index_t, local_index_t >(),
                pybind11::arg( "red" ), pybind11::arg( "green" ),
                pybind11::arg( "blue" ),
                "Build a color from its red, green and blue levels (0 to 255)" )
            .def( "red", &RGBColor::red, "Return the red level (0 to 255)" )
            .def(
                "green", &RGBColor::green, "Return the green level (0 to 255)" )
            .def( "blue", &RGBColor::blue, "Return the blue level (0 to 255)" )
            .def( "set_red", &RGBColor::set_red, pybind11::arg( "red_scale" ),
                "Set the red level (0 to 255)" )
            .def( "set_green", &RGBColor::set_green,
                pybind11::arg( "green_scale" ),
                "Set the green level (0 to 255)" )
            .def( "set_blue", &RGBColor::set_blue,
                pybind11::arg( "blue_scale" ), "Set the blue level (0 to 255)" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__",
                []( const RGBColor& value ) {
                    return absl::HashOf(
                        value.red(), value.green(), value.blue() );
                } )
            .def( pybind11::self + pybind11::self, pybind11::arg( "other" ),
                "Return the mix of the two colors: each channel is the average "
                "of the channels of the two colors (rounded down)" )
            .def( pybind11::self += pybind11::self, pybind11::arg( "other" ),
                "Replace the color by the mix of the two colors: each channel "
                "is the average of the channels of the two colors (rounded "
                "down)" );
    }
} // namespace geode
