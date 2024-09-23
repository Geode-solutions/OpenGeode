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

#include <fstream>

#include <geode/image/core/bitsery_archive.hpp>
#include <geode/image/io/raster_image_input.hpp>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeRasterImageInput : public RasterImageInput< dimension >
    {
    public:
        explicit OpenGeodeRasterImageInput( std::string_view filename )
            : RasterImageInput< dimension >( filename )
        {
        }

        [[nodiscard]] RasterImage< dimension > read() final
        {
            std::ifstream file{ to_string( this->filename() ),
                std::ifstream::binary };
            OPENGEODE_EXCEPTION( file,
                "[RasterImageInput] Failed to open file: ",
                to_string( this->filename() ) );
            TContext context{};
            BitseryExtensions::register_deserialize_pcontext(
                std::get< 0 >( context ) );
            Deserializer archive{ context, file };
            RasterImage< dimension > image;
            archive.object( image );
            const auto& adapter = archive.adapter();
            OPENGEODE_EXCEPTION(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                "[Bitsery::read] Error while reading file: ",
                this->filename() );
            return image;
        }
    };
    ALIAS_2D_AND_3D( OpenGeodeRasterImageInput );
} // namespace geode
