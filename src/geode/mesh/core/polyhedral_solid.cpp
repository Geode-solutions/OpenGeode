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

#include <geode/mesh/core/polyhedral_solid.h>

#include <absl/container/flat_hash_set.h>

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/detail/mapping_after_deletion.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/vector.h>

#include <geode/mesh/builder/polyhedral_solid_builder.h>
#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/detail/facet_storage.h>
#include <geode/mesh/core/mesh_factory.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< PolyhedralSolid< dimension > >
        PolyhedralSolid< dimension >::create()
    {
        return MeshFactory::create_default_mesh< PolyhedralSolid< dimension > >(
            PolyhedralSolid< dimension >::type_name_static() );
    }

    template < index_t dimension >
    std::unique_ptr< PolyhedralSolid< dimension > >
        PolyhedralSolid< dimension >::create( const MeshImpl& impl )
    {
        return MeshFactory::create_mesh< PolyhedralSolid< dimension > >( impl );
    }

    template < index_t dimension >
    std::unique_ptr< PolyhedralSolid< dimension > >
        PolyhedralSolid< dimension >::clone() const
    {
        auto clone = create( this->impl_name() );
        auto builder = PolyhedralSolidBuilder< dimension >::create( *clone );
        builder->copy( *this, {} );
        return clone;
    }

    template class opengeode_mesh_api PolyhedralSolid< 3 >;
} // namespace geode
