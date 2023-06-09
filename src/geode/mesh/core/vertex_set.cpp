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

#include <geode/mesh/core/vertex_set.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/builder/vertex_set_builder.h>
#include <geode/mesh/core/mesh_factory.h>

namespace geode
{
    class VertexSet::Impl
    {
    public:
        AttributeManager& vertex_attribute_manager() const
        {
            return vertex_attribute_manager_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.object( impl.vertex_attribute_manager_ );
                } } } );
        }

    private:
        mutable AttributeManager vertex_attribute_manager_;
    };

    VertexSet::VertexSet() {} // NOLINT

    VertexSet::VertexSet( VertexSet&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    VertexSet& VertexSet::operator=( VertexSet&& other )
    {
        impl_ = std::move( other.impl_ );
        return *this;
    }

    VertexSet::~VertexSet() {} // NOLINT

    std::unique_ptr< VertexSet > VertexSet::create()
    {
        return MeshFactory::create_default_mesh< VertexSet >(
            VertexSet::type_name_static() );
    }

    std::unique_ptr< VertexSet > VertexSet::create( const MeshImpl& impl )
    {
        return MeshFactory::create_mesh< VertexSet >( impl );
    }

    index_t VertexSet::nb_vertices() const
    {
        return vertex_attribute_manager().nb_elements();
    }

    AttributeManager& VertexSet::vertex_attribute_manager() const
    {
        return impl_->vertex_attribute_manager();
    }

    template < typename Archive >
    void VertexSet::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, VertexSet >{
                       { []( Archive& a, VertexSet& vertex_set ) {
                            a.object( vertex_set.impl_ );
                        },
                           []( Archive& a, VertexSet& vertex_set ) {
                               a.object( vertex_set.impl_ );
                               a.ext( vertex_set,
                                   bitsery::ext::BaseClass< Identifier >{} );
                           } } } );
    }

    std::unique_ptr< VertexSet > VertexSet::clone() const
    {
        auto clone = create( impl_name() );
        auto builder = VertexSetBuilder::create( *clone );
        builder->copy( *this );
        return clone;
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, VertexSet );
} // namespace geode
