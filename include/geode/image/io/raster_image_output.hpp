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

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <geode/basic/factory.hpp>
#include <geode/basic/output.hpp>

#include <geode/image/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( RasterImage );
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a RasterImage.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] raster RasterImage to save.
     * @param[in] filename Path to the file where save the RasterImage.
     */
    template < index_t dimension >
    std::vector< std::string > save_raster_image(
        const RasterImage< dimension >& raster, std::string_view filename );

    template < index_t dimension >
    class RasterImageOutput : public Output< RasterImage< dimension > >
    {
    protected:
        explicit RasterImageOutput( std::string_view filename )
            : Output< RasterImage< dimension > >{ filename }
        {
        }
    };

    template < index_t dimension >
    [[nodiscard]] bool is_raster_image_saveable(
        const RasterImage< dimension >& raster, std::string_view filename );

    template < index_t dimension >
    using RasterImageOutputFactory = Factory< std::string,
        RasterImageOutput< dimension >,
        std::string_view >;
    ALIAS_2D_AND_3D( RasterImageOutputFactory );
} // namespace geode
