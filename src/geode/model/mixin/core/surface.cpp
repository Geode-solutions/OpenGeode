/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/model/mixin/core/surface.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/model/mixin/core/detail/mesh_storage.h>

#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/triangulated_surface.h>

namespace geode
{
    template < index_t dimension >
    class Surface< dimension >::Impl
        : public detail::MeshStorage< PolygonalSurface< dimension > >
    {
        using base_class = detail::MeshStorage< PolygonalSurface< dimension > >;

    public:
        Impl() : base_class( &create_mesh ) {}

        static void create_mesh( const MeshType& type, base_class& storage )
        {
            if( TriangulatedSurfaceFactory< dimension >::has_creator( type ) )
            {
                storage.set_mesh(
                    TriangulatedSurface< dimension >::create( type ) );
            }
            else if( PolygonalSurfaceFactory< dimension >::has_creator( type ) )
            {
                storage.set_mesh(
                    PolygonalSurface< dimension >::create( type ) );
            }
            else
            {
                throw OpenGeodeException( "Unknown mesh type: ", type.get() );
            }
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.ext(
                        impl, bitsery::ext::BaseClass< detail::MeshStorage<
                                  PolygonalSurface< dimension > > >{} );
                } );
        }
    };

    template < index_t dimension >
    Surface< dimension >::~Surface() // NOLINT
    {
    }

    template < index_t dimension >
    Surface< dimension >::Surface()
    {
        impl_->set_mesh( PolygonalSurface< dimension >::create() );
    }

    template < index_t dimension >
    Surface< dimension >::Surface( const MeshType& type )
    {
        Impl::create_mesh( type, *impl_ );
    }

    template < index_t dimension >
    const PolygonalSurface< dimension >& Surface< dimension >::mesh() const
    {
        return impl_->mesh();
    }

    template < index_t dimension >
    PolygonalSurface< dimension >& Surface< dimension >::modifiable_mesh()
    {
        return impl_->modifiable_mesh();
    }

    template < index_t dimension >
    void Surface< dimension >::ensure_mesh_type()
    {
        return impl_->ensure_mesh_type();
    }

    template < index_t dimension >
    template < typename Archive >
    void Surface< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, geode::Growable< Archive, Surface >{},
            []( Archive& archive, Surface& surface ) {
                archive.object( surface.impl_ );
                archive.ext( surface,
                    bitsery::ext::BaseClass< Component< dimension > >{} );
            } );
    }

    template class opengeode_model_api Surface< 2 >;
    template class opengeode_model_api Surface< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Surface< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Surface< 3 > );
} // namespace geode
