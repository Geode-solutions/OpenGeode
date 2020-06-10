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

#include <geode/mesh/builder/solid_mesh_builder.h>

#include <numeric>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/detail/mapping_after_deletion.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/solid_mesh.h>

namespace
{
    template < geode::index_t dimension >
    void check_polyhedron_id( const geode::SolidMesh< dimension >& solid,
        const geode::index_t polyhedron_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        OPENGEODE_ASSERT( polyhedron_id < solid.nb_polyhedra(),
            "[check_polyhedron_id] Trying to access an invalid polyhedron" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_vertex_id( const geode::SolidMesh< dimension >& solid,
        const geode::index_t polyhedron_id,
        const geode::index_t vertex_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT(
            vertex_id < solid.nb_polyhedron_vertices( polyhedron_id ),
            "[check_polyhedron_vertex_id] Trying to access an invalid "
            "polyhedron vertex" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_facet_id( const geode::SolidMesh< dimension >& solid,
        const geode::index_t polyhedron_id,
        const geode::index_t facet_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        geode_unused( facet_id );
        OPENGEODE_ASSERT(
            facet_id < solid.nb_polyhedron_facets( polyhedron_id ),
            "[check_polyhedron_facet_id] Trying to access an invalid "
            "polyhedron facet" );
    }

    template < geode::index_t dimension >
    void check_polyhedron_facet_vertex_id(
        const geode::SolidMesh< dimension >& solid,
        const geode::index_t polyhedron_id,
        const geode::index_t facet_id,
        const geode::index_t vertex_id )
    {
        geode_unused( solid );
        geode_unused( polyhedron_id );
        geode_unused( facet_id );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT( vertex_id < solid.nb_polyhedron_facet_vertices(
                              { polyhedron_id, facet_id } ),
            "[check_polyhedron_facet_vertex_id] Trying to access an invalid "
            "polyhedron facet vertex" );
    }

    template < geode::index_t dimension >
    void update_polyhedron_adjacencies(
        const geode::SolidMesh< dimension >& solid,
        geode::SolidMeshBuilder< dimension >& builder,
        absl::Span< const geode::index_t > old2new )
    {
        for( const auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            for( const auto f :
                geode::Range{ solid.nb_polyhedron_facets( p ) } )
            {
                const geode::PolyhedronFacet id{ p, f };
                const auto adj = solid.polyhedron_adjacent( id );
                if( adj )
                {
                    const auto new_adjacent = old2new[adj.value()];
                    builder.set_polyhedron_adjacent( id, new_adjacent );
                }
            }
        }
    }

    template < geode::index_t dimension >
    void update_polyhedron_around_vertices(
        const geode::SolidMesh< dimension >& solid,
        geode::SolidMeshBuilder< dimension >& builder,
        absl::Span< const geode::index_t > vertices_old2new )
    {
        for( const auto v : geode::Range{ solid.nb_vertices() } )
        {
            if( vertices_old2new[v] != geode::NO_ID )
            {
                const auto new_polyhedron_around =
                    builder.polyhedron_around_vertex( vertices_old2new[v] );
                builder.associate_polyhedron_vertex_to_vertex(
                    new_polyhedron_around, v );
            }
            else
            {
                builder.associate_polyhedron_vertex_to_vertex(
                    geode::PolyhedronVertex{}, v );
            }
        }
    }

    template < geode::index_t dimension >
    std::vector< bool > find_polyhedra_to_delete(
        const geode::SolidMesh< dimension >& solid,
        absl::Span< const geode::index_t > vertices_old2new )
    {
        std::vector< bool > polyhedra_to_delete( solid.nb_polyhedra(), false );
        for( const auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            for( const auto v :
                geode::Range{ solid.nb_polyhedron_vertices( p ) } )
            {
                const geode::PolyhedronVertex id{ p, v };
                const auto new_vertex =
                    vertices_old2new[solid.polyhedron_vertex( id )];
                if( new_vertex == geode::NO_ID )
                {
                    polyhedra_to_delete[p] = true;
                }
            }
        }
        return polyhedra_to_delete;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< SolidMeshBuilder< dimension > >
        SolidMeshBuilder< dimension >::create( SolidMesh< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            SolidMeshBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::set_mesh(
        SolidMesh< dimension >& mesh, MeshBuilderFactoryKey key )
    {
        solid_mesh_ = &mesh;
        VertexSetBuilder::set_mesh( mesh, key );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::set_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        const auto polyhedron_vertex_id =
            solid_mesh_->polyhedron_vertex( polyhedron_vertex );
        if( polyhedron_vertex_id != NO_ID )
        {
            const auto polyhedron_around =
                solid_mesh_->polyhedron_around_vertex( polyhedron_vertex_id );
            if( polyhedron_around == polyhedron_vertex )
            {
                const auto polyhedra_around =
                    solid_mesh_->polyhedra_around_vertex(
                        polyhedron_vertex_id );
                if( polyhedra_around.size() < 2 )
                {
                    associate_polyhedron_vertex_to_vertex(
                        PolyhedronVertex{}, polyhedron_vertex_id );
                }
                else
                {
                    associate_polyhedron_vertex_to_vertex(
                        polyhedra_around[1], polyhedron_vertex_id );
                }
            }
        }

        for( const auto f : Range{ solid_mesh_->nb_polyhedron_facets(
                 polyhedron_vertex.polyhedron_id ) } )
        {
            const PolyhedronFacet id{ polyhedron_vertex.polyhedron_id, f };
            const auto nb_facet_vertices =
                solid_mesh_->nb_polyhedron_facet_vertices( id );
            absl::FixedArray< PolyhedronVertex > facet_vertices(
                nb_facet_vertices );
            for( const auto v : Range{ nb_facet_vertices } )
            {
                facet_vertices[v] =
                    solid_mesh_->polyhedron_facet_vertex_id( { id, v }, {} );
            }
            const auto position_it =
                absl::c_find( facet_vertices, polyhedron_vertex );
            if( position_it != facet_vertices.end() )
            {
                PolyhedronFacetVertices facet_vertices_id;
                facet_vertices_id.reserve( nb_facet_vertices );
                for( const auto& v : facet_vertices )
                {
                    facet_vertices_id.emplace_back(
                        solid_mesh_->polyhedron_vertex( v ) );
                }

                const auto position = static_cast< index_t >(
                    std::distance( facet_vertices.begin(), position_it ) );
                solid_mesh_->update_facet_vertex(
                    facet_vertices_id, position, vertex_id, {} );

                std::array< index_t, 2 > next_edge_vertices{
                    facet_vertices_id[position],
                    facet_vertices_id[( position + 1 ) % nb_facet_vertices]
                };
                solid_mesh_->update_edge_vertex(
                    next_edge_vertices, 0, vertex_id, {} );
                std::array< index_t, 2 > previous_edge_vertices{
                    facet_vertices_id[( position + nb_facet_vertices - 1 )
                                      % nb_facet_vertices],
                    facet_vertices_id[position]
                };
                solid_mesh_->update_edge_vertex(
                    previous_edge_vertices, 1, vertex_id, {} );
            }
        }
        update_polyhedron_vertex( polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::update_polyhedron_vertices(
        absl::Span< const index_t > old2new )
    {
        update_polyhedron_around_vertices( *solid_mesh_, *this, old2new );
        const auto polyhedra_to_delete =
            find_polyhedra_to_delete( *solid_mesh_, old2new );
        delete_polyhedra( polyhedra_to_delete );
        for( const auto p : Range{ solid_mesh_->nb_polyhedra() } )
        {
            for( const auto v :
                Range{ solid_mesh_->nb_polyhedron_vertices( p ) } )
            {
                const PolyhedronVertex id{ p, v };
                const auto new_vertex =
                    old2new[solid_mesh_->polyhedron_vertex( id )];
                OPENGEODE_ASSERT( new_vertex != NO_ID,
                    "[SolidMeshBuilder::update_polyhedron_vertices] No "
                    "more polyhedra with vertices to delete should remain at "
                    "this point" );
                update_polyhedron_vertex( id, new_vertex );
            }
        }
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::update_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        check_polyhedron_id( *solid_mesh_, polyhedron_vertex.polyhedron_id );
        check_polyhedron_vertex_id( *solid_mesh_,
            polyhedron_vertex.polyhedron_id, polyhedron_vertex.vertex_id );
        OPENGEODE_ASSERT( vertex_id < solid_mesh_->nb_vertices(),
            "[SolidMeshBuilder::update_polyhedron_vertex] Accessing a "
            "vertex that does not exist" );
        associate_polyhedron_vertex_to_vertex( polyhedron_vertex, vertex_id );
        do_set_polyhedron_vertex( polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    index_t SolidMeshBuilder< dimension >::create_polyhedron(
        absl::Span< const index_t > vertices,
        absl::Span< const std::vector< index_t > > facets )
    {
        const auto added_polyhedron = solid_mesh_->nb_polyhedra();
        solid_mesh_->polyhedron_attribute_manager().resize(
            added_polyhedron + 1 );
        for( const auto v : Range{ vertices.size() } )
        {
            associate_polyhedron_vertex_to_vertex(
                { added_polyhedron, v }, vertices[v] );
        }
        do_create_facets( vertices, facets );
        do_create_edges( vertices, facets );
        do_create_polyhedron( vertices, facets );
        return added_polyhedron;
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::do_create_facets(
        absl::Span< const index_t > vertices,
        absl::Span< const std::vector< index_t > > facets )
    {
        for( auto&& facet_vertices :
            get_polyhedron_facet_vertices( vertices, facets ) )
        {
            this->find_or_create_facet( facet_vertices );
        }
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::do_create_edges(
        absl::Span< const index_t > vertices,
        absl::Span< const std::vector< index_t > > facets )
    {
        for( auto&& edge_vertices :
            get_polyhedron_edge_vertices( vertices, facets ) )
        {
            if( edge_vertices[0] < edge_vertices[1] )
            {
                this->find_or_create_edge( edge_vertices );
            }
        }
    }

    template < index_t dimension >
    std::vector< PolyhedronFacetVertices >
        SolidMeshBuilder< dimension >::get_polyhedron_facet_vertices(
            absl::Span< const index_t > vertices,
            absl::Span< const std::vector< index_t > > facets ) const
    {
        std::vector< PolyhedronFacetVertices > polyhedron_facet_vertices(
            facets.size() );
        for( const auto f : Range{ facets.size() } )
        {
            polyhedron_facet_vertices[f].resize( facets[f].size() );
            for( const auto v : Range{ facets[f].size() } )
            {
                OPENGEODE_ASSERT( facets[f][v] < vertices.size(),
                    "[SolidMeshBuilder::get_polyhedron_facet_vertices] "
                    "Wrong facet definition" );
                polyhedron_facet_vertices[f][v] = vertices[facets[f][v]];
            }
        }
        return polyhedron_facet_vertices;
    }

    template < index_t dimension >
    std::vector< std::array< index_t, 2 > >
        SolidMeshBuilder< dimension >::get_polyhedron_edge_vertices(
            absl::Span< const index_t > vertices,
            absl::Span< const std::vector< index_t > > facets ) const
    {
        std::vector< std::array< index_t, 2 > > polyhedron_edge_vertices;
        polyhedron_edge_vertices.reserve( 3 * facets.size() );
        for( const auto f : Range{ facets.size() } )
        {
            for( const auto v : Range{ facets[f].size() } )
            {
                OPENGEODE_ASSERT( facets[f][v] < vertices.size(),
                    "[SolidMeshBuilder::get_polyhedron_edge_vertices] "
                    "Wrong facet definition" );
                polyhedron_edge_vertices.push_back( { vertices[facets[f][v]],
                    vertices[facets[f][( v + 1 ) % facets[f].size()]] } );
            }
        }
        return polyhedron_edge_vertices;
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::associate_polyhedron_vertex_to_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        solid_mesh_->associate_polyhedron_vertex_to_vertex(
            polyhedron_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    index_t SolidMeshBuilder< dimension >::find_or_create_facet(
        PolyhedronFacetVertices facet_vertices )
    {
        return solid_mesh_->find_or_create_facet(
            std::move( facet_vertices ), {} );
    }

    template < index_t dimension >
    index_t SolidMeshBuilder< dimension >::find_or_create_edge(
        std::array< index_t, 2 > edge_vertices )
    {
        return solid_mesh_->find_or_create_edge(
            std::move( edge_vertices ), {} );
    }

    template < index_t dimension >
    const PolyhedronVertex&
        SolidMeshBuilder< dimension >::polyhedron_around_vertex(
            index_t vertex_id ) const
    {
        return solid_mesh_->polyhedron_around_vertex( vertex_id );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::do_delete_vertices(
        const std::vector< bool >& to_delete )
    {
        const auto old2new = detail::mapping_after_deletion( to_delete );
        update_polyhedron_vertices( old2new );
        update_facet_vertices( old2new );
        update_edge_vertices( old2new );
        do_delete_solid_vertices( to_delete );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::set_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet, index_t adjacent_id )
    {
        check_polyhedron_id( *solid_mesh_, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id( *solid_mesh_, polyhedron_facet.polyhedron_id,
            polyhedron_facet.facet_id );
        OPENGEODE_ASSERT(
            adjacent_id < solid_mesh_->nb_polyhedra() || adjacent_id == NO_ID,
            "[SolidMeshBuilder::set_polyhedron_adjacent] Accessing a "
            "polyhedron that does not exist" );
        do_set_polyhedron_adjacent( polyhedron_facet, adjacent_id );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::compute_polyhedron_adjacencies()
    {
        absl::FixedArray< index_t > polyhedra_to_connect(
            solid_mesh_->nb_polyhedra() );
        absl::c_iota( polyhedra_to_connect, 0 );
        compute_polyhedron_adjacencies( polyhedra_to_connect );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::compute_polyhedron_adjacencies(
        absl::Span< const index_t > polyhedra_to_connect )
    {
        absl::FixedArray< absl::InlinedVector< PolyhedronFacet, 4 > >
            polyhedron_facet_around_vertex( solid_mesh_->nb_vertices() );
        for( const auto polyhedron : polyhedra_to_connect )
        {
            for( const auto f :
                Range{ solid_mesh_->nb_polyhedron_facets( polyhedron ) } )
            {
                const PolyhedronFacet facet{ polyhedron, f };
                for( const auto v : Range{
                         solid_mesh_->nb_polyhedron_facet_vertices( facet ) } )
                {
                    const auto vertex =
                        solid_mesh_->polyhedron_facet_vertex( { facet, v } );
                    polyhedron_facet_around_vertex[vertex].push_back( facet );
                }
            }
        }

        for( const auto polyhedron : polyhedra_to_connect )
        {
            for( const auto f :
                Range{ solid_mesh_->nb_polyhedron_facets( polyhedron ) } )
            {
                const PolyhedronFacet facet{ polyhedron, f };
                if( !solid_mesh_->is_polyhedron_facet_on_border( facet ) )
                {
                    continue;
                }

                const auto nb_vertices =
                    solid_mesh_->nb_polyhedron_facet_vertices( facet );
                const auto first_vertex =
                    solid_mesh_->polyhedron_facet_vertex( { facet, 0 } );
                for( const auto& facet2 :
                    polyhedron_facet_around_vertex[first_vertex] )
                {
                    const auto nb_vertices2 =
                        solid_mesh_->nb_polyhedron_facet_vertices( facet2 );
                    if( nb_vertices != nb_vertices2 )
                    {
                        continue;
                    }
                    for( const auto offset : Range{ nb_vertices } )
                    {
                        bool found_facet{ true };
                        for( const auto v : Range{ nb_vertices } )
                        {
                            const auto vertex =
                                solid_mesh_->polyhedron_facet_vertex(
                                    { facet, v } );
                            const auto vertex2 =
                                solid_mesh_->polyhedron_facet_vertex(
                                    { facet2, ( nb_vertices - v + offset )
                                                  % nb_vertices } );
                            if( vertex != vertex2 )
                            {
                                found_facet = false;
                                break;
                            }
                        }
                        if( found_facet )
                        {
                            do_set_polyhedron_adjacent(
                                facet, facet2.polyhedron_id );
                            do_set_polyhedron_adjacent(
                                facet2, facet.polyhedron_id );
                            goto next_facet;
                        }
                    }
                }
            next_facet:;
            }
        }
    }

    template < index_t dimension >
    std::vector< index_t > SolidMeshBuilder< dimension >::delete_polyhedra(
        const std::vector< bool >& to_delete )
    {
        remove_polyhedra_facets( to_delete );
        remove_polyhedra_edges( to_delete );

        const auto old2new = detail::mapping_after_deletion( to_delete );
        for( const auto v : Range{ solid_mesh_->nb_vertices() } )
        {
            const auto& polyhedron_vertex =
                solid_mesh_->polyhedron_around_vertex( v );
            if( polyhedron_vertex.polyhedron_id == NO_ID )
            {
                continue;
            }
            PolyhedronVertex new_polyhedron_vertex{ polyhedron_vertex };
            new_polyhedron_vertex.polyhedron_id =
                old2new[polyhedron_vertex.polyhedron_id];
            if( new_polyhedron_vertex.polyhedron_id == NO_ID )
            {
                for( auto&& polyhedron :
                    solid_mesh_->polyhedra_around_vertex( v ) )
                {
                    polyhedron.polyhedron_id =
                        old2new[polyhedron.polyhedron_id];
                    if( polyhedron.polyhedron_id != NO_ID )
                    {
                        new_polyhedron_vertex = std::move( polyhedron );
                        break;
                    }
                }
            }
            associate_polyhedron_vertex_to_vertex( new_polyhedron_vertex, v );
        }

        update_polyhedron_adjacencies( *solid_mesh_, *this, old2new );
        solid_mesh_->polyhedron_attribute_manager().delete_elements(
            to_delete );
        do_delete_polyhedra( to_delete );
        return old2new;
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::remove_polyhedra_facets(
        const std::vector< bool >& to_delete )
    {
        for( const auto p : Range{ solid_mesh_->nb_polyhedra() } )
        {
            if( to_delete[p] )
            {
                for( const auto f :
                    Range{ solid_mesh_->nb_polyhedron_facets( p ) } )
                {
                    const PolyhedronFacet id{ p, f };
                    PolyhedronFacetVertices facet_vertices(
                        solid_mesh_->nb_polyhedron_facet_vertices( id ) );
                    for( const auto v : Range{
                             solid_mesh_->nb_polyhedron_facet_vertices( id ) } )
                    {
                        facet_vertices[v] =
                            solid_mesh_->polyhedron_facet_vertex( { id, v } );
                    }
                    solid_mesh_->remove_facet(
                        std::move( facet_vertices ), {} );
                }
            }
        }
        solid_mesh_->remove_isolated_facets( {} );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::remove_polyhedra_edges(
        const std::vector< bool >& to_delete )
    {
        for( const auto p : Range{ solid_mesh_->nb_polyhedra() } )
        {
            if( to_delete[p] )
            {
                for( const auto f :
                    Range{ solid_mesh_->nb_polyhedron_facets( p ) } )
                {
                    const PolyhedronFacet id{ p, f };
                    const auto nb_facet_vertices =
                        solid_mesh_->nb_polyhedron_facet_vertices( id );
                    for( const auto v : Range{ nb_facet_vertices } )
                    {
                        const auto v0 =
                            solid_mesh_->polyhedron_facet_vertex( { id, v } );
                        const auto v1 = solid_mesh_->polyhedron_facet_vertex(
                            { id, ( v + 1 ) % nb_facet_vertices } );
                        solid_mesh_->remove_edge( { v0, v1 }, {} );
                    }
                }
            }
        }

        solid_mesh_->remove_isolated_edges( {} );
    }

    template < index_t dimension >
    std::vector< index_t >
        SolidMeshBuilder< dimension >::delete_isolated_vertices()
    {
        std::vector< bool > to_delete( solid_mesh_->nb_vertices(), false );
        for( const auto v : Range{ solid_mesh_->nb_vertices() } )
        {
            const auto& polyhedron_vertex =
                solid_mesh_->polyhedron_around_vertex( v );
            if( polyhedron_vertex.polyhedron_id == NO_ID )
            {
                to_delete[v] = true;
            }
        }
        return delete_vertices( to_delete );
    }

    template < index_t dimension >
    std::vector< index_t >
        SolidMeshBuilder< dimension >::delete_isolated_facets()
    {
        return solid_mesh_->remove_isolated_facets( {} );
    }

    template < index_t dimension >
    std::vector< index_t >
        SolidMeshBuilder< dimension >::delete_isolated_edges()
    {
        return solid_mesh_->remove_isolated_edges( {} );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::update_facet_vertices(
        absl::Span< const index_t > old2new )
    {
        solid_mesh_->update_facet_vertices( old2new, {} );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::update_edge_vertices(
        absl::Span< const index_t > old2new )
    {
        solid_mesh_->update_edge_vertices( old2new, {} );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::update_facet_vertex(
        PolyhedronFacetVertices facet_vertices,
        index_t facet_vertex_id,
        index_t new_vertex_id )
    {
        OPENGEODE_ASSERT( facet_vertex_id < facet_vertices.size(),
            "[SolidMeshBuilder::update_facet_vertex] "
            "Accessing an invalid vertex in facet" );
        solid_mesh_->update_facet_vertex(
            std::move( facet_vertices ), facet_vertex_id, new_vertex_id, {} );
    }

    template < index_t dimension >
    std::vector< index_t > SolidMeshBuilder< dimension >::delete_facets(
        const std::vector< bool >& to_delete )
    {
        return solid_mesh_->delete_facets( to_delete, {} );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::set_point(
        index_t vertex_id, const Point< dimension >& point )
    {
        OPENGEODE_ASSERT( vertex_id < solid_mesh_->nb_vertices(),
            "[SolidMeshBuilder::set_point] Accessing a vertex that does "
            "not exist" );
        do_set_point( vertex_id, point );
    }

    template < index_t dimension >
    index_t SolidMeshBuilder< dimension >::create_point(
        const Point< dimension >& point )
    {
        const auto added_vertex = solid_mesh_->nb_vertices();
        create_vertex();
        set_point( added_vertex, point );
        return added_vertex;
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::copy(
        const SolidMesh< dimension >& solid_mesh )
    {
        VertexSetBuilder::copy( solid_mesh );
        for( const auto p : Range{ solid_mesh.nb_vertices() } )
        {
            set_point( p, solid_mesh.point( p ) );
        }
        for( const auto p : Range{ solid_mesh.nb_polyhedra() } )
        {
            absl::FixedArray< index_t > vertices(
                solid_mesh.nb_polyhedron_vertices( p ) );
            for( const auto v :
                Range{ solid_mesh.nb_polyhedron_vertices( p ) } )
            {
                vertices[v] = solid_mesh.polyhedron_vertex( { p, v } );
            }
            std::vector< std::vector< index_t > > facets(
                solid_mesh.nb_polyhedron_facets( p ) );
            for( const auto f : Range{ solid_mesh.nb_polyhedron_facets( p ) } )
            {
                auto& facet = facets[f];
                facet.resize(
                    solid_mesh.nb_polyhedron_facet_vertices( { p, f } ) );
                for( auto v : Range{
                         solid_mesh.nb_polyhedron_facet_vertices( { p, f } ) } )
                {
                    const auto it = absl::c_find( vertices,
                        solid_mesh.polyhedron_facet_vertex( { { p, f }, v } ) );
                    OPENGEODE_ASSERT( it != vertices.end(),
                        "[SolidMeshBuilder::copy] Wrong indexing between "
                        "polyhedron_vertex and polyhedron_facet_vertex" );
                    facet[v] = static_cast< index_t >(
                        std::distance( vertices.begin(), it ) );
                }
            }
            create_polyhedron( vertices, facets );
        }
        solid_mesh_->polyhedron_attribute_manager().copy(
            solid_mesh.polyhedron_attribute_manager() );
        solid_mesh_->overwrite_facets( solid_mesh, {} );
        solid_mesh_->overwrite_edges( solid_mesh, {} );
    }

    template class opengeode_mesh_api SolidMeshBuilder< 3 >;
} // namespace geode
