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

#pragma once

#include <absl/strings/string_view.h>

#include <geode/basic/factory.h>
#include <geode/basic/input.h>

#include <geode/image/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( RasterImage );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an RasterImage.
     * The adequate loader is called depending on the filename extension.
     * Default data structure implémentation is used.
     * @param[in] filename Path to the file to load.
     */
    template < index_t dimension >
    RasterImage< dimension > load_raster_image( absl::string_view filename );

    template < index_t dimension >
    class RasterImageInput : public Input< RasterImage< dimension > >
    {
    public:
        using Base = Input< RasterImage< dimension > >;
        using Base::InputData;
        using Base::MissingFiles;

    protected:
        RasterImageInput( absl::string_view filename ) : Base{ filename } {}
    };

    template < index_t dimension >
    typename RasterImageInput< dimension >::MissingFiles
        check_raster_image_missing_files( absl::string_view filename );

    template < index_t dimension >
    bool is_raster_image_loadable( absl::string_view filename );

    template < index_t dimension >
    using RasterImageInputFactory = Factory< std::string,
        RasterImageInput< dimension >,
        absl::string_view >;
    FACTORY_2D_AND_3D( RasterImageInputFactory, raster_image_input_factory );
} // namespace geode
