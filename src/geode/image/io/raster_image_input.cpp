/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/image/io/raster_image_input.h>

#include <geode/basic/detail/geode_input_impl.h>

#include <geode/image/core/raster_image.h>

namespace geode
{
    template < index_t dimension >
    RasterImage< dimension > load_raster_image( absl::string_view filename )
    {
        try
        {
            const auto type = absl::StrCat( "RasterImage", dimension, "D" );
            auto raster = detail::geode_object_input_impl<
                RasterImageInputFactory< dimension > >( type, filename );
            Logger::info( type, " has: ", raster.nb_cells(), " cells" );
            return raster;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot load RasterImage from file: ",
                filename };
        }
    }

    template < index_t dimension >
    typename RasterImageInput< dimension >::MissingFiles
        check_raster_image_missing_files( absl::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            RasterImageInputFactory< dimension > >( filename );
        return input->check_missing_files();
    }

    template RasterImage< 2 > opengeode_image_api load_raster_image(
        absl::string_view );
    template RasterImage< 3 > opengeode_image_api load_raster_image(
        absl::string_view );

    template RasterImageInput< 2 >::MissingFiles opengeode_image_api
        check_raster_image_missing_files< 2 >( absl::string_view );
    template RasterImageInput< 3 >::MissingFiles opengeode_image_api
        check_raster_image_missing_files< 3 >( absl::string_view );
} // namespace geode
