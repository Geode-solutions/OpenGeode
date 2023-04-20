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

#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/edged_curve.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( RasterImage );
    FORWARD_DECLARATION_DIMENSION_CLASS( Texture );
    ALIAS_1D( Point );
    ALIAS_1D( RasterImage );
    class AttributeManager;
} // namespace geode

namespace geode
{
    template <>
    class opengeode_mesh_api Texture< 1 >
    {
        OPENGEODE_DISABLE_COPY( Texture );
        friend class bitsery::Access;

    public:
        Texture( AttributeManager& manager, absl::string_view name );
        Texture( Texture&& );
        ~Texture();

        const RasterImage1D& image() const;

        void set_image( RasterImage1D&& image );

        const Point1D& texture_coordinates( const EdgeVertex& vertex ) const;

        void set_texture_coordinates(
            const EdgeVertex& vertex, const Point1D& coordinates ) const;

    private:
        Texture();

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_1D( Texture );
} // namespace geode