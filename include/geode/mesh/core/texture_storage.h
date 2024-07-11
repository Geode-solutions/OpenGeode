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

#include <absl/container/fixed_array.h>

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Texture );
    FORWARD_DECLARATION_DIMENSION_CLASS( TextureManager );
    class AttributeManager;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class TextureStorage
    {
        friend class bitsery::Access;
        PASSKEY( TextureManager< dimension >, TextureManagerKey );

    public:
        TextureStorage();
        TextureStorage( TextureStorage&& other ) noexcept;
        ~TextureStorage();

    public:
        index_t nb_textures( TextureManagerKey ) const;

        Texture< dimension >& find_or_create_texture( AttributeManager& manager,
            std::string_view name,
            TextureManagerKey );

        const Texture< dimension >& find_texture(
            std::string_view name, TextureManagerKey ) const;

        absl::FixedArray< std::string_view > texture_names(
            TextureManagerKey ) const;

        bool texture_exists( std::string_view name, TextureManagerKey ) const;

        void delete_texture( std::string_view name, TextureManagerKey );

    private:
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_1D_AND_2D_AND_3D( TextureStorage );
} // namespace geode