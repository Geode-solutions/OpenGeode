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

#include <geode/image/common.h>

#include <geode/basic/library.h>

#include <geode/image/core/bitsery_archive.h>
#include <geode/image/core/raster_image.h>
#include <geode/image/io/geode/geode_bitsery_raster_input.h>
#include <geode/image/io/geode/geode_bitsery_raster_output.h>
#include <geode/image/io/raster_image_input.h>
#include <geode/image/io/raster_image_output.h>

#define BITSERY_INPUT_RASTER_REGISTER_XD( dimension )                          \
    geode::RasterImageInputFactory##dimension##D::register_creator<            \
        geode::OpenGeodeRasterImageInput##dimension##D >(                      \
        geode::RasterImage##dimension##D ::native_extension_static() )

#define BITSERY_INPUT_RASTER_REGISTER_2D_3D()                                  \
    BITSERY_INPUT_RASTER_REGISTER_XD( 2 );                                     \
    BITSERY_INPUT_RASTER_REGISTER_XD( 3 )

#define BITSERY_OUTPUT_RASTER_REGISTER_XD( dimension )                         \
    geode::RasterImageOutputFactory##dimension##D::register_creator<           \
        geode::OpenGeodeRasterImageOutput##dimension##D >(                     \
        geode::RasterImage##dimension##D ::native_extension_static() )

#define BITSERY_OUTPUT_RASTER_REGISTER_2D_3D()                                 \
    BITSERY_OUTPUT_RASTER_REGISTER_XD( 2 );                                    \
    BITSERY_OUTPUT_RASTER_REGISTER_XD( 3 )

namespace geode
{
    OPENGEODE_LIBRARY_IMPLEMENTATION( OpenGeodeImage )
    {
        OpenGeodeBasicLibrary::initialize();
        BITSERY_INPUT_RASTER_REGISTER_2D_3D();
        BITSERY_OUTPUT_RASTER_REGISTER_2D_3D();
        BitseryExtensions::register_functions(
            register_image_serialize_pcontext,
            register_image_deserialize_pcontext );
    }
} // namespace geode
