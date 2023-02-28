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

#include <geode/mesh/core/texture2d.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/image/core/raster_image.h>

#include <geode/mesh/core/private/texture_impl.h>

namespace geode
{
    class Texture< 2 >::Impl : public detail::TextureImpl< 2 >
    {
        friend class bitsery::Access;

    public:
        Impl() = default;
        Impl( AttributeManager& manager, absl::string_view name )
            : detail::TextureImpl< 2 >{ manager, name }
        {
        }

        const Point2D& texture_coordinates( const PolygonVertex& vertex ) const
        {
            return texture_coordinates_impl(
                vertex.polygon_id, vertex.vertex_id );
        }

        void set_texture_coordinates(
            const PolygonVertex& vertex, const Point2D& coordinates ) const
        {
            set_texture_coordinates_impl(
                vertex.polygon_id, vertex.vertex_id, coordinates );
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.ext( impl,
                        bitsery::ext::BaseClass< detail::TextureImpl< 2 > >{} );
                } } } );
        }
    };

    Texture< 2 >::Texture( AttributeManager& manager, absl::string_view name )
        : impl_{ manager, name }
    {
    }

    Texture< 2 >::Texture( Texture&& other ) : impl_{ std::move( other.impl_ ) }
    {
    }

    Texture< 2 >::Texture() {}

    Texture< 2 >::~Texture() {}

    const RasterImage2D& Texture< 2 >::image() const
    {
        return impl_->image();
    }

    void Texture< 2 >::set_image( RasterImage2D&& image )
    {
        impl_->set_image( std::move( image ) );
    }

    const Point2D& Texture< 2 >::texture_coordinates(
        const PolygonVertex& vertex ) const
    {
        return impl_->texture_coordinates( vertex );
    }

    void Texture< 2 >::set_texture_coordinates(
        const PolygonVertex& vertex, const Point2D& coordinates ) const
    {
        impl_->set_texture_coordinates( vertex, coordinates );
    }

    template < typename Archive >
    void Texture< 2 >::serialize( Archive& archive )
    {
        archive.ext( *this, Growable< Archive, Texture< 2 > >{
                                { []( Archive& a, Texture< 2 >& texture ) {
                                    a.object( texture.impl_ );
                                } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, Texture< 2 > );
} // namespace geode