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

#include <geode/georepresentation/core/block.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/georepresentation/core/detail/mesh_storage.h>

#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>

namespace geode
{
    template < index_t dimension >
    class Block< dimension >::Impl
        : public detail::MeshStorage< PolyhedralSolid< dimension > >
    {
        using base_class = detail::MeshStorage< PolyhedralSolid< dimension > >;

    public:
        Impl() : base_class( &create_mesh ) {}

        static void create_mesh( const MeshType& type, base_class& storage )
        {
            if( TetrahedralSolidFactory< dimension >::has_creator( type ) )
            {
                storage.set_mesh(
                    TetrahedralSolid< dimension >::create( type ) );
            }
            else if( PolyhedralSolidFactory< dimension >::has_creator( type ) )
            {
                storage.set_mesh(
                    PolyhedralSolid< dimension >::create( type ) );
            }
            else
            {
                throw OpenGeodeException( "Unknown mesh type: ", type.get() );
            }
        }
    };

    template < index_t dimension >
    Block< dimension >::Block()
    {
        impl_->set_mesh( PolyhedralSolid< dimension >::create() );
    }

    template < index_t dimension >
    Block< dimension >::Block( const MeshType& type )
    {
        Impl::create_mesh( type, *impl_ );
    }

    template < index_t dimension >
    Block< dimension >::~Block() // NOLINT
    {
    }

    template < index_t dimension >
    const PolyhedralSolid< dimension >& Block< dimension >::mesh() const
    {
        return impl_->mesh();
    }

    template < index_t dimension >
    PolyhedralSolid< dimension >& Block< dimension >::modifiable_mesh()
    {
        return impl_->modifiable_mesh();
    }

    template < index_t dimension >
    void Block< dimension >::ensure_mesh_type()
    {
        return impl_->ensure_mesh_type();
    }

    template < index_t dimension >
    template < typename Archive >
    void Block< dimension >::serialize( Archive& archive )
    {
        archive.object( impl_ );
        archive.ext(
            *this, bitsery::ext::BaseClass< Component< dimension > >{} );
    }

    template class opengeode_georepresentation_api Block< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_georepresentation_api, Block< 3 > );
} // namespace geode
