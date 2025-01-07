/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/model/mixin/core/surface.hpp>

#include <memory>
#include <string_view>

#include <bitsery/ext/inheritance.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/mesh_id.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/component.hpp>
#include <geode/model/mixin/core/detail/mesh_storage.hpp>

namespace geode
{
    template < index_t dimension >
    class Surface< dimension >::Impl : public detail::MeshStorage< Mesh >
    {
    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive,
                                    Impl >{ { []( Archive& archive2,
                                                  Impl& impl ) {
                archive2.ext( impl,
                    bitsery::ext::BaseClass< detail::MeshStorage< Mesh > >{} );
            } } } );
        }
    };

    template < index_t dimension >
    Surface< dimension >::~Surface() = default;

    template < index_t dimension >
    Surface< dimension >::Surface( Surface< dimension >&& ) noexcept = default;

    template < index_t dimension >
    Surface< dimension >::Surface( SurfacesKey /*unused*/ ) : Surface()
    {
    }

    template < index_t dimension >
    Surface< dimension >::Surface(
        const MeshImpl& impl, SurfacesKey /*unused*/ )
        : Surface( impl )
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
        impl_->set_mesh( this->id(), Mesh::create( impl ) );
    }

    template < index_t dimension >
    auto Surface< dimension >::get_mesh() const -> const Mesh&
    {
        return impl_->mesh();
    }

    template < index_t dimension >
    auto Surface< dimension >::get_modifiable_mesh() -> Mesh&
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
        archive.ext( *this,
            Growable< Archive, Surface >{
                { []( Archive& archive2, Surface& surface ) {
                     archive2.object( surface.impl_ );
                     archive2.ext( surface,
                         bitsery::ext::BaseClass< Component< dimension > >{} );
                     IdentifierBuilder mesh_builder{
                         surface.get_modifiable_mesh()
                     };
                     mesh_builder.set_id( surface.id() );
                 },
                    []( Archive& archive2, Surface& surface ) {
                        archive2.object( surface.impl_ );
                        archive2.ext( surface, bitsery::ext::BaseClass<
                                                   Component< dimension > >{} );
                    } } } );
    }

    template < index_t dimension >
    void Surface< dimension >::set_mesh(
        std::unique_ptr< Mesh > mesh, SurfacesKey /*unused*/ )
    {
        impl_->set_mesh( this->id(), std::move( mesh ) );
    }

    template < index_t dimension >
    void Surface< dimension >::set_mesh(
        std::unique_ptr< Mesh > mesh, SurfacesBuilderKey /*unused*/ )
    {
        impl_->set_mesh( this->id(), std::move( mesh ) );
    }

    template < index_t dimension >
    void Surface< dimension >::set_surface_name(
        std::string_view name, SurfacesBuilderKey /*unused*/ )
    {
        this->set_name( name );
    }

    template < index_t dimension >
    auto Surface< dimension >::steal_mesh(
        SurfacesBuilderKey /*unused*/ ) -> std::unique_ptr< Mesh >
    {
        return impl_->steal_mesh();
    }

    template class opengeode_model_api Surface< 2 >;
    template class opengeode_model_api Surface< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Surface< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Surface< 3 > );
} // namespace geode
