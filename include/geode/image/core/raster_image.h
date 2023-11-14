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

#include <geode/basic/cell_array.h>
#include <geode/basic/identifier.h>
#include <geode/basic/pimpl.h>

#include <geode/image/common.h>

namespace geode
{
    class RGBColor;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class RasterImage : public CellArray< dimension >, public Identifier
    {
        friend class bitsery::Access;

    public:
        static constexpr auto dim = dimension;
        using CellIndices = typename CellArray< dimension >::CellIndices;

        RasterImage();
        RasterImage( std::array< index_t, dimension > cells_number );
        RasterImage( RasterImage&& other ) noexcept;
        RasterImage& operator=( RasterImage&& other ) noexcept;
        ~RasterImage();

        static std::string native_extension_static()
        {
            static const auto extension =
                absl::StrCat( "og_img", dimension, "d" );
            return extension;
        }

        std::string native_extension() const
        {
            return native_extension_static();
        }

        index_t cell_index( const CellIndices& index ) const override;

        CellIndices cell_indices( index_t index ) const override;

        const RGBColor& color( index_t index ) const;

        void set_color( index_t index, RGBColor color );

        RasterImage clone() const;

    private:
        template < typename Archive >
        void serialize( Archive& archive );

        using CellArray< dimension >::set_array_dimensions;
        using CellArray< dimension >::copy;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_1D_AND_2D_AND_3D( RasterImage );
} // namespace geode
