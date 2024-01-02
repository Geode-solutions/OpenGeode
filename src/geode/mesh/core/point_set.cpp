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

#include <bitsery/ext/inheritance.h>

#include <geode/mesh/core/point_set.h>

#include <geode/basic/bitsery_archive.h>

#include <geode/geometry/bounding_box.h>

#include <geode/mesh/builder/point_set_builder.h>
#include <geode/mesh/core/mesh_factory.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< PointSet< dimension > > PointSet< dimension >::create()
    {
        return MeshFactory::create_default_mesh< PointSet< dimension > >(
            PointSet< dimension >::type_name_static() );
    }

    template < index_t dimension >
    std::unique_ptr< PointSet< dimension > > PointSet< dimension >::create(
        const MeshImpl& impl )
    {
        return MeshFactory::create_mesh< PointSet< dimension > >( impl );
    }

    template < index_t dimension >
    MeshType PointSet< dimension >::type_name_static()
    {
        return MeshType{ absl::StrCat( "PointSet", dimension, "D" ) };
    }

    template < index_t dimension >
    template < typename Archive >
    void PointSet< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, PointSet >{
                { []( Archive& a, PointSet& point_set ) {
                     a.ext( point_set, bitsery::ext::BaseClass< VertexSet >{} );
                 },
                    []( Archive& a, PointSet& point_set ) {
                        a.ext(
                            point_set, bitsery::ext::BaseClass< VertexSet >{} );
                        a.ext( point_set, bitsery::ext::BaseClass<
                                              CoordinateReferenceSystemManagers<
                                                  dimension > >{} );
                    } } } );
    }

    template < index_t dimension >
    std::unique_ptr< PointSet< dimension > >
        PointSet< dimension >::clone() const
    {
        auto clone = create( impl_name() );
        auto builder = PointSetBuilder< dimension >::create( *clone );
        builder->copy( *this );
        return clone;
    }

    template < index_t dimension >
    BoundingBox< dimension > PointSet< dimension >::bounding_box() const
    {
        BoundingBox< dimension > box;
        for( const auto p : Range{ nb_vertices() } )
        {
            box.add_point( this->point( p ) );
        }
        return box;
    }

    template class opengeode_mesh_api PointSet< 2 >;
    template class opengeode_mesh_api PointSet< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PointSet< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, PointSet< 3 > );
} // namespace geode
