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

#include <geode/mesh/core/tetrahedral_solid.h>

#include <geode/basic/bitsery_archive.h>

#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>

#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/core/detail/geode_elements.h>
#include <geode/mesh/core/mesh_factory.h>

namespace
{
    template < geode::index_t dimension >
    std::pair< bool, bool > propagate_around_edge(
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
                absl::InlinedVector< geode::local_index_t, 1 > v0;
                absl::InlinedVector< geode::local_index_t, 1 > v1;
                const auto vertices =
                    solid.polyhedron_vertices( adj_facet.polyhedron_id );
                for( const auto v : geode::LRange{ 4 } )
                {
                    if( vertices[v] == edge_vertices[0] )
                    {
                        v0.push_back( v );
                    }
                    else if( vertices[v] == edge_vertices[1] )
                    {
                        v1.push_back( v );
                    }
                }
                if( v0.size() != 1 || v1.size() != 1 )
                {
                    return std::make_pair( false, false );
                }
                for( const auto f : geode::LRange{ 4 } )
                {
                    if( f == adj_facet.facet_id )
                    {
                        continue;
                    }
                    const auto vertex_id = vertices[f];
                    if( vertex_id == edge_vertices[0]
                        || vertex_id == edge_vertices[1] )
                    {
                        continue;
                    }
                    facet = { adj_facet.polyhedron_id, f };
                    break;
                }
                OPENGEODE_ASSERT(
                    facet.polyhedron_id == adj_facet.polyhedron_id,
                    "[TetrahedralSolid3D::propagate_around_edge] Next "
                    "facet not found" );
            }
            else
            {
                return std::make_pair( true, false );
            }
        } while( facet.polyhedron_id != first_polyhedron );
        return std::make_pair( true, true );
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
        TetrahedralSolid< dimension >::edge_incident_facets(
            index_t tetrahedron_id,
            const std::array< index_t, 2 >& edge_vertices ) const
    {
        std::array< PolyhedronFacet, 2 > facets;
        index_t count{ 0 };
        for( const auto v : LRange{ 4 } )
        {
            const auto solid_vertex =
                this->polyhedron_vertex( { tetrahedron_id, v } );
            if( solid_vertex != edge_vertices[0]
                && solid_vertex != edge_vertices[1] )
            {
                OPENGEODE_EXCEPTION( count < 2,
                    "[TetrahedralSolid::edge_incident_facets] Given edge "
                    "vertices are not vertices of given tetrahedron" );
                facets[count] = { tetrahedron_id, v };
                count++;
            }
        }
        return facets;
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
    Tetrahedron TetrahedralSolid< dimension >::tetrahedron(
        index_t tetrahedron_id ) const
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
            for( const auto v : LRange{ 4 } )
            {
                if( v != polyhedron.vertex_id
                    && this->polyhedron_vertex(
                           { polyhedron.polyhedron_id, v } )
                           == vertices[1] )
                {
                    result.push_back( polyhedron.polyhedron_id );
                    break;
                }
            }
        }
        return result;
    }

    template < index_t dimension >
    PolyhedraAroundEdge TetrahedralSolid< dimension >::polyhedra_around_edge(
        const std::array< index_t, 2 >& vertices,
        index_t first_polyhedron ) const
    {
        PolyhedraAroundEdge result{ first_polyhedron };
        std::array< index_t, 2 > excluded_facets{ NO_ID, NO_ID };
        local_index_t count{ 0 };
        const auto polyhedron_vertices =
            this->polyhedron_vertices( first_polyhedron );
        for( const auto v : LRange{ 4 } )
        {
            if( polyhedron_vertices[v] == vertices[0]
                || polyhedron_vertices[v] == vertices[1] )
            {
                excluded_facets[count++] = v;
            }
        }
        for( const auto f : LRange{ 4 } )
        {
            if( f == excluded_facets[0] || f == excluded_facets[1] )
            {
                continue;
            }
            const auto vertex_id = polyhedron_vertices[f];
            if( vertex_id == vertices[0] || vertex_id == vertices[1] )
            {
                continue;
            }
            const auto status = propagate_around_edge(
                *this, { first_polyhedron, f }, vertices, result );
            if( !status.first )
            {
                return polyhedra_around_edge( vertices );
            }
            if( status.second )
            {
                result.pop_back();
                return result;
            }
        }
        return result;
    }

    template < index_t dimension >
    PolyhedraAroundEdge TetrahedralSolid< dimension >::polyhedra_around_edge(
        const PolyhedronFacetEdge& edge ) const
    {
        return polyhedra_around_edge(
            this->polyhedron_facet_edge_vertices( edge ),
            edge.polyhedron_facet.polyhedron_id );
    }

    template < index_t dimension >
    PolyhedronEdgesVertices
        TetrahedralSolid< dimension >::polyhedron_edges_vertices(
            index_t polyhedron ) const
    {
        const std::array< index_t, 4 > vertices{ this->polyhedron_vertex(
                                                     { polyhedron, 0 } ),
            this->polyhedron_vertex( { polyhedron, 1 } ),
            this->polyhedron_vertex( { polyhedron, 2 } ),
            this->polyhedron_vertex( { polyhedron, 3 } ) };
        PolyhedronEdgesVertices result;
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
    PolyhedronFacetsVertices
        TetrahedralSolid< dimension >::polyhedron_facets_vertices(
            index_t polyhedron ) const
    {
        const std::array< index_t, 4 > vertices{ this->polyhedron_vertex(
                                                     { polyhedron, 0 } ),
            this->polyhedron_vertex( { polyhedron, 1 } ),
            this->polyhedron_vertex( { polyhedron, 2 } ),
            this->polyhedron_vertex( { polyhedron, 3 } ) };
        PolyhedronFacetsVertices result;
        for( const auto& facet : detail::tetrahedron_facet_vertices )
        {
            result.emplace_back( PolyhedronFacetVertices{
                vertices[facet[0]], vertices[facet[1]], vertices[facet[2]] } );
        }
        return result;
    }

    template < index_t dimension >
    absl::optional< PolyhedronFacet >
        TetrahedralSolid< dimension >::polyhedron_adjacent_facet(
            const PolyhedronFacet& polyhedron_facet ) const
    {
        const auto opt_polyhedron_adj =
            this->polyhedron_adjacent( polyhedron_facet );
        if( !opt_polyhedron_adj )
        {
            return absl::nullopt;
        }
        std::array< index_t, 3 > vertices;
        for( const auto v : LRange{ 3 } )
        {
            vertices[v] =
                this->polyhedron_facet_vertex( { polyhedron_facet, v } );
        }
        const auto polyhedron_adj = opt_polyhedron_adj.value();
        const auto adj_vertices = this->polyhedron_vertices( polyhedron_adj );
        std::array< bool, 4 > candidates{ true, true, true, true };
        for( const auto v : LRange{ 4 } )
        {
            const auto opp_adj_vertex_id = adj_vertices[v];
            if( opp_adj_vertex_id != vertices[0]
                && opp_adj_vertex_id != vertices[1]
                && opp_adj_vertex_id != vertices[2] )
            {
                for( const auto other_v : LRange{ 4 } )
                {
                    if( other_v == v )
                    {
                        continue;
                    }
                    candidates[other_v] = false;
                }
            }
        }
        for( const auto f : LRange{ 4 } )
        {
            if( !candidates[f] )
            {
                continue;
            }
            if( this->polyhedron_adjacent( { polyhedron_adj, f } )
                != polyhedron_facet.polyhedron_id )
            {
                continue;
            }
            return absl::optional< PolyhedronFacet >{ absl::in_place,
                polyhedron_adj, f };
        }
        throw OpenGeodeException{ "[SolidMesh::polyhedron_adjacent_"
                                  "facet] Wrong adjacency with polyhedra: ",
            polyhedron_facet.polyhedron_id, " and ", polyhedron_adj };
        return absl::nullopt;
    }

    template < index_t dimension >
    template < typename Archive >
    void TetrahedralSolid< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, TetrahedralSolid >{
                { []( Archive& a, TetrahedralSolid& tetrahedral_solid ) {
                    a.ext( tetrahedral_solid,
                        bitsery::ext::BaseClass< SolidMesh< dimension > >{} );
                } } } );
    }

    template class opengeode_mesh_api TetrahedralSolid< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, TetrahedralSolid< 3 > );
} // namespace geode
