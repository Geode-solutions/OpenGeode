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

#include "../../common.hpp"

#include <geode/geometry/point.hpp>

#include <geode/image/core/raster_image.hpp>

#include <geode/mesh/core/texture1d.hpp>
#include <geode/mesh/core/texture2d.hpp>
#include <geode/mesh/core/texture3d.hpp>

#define PYTHON_TEXTURE( dimension )                                            \
    const auto name##dimension =                                               \
        "Texture" + std::to_string( dimension ) + "D";                         \
    pybind11::class_< Texture< dimension > >(                                  \
        module, name##dimension.c_str() )                                      \
        .def( "image", &Texture< dimension >::image )                          \
        .def( "set_image",                                                     \
            []( Texture< dimension >& texture,                                 \
                RasterImage< dimension >& image ) {                            \
                texture.set_image( std::move( image ) );                       \
            } )                                                                \
        .def( "texture_coordinates",                                           \
            &Texture< dimension >::texture_coordinates )                       \
        .def( "set_texture_coordinates",                                       \
            &Texture< dimension >::set_texture_coordinates )

namespace geode
{
    void define_texture( pybind11::module& module )
    {
        PYTHON_TEXTURE( 1 );
        PYTHON_TEXTURE( 2 );
        PYTHON_TEXTURE( 3 );
    }
} // namespace geode
