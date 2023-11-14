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

#include <geode/mesh/core/texture1d.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/image/core/raster_image.h>

#include <geode/mesh/core/private/texture_impl.h>

namespace geode
{
    class Texture< 1 >::Impl : public detail::TextureImpl< 1 >
    {
        friend class bitsery::Access;

    public:
        Impl() = default;
        Impl( AttributeManager& manager, absl::string_view name )
            : detail::TextureImpl< 1 >{ manager, name }
        {
        }

        const Point1D& texture_coordinates( const EdgeVertex& vertex ) const
        {
            return texture_coordinates_impl( vertex.edge_id, vertex.vertex_id );
        }

        void set_texture_coordinates(
            const EdgeVertex& vertex, const Point1D& coordinates ) const
        {
            set_texture_coordinates_impl(
                vertex.edge_id, vertex.vertex_id, coordinates );
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.ext( impl,
                        bitsery::ext::BaseClass< detail::TextureImpl< 1 > >{} );
                } } } );
        }
    };

    Texture< 1 >::Texture( AttributeManager& manager, absl::string_view name )
        : impl_{ manager, name }
    {
    }

    Texture< 1 >::Texture( Texture&& ) noexcept = default;

    Texture< 1 >::Texture() = default;

    Texture< 1 >::~Texture() = default;

    const RasterImage1D& Texture< 1 >::image() const
    {
        return impl_->image();
    }

    void Texture< 1 >::set_image( RasterImage1D&& image )
    {
        impl_->set_image( std::move( image ) );
    }

    const Point1D& Texture< 1 >::texture_coordinates(
        const EdgeVertex& vertex ) const
    {
        return impl_->texture_coordinates( vertex );
    }

    void Texture< 1 >::set_texture_coordinates(
        const EdgeVertex& vertex, const Point1D& coordinates ) const
    {
        impl_->set_texture_coordinates( vertex, coordinates );
    }

    template < typename Archive >
    void Texture< 1 >::serialize( Archive& archive )
    {
        archive.ext( *this, Growable< Archive, Texture1D >{
                                { []( Archive& a, Texture1D& texture ) {
                                    a.object( texture.impl_ );
                                } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, Texture< 1 > );
} // namespace geode