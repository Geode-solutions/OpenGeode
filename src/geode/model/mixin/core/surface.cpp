/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/detail/mesh_storage.h>

namespace geode
{
    template < index_t dimension >
    class Surface< dimension >::Impl
        : public detail::MeshStorage< SurfaceMesh< dimension > >
    {
    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.ext(
                        impl, bitsery::ext::BaseClass< detail::MeshStorage<
                                  SurfaceMesh< dimension > > >{} );
                } );
        }
    };

    template < index_t dimension >
    Surface< dimension >::~Surface() // NOLINT
    {
    }

    template < index_t dimension >
    Surface< dimension >::Surface( Surface< dimension >&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    Surface< dimension >::Surface()
        : Surface( MeshFactory::default_impl(
            PolygonalSurface< dimension >::type_name_static() ) )
    {
    }

    template < index_t dimension >
    Surface< dimension >::Surface( const MeshImpl& impl )
    {
        impl_->set_mesh( SurfaceMesh< dimension >::create( impl ) );
    }

    template < index_t dimension >
    const SurfaceMesh< dimension >& Surface< dimension >::mesh() const
    {
        return impl_->mesh();
    }

    template < index_t dimension >
    SurfaceMesh< dimension >& Surface< dimension >::modifiable_mesh()
    {
        return impl_->modifiable_mesh();
    }

    template < index_t dimension >
    const MeshImpl& Surface< dimension >::mesh_type() const
    {
        return impl_->mesh_type();
    }

    template < index_t dimension >
    template < typename Archive >
    void Surface< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, Surface >{},
            []( Archive& archive, Surface& surface ) {
                archive.object( surface.impl_ );
                archive.ext( surface,
                    bitsery::ext::BaseClass< Component< dimension > >{} );
            } );
    }

    template < index_t dimension >
    void Surface< dimension >::set_mesh(
        std::unique_ptr< SurfaceMesh< dimension > > mesh, SurfacesKey )
    {
        impl_->set_mesh( std::move( mesh ) );
    }

    template < index_t dimension >
    void Surface< dimension >::set_mesh(
        std::unique_ptr< SurfaceMesh< dimension > > mesh, SurfacesBuilderKey )
    {
        impl_->set_mesh( std::move( mesh ) );
    }

    template class opengeode_model_api Surface< 2 >;
    template class opengeode_model_api Surface< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Surface< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Surface< 3 > );
} // namespace geode
