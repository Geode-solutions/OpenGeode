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

#include <geode/model/mixin/builder/surfaces_builder.h>

#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/surface.h>
#include <geode/model/mixin/core/surfaces.h>

namespace geode
{
    template < index_t dimension >
    const uuid& SurfacesBuilder< dimension >::create_surface()
    {
        return surfaces_.create_surface( {} );
    }

    template < index_t dimension >
    const uuid& SurfacesBuilder< dimension >::create_surface(
        const MeshImpl& impl )
    {
        return surfaces_.create_surface( impl, {} );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::create_surface( uuid surface_id )
    {
        surfaces_.create_surface( std::move( surface_id ), {} );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::create_surface(
        uuid surface_id, const MeshImpl& impl )
    {
        surfaces_.create_surface( std::move( surface_id ), impl, {} );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::delete_surface(
        const Surface< dimension >& surface )
    {
        surfaces_.delete_surface( surface, {} );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::load_surfaces(
        std::string_view directory )
    {
        return surfaces_.load_surfaces( directory, {} );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::set_surface_name(
        const uuid& id, std::string_view name )
    {
        surfaces_.modifiable_surface( id, {} ).set_surface_name( name, {} );
        surface_mesh_builder( id )->set_name( name );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::set_surface_mesh(
        const uuid& id, std::unique_ptr< SurfaceMesh< dimension > > mesh )
    {
        surfaces_.modifiable_surface( id, {} ).set_mesh( std::move( mesh ),
            typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template < index_t dimension >
    SurfaceMesh< dimension >&
        SurfacesBuilder< dimension >::modifiable_surface_mesh( const uuid& id )
    {
        return surfaces_.modifiable_surface( id, {} ).modifiable_mesh(
            typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template class opengeode_model_api SurfacesBuilder< 2 >;
    template class opengeode_model_api SurfacesBuilder< 3 >;
} // namespace geode
