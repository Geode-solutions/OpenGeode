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

#include <geode/mesh/core/regular_grid_surface.hpp>

#include <geode/basic/bitsery_archive.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/regular_grid_surface_builder.hpp>
#include <geode/mesh/core/mesh_factory.hpp>

namespace geode
{
    std::unique_ptr< RegularGrid< 2 > > RegularGrid< 2 >::create()
    {
        return MeshFactory::create_default_mesh< RegularGrid< 2 > >(
            RegularGrid< 2 >::type_name_static() );
    }

    std::unique_ptr< RegularGrid< 2 > > RegularGrid< 2 >::create(
        const MeshImpl& impl )
    {
        return MeshFactory::create_mesh< RegularGrid< 2 > >( impl );
    }

    MeshType RegularGrid< 2 >::type_name_static()
    {
        return MeshType{ "RegularGrid2D" };
    }

    std::unique_ptr< RegularGrid< 2 > > RegularGrid< 2 >::clone() const
    {
        auto clone = create( this->impl_name() );
        auto builder = RegularGridBuilder< 2 >::create( *clone );
        builder->copy_identifier( *this );
        builder->copy( *this );
        return clone;
    }

    AttributeManager& RegularGrid< 2 >::cell_attribute_manager() const
    {
        return polygon_attribute_manager();
    }

    AttributeManager& RegularGrid< 2 >::grid_vertex_attribute_manager() const
    {
        return vertex_attribute_manager();
    }

    SurfaceMesh< 2 >::VerticesAroundVertex
        RegularGrid< 2 >::vertices_around_vertex( index_t vertex_id ) const
    {
        VerticesAroundVertex result;
        const auto indices = this->vertex_indices( vertex_id );
        for( const auto d : LRange{ 2 } )
        {
            if( const auto next = this->next_vertex( indices, d ) )
            {
                result.push_back( this->vertex_index( next.value() ) );
            }
            if( const auto previous = this->previous_vertex( indices, d ) )
            {
                result.push_back( this->vertex_index( previous.value() ) );
            }
        }
        return result;
    }

    template < typename Archive >
    void RegularGrid< 2 >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, RegularGrid< 2 > >{
                { []( Archive& a, RegularGrid< 2 >& grid ) {
                     a.ext(
                         grid, bitsery::ext::BaseClass< SurfaceMesh< 2 > >{} );
                     a.ext( grid, bitsery::ext::BaseClass< Grid< 2 > >{} );
                     GridBuilder2D builder{ grid };
                     builder.set_grid_origin( grid.grid_point( { 0, 0 } ) );
                 },
                    []( Archive& a, RegularGrid< 2 >& grid ) {
                        a.ext( grid,
                            bitsery::ext::BaseClass< SurfaceMesh< 2 > >{} );
                        a.ext( grid, bitsery::ext::BaseClass< Grid< 2 > >{} );
                    } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, RegularGrid< 2 > );
} // namespace geode
