/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geometry/basic_objects/tetra.h>
#include <geode/geometry/basic_objects/triangle.h>

#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/core/detail/geode_elements.h>
#include <geode/mesh/core/mesh_factory.h>

namespace
{
    template < geode::index_t dimension >
    bool propagate_around_edge(
        const geode::TetrahedralSolid< dimension >& solid,
        geode::PolyhedronFacet facet,
        const std::array< geode::index_t, 2 >& edge_vertices,
        geode::PolyhedraAroundEdge& result )
    {
        const auto first_polyhedron = facet.polyhedron_id;
        do
        {
            if( const auto adj = solid.polyhedron_adjacent_facet( facet ) )
            {
                const auto adj_facet = adj.value();
                result.push_back( adj_facet.polyhedron_id );

                const auto v0 =
                    solid
                        .vertex_in_polyhedron(
                            adj_facet.polyhedron_id, edge_vertices[0] )
                        .value();
                const auto v1 =
                    solid
                        .vertex_in_polyhedron(
                            adj_facet.polyhedron_id, edge_vertices[1] )
                        .value();
                facet = { adj_facet.polyhedron_id,
                    static_cast< geode::local_index_t >(
                        6 - v0 - v1 - adj_facet.facet_id ) };
            }
            else
            {
                return false;
            }
        } while( facet.polyhedron_id != first_polyhedron );
        return true;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TetrahedralSolid< dimension > >
        TetrahedralSolid< dimension >::create()
    {
        return MeshFactory::create_default_mesh<
            TetrahedralSolid< dimension > >(
            TetrahedralSolid< dimension >::type_name_static() );
    }

    template < index_t dimension >
    std::unique_ptr< TetrahedralSolid< dimension > >
        TetrahedralSolid< dimension >::create( const MeshImpl& impl )
    {
        return MeshFactory::create_mesh< TetrahedralSolid< dimension > >(
            impl );
    }

    template < index_t dimension >
    std::unique_ptr< TetrahedralSolid< dimension > >
        TetrahedralSolid< dimension >::clone() const
    {
        auto clone = create( this->impl_name() );
        auto builder = TetrahedralSolidBuilder< dimension >::create( *clone );
        builder->copy( *this );
        return clone;
    }

    template < index_t dimension >
    std::array< PolyhedronFacet, 2 >
        TetrahedralSolid< dimension >::opposite_edge_incident_facets(
            index_t tetrahedron_id,
            const std::array< index_t, 2 >& edge_vertices ) const
    {
        std::array< PolyhedronFacet, 2 > opposite_facets;
        for( const auto v : LRange{ 4 } )
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

    template < index_t dimension >
    Tetra TetrahedralSolid< dimension >::tetra( index_t tetrahedron_id ) const
    {
        return { this->point(
                     this->polyhedron_vertex( { tetrahedron_id, 0 } ) ),
            this->point( this->polyhedron_vertex( { tetrahedron_id, 1 } ) ),
            this->point( this->polyhedron_vertex( { tetrahedron_id, 2 } ) ),
            this->point( this->polyhedron_vertex( { tetrahedron_id, 3 } ) ) };
    }

    template < index_t dimension >
    Triangle< dimension > TetrahedralSolid< dimension >::triangle(
        const PolyhedronFacet& facet ) const
    {
        const auto vertices = this->polyhedron_facet_vertices( facet );
        return { this->point( vertices[0] ), this->point( vertices[1] ),
            this->point( vertices[2] ) };
    }

    template < index_t dimension >
    PolyhedraAroundEdge TetrahedralSolid< dimension >::polyhedra_around_edge(
        const std::array< index_t, 2 >& vertices ) const
    {
        PolyhedraAroundEdge result;
        for( const auto& polyhedron :
            this->polyhedra_around_vertex( vertices[0] ) )
        {
            if( this->vertex_in_polyhedron(
                    polyhedron.polyhedron_id, vertices[1] ) )
            {
                result.push_back( polyhedron.polyhedron_id );
            }
        }
        return result;
    }

    template < index_t dimension >
    std::vector< std::array< index_t, 2 > >
        TetrahedralSolid< dimension >::polyhedron_edges_vertices(
            index_t polyhedron ) const
    {
        const std::array< index_t, 4 > vertices{ this->polyhedron_vertex(
                                                     { polyhedron, 0 } ),
            this->polyhedron_vertex( { polyhedron, 1 } ),
            this->polyhedron_vertex( { polyhedron, 2 } ),
            this->polyhedron_vertex( { polyhedron, 3 } ) };
        std::vector< std::array< index_t, 2 > > result;
        result.reserve( detail::tetrahedron_edge_vertices.size() );
        for( const auto& edge : detail::tetrahedron_edge_vertices )
        {
            result.emplace_back( std::array< index_t, 2 >{
                vertices[edge[0]], vertices[edge[1]] } );
        }
        return result;
    }

    template < index_t dimension >
    PolyhedronFacets TetrahedralSolid< dimension >::polyhedron_vertex_facets(
        const PolyhedronVertex& polyhedron_vertex ) const
    {
        PolyhedronFacets facets;
        for( const auto facet :
            detail::tetrahedron_facet_vertices[polyhedron_vertex.vertex_id] )
        {
            facets.emplace_back( polyhedron_vertex.polyhedron_id, facet );
        }
        return facets;
    }

    template < index_t dimension >
    std::vector< PolyhedronFacetVertices >
        TetrahedralSolid< dimension >::polyhedron_facets_vertices(
            index_t polyhedron ) const
    {
        const std::array< index_t, 4 > vertices{ this->polyhedron_vertex(
                                                     { polyhedron, 0 } ),
            this->polyhedron_vertex( { polyhedron, 1 } ),
            this->polyhedron_vertex( { polyhedron, 2 } ),
            this->polyhedron_vertex( { polyhedron, 3 } ) };
        std::vector< PolyhedronFacetVertices > result;
        result.reserve( detail::tetrahedron_facet_vertices.size() );
        for( const auto& facet : detail::tetrahedron_facet_vertices )
        {
            result.emplace_back( PolyhedronFacetVertices{
                vertices[facet[0]], vertices[facet[1]], vertices[facet[2]] } );
        }
        return result;
    }

    template < index_t dimension >
    PolyhedraAroundEdge TetrahedralSolid< dimension >::polyhedra_around_edge(
        const std::array< index_t, 2 >& vertices,
        index_t first_polyhedron ) const
    {
        PolyhedraAroundEdge result{ first_polyhedron };
        const auto v0 =
            this->vertex_in_polyhedron( first_polyhedron, vertices[0] ).value();
        const auto v1 =
            this->vertex_in_polyhedron( first_polyhedron, vertices[1] ).value();
        for( const auto f : LRange{ 4 } )
        {
            if( f == v0 || f == v1 )
            {
                continue;
            }
            if( propagate_around_edge(
                    *this, { first_polyhedron, f }, vertices, result ) )
            {
                result.pop_back();
                return result;
            }
        }
        return result;
    }

    template class opengeode_mesh_api TetrahedralSolid< 3 >;
} // namespace geode
