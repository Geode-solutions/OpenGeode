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

#pragma once

#include <fstream>

#include <geode/image/core/bitsery_archive.h>
#include <geode/image/io/raster_image_output.h>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeRasterImageOutput : public RasterImageOutput< dimension >
    {
    public:
        OpenGeodeRasterImageOutput( absl::string_view filename )
            : RasterImageOutput< dimension >( filename )
        {
        }

        std::vector< std::string > write(
            const RasterImage< dimension >& mesh ) const final
        {
            std::ofstream file{ to_string( this->filename() ),
                std::ofstream::binary };
            TContext context{};
            BitseryExtensions::register_serialize_pcontext(
                std::get< 0 >( context ) );
            Serializer archive{ context, file };
            archive.object( mesh );
            archive.adapter().flush();
            OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
                "[Bitsery::write] Error while writing file: ",
                this->filename() );
            return { to_string( this->filename() ) };
        }
    };
    ALIAS_2D_AND_3D( OpenGeodeRasterImageOutput );
} // namespace geode
