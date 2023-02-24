/*
 * Copyright (c) 2029 - 2023 Geode-solutions
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

#include <geode/mesh/core/texture_manager.h>

#include <absl/container/flat_hash_map.h>

#include <bitsery/ext/std_map.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/texture1d.h>
#include <geode/mesh/core/texture2d.h>
#include <geode/mesh/core/texture3d.h>
#include <geode/mesh/core/texture_storage.h>

namespace geode
{
    template < index_t dimension >
    class TextureManager< dimension >::Impl
    {
    public:
        Impl( AttributeManager& manager, TextureStorage< dimension >& textures )
            : manager_( manager ), textures_( textures )
        {
        }

        Texture< dimension >& find_or_create_texture( absl::string_view name )
        {
            return textures_.find_or_create_texture( manager_, name, {} );
        }

        const Texture< dimension >& find_texture( absl::string_view name ) const
        {
            return textures_.find_texture( name, {} );
        }

        absl::FixedArray< absl::string_view > texture_names() const
        {
            return textures_.texture_names( {} );
        }

        bool texture_exists( absl::string_view name ) const
        {
            return textures_.texture_exists( name, {} );
        }

        void delete_texture( absl::string_view name )
        {
            return textures_.delete_texture( name, {} );
        }

    private:
        AttributeManager& manager_;
        TextureStorage< dimension >& textures_;
    };

    template < index_t dimension >
    TextureManager< dimension >::TextureManager(
        AttributeManager& manager, TextureStorage< dimension >& textures )
        : impl_{ manager, textures }
    {
    }

    template < index_t dimension >
    TextureManager< dimension >::TextureManager( TextureManager&& other )
        : impl_{ std::move( other.impl_ ) }
    {
    }

    template < index_t dimension >
    TextureManager< dimension >::~TextureManager()
    {
    }

    template < index_t dimension >
    Texture< dimension >& TextureManager< dimension >::find_or_create_texture(
        absl::string_view name )
    {
        return impl_->find_or_create_texture( name );
    }

    template < index_t dimension >
    const Texture< dimension >& TextureManager< dimension >::find_texture(
        absl::string_view name ) const
    {
        return impl_->find_texture( name );
    }

    template < index_t dimension >
    absl::FixedArray< absl::string_view >
        TextureManager< dimension >::texture_names() const
    {
        return impl_->texture_names();
    }

    template < index_t dimension >
    bool TextureManager< dimension >::texture_exists(
        absl::string_view name ) const
    {
        return impl_->texture_exists( name );
    }

    template < index_t dimension >
    void TextureManager< dimension >::delete_texture( absl::string_view name )
    {
        impl_->delete_texture( name );
    }

    template class opengeode_mesh_api TextureManager< 1 >;
    template class opengeode_mesh_api TextureManager< 2 >;
    template class opengeode_mesh_api TextureManager< 3 >;
} // namespace geode