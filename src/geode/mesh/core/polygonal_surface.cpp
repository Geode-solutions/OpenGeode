/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/mesh/core/polygonal_surface.h>

#include <geode/mesh/builder/polygonal_surface_builder.h>
#include <geode/mesh/core/mesh_factory.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< PolygonalSurface< dimension > >
        PolygonalSurface< dimension >::create()
    {
        return MeshFactory::create_default_mesh<
            PolygonalSurface< dimension > >(
            PolygonalSurface< dimension >::type_name_static() );
    }

    template < index_t dimension >
    std::unique_ptr< PolygonalSurface< dimension > >
        PolygonalSurface< dimension >::create( const MeshImpl& impl )
    {
        return MeshFactory::create_mesh< PolygonalSurface< dimension > >(
            impl );
    }

    template < index_t dimension >
    std::unique_ptr< PolygonalSurface< dimension > >
        PolygonalSurface< dimension >::clone() const
    {
        auto clone = create( this->impl_name() );
        auto builder = PolygonalSurfaceBuilder< dimension >::create( *clone );
        builder->copy( *this, {} );
        return clone;
    }

    template class opengeode_mesh_api PolygonalSurface< 2 >;
    template class opengeode_mesh_api PolygonalSurface< 3 >;
} // namespace geode
