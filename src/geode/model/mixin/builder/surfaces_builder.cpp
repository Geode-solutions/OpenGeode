/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/model/mixin/builder/surfaces_builder.hpp>

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surfaces.hpp>

namespace geode
{
    template < index_t dimension >
    const uuid& SurfacesBuilder< dimension >::create_surface()
    {
        return surfaces_.create_surface(
            typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template < index_t dimension >
    const uuid& SurfacesBuilder< dimension >::create_surface(
        const MeshImpl& impl )
    {
        return surfaces_.create_surface(
            impl, typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::create_surface( uuid surface_id )
    {
        surfaces_.create_surface( std::move( surface_id ),
            typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::create_surface(
        uuid surface_id, const MeshImpl& impl )
    {
        surfaces_.create_surface( std::move( surface_id ), impl,
            typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::delete_surface(
        const Surface< dimension >& surface )
    {
        surfaces_.delete_surface(
            surface, typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::load_surfaces(
        std::string_view directory )
    {
        return surfaces_.load_surfaces(
            directory, typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::set_surface_name(
        const Surface< dimension >& surface, std::string_view name )
    {
        surfaces_
            .modifiable_surface( surface.id(),
                typename Surface< dimension >::SurfacesBuilderKey{} )
            .set_surface_name(
                name, typename Surface< dimension >::SurfacesBuilderKey{} );
        surface_mesh_builder( surface )->set_name( name );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::set_surface_active(
        const Surface< dimension >& surface, bool active )
    {
        surfaces_
            .modifiable_surface( surface.id(),
                typename Surface< dimension >::SurfacesBuilderKey{} )
            .set_surface_active(
                active, typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template < index_t dimension >
    void SurfacesBuilder< dimension >::set_surface_mesh(
        const uuid& id, std::unique_ptr< SurfaceMesh< dimension > > mesh )
    {
        surfaces_
            .modifiable_surface(
                id, typename Surface< dimension >::SurfacesBuilderKey{} )
            .set_mesh( std::move( mesh ),
                typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template < index_t dimension >
    SurfaceMesh< dimension >&
        SurfacesBuilder< dimension >::modifiable_surface_mesh( const uuid& id )
    {
        return surfaces_
            .modifiable_surface(
                id, typename Surface< dimension >::SurfacesBuilderKey{} )
            .modifiable_mesh(
                typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template < index_t dimension >
    std::unique_ptr< SurfaceMesh< dimension > >
        SurfacesBuilder< dimension >::steal_surface_mesh(
            const Surface< dimension >& surface )
    {
        return surfaces_
            .modifiable_surface( surface.id(),
                typename Surface< dimension >::SurfacesBuilderKey{} )
            .steal_mesh( typename Surface< dimension >::SurfacesBuilderKey{} );
    }

    template class opengeode_model_api SurfacesBuilder< 2 >;
    template class opengeode_model_api SurfacesBuilder< 3 >;
} // namespace geode
