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

#include <geode/model/mixin/core/block.hpp>

#include <memory>
#include <string_view>

#include <bitsery/ext/inheritance.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/mesh_id.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>

#include <geode/model/mixin/core/component.hpp>
#include <geode/model/mixin/core/detail/mesh_storage.hpp>

namespace geode
{
    template < index_t dimension >
    class Block< dimension >::Impl : public detail::MeshStorage< Mesh >
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
    Block< dimension >::Block()
        : Block( MeshFactory::default_impl(
              PolyhedralSolid< dimension >::type_name_static() ) )
    {
    }

    template < index_t dimension >
    Block< dimension >::Block( Block&& ) noexcept = default;

    template < index_t dimension >
    Block< dimension >::Block( BlocksKey /*unused*/ ) : Block()
    {
    }

    template < index_t dimension >
    Block< dimension >::Block( const MeshImpl& impl, BlocksKey /*unused*/ )
        : Block( impl )
    {
    }

    template < index_t dimension >
    Block< dimension >::Block( const MeshImpl& impl )
    {
        impl_->set_mesh( this->id(), Mesh::create( impl ) );
    }

    template < index_t dimension >
    Block< dimension >::~Block() = default;

    template < index_t dimension >
    auto Block< dimension >::get_mesh() const -> const Mesh&
    {
        return impl_->mesh();
    }

    template < index_t dimension >
    auto Block< dimension >::get_modifiable_mesh() -> Mesh&
    {
        return impl_->modifiable_mesh();
    }

    template < index_t dimension >
    const MeshImpl& Block< dimension >::mesh_type() const
    {
        return impl_->mesh_type();
    }

    template < index_t dimension >
    template < typename Archive >
    void Block< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, Block >{
                { []( Archive& archive2, Block& block ) {
                     archive2.object( block.impl_ );
                     archive2.ext( block,
                         bitsery::ext::BaseClass< Component< dimension > >{} );
                     IdentifierBuilder mesh_builder{
                         block.get_modifiable_mesh()
                     };
                     mesh_builder.set_id( block.id() );
                 },
                    []( Archive& archive2, Block& block ) {
                        archive2.object( block.impl_ );
                        archive2.ext( block, bitsery::ext::BaseClass<
                                                 Component< dimension > >{} );
                    } } } );
    }

    template < index_t dimension >
    void Block< dimension >::set_mesh(
        std::unique_ptr< Mesh > mesh, BlocksKey /*unused*/ )
    {
        impl_->set_mesh( this->id(), std::move( mesh ) );
    }

    template < index_t dimension >
    void Block< dimension >::set_mesh(
        std::unique_ptr< Mesh > mesh, BlocksBuilderKey /*unused*/ )
    {
        impl_->set_mesh( this->id(), std::move( mesh ) );
    }

    template < index_t dimension >
    void Block< dimension >::set_block_name(
        std::string_view name, BlocksBuilderKey /*unused*/ )
    {
        this->set_name( name );
    }

    template < index_t dimension >
    void Block< dimension >::set_block_active(
        bool is_active, BlocksBuilderKey /*unused*/ )
    {
        this->set_active( is_active );
    }

    template < index_t dimension >
    auto Block< dimension >::steal_mesh( BlocksBuilderKey /*unused*/ )
        -> std::unique_ptr< Mesh >
    {
        return impl_->steal_mesh();
    }

    template class opengeode_model_api Block< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Block< 3 > );
} // namespace geode
