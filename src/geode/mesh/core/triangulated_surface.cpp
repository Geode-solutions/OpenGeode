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

#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/basic/bitsery_archive.hpp>

#include <geode/geometry/basic_objects/triangle.hpp>

#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/mesh_factory.hpp>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TriangulatedSurface< dimension > >
        TriangulatedSurface< dimension >::create()
    {
        return MeshFactory::create_default_mesh<
            TriangulatedSurface< dimension > >(
            TriangulatedSurface< dimension >::type_name_static() );
    }

    template < index_t dimension >
    std::unique_ptr< TriangulatedSurface< dimension > >
        TriangulatedSurface< dimension >::create( const MeshImpl& type )
    {
        return MeshFactory::create_mesh< TriangulatedSurface< dimension > >(
            type );
    }

    template < index_t dimension >
    MeshType TriangulatedSurface< dimension >::type_name_static()
    {
        return MeshType{ absl::StrCat(
            "TriangulatedSurface", dimension, "D" ) };
    }

    template < index_t dimension >
    std::unique_ptr< TriangulatedSurface< dimension > >
        TriangulatedSurface< dimension >::clone() const
    {
        auto clone = create( this->impl_name() );
        auto builder =
            TriangulatedSurfaceBuilder< dimension >::create( *clone );
        builder->copy_identifier( *this );
        builder->copy( *this );
        return clone;
    }

    template < index_t dimension >
    Triangle< dimension > TriangulatedSurface< dimension >::triangle(
        index_t triangle_id ) const
    {
        return { this->point( this->polygon_vertex( { triangle_id, 0 } ) ),
            this->point( this->polygon_vertex( { triangle_id, 1 } ) ),
            this->point( this->polygon_vertex( { triangle_id, 2 } ) ) };
    }

    template < index_t dimension >
    template < typename Archive >
    void TriangulatedSurface< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, TriangulatedSurface >{
                { []( Archive& a, TriangulatedSurface& triangulated_surface ) {
                    a.ext( triangulated_surface,
                        bitsery::ext::BaseClass< SurfaceMesh< dimension > >{} );
                } } } );
    }

    template class opengeode_mesh_api TriangulatedSurface< 2 >;
    template class opengeode_mesh_api TriangulatedSurface< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, TriangulatedSurface< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, TriangulatedSurface< 3 > );
} // namespace geode
