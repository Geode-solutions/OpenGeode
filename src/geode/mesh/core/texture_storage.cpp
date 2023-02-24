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

#include <geode/mesh/core/texture_storage.h>

#include <absl/container/flat_hash_map.h>

#include <bitsery/ext/std_map.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/texture1d.h>
#include <geode/mesh/core/texture2d.h>
#include <geode/mesh/core/texture3d.h>

namespace geode
{
    template < index_t dimension >
    class TextureStorage< dimension >::Impl
    {
        friend class bitsery::Access;

    public:
        Texture< dimension >& find_or_create_texture(
            AttributeManager& manager, absl::string_view name )
        {
            if( !texture_exists( name ) )
            {
                textures_.emplace(
                    name, Texture< dimension >{ manager, name } );
            }
            return textures_.at( name );
        }

        const Texture< dimension >& find_texture( absl::string_view name ) const
        {
            OPENGEODE_EXCEPTION( texture_exists( name ),
                "[TextureStorage::find_texture] Texture ", name,
                " does not exists" );
            return textures_.at( name );
        }

        absl::FixedArray< absl::string_view > texture_names() const
        {
            absl::FixedArray< absl::string_view > names( textures_.size() );
            index_t count{ 0 };
            for( const auto& it : textures_ )
            {
                names[count++] = it.first;
            }
            return names;
        }

        bool texture_exists( absl::string_view name ) const
        {
            return textures_.find( name ) != textures_.end();
        }

        void delete_texture( absl::string_view name )
        {
            const auto it = textures_.find( name );
            if( it != textures_.end() )
            {
                textures_.erase( it );
            }
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& a, Impl& impl ) {
                    a.ext( impl.textures_,
                        bitsery::ext::StdMap{ impl.textures_.max_size() },
                        []( Archive& a2, std::string& name,
                            Texture< dimension >& texture ) {
                            a2.text1b( name, name.max_size() );
                            a2.object( texture );
                        } );
                } );
        }

    private:
        absl::flat_hash_map< std::string, Texture< dimension > > textures_;
    };

    template < index_t dimension >
    TextureStorage< dimension >::TextureStorage()
    {
    }

    template < index_t dimension >
    TextureStorage< dimension >::TextureStorage( TextureStorage&& other )
        : impl_{ std::move( other.impl_ ) }
    {
    }

    template < index_t dimension >
    TextureStorage< dimension >::~TextureStorage()
    {
    }

    template < index_t dimension >
    Texture< dimension >& TextureStorage< dimension >::find_or_create_texture(
        AttributeManager& manager, absl::string_view name, TextureManagerKey )
    {
        return impl_->find_or_create_texture( manager, name );
    }

    template < index_t dimension >
    const Texture< dimension >& TextureStorage< dimension >::find_texture(
        absl::string_view name, TextureManagerKey ) const
    {
        return impl_->find_texture( name );
    }

    template < index_t dimension >
    absl::FixedArray< absl::string_view >
        TextureStorage< dimension >::texture_names( TextureManagerKey ) const
    {
        return impl_->texture_names();
    }

    template < index_t dimension >
    bool TextureStorage< dimension >::texture_exists(
        absl::string_view name, TextureManagerKey ) const
    {
        return impl_->texture_exists( name );
    }

    template < index_t dimension >
    void TextureStorage< dimension >::delete_texture(
        absl::string_view name, TextureManagerKey )
    {
        impl_->delete_texture( name );
    }

    template < index_t dimension >
    template < typename Archive >
    void TextureStorage< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, TextureStorage >{},
            []( Archive& a, TextureStorage& manager ) {
                a.object( manager.impl_ );
            } );
    }

    template class opengeode_mesh_api TextureStorage< 1 >;
    template class opengeode_mesh_api TextureStorage< 2 >;
    template class opengeode_mesh_api TextureStorage< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, TextureStorage< 1 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, TextureStorage< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, TextureStorage< 3 > );
} // namespace geode