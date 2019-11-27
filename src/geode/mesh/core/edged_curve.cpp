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

#include <geode/mesh/core/edged_curve.h>

#include <geode/basic/bitsery_archive.h>

#include <geode/geometry/vector.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/core/geode_edged_curve.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< EdgedCurve< dimension > > EdgedCurve< dimension >::create()
    {
        return std::unique_ptr< EdgedCurve< dimension > >{
            new OpenGeodeEdgedCurve< dimension >
        };
    }

    template < index_t dimension >
    std::unique_ptr< EdgedCurve< dimension > > EdgedCurve< dimension >::create(
        const MeshType& type )
    {
        try
        {
            return EdgedCurveFactory< dimension >::create( type );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Could not create EdgedCurve data structure: ", type.get()
            };
        }
    }

    template < index_t dimension >
    double EdgedCurve< dimension >::edge_length( index_t edge_id ) const
    {
        const auto& e0 = this->point( edge_vertex( { edge_id, 0 } ) );
        const auto& e1 = this->point( edge_vertex( { edge_id, 1 } ) );
        return Vector< dimension >{ e0, e1 }.length();
    }

    template < index_t dimension >
    Point< dimension > EdgedCurve< dimension >::edge_barycenter(
        index_t edge_id ) const
    {
        const auto& e0 = this->point( edge_vertex( { edge_id, 0 } ) );
        const auto& e1 = this->point( edge_vertex( { edge_id, 1 } ) );
        return ( e0 + e1 ) / 2.;
    }

    template < index_t dimension >
    const Point< dimension >& EdgedCurve< dimension >::point(
        index_t vertex_id ) const
    {
        OPENGEODE_ASSERT( vertex_id < nb_vertices(),
            "[EdgedCurve::point] Trying to access an invalid point" );
        return get_point( vertex_id );
    }

    template < index_t dimension >
    template < typename Archive >
    void EdgedCurve< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, EdgedCurve >{},
            []( Archive& archive, EdgedCurve& edged_curve ) {
                archive.ext( edged_curve, bitsery::ext::BaseClass< Graph >{} );
            } );
    }

    template < index_t dimension >
    std::unique_ptr< EdgedCurve< dimension > >
        EdgedCurve< dimension >::clone() const
    {
        auto clone = create( type_name() );
        auto builder = EdgedCurveBuilder< dimension >::create( *clone );
        builder->copy( *this );
        return clone;
    }

    template class opengeode_mesh_api EdgedCurve< 2 >;
    template class opengeode_mesh_api EdgedCurve< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, EdgedCurve< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, EdgedCurve< 3 > );
} // namespace geode
