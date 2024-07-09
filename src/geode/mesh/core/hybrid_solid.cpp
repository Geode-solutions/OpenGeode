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

#include <geode/mesh/core/hybrid_solid.h>

#include <geode/basic/bitsery_archive.h>

#include <geode/mesh/builder/hybrid_solid_builder.h>
#include <geode/mesh/core/mesh_factory.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< HybridSolid< dimension > >
        HybridSolid< dimension >::create()
    {
        return mesh_factory.create_default_mesh< HybridSolid< dimension > >(
            HybridSolid< dimension >::type_name_static() );
    }

    template < index_t dimension >
    std::unique_ptr< HybridSolid< dimension > >
        HybridSolid< dimension >::create( const MeshImpl& impl )
    {
        return mesh_factory.create_mesh< HybridSolid< dimension > >( impl );
    }

    template < index_t dimension >
    MeshType HybridSolid< dimension >::type_name_static()
    {
        return MeshType{ absl::StrCat( "HybridSolid", dimension, "D" ) };
    }

    template < index_t dimension >
    std::unique_ptr< HybridSolid< dimension > >
        HybridSolid< dimension >::clone() const
    {
        auto clone = create( this->impl_name() );
        auto builder = HybridSolidBuilder< dimension >::create( *clone );
        builder->copy_identifier( *this );
        builder->copy( *this );
        return clone;
    }

    template < index_t dimension >
    template < typename Archive >
    void HybridSolid< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, HybridSolid >{
                { []( Archive& a, HybridSolid& hybrid_solid ) {
                    a.ext( hybrid_solid,
                        bitsery::ext::BaseClass< SolidMesh< dimension > >{} );
                } } } );
    }

    template class opengeode_mesh_api HybridSolid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, HybridSolid< 3 > );
} // namespace geode
