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

#include <geode/model/mixin/core/line.h>

#include <memory>

#include <bitsery/ext/inheritance.h>

#include <absl/strings/string_view.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/mesh_id.h>

#include <geode/model/mixin/core/component.h>
#include <geode/model/mixin/core/detail/mesh_storage.h>

namespace geode
{
    template < index_t dimension >
    class Line< dimension >::Impl : public detail::MeshStorage< Mesh >
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
    Line< dimension >::~Line() // NOLINT
    {
    }

    template < index_t dimension >
    Line< dimension >::Line()
        : Line( MeshFactory::default_impl( Mesh::type_name_static() ) )
    {
    }

    template < index_t dimension >
    Line< dimension >::Line( Line&& other ) noexcept
        : Component< dimension >{ std::move( other ) },
          impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    Line< dimension >::Line( LinesKey /*unused*/ ) : Line()
    {
    }

    template < index_t dimension >
    Line< dimension >::Line( const MeshImpl& impl, LinesKey /*unused*/ )
        : Line( impl )
    {
    }

    template < index_t dimension >
    Line< dimension >::Line( const MeshImpl& impl )
    {
        impl_->set_mesh( this->id(), Mesh::create( impl ) );
    }

    template < index_t dimension >
    auto Line< dimension >::mesh() const -> const Mesh&
    {
        return impl_->mesh();
    }

    template < index_t dimension >
    auto Line< dimension >::modifiable_mesh( LinesKey /*unused*/ ) -> Mesh&
    {
        return modifiable_mesh();
    }

    template < index_t dimension >
    auto Line< dimension >::modifiable_mesh() -> Mesh&
    {
        return impl_->modifiable_mesh();
    }

    template < index_t dimension >
    const MeshImpl& Line< dimension >::mesh_type() const
    {
        return impl_->mesh_type();
    }

    template < index_t dimension >
    template < typename Archive >
    void Line< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, Line >{
                { []( Archive& archive2, Line& line ) {
                     archive2.object( line.impl_ );
                     archive2.ext( line,
                         bitsery::ext::BaseClass< Component< dimension > >{} );
                     IdentifierBuilder mesh_builder{ line.modifiable_mesh() };
                     mesh_builder.set_id( line.id() );
                 },
                    []( Archive& archive2, Line& line ) {
                        archive2.object( line.impl_ );
                        archive2.ext( line, bitsery::ext::BaseClass<
                                                Component< dimension > >{} );
                    } } } );
    }

    template < index_t dimension >
    void Line< dimension >::set_mesh(
        std::unique_ptr< Mesh > mesh, LinesKey /*unused*/ )
    {
        impl_->set_mesh( this->id(), std::move( mesh ) );
    }

    template < index_t dimension >
    void Line< dimension >::set_mesh(
        std::unique_ptr< Mesh > mesh, LinesBuilderKey /*unused*/ )
    {
        impl_->set_mesh( this->id(), std::move( mesh ) );
    }

    template < index_t dimension >
    void Line< dimension >::set_line_name(
        absl::string_view name, LinesBuilderKey /*unused*/ )
    {
        this->set_name( name );
    }

    template < index_t dimension >
    auto Line< dimension >::modifiable_mesh( LinesBuilderKey /*unused*/ )
        -> Mesh&
    {
        return modifiable_mesh();
    }

    template class opengeode_model_api Line< 2 >;
    template class opengeode_model_api Line< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Line< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_model_api, Line< 3 > );
} // namespace geode
