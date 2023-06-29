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

#include <geode/mesh/core/regular_grid_solid.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/regular_grid_solid_builder.h>
#include <geode/mesh/core/mesh_factory.h>

namespace geode
{
    std::unique_ptr< RegularGrid< 3 > > RegularGrid< 3 >::create()
    {
        return MeshFactory::create_default_mesh< RegularGrid< 3 > >(
            RegularGrid< 3 >::type_name_static() );
    }

    std::unique_ptr< RegularGrid< 3 > > RegularGrid< 3 >::create(
        const MeshImpl& impl )
    {
        return MeshFactory::create_mesh< RegularGrid< 3 > >( impl );
    }

    std::unique_ptr< RegularGrid< 3 > > RegularGrid< 3 >::clone() const
    {
        auto clone = create( this->impl_name() );
        auto builder = RegularGridBuilder< 3 >::create( *clone );
        builder->copy( *this );
        return clone;
    }

    AttributeManager& RegularGrid< 3 >::cell_attribute_manager() const
    {
        return polyhedron_attribute_manager();
    }

    template < typename Archive >
    void RegularGrid< 3 >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, RegularGrid< 3 > >{
                { []( Archive& a, RegularGrid< 3 >& grid ) {
                     a.ext( grid, bitsery::ext::BaseClass< SolidMesh< 3 > >{} );
                     a.ext( grid, bitsery::ext::BaseClass< Grid< 3 > >{} );
                     GridBuilder3D builder{ grid };
                     builder.set_grid_origin( grid.point( 0 ) );
                 },
                    []( Archive& a, RegularGrid< 3 >& grid ) {
                        a.ext(
                            grid, bitsery::ext::BaseClass< SolidMesh< 3 > >{} );
                        a.ext( grid, bitsery::ext::BaseClass< Grid< 3 > >{} );
                    } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, RegularGrid< 3 > );
} // namespace geode
