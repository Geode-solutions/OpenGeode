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

#include <geode/mesh/core/tetrahedral_solid.h>

#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/core/geode_tetrahedral_solid.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TetrahedralSolid< dimension > >
        TetrahedralSolid< dimension >::create()
    {
        return std::unique_ptr< TetrahedralSolid< dimension > >{
            new OpenGeodeTetrahedralSolid< dimension >
        };
    }

    template < index_t dimension >
    std::unique_ptr< TetrahedralSolid< dimension > >
        TetrahedralSolid< dimension >::create( const MeshType& type )
    {
        try
        {
            return TetrahedralSolidFactory< dimension >::create( type );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Could not create TetrahedralSolid data structure: ", type.get()
            };
        }
    }

    template < index_t dimension >
    std::unique_ptr< TetrahedralSolid< dimension > >
        TetrahedralSolid< dimension >::clone() const
    {
        auto clone = create( this->type_name() );
        auto builder = TetrahedralSolidBuilder< dimension >::create( *clone );
        builder->copy( *this, {} );
        return clone;
    }

    template < index_t dimension >
    std::array< PolyhedronFacet, 2 >
        TetrahedralSolid< dimension >::opposite_edge_incident_facets(
            index_t tetrahedron_id,
            const std::array< index_t, 2 >& edge_vertices ) const
    {
        std::array< PolyhedronFacet, 2 > opposite_facets{};
        for( const auto v : Range{ 4 } )
        {
            const PolyhedronVertex vertex{ tetrahedron_id, v };
            if( this->polyhedron_vertex( vertex ) == edge_vertices[0] )
            {
                opposite_facets[0] = { vertex.polyhedron_id, vertex.vertex_id };
            }
            else if( this->polyhedron_vertex( vertex ) == edge_vertices[1] )
            {
                opposite_facets[1] = { vertex.polyhedron_id, vertex.vertex_id };
            }
        }
        OPENGEODE_EXCEPTION( opposite_facets[0].facet_id != NO_ID
                                 && opposite_facets[1].facet_id != NO_ID,
            "[TetrahedralSolid::opposite_edge_incident_facets] Given edge "
            "vertices are not vertices of given tetrahedron" );
        return opposite_facets;
    }

    template class opengeode_mesh_api TetrahedralSolid< 3 >;
} // namespace geode
