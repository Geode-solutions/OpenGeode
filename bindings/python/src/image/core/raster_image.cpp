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

#include "../../common.h"

#include <geode/image/core/raster_image.hpp>
#include <geode/image/core/rgb_color.hpp>

#define PYTHON_RASTER_IMAGE( dimension )                                       \
    const auto name##dimension =                                               \
        "RasterImage" + std::to_string( dimension ) + "D";                     \
    pybind11::class_< RasterImage##dimension##D, CellArray##dimension##D >(    \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init< std::array< index_t, dimension > >() )           \
        .def(                                                                  \
            "native_extension", &RasterImage##dimension##D::native_extension ) \
        .def( "color", &RasterImage##dimension##D::color )                     \
        .def( "set_color", &RasterImage##dimension##D::set_color )             \
        .def( "clone", &RasterImage##dimension##D::clone )

namespace geode
{
    void define_raster_image( pybind11::module& module )
    {
        PYTHON_RASTER_IMAGE( 2 );
        PYTHON_RASTER_IMAGE( 3 );
    }
} // namespace geode
