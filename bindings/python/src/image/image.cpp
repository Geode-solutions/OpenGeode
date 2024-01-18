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

#include "../common.h"
#include <pybind11/iostream.h>

#include <gdal_priv.h>

#include <geode/image/common.h>

namespace geode
{
    void define_greyscale_color( pybind11::module& );
    void define_raster_image( pybind11::module& );
    void define_raster_image_io( pybind11::module& );
    void define_rgb_color( pybind11::module& );
} // namespace geode

PYBIND11_MODULE( opengeode_py_image, module )
{
    pybind11::add_ostream_redirect( module );
    module.doc() = "OpenGeode Python binding for image";
    pybind11::class_< geode::OpenGeodeImageLibrary >(
        module, "OpenGeodeImageLibrary" )
        .def( "initialize", [] {
            geode::OpenGeodeImageLibrary::initialize();
            GDALAllRegister();
        } );
    geode::define_greyscale_color( module );
    geode::define_raster_image( module );
    geode::define_raster_image_io( module );
    geode::define_rgb_color( module );
}