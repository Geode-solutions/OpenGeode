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

#include <geode/image/io/raster_image_output.h>

#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_output_impl.h>

#include <geode/image/core/raster_image.h>

namespace geode
{
    template < index_t dimension >
    std::vector< std::string > save_raster_image(
        const RasterImage< dimension >& raster, absl::string_view filename )
    {
        const auto type = absl::StrCat( "RasterImage", dimension, "D" );
        try
        {
            return detail::geode_object_output_impl<
                RasterImageOutputFactory< dimension > >(
                type, raster, filename );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< RasterImageOutputFactory< dimension > >(
                type );
            throw OpenGeodeException{ "Cannot save RasterImage in file: ",
                filename };
        }
    }

    template < index_t dimension >
    bool is_raster_image_saveable(
        const RasterImage< dimension >& raster, absl::string_view filename )
    {
        const auto output = detail::geode_object_output_writer<
            RasterImageOutputFactory< dimension > >( filename );
        return output->is_saveable( raster );
    }

    template std::vector< std::string > opengeode_image_api save_raster_image(
        const RasterImage< 2 >&, absl::string_view );
    template std::vector< std::string > opengeode_image_api save_raster_image(
        const RasterImage< 3 >&, absl::string_view );

    template bool opengeode_image_api is_raster_image_saveable(
        const RasterImage< 2 >&, absl::string_view );
    template bool opengeode_image_api is_raster_image_saveable(
        const RasterImage< 3 >&, absl::string_view );
} // namespace geode
