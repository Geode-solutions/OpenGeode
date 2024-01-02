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

#include <geode/mesh/builder/solid_mesh_builder.h>

#include <geode/basic/attribute_manager.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/builder/solid_edges_builder.h>
#include <geode/mesh/builder/solid_facets_builder.h>
#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/solid_edges.h>
#include <geode/mesh/core/solid_facets.h>
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
    void update_polyhedron_around_vertices_from_vertices(
        const geode::SolidMesh< dimension >& solid,
        geode::SolidMeshBuilder< dimension >& builder,
        absl::Span< const geode::index_t > vertices_old2new )
    {
        for( const auto v : geode::Range{ solid.nb_vertices() } )
        {
            if( vertices_old2new[v] != geode::NO_ID )
            {
                const auto new_polyhedron_around =
                    solid.polyhedron_around_vertex( vertices_old2new[v] );
                if( new_polyhedron_around )
                {
                    builder.associate_polyhedron_vertex_to_vertex(
                        new_polyhedron_around.value(), v );
                }
                else
                {
                    builder.disassociate_polyhedron_vertex_to_vertex( v );
                    builder.reset_polyhedra_around_vertex( v );
                }
            }
            else
            {
                builder.disassociate_polyhedron_vertex_to_vertex( v );
                builder.reset_polyhedra_around_vertex( v );
            }
        }
    }

    template < geode::index_t dimension >
    void update_polyhedron_around_vertices_from_polyhedron(
        const geode::SolidMesh< dimension >& solid,
        geode::SolidMeshBuilder< dimension >& builder,
        absl::Span< const geode::index_t > old2new )
    {
        for( const auto v : geode::Range{ solid.nb_vertices() } )
        {
            const auto polyhedron_vertex = solid.polyhedron_around_vertex( v );
            if( !polyhedron_vertex )
            {
                continue;
            }
            auto new_polyhedron_vertex = polyhedron_vertex.value();
            new_polyhedron_vertex.polyhedron_id =
                old2new[polyhedron_vertex->polyhedron_id];
            if( new_polyhedron_vertex.polyhedron_id == geode::NO_ID )
            {
                for( const auto& polyhedron :
                    solid.polyhedra_around_vertex( v ) )
                {
                    const auto new_id = old2new[polyhedron.polyhedron_id];
                    if( new_id != geode::NO_ID )
                    {
                        new_polyhedron_vertex = { new_id,
                            polyhedron.vertex_id };
                        break;
                    }
                }
            }
            if( new_polyhedron_vertex.polyhedron_id == geode::NO_ID )
            {
                builder.disassociate_polyhedron_vertex_to_vertex( v );
            }
            else
            {
                builder.associate_polyhedron_vertex_to_vertex(
                    new_polyhedron_vertex, v );
            }
        }
        for( const auto p : geode::Indices{ old2new } )
        {
            if( p == old2new[p] )
            {
                continue;
            }
            for( const auto v : solid.polyhedron_vertices( p ) )
            {
                builder.reset_polyhedra_around_vertex( v );
            }
        }
    }

    template < geode::index_t dimension >
    void reset_polyhedra_around_facet_vertices(
        const geode::SolidMesh< dimension >& solid,
        geode::SolidMeshBuilder< dimension >& builder,
        const geode::PolyhedronFacet& facet )
    {
        for( const auto vertex : solid.polyhedron_facet_vertices( facet ) )
        {
            builder.reset_polyhedra_around_vertex( vertex );
        }
    }

    template < geode::index_t dimension >
    void check_no_polyhedron_to_delete(
        const geode::SolidMesh< dimension >& solid,
        absl::Span< const geode::index_t > vertices_old2new )
    {
        for( const auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            for( const auto v :
                geode::LRange{ solid.nb_polyhedron_vertices( p ) } )
            {
                const auto new_vertex =
                    vertices_old2new[solid.polyhedron_vertex( { p, v } )];
                OPENGEODE_EXCEPTION( new_vertex != geode::NO_ID,
                    "[SolidMesh::update_polyhedron_vertices] No polyhedron "
                    "should be removed" );
            }
        }
    }

    template < geode::index_t dimension >
    void copy_points( const geode::SolidMesh< dimension >& solid,
        geode::SolidMeshBuilder< dimension >& builder )
    {
        for( const auto p : geode::Range{ solid.nb_vertices() } )
        {
            builder.set_point( p, solid.point( p ) );
        }
    }

    template < geode::index_t dimension >
    void copy_polyhedra( const geode::SolidMesh< dimension >& solid,
        geode::SolidMeshBuilder< dimension >& builder )
    {
        for( const auto p : geode::Range{ solid.nb_polyhedra() } )
        {
            absl::FixedArray< geode::index_t > vertices(
                solid.nb_polyhedron_vertices( p ) );
            for( const auto v :
                geode::LRange{ solid.nb_polyhedron_vertices( p ) } )
            {
                vertices[v] = solid.polyhedron_vertex( { p, v } );
            }
            absl::FixedArray< std::vector< geode::local_index_t > > facets(
                solid.nb_polyhedron_facets( p ) );
            for( const auto f :
                geode::LRange{ solid.nb_polyhedron_facets( p ) } )
            {
                auto& facet = facets[f];
                facet.resize( solid.nb_polyhedron_facet_vertices( { p, f } ) );
                for( const auto v : geode::LRange{
                         solid.nb_polyhedron_facet_vertices( { p, f } ) } )
                {
                    const auto it = absl::c_find( vertices,
                        solid.polyhedron_facet_vertex( { { p, f }, v } ) );
                    OPENGEODE_ASSERT( it != vertices.end(),
                        "[SolidMeshBuilder::copy] Wrong indexing between "
                        "polyhedron_vertex and polyhedron_facet_vertex" );
                    facet[v] = static_cast< geode::local_index_t >(
                        std::distance( vertices.begin(), it ) );
                }
            }
            builder.create_polyhedron( vertices, facets );
        }
    }

    template < geode::index_t dimension >
    absl::optional< geode::index_t > polyhedron_vertex_in_facet(
        const geode::SolidMesh< dimension >& solid,
        const geode::PolyhedronFacet& facet,
        geode::local_index_t vertex_id )
    {
        for( const auto v :
            geode::LRange{ solid.nb_polyhedron_facet_vertices( facet ) } )
        {
            if( solid.polyhedron_facet_vertex_id( { facet, v } ).vertex_id
                == vertex_id )
            {
                return v;
            }
        }
        return absl::nullopt;
    }

    template < geode::index_t dimension >
    void update_edge_and_facet( const geode::SolidMesh< dimension >& solid,
        geode::SolidMeshBuilder< dimension >& builder,
        const geode::PolyhedronVertex& polyhedron_vertex,
        geode::index_t new_vertex_id )
    {
        for( const auto f : geode::LRange{ solid.nb_polyhedron_facets(
                 polyhedron_vertex.polyhedron_id ) } )
        {
            const geode::PolyhedronFacet facet{ polyhedron_vertex.polyhedron_id,
                f };
            const auto nb_facet_vertices =
                solid.nb_polyhedron_facet_vertices( facet );
            absl::FixedArray< geode::PolyhedronVertex > facet_vertices(
                nb_facet_vertices );
            for( const auto v : geode::LRange{ nb_facet_vertices } )
            {
                facet_vertices[v] =
                    solid.polyhedron_facet_vertex_id( { facet, v } );
            }
            const auto position_it =
                absl::c_find( facet_vertices, polyhedron_vertex );
            if( position_it != facet_vertices.end() )
            {
                geode::PolyhedronFacetVertices facet_vertices_id;
                facet_vertices_id.reserve( nb_facet_vertices );
                for( const auto& v : facet_vertices )
                {
                    facet_vertices_id.emplace_back(
                        solid.polyhedron_vertex( v ) );
                }

                const auto position = static_cast< geode::index_t >(
                    std::distance( facet_vertices.begin(), position_it ) );

                if( solid.are_facets_enabled() )
                {
                    auto facets = builder.facets_builder();
                    facets.update_facet_vertex(
                        facet_vertices_id, position, new_vertex_id );
                }

                if( solid.are_edges_enabled() )
                {
                    auto edges = builder.edges_builder();
                    const auto next =
                        position + 1 == nb_facet_vertices ? 0 : position + 1;
                    std::array< geode::index_t, 2 > next_edge_vertices{
                        facet_vertices_id[position], facet_vertices_id[next]
                    };
                    if( next_edge_vertices[0] < next_edge_vertices[1] )
                    {
                        edges.update_edge_vertex(
                            next_edge_vertices, 0, new_vertex_id );
                    }
                    const auto prev =
                        position == 0 ? nb_facet_vertices - 1 : position - 1;
                    std::array< geode::index_t, 2 > previous_edge_vertices{
                        facet_vertices_id[prev], facet_vertices_id[position]
                    };
                    if( previous_edge_vertices[0] < previous_edge_vertices[1] )
                    {
                        edges.update_edge_vertex(
                            previous_edge_vertices, 1, new_vertex_id );
                    }
                }
            }
        }
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    SolidMeshBuilder< dimension >::SolidMeshBuilder(
        SolidMesh< dimension >& mesh )
        : VertexSetBuilder( mesh ),
          CoordinateReferenceSystemManagersBuilder< dimension >( mesh ),
          solid_mesh_( mesh )
    {
    }

    template < index_t dimension >
    SolidMeshBuilder< dimension >::~SolidMeshBuilder()
    {
    }

    template < index_t dimension >
    std::unique_ptr< SolidMeshBuilder< dimension > >
        SolidMeshBuilder< dimension >::create( SolidMesh< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            SolidMeshBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::replace_vertex(
        index_t old_vertex_id, index_t new_vertex_id )
    {
        if( old_vertex_id == new_vertex_id )
        {
            return;
        }
        const auto& polyhedra_around =
            solid_mesh_.polyhedra_around_vertex( old_vertex_id );
        disassociate_polyhedron_vertex_to_vertex( old_vertex_id );
        for( const auto& polyhedron_around : polyhedra_around )
        {
            if( solid_mesh_.are_edges_enabled()
                || solid_mesh_.are_facets_enabled() )
            {
                update_edge_and_facet(
                    solid_mesh_, *this, polyhedron_around, new_vertex_id );
            }
            update_polyhedron_vertex( polyhedron_around, new_vertex_id );
        }
        reset_polyhedra_around_vertex( old_vertex_id );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::set_polyhedron_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        const auto polyhedron_vertex_id =
            solid_mesh_.polyhedron_vertex( polyhedron_vertex );
        if( polyhedron_vertex_id == vertex_id )
        {
            return;
        }
        if( polyhedron_vertex_id != NO_ID )
        {
            const auto polyhedron_around =
                solid_mesh_.polyhedron_around_vertex( polyhedron_vertex_id );
            if( polyhedron_around == polyhedron_vertex )
            {
                const auto& polyhedra_around =
                    solid_mesh_.polyhedra_around_vertex( polyhedron_vertex_id );
                if( polyhedra_around.size() < 2 )
                {
                    disassociate_polyhedron_vertex_to_vertex(
                        polyhedron_vertex_id );
                }
                else
                {
                    associate_polyhedron_vertex_to_vertex(
                        polyhedra_around[1], polyhedron_vertex_id );
                }
            }
            reset_polyhedra_around_vertex( polyhedron_vertex_id );
        }

        if( solid_mesh_.are_edges_enabled()
            || solid_mesh_.are_facets_enabled() )
        {
            update_edge_and_facet(
                solid_mesh_, *this, polyhedron_vertex, vertex_id );
        }
        update_polyhedron_vertex( polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::update_polyhedron_vertices(
        absl::Span< const index_t > old2new )
    {
        check_no_polyhedron_to_delete( solid_mesh_, old2new );
        update_polyhedron_around_vertices_from_vertices(
            solid_mesh_, *this, old2new );
        for( const auto p : Range{ solid_mesh_.nb_polyhedra() } )
        {
            for( const auto v :
                LRange{ solid_mesh_.nb_polyhedron_vertices( p ) } )
            {
                const PolyhedronVertex id{ p, v };
                const auto new_vertex =
                    old2new[solid_mesh_.polyhedron_vertex( id )];
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
        check_polyhedron_id( solid_mesh_, polyhedron_vertex.polyhedron_id );
        check_polyhedron_vertex_id( solid_mesh_,
            polyhedron_vertex.polyhedron_id, polyhedron_vertex.vertex_id );
        OPENGEODE_ASSERT( vertex_id < solid_mesh_.nb_vertices(),
            "[SolidMeshBuilder::update_polyhedron_vertex] Accessing a "
            "vertex that does not exist" );
        associate_polyhedron_vertex_to_vertex( polyhedron_vertex, vertex_id );
        do_set_polyhedron_vertex( polyhedron_vertex, vertex_id );
    }

    template < index_t dimension >
    index_t SolidMeshBuilder< dimension >::create_polyhedron(
        absl::Span< const index_t > vertices,
        absl::Span< const std::vector< local_index_t > > facets )
    {
        const auto added_polyhedron = solid_mesh_.nb_polyhedra();
        solid_mesh_.polyhedron_attribute_manager().resize(
            added_polyhedron + 1 );
        for( const auto v : LIndices{ vertices } )
        {
            associate_polyhedron_vertex_to_vertex(
                { added_polyhedron, v }, vertices[v] );
        }
        do_create_polyhedron( vertices, facets );
        if( solid_mesh_.are_edges_enabled() )
        {
            auto edges = edges_builder();
            for( auto&& edge_vertices :
                solid_mesh_.polyhedron_edges_vertices( added_polyhedron ) )
            {
                edges.find_or_create_edge( std::move( edge_vertices ) );
            }
        }
        if( solid_mesh_.are_facets_enabled() )
        {
            auto builder = facets_builder();
            for( auto&& facet_vertices :
                solid_mesh_.polyhedron_facets_vertices( added_polyhedron ) )
            {
                builder.find_or_create_facet( std::move( facet_vertices ) );
            }
        }
        return added_polyhedron;
    }

    template < index_t dimension >
    std::vector< PolyhedronFacetVertices >
        SolidMeshBuilder< dimension >::get_polyhedron_facet_vertices(
            absl::Span< const index_t > vertices,
            absl::Span< const std::vector< index_t > > facets ) const
    {
        std::vector< PolyhedronFacetVertices > polyhedron_facet_vertices(
            facets.size() );
        for( const auto f : Indices{ facets } )
        {
            polyhedron_facet_vertices[f].resize( facets[f].size() );
            for( const auto v : Indices{ facets[f] } )
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
    void SolidMeshBuilder< dimension >::reset_polyhedra_around_vertex(
        index_t vertex_id )
    {
        solid_mesh_.reset_polyhedra_around_vertex( vertex_id, {} );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::associate_polyhedron_vertex_to_vertex(
        const PolyhedronVertex& polyhedron_vertex, index_t vertex_id )
    {
        OPENGEODE_ASSERT( polyhedron_vertex.polyhedron_id != NO_ID,
            "[SolidMeshBuilder::associate_polyhedron_vertex_to_vertex] "
            "PolyhedronVertex invalid" );
        OPENGEODE_ASSERT( polyhedron_vertex.vertex_id != NO_ID,
            "[SolidMeshBuilder::associate_polyhedron_vertex_to_vertex] "
            "PolyhedronVertex invalid" );
        solid_mesh_.associate_polyhedron_vertex_to_vertex(
            polyhedron_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    void
        SolidMeshBuilder< dimension >::disassociate_polyhedron_vertex_to_vertex(
            index_t vertex_id )
    {
        solid_mesh_.associate_polyhedron_vertex_to_vertex(
            PolyhedronVertex{}, vertex_id, {} );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::do_delete_vertices(
        const std::vector< bool >& to_delete,
        absl::Span< const index_t > old2new )
    {
        update_polyhedron_vertices( old2new );
        if( solid_mesh_.are_facets_enabled() )
        {
            facets_builder().update_facet_vertices( old2new );
        }
        if( solid_mesh_.are_edges_enabled() )
        {
            edges_builder().update_edge_vertices( old2new );
        }
        do_delete_solid_vertices( to_delete, old2new );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::do_permute_vertices(
        absl::Span< const index_t > permutation,
        absl::Span< const index_t > old2new )
    {
        update_polyhedron_vertices( old2new );
        if( solid_mesh_.are_facets_enabled() )
        {
            facets_builder().update_facet_vertices( old2new );
        }
        if( solid_mesh_.are_edges_enabled() )
        {
            edges_builder().update_edge_vertices( old2new );
        }
        do_permute_solid_vertices( permutation, old2new );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::set_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet, index_t adjacent_id )
    {
        check_polyhedron_id( solid_mesh_, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id( solid_mesh_, polyhedron_facet.polyhedron_id,
            polyhedron_facet.facet_id );
        OPENGEODE_ASSERT( adjacent_id < solid_mesh_.nb_polyhedra(),
            "[SolidMeshBuilder::set_polyhedron_adjacent] Accessing a "
            "polyhedron that does not exist" );
        reset_polyhedra_around_facet_vertices(
            solid_mesh_, *this, polyhedron_facet );
        do_set_polyhedron_adjacent( polyhedron_facet, adjacent_id );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::unset_polyhedron_adjacent(
        const PolyhedronFacet& polyhedron_facet )
    {
        check_polyhedron_id( solid_mesh_, polyhedron_facet.polyhedron_id );
        check_polyhedron_facet_id( solid_mesh_, polyhedron_facet.polyhedron_id,
            polyhedron_facet.facet_id );
        reset_polyhedra_around_facet_vertices(
            solid_mesh_, *this, polyhedron_facet );
        do_unset_polyhedron_adjacent( polyhedron_facet );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::compute_polyhedron_adjacencies()
    {
        absl::FixedArray< index_t > polyhedra_to_connect(
            solid_mesh_.nb_polyhedra() );
        absl::c_iota( polyhedra_to_connect, 0 );
        compute_polyhedron_adjacencies( polyhedra_to_connect );
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::compute_polyhedron_adjacencies(
        absl::Span< const index_t > polyhedra_to_connect )
    {
        using Facet = detail::VertexCycle< PolyhedronFacetVertices >;
        absl::flat_hash_map< Facet, PolyhedronFacet > facets;
        for( const auto polyhedron : polyhedra_to_connect )
        {
            for( const auto f :
                LRange{ solid_mesh_.nb_polyhedron_facets( polyhedron ) } )
            {
                const PolyhedronFacet facet{ polyhedron, f };
                if( !solid_mesh_.is_polyhedron_facet_on_border( facet ) )
                {
                    continue;
                }
                const auto output = facets.emplace(
                    solid_mesh_.polyhedron_facet_vertices( facet ), facet );
                if( !output.second )
                {
                    const auto it = output.first;
                    do_set_polyhedron_adjacent( it->second, polyhedron );
                    do_set_polyhedron_adjacent(
                        facet, it->second.polyhedron_id );
                    facets.erase( it );
                }
            }
        }
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::remove_polyhedra_edges(
        const std::vector< bool >& to_delete )
    {
        auto edges = edges_builder();
        for( const auto p : Range{ solid_mesh_.nb_polyhedra() } )
        {
            if( !to_delete[p] )
            {
                continue;
            }
            for( auto&& edge : solid_mesh_.polyhedron_edges_vertices( p ) )
            {
                edges.remove_edge( std::move( edge ) );
            }
        }
    }

    template < index_t dimension >
    SolidEdgesBuilder< dimension >
        SolidMeshBuilder< dimension >::edges_builder()
    {
        return { solid_mesh_.edges( {} ) };
    }

    template < index_t dimension >
    std::vector< index_t > SolidMeshBuilder< dimension >::delete_polyhedra(
        const std::vector< bool >& to_delete )
    {
        const auto old2new = detail::mapping_after_deletion( to_delete );
        if( absl::c_find( to_delete, true ) == to_delete.end() )
        {
            return old2new;
        }
        if( solid_mesh_.are_facets_enabled() )
        {
            remove_polyhedra_facets( to_delete );
        }
        if( solid_mesh_.are_edges_enabled() )
        {
            remove_polyhedra_edges( to_delete );
        }
        update_polyhedron_around_vertices_from_polyhedron(
            solid_mesh_, *this, old2new );
        update_polyhedron_adjacencies( old2new );
        solid_mesh_.polyhedron_attribute_manager().delete_elements( to_delete );
        do_delete_polyhedra( to_delete, old2new );
        return old2new;
    }

    template < index_t dimension >
    std::vector< index_t > SolidMeshBuilder< dimension >::permute_polyhedra(
        absl::Span< const index_t > permutation )
    {
        const auto old2new = old2new_permutation( permutation );
        update_polyhedron_around_vertices_from_polyhedron(
            solid_mesh_, *this, old2new );
        update_polyhedron_adjacencies( old2new );
        solid_mesh_.polyhedron_attribute_manager().permute_elements(
            permutation );
        do_permute_polyhedra( permutation, old2new );
        return old2new;
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::remove_polyhedra_facets(
        const std::vector< bool >& to_delete )
    {
        auto facets = facets_builder();
        for( const auto p : Range{ solid_mesh_.nb_polyhedra() } )
        {
            if( to_delete[p] )
            {
                for( auto&& facet :
                    solid_mesh_.polyhedron_facets_vertices( p ) )
                {
                    facets.remove_facet( std::move( facet ) );
                }
            }
        }
    }

    template < index_t dimension >
    SolidFacetsBuilder< dimension >
        SolidMeshBuilder< dimension >::facets_builder()
    {
        return { solid_mesh_.facets( {} ) };
    }

    template < index_t dimension >
    std::vector< index_t >
        SolidMeshBuilder< dimension >::delete_isolated_vertices()
    {
        std::vector< bool > to_delete( solid_mesh_.nb_vertices(), false );
        for( const auto v : Range{ solid_mesh_.nb_vertices() } )
        {
            to_delete[v] = !solid_mesh_.polyhedron_around_vertex( v );
        }
        return delete_vertices( to_delete );
    }

    template < index_t dimension >
    index_t SolidMeshBuilder< dimension >::create_point(
        Point< dimension > point )
    {
        const auto added_vertex = solid_mesh_.nb_vertices();
        create_vertex();
        this->set_point( added_vertex, std::move( point ) );
        return added_vertex;
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::update_polyhedron_info(
        index_t polyhedron_id, absl::Span< const index_t > vertices )
    {
        local_index_t vertex_id{ 0 };
        for( const auto& vertex : vertices )
        {
            this->associate_polyhedron_vertex_to_vertex(
                { polyhedron_id, vertex_id++ }, vertex );
        }
        if( solid_mesh_.are_facets_enabled() )

        {
            auto facets = this->facets_builder();
            for( auto&& facet_vertices :
                solid_mesh_.polyhedron_facets_vertices( polyhedron_id ) )
            {
                facets.find_or_create_facet( facet_vertices );
            }
        }
        if( solid_mesh_.are_edges_enabled() )
        {
            auto edges = this->edges_builder();
            for( auto&& edge_vertices :
                solid_mesh_.polyhedron_edges_vertices( polyhedron_id ) )
            {
                edges.find_or_create_edge( edge_vertices );
            }
        }
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::update_polyhedron_adjacencies(
        absl::Span< const index_t > old2new )
    {
        for( const auto p : Range{ solid_mesh_.nb_polyhedra() } )
        {
            if( old2new[p] == NO_ID )
            {
                continue;
            }
            for( const auto f :
                LRange{ solid_mesh_.nb_polyhedron_facets( p ) } )
            {
                const PolyhedronFacet id{ p, f };
                if( const auto adj = solid_mesh_.polyhedron_adjacent( id ) )
                {
                    const auto new_adjacent = old2new[adj.value()];
                    if( new_adjacent == adj.value() )
                    {
                        continue;
                    }
                    if( new_adjacent == NO_ID )
                    {
                        do_unset_polyhedron_adjacent( id );
                    }
                    else
                    {
                        do_set_polyhedron_adjacent( id, new_adjacent );
                    }
                }
            }
        }
    }

    template < index_t dimension >
    void SolidMeshBuilder< dimension >::copy(
        const SolidMesh< dimension >& solid_mesh )
    {
        OPENGEODE_EXCEPTION(
            solid_mesh_.nb_vertices() == 0 && solid_mesh_.nb_polyhedra() == 0,
            "[SolidMeshBuilder::copy] Cannot copy a mesh into an already "
            "initialized mesh." );
        if( solid_mesh_.are_edges_enabled() )
        {
            OPENGEODE_EXCEPTION( solid_mesh_.edges().nb_edges() == 0,
                "[SolidMeshBuilder::copy] Cannot copy a mesh into an already "
                "initialized mesh." );
            solid_mesh_.disable_edges();
        }
        if( solid_mesh_.are_facets_enabled() )
        {
            OPENGEODE_EXCEPTION( solid_mesh_.facets().nb_facets() == 0,
                "[SolidMeshBuilder::copy] Cannot copy a mesh into an already "
                "initialized mesh." );
            solid_mesh_.disable_facets();
        }
        VertexSetBuilder::copy( solid_mesh );
        if( solid_mesh.impl_name() == solid_mesh_.impl_name() )
        {
            do_copy_points( solid_mesh );
            do_copy_polyhedra( solid_mesh );
        }
        else
        {
            copy_points( solid_mesh, *this );
            copy_polyhedra( solid_mesh, *this );
        }
        solid_mesh_.polyhedron_attribute_manager().copy(
            solid_mesh.polyhedron_attribute_manager() );
        if( solid_mesh.are_edges_enabled() )
        {
            solid_mesh_.copy_edges( solid_mesh, {} );
        }
        if( solid_mesh.are_facets_enabled() )
        {
            solid_mesh_.copy_facets( solid_mesh, {} );
        }
    }

    template class opengeode_mesh_api SolidMeshBuilder< 3 >;
} // namespace geode
