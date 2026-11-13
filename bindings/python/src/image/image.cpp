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
#include <pybind11/iostream.h>

#include <geode/image/common.hpp>

namespace geode
{
    void define_greyscale_color( pybind11::module& );
    void define_raster_image( pybind11::module& );
    void define_raster_image_io( pybind11::module& );
    void define_rgb_color( pybind11::module& );
} // namespace geode

PYBIND11_MODULE( opengeode_py_image, module )
{
    // The classes of this module derive from the ones of opengeode_py_basic
    geode::import_binding_dependency( module, "opengeode_py_basic" );
    pybind11::add_ostream_redirect( module );
    module.doc() =
        "Raster images of OpenGeode: RasterImage1D/2D/3D (regular grid of "
        "RGBColor cells), the colors (RGBColor, GreyscaleColor) and the "
        "functions loading and saving raster images (load_raster_imageND, "
        "save_raster_imageND...)";
    pybind11::class_< geode::OpenGeodeImageLibrary >( module,
        "OpenGeodeImageLibrary",
        "Initialization of the OpenGeode image library" )
        .def_static( "initialize", &geode::OpenGeodeImageLibrary::initialize,
            "Initialize the library: must be called once before using it" );
    geode::define_greyscale_color( module );
    geode::define_rgb_color( module );
    geode::define_raster_image( module );
    geode::define_raster_image_io( module );
}