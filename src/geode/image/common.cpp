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

namespace
{
    void register_raster_image_input()
    {
        geode::raster_image_input_factory_2d
            .register_creator< geode::OpenGeodeRasterImageInput2D >(
                geode::RasterImage2D ::native_extension_static() );
        geode::raster_image_input_factory_3d
            .register_creator< geode::OpenGeodeRasterImageInput3D >(
                geode::RasterImage3D ::native_extension_static() );
    }

    void register_raster_image_output()
    {
        geode::raster_image_output_factory_2d
            .register_creator< geode::OpenGeodeRasterImageOutput2D >(
                geode::RasterImage2D ::native_extension_static() );
        geode::raster_image_output_factory_3d
            .register_creator< geode::OpenGeodeRasterImageOutput3D >(
                geode::RasterImage3D ::native_extension_static() );
    }
} // namespace

namespace geode
{
    OPENGEODE_LIBRARY_IMPLEMENTATION( OpenGeodeImage )
    {
        OpenGeodeBasicLibrary::initialize();
        register_raster_image_input();
        register_raster_image_output();
        BitseryExtensions::register_functions(
            register_image_serialize_pcontext,
            register_image_deserialize_pcontext );
    }
} // namespace geode
