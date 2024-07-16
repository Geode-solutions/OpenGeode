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

#include <geode/model/mixin/core/corner.hpp>

#include <memory>
#include <string_view>

#include <bitsery/ext/inheritance.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/mesh_id.hpp>
#include <geode/mesh/core/point_set.hpp>

#include <geode/model/mixin/core/component.hpp>
#include <geode/model/mixin/core/detail/mesh_storage.hpp>

namespace geode
{
    template < index_t dimension >
    class Corner< dimension >::Impl : public detail::MeshStorage< Mesh >
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
    Corner< dimension >::~Corner() = default;

    template < index_t dimension >
    Corner< dimension >::Corner()
        : Corner( MeshFactory::default_impl( Mesh::type_name_static() ) )
    {
    }

    template < index_t dimension >
    Corner< dimension >::Corner( Corner&& ) noexcept = default;

    template < index_t dimension >
    Corner< dimension >::Corner( CornersKey /*unused*/ ) : Corner()
    {
    }

    template < index_t dimension >
    Corner< dimension >::Corner( const MeshImpl& impl, CornersKey /*unused*/ )
        : Corner( impl )
    {
    }

    template < index_t dimension >
    Corner< dimension >::Corner( const MeshImpl& impl )
    {
        impl_->set_mesh( this->id(), Mesh::create( impl ) );
    }

    template < index_t dimension >
    auto Corner< dimension >::mesh() const -> const Mesh&
    {
        return impl_->mesh();
    }

    template < index_t dimension >
    auto Corner< dimension >::modifiable_mesh( CornersKey /*unused*/ ) -> Mesh&
    {
        return modifiable_mesh();
    }

    template < index_t dimension >
    auto Corner< dimension >::modifiable_mesh() -> Mesh&
    {
        return impl_->modifiable_mesh();
    }

    template < index_t dimension >
    const MeshImpl& Corner< dimension >::mesh_type() const
    {
        return impl_->mesh_type();
    }

    template < index_t dimension >
    void Corner< dimension >::set_corner_name(
        std::string_view name, CornersBuilderKey /*unused*/ )
    {
        this->set_name( name );
    }

    template < index_t dimension >
    auto Corner< dimension >::modifiable_mesh(
        CornersBuilderKey /*unused*/ ) -> Mesh&
    {
        return modifiable_mesh();
    }

    template < index_t dimension >
    template < typename Archive >
    void Corner< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, Corner >{
                { []( Archive& archive2, Corner& corner ) {
                     archive2.object( corner.impl_ );
                     archive2.ext( corner,
                         bitsery::ext::BaseClass< Component< dimension > >{} );
                     IdentifierBuilder mesh_builder{ corner.modifiable_mesh() };
                     mesh_builder.set_id( corner.id() );
                 },
                    []( Archive& archive2, Corner& corner ) {
                        archive2.object( corner.impl_ );
                        archive2.ext( corner, bitsery::ext::BaseClass<
                                                  Component< dimension > >{} );
                    } } } );
    }

    template < index_t dimension >
    void Corner< dimension >::set_mesh(
        std::unique_ptr< Mesh > mesh, CornersKey /*unused*/ )
    {
        impl_->set_mesh( this->id(), std::move( mesh ) );
    }

    template < index_t dimension >
    void Corner< dimension >::set_mesh(
        std::unique_ptr< Mesh > mesh, CornersBuilderKey /*unused*/ )
    {
        impl_->set_mesh( this->id(), std::move( mesh ) );
    }

    template class opengeode_model_api Corner< 2 >;
    template class opengeode_model_api Corner< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Corner< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Corner< 3 > );
} // namespace geode
