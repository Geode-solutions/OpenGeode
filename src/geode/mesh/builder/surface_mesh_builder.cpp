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

#include <geode/mesh/builder/surface_mesh_builder.h>

#include <geode/basic/attribute_manager.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/builder/surface_edges_builder.h>
#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/surface_edges.h>
#include <geode/mesh/core/surface_mesh.h>

namespace
{
    template < geode::index_t dimension >
    void check_polygon_id( const geode::SurfaceMesh< dimension >& surface,
        const geode::index_t polygon_id )
    {
        geode_unused( surface );
        geode_unused( polygon_id );
        OPENGEODE_ASSERT( polygon_id < surface.nb_polygons(),
            "[check_polygon_id] Trying to access an invalid polygon" );
    }

    template < geode::index_t dimension >
    void check_polygon_vertex_id(
        const geode::SurfaceMesh< dimension >& surface,
        const geode::index_t polygon_id,
        const geode::index_t vertex_id )
    {
        geode_unused( surface );
        geode_unused( polygon_id );
        geode_unused( vertex_id );
        OPENGEODE_ASSERT( vertex_id < surface.nb_polygon_vertices( polygon_id ),
            "[check_polygon_vertex_id] Trying to access an invalid polygon "
            "local vertex" );
    }

    template < geode::index_t dimension >
    void check_polygon_edge_id( const geode::SurfaceMesh< dimension >& surface,
        const geode::index_t polygon_id,
        const geode::index_t edge_id )
    {
        geode_unused( surface );
        geode_unused( polygon_id );
        geode_unused( edge_id );
        OPENGEODE_ASSERT( edge_id < surface.nb_polygon_edges( polygon_id ),
            "[check_polygon_edge_id] Trying to access an invalid polygon local "
            "edge" );
    }

    template < geode::index_t dimension >
    void update_polygon_adjacencies(
        const geode::SurfaceMesh< dimension >& surface,
        geode::SurfaceMeshBuilder< dimension >& builder,
        absl::Span< const geode::index_t > old2new )
    {
        for( const auto p : geode::Range{ surface.nb_polygons() } )
        {
            for( const auto e : geode::LRange{ surface.nb_polygon_edges( p ) } )
            {
                const geode::PolygonEdge id{ p, e };
                if( const auto adj = surface.polygon_adjacent( id ) )
                {
                    const auto new_adjacent = old2new[adj.value()];
                    if( new_adjacent == geode::NO_ID )
                    {
                        builder.unset_polygon_adjacent( id );
                    }
                    else
                    {
                        builder.set_polygon_adjacent( id, new_adjacent );
                    }
                }
            }
        }
    }

    template < geode::index_t dimension >
    void update_polygon_around_vertices(
        const geode::SurfaceMesh< dimension >& surface,
        geode::SurfaceMeshBuilder< dimension >& builder,
        absl::Span< const geode::index_t > vertices_old2new )
    {
        for( const auto v : geode::Range{ surface.nb_vertices() } )
        {
            const auto new_vertex = vertices_old2new[v];
            if( new_vertex == v )
            {
                continue;
            }
            if( new_vertex != geode::NO_ID )
            {
                if( const auto new_polygon_around =
                        surface.polygon_around_vertex( new_vertex ) )
                {
                    builder.associate_polygon_vertex_to_vertex(
                        new_polygon_around.value(), v );
                }
                else
                {
                    builder.disassociate_polygon_vertex_to_vertex( v );
                    builder.reset_polygons_around_vertex( v );
                }
            }
            else
            {
                builder.disassociate_polygon_vertex_to_vertex( v );
                builder.reset_polygons_around_vertex( v );
            }
        }
    }

    template < geode::index_t dimension >
    void check_no_polygon_to_delete(
        const geode::SurfaceMesh< dimension >& surface,
        absl::Span< const geode::index_t > vertices_old2new )
    {
        for( const auto p : geode::Range{ surface.nb_polygons() } )
        {
            for( const auto v :
                geode::LRange{ surface.nb_polygon_vertices( p ) } )
            {
                const auto new_vertex =
                    vertices_old2new[surface.polygon_vertex( { p, v } )];
                OPENGEODE_EXCEPTION( new_vertex != geode::NO_ID,
                    "[SurfaceMesh::update_polygon_vertices] No polygon should "
                    "be removed" );
            }
        }
    }

    template < geode::index_t dimension >
    absl::FixedArray< geode::index_t > get_polygon_vertices(
        const geode::SurfaceMesh< dimension >& surface,
        geode::index_t polygon_id )
    {
        const auto nb_vertices = surface.nb_polygon_vertices( polygon_id );
        absl::FixedArray< geode::index_t > vertices_id( nb_vertices );
        for( const auto v : geode::LRange{ nb_vertices } )
        {
            vertices_id[v] = surface.polygon_vertex( { polygon_id, v } );
        }
        return vertices_id;
    }

    template < geode::index_t dimension >
    absl::optional< geode::PolygonEdge > find_polygon_adjacent_edge(
        const geode::SurfaceMesh< dimension >& surface,
        const geode::PolygonEdge& polygon_edge,
        const std::array< geode::index_t, 2 >& vertices )
    {
        const auto polygon_adj = surface.polygon_adjacent( polygon_edge );
        if( !polygon_adj )
        {
            return absl::nullopt;
        }
        const auto polygon_adj_id = polygon_adj.value();
        for( const auto e :
            geode::LRange{ surface.nb_polygon_edges( polygon_adj_id ) } )
        {
            const geode::PolygonEdge adj_edge{ polygon_adj_id, e };
            const auto adj_v0 = surface.polygon_vertex( adj_edge );
            const auto adj_v1 = surface.polygon_edge_vertex( adj_edge, 1 );
            if( ( vertices[0] == adj_v1 && vertices[1] == adj_v0 )
                || ( vertices[0] == adj_v0 && vertices[1] == adj_v1 ) )
            {
                return adj_edge;
            }
        }
        return absl::nullopt;
    }

    template < geode::index_t dimension >
    void update_polygon_around( const geode::SurfaceMesh< dimension >& surface,
        geode::SurfaceMeshBuilder< dimension >& builder,
        absl::Span< const geode::index_t > old2new )
    {
        for( const auto v : geode::Range{ surface.nb_vertices() } )
        {
            const auto polygon_vertex = surface.polygon_around_vertex( v );
            if( !polygon_vertex )
            {
                continue;
            }
            auto new_polygon_vertex = polygon_vertex.value();
            new_polygon_vertex.polygon_id = old2new[polygon_vertex->polygon_id];
            if( new_polygon_vertex.polygon_id == geode::NO_ID )
            {
                for( const auto& polygon : surface.polygons_around_vertex( v ) )
                {
                    const auto new_polygon = old2new[polygon.polygon_id];
                    if( new_polygon != geode::NO_ID )
                    {
                        new_polygon_vertex = { new_polygon, polygon.vertex_id };
                        break;
                    }
                }
                builder.disassociate_polygon_vertex_to_vertex( v );
                builder.reset_polygons_around_vertex( v );
            }
            else
            {
                for( const auto& polygon : surface.polygons_around_vertex( v ) )
                {
                    if( old2new[polygon.polygon_id] != polygon.polygon_id )
                    {
                        builder.reset_polygons_around_vertex( v );
                        break;
                    }
                }
                builder.associate_polygon_vertex_to_vertex(
                    new_polygon_vertex, v );
            }
        }
    }

    template < geode::index_t dimension >
    absl::optional< geode::PolygonEdge > non_manifold_polygon_adjacent_edge(
        const geode::SurfaceMesh< dimension >& surface,
        const geode::PolygonEdge& polygon_edge,
        const std::array< geode::index_t, 2 >& vertices )
    {
        const auto adj_edge =
            find_polygon_adjacent_edge( surface, polygon_edge, vertices );
        if( !adj_edge )
        {
            return absl::nullopt;
        }
        const auto polygon = surface.polygon_adjacent( adj_edge.value() );
        if( !polygon )
        {
            return absl::nullopt;
        }
        // Non-manifold edge
        if( polygon != polygon_edge.polygon_id )
        {
            return adj_edge;
        }
        return absl::nullopt;
    }

    template < geode::index_t dimension >
    void reset_polygons_around_edge_vertices(
        const geode::SurfaceMesh< dimension >& surface,
        geode::SurfaceMeshBuilder< dimension >& builder,
        const geode::PolygonEdge& edge )
    {
        for( const auto vertex : surface.polygon_edge_vertices( edge ) )
        {
            builder.reset_polygons_around_vertex( vertex );
        }
    }

    template < geode::index_t dimension >
    void copy_points( const geode::SurfaceMesh< dimension >& surface,
        geode::SurfaceMeshBuilder< dimension >& builder )
    {
        for( const auto p : geode::Range{ surface.nb_vertices() } )
        {
            builder.set_point( p, surface.point( p ) );
        }
    }

    template < geode::index_t dimension >
    void copy_polygons( const geode::SurfaceMesh< dimension >& surface,
        geode::SurfaceMeshBuilder< dimension >& builder )
    {
        for( const auto p : geode::Range{ surface.nb_polygons() } )
        {
            absl::FixedArray< geode::index_t > vertices(
                surface.nb_polygon_vertices( p ) );
            for( const auto v :
                geode::LRange{ surface.nb_polygon_vertices( p ) } )
            {
                vertices[v] = surface.polygon_vertex( { p, v } );
            }
            builder.create_polygon( vertices );
        }
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    SurfaceMeshBuilder< dimension >::SurfaceMeshBuilder(
        SurfaceMesh< dimension >& mesh )
        : VertexSetBuilder( mesh ), surface_mesh_( mesh )
    {
    }

    template < index_t dimension >
    std::unique_ptr< SurfaceMeshBuilder< dimension > >
        SurfaceMeshBuilder< dimension >::create(
            SurfaceMesh< dimension >& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder<
            SurfaceMeshBuilder< dimension > >( mesh );
    }

    template < index_t dimension >
    index_t SurfaceMeshBuilder< dimension >::create_polygon(
        absl::Span< const index_t > vertices )
    {
        const auto added_polygon = surface_mesh_.nb_polygons();
        surface_mesh_.polygon_attribute_manager().resize( added_polygon + 1 );
        for( const auto v : LIndices{ vertices } )
        {
            associate_polygon_vertex_to_vertex(
                { added_polygon, v }, vertices[v] );
        }
        if( surface_mesh_.are_edges_enabled() )
        {
            auto edges = edges_builder();
            for( const auto e : Range{ vertices.size() - 1 } )
            {
                edges.find_or_create_edge( { vertices[e], vertices[e + 1] } );
            }
            edges.find_or_create_edge( { vertices.back(), vertices.front() } );
        }
        do_create_polygon( vertices );
        return added_polygon;
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::reset_polygons_around_vertex(
        index_t vertex_id )
    {
        surface_mesh_.reset_polygons_around_vertex( vertex_id, {} );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::associate_polygon_vertex_to_vertex(
        const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        OPENGEODE_ASSERT( polygon_vertex.polygon_id != NO_ID,
            "[SurfaceMeshBuilder::associate_polygon_vertex_to_vertex] "
            "PolygonVertex invalid" );
        OPENGEODE_ASSERT( polygon_vertex.vertex_id != NO_ID,
            "[SurfaceMeshBuilder::associate_polygon_vertex_to_vertex] "
            "PolygonVertex invalid" );
        surface_mesh_.associate_polygon_vertex_to_vertex(
            polygon_vertex, vertex_id, {} );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::disassociate_polygon_vertex_to_vertex(
        index_t vertex_id )
    {
        surface_mesh_.associate_polygon_vertex_to_vertex(
            PolygonVertex{}, vertex_id, {} );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::replace_vertex(
        index_t old_vertex_id, index_t new_vertex_id )
    {
        if( old_vertex_id == new_vertex_id )
        {
            return;
        }
        const auto& polygons_around =
            surface_mesh_.polygons_around_vertex( old_vertex_id );
        disassociate_polygon_vertex_to_vertex( old_vertex_id );
        for( const auto& polygon_around : polygons_around )
        {
            if( surface_mesh_.are_edges_enabled() )
            {
                const auto previous_id = surface_mesh_.polygon_vertex(
                    surface_mesh_.previous_polygon_vertex( polygon_around ) );
                const auto next_id = surface_mesh_.polygon_vertex(
                    surface_mesh_.next_polygon_edge( polygon_around ) );
                auto edges = edges_builder();
                edges.update_edge_vertex(
                    { old_vertex_id, next_id }, 0, new_vertex_id );
                edges.update_edge_vertex(
                    { previous_id, old_vertex_id }, 1, new_vertex_id );
            }
            update_polygon_vertex( polygon_around, new_vertex_id );
        }
        reset_polygons_around_vertex( old_vertex_id );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::set_polygon_vertex(
        const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        const auto polygon_vertex_id =
            surface_mesh_.polygon_vertex( polygon_vertex );
        if( polygon_vertex_id == vertex_id )
        {
            return;
        }
        if( polygon_vertex_id != NO_ID )
        {
            const auto polygon_around =
                surface_mesh_.polygon_around_vertex( polygon_vertex_id );
            if( polygon_around == polygon_vertex )
            {
                const auto& polygons_around =
                    surface_mesh_.polygons_around_vertex( polygon_vertex_id );
                if( polygons_around.size() < 2 )
                {
                    disassociate_polygon_vertex_to_vertex( polygon_vertex_id );
                }
                else
                {
                    associate_polygon_vertex_to_vertex(
                        polygons_around[1], polygon_vertex_id );
                }
            }
            reset_polygons_around_vertex( polygon_vertex_id );
        }

        if( surface_mesh_.are_edges_enabled() )
        {
            const auto previous_id = surface_mesh_.polygon_vertex(
                surface_mesh_.previous_polygon_vertex( polygon_vertex ) );
            const auto next_id = surface_mesh_.polygon_vertex(
                surface_mesh_.next_polygon_edge( polygon_vertex ) );
            auto edges = edges_builder();
            edges.update_edge_vertex(
                { polygon_vertex_id, next_id }, 0, vertex_id );
            edges.update_edge_vertex(
                { previous_id, polygon_vertex_id }, 1, vertex_id );
        }
        update_polygon_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::update_polygon_vertices(
        absl::Span< const index_t > old2new )
    {
        check_no_polygon_to_delete( surface_mesh_, old2new );
        update_polygon_around_vertices( surface_mesh_, *this, old2new );
        for( const auto p : Range{ surface_mesh_.nb_polygons() } )
        {
            for( const auto v :
                LRange{ surface_mesh_.nb_polygon_vertices( p ) } )
            {
                const PolygonVertex id{ p, v };
                const auto old_vertex = surface_mesh_.polygon_vertex( id );
                const auto new_vertex = old2new[old_vertex];
                OPENGEODE_ASSERT( new_vertex != NO_ID,
                    "[SurfaceMeshBuilder::update_polygon_vertices] No "
                    "more polygons with vertices to delete should remain at "
                    "this point" );
                if( old_vertex != new_vertex )
                {
                    update_polygon_vertex( id, new_vertex );
                }
            }
        }
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::update_polygon_vertex(
        const PolygonVertex& polygon_vertex, index_t vertex_id )
    {
        check_polygon_id( surface_mesh_, polygon_vertex.polygon_id );
        check_polygon_vertex_id( surface_mesh_, polygon_vertex.polygon_id,
            polygon_vertex.vertex_id );
        OPENGEODE_ASSERT( vertex_id < surface_mesh_.nb_vertices(),
            "[SurfaceMeshBuilder::update_polygon_vertex] Accessing a "
            "vertex that does not exist" );
        associate_polygon_vertex_to_vertex( polygon_vertex, vertex_id );
        reset_polygons_around_vertex( vertex_id );
        do_set_polygon_vertex( polygon_vertex, vertex_id );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::do_delete_vertices(
        const std::vector< bool >& to_delete,
        absl::Span< const index_t > old2new )
    {
        update_polygon_vertices( old2new );
        if( surface_mesh_.are_edges_enabled() )
        {
            edges_builder().update_edge_vertices( old2new );
        }
        do_delete_surface_vertices( to_delete, old2new );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::do_permute_vertices(
        absl::Span< const index_t > permutation,
        absl::Span< const index_t > old2new )
    {
        update_polygon_vertices( old2new );
        if( surface_mesh_.are_edges_enabled() )
        {
            edges_builder().update_edge_vertices( old2new );
        }
        do_permute_surface_vertices( permutation, old2new );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::set_polygon_adjacent(
        const PolygonEdge& polygon_edge, index_t adjacent_id )
    {
        check_polygon_id( surface_mesh_, polygon_edge.polygon_id );
        check_polygon_edge_id(
            surface_mesh_, polygon_edge.polygon_id, polygon_edge.edge_id );
        OPENGEODE_ASSERT( adjacent_id < surface_mesh_.nb_polygons(),
            "[SurfaceMeshBuilder::set_polygon_adjacent] Accessing a "
            "polygon that does not exist" );
        reset_polygons_around_edge_vertices(
            surface_mesh_, *this, polygon_edge );
        do_set_polygon_adjacent( polygon_edge, adjacent_id );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::unset_polygon_adjacent(
        const PolygonEdge& polygon_edge )
    {
        check_polygon_id( surface_mesh_, polygon_edge.polygon_id );
        check_polygon_edge_id(
            surface_mesh_, polygon_edge.polygon_id, polygon_edge.edge_id );
        reset_polygons_around_edge_vertices(
            surface_mesh_, *this, polygon_edge );
        do_unset_polygon_adjacent( polygon_edge );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::compute_polygon_adjacencies()
    {
        std::vector< index_t > polygons_to_connect(
            surface_mesh_.nb_polygons() );
        absl::c_iota( polygons_to_connect, 0 );
        compute_polygon_adjacencies( polygons_to_connect );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::compute_polygon_adjacencies(
        absl::Span< const index_t > polygons_to_connect )
    {
        if( surface_mesh_.are_edges_enabled() )
        {
            const auto& edges = surface_mesh_.edges();
            absl::FixedArray< absl::InlinedVector< PolygonEdge, 2 > >
                polygon_edges_around( edges.nb_edges() );
            for( const auto polygon : polygons_to_connect )
            {
                const auto vertices_id =
                    get_polygon_vertices( surface_mesh_, polygon );
                const local_index_t nb_vertices = vertices_id.size();
                for( const auto e : LRange{ nb_vertices } )
                {
                    PolygonEdge edge{ polygon, e };
                    const auto next = e + 1 == nb_vertices ? 0 : e + 1;
                    const auto edge_id = edges.edge_from_vertices(
                        { vertices_id[e], vertices_id[next] } );
                    polygon_edges_around[edge_id.value()].emplace_back(
                        std::move( edge ) );
                }
            }
            for( const auto& polygon_edges : polygon_edges_around )
            {
                if( polygon_edges.size() != 2 )
                {
                    continue;
                }
                do_set_polygon_adjacent(
                    polygon_edges[0], polygon_edges[1].polygon_id );
                do_set_polygon_adjacent(
                    polygon_edges[1], polygon_edges[0].polygon_id );
            }
        }
        else
        {
            using Edge = detail::VertexCycle< std::array< index_t, 2 > >;
            absl::flat_hash_map< Edge, PolygonEdge > edges;
            for( const auto polygon : polygons_to_connect )
            {
                for( const auto e :
                    LRange{ surface_mesh_.nb_polygon_edges( polygon ) } )
                {
                    const PolygonEdge edge{ polygon, e };
                    if( !surface_mesh_.is_edge_on_border( edge ) )
                    {
                        continue;
                    }
                    auto output = edges.emplace(
                        surface_mesh_.polygon_edge_vertices( edge ), edge );
                    if( !output.second )
                    {
                        auto& adj_edge = output.first->second;
                        do_set_polygon_adjacent( edge, adj_edge.polygon_id );
                        if( surface_mesh_.is_edge_on_border( adj_edge ) )
                        {
                            do_set_polygon_adjacent( adj_edge, polygon );
                        }
                        else
                        {
                            const auto adj_adj_edge =
                                find_polygon_adjacent_edge( surface_mesh_,
                                    adj_edge,
                                    surface_mesh_.polygon_edge_vertices(
                                        edge ) );
                            do_unset_polygon_adjacent( adj_adj_edge.value() );
                        }
                        adj_edge = edge;
                    }
                }
            }
            for( const auto& polygon_edges : edges )
            {
                const auto& polygon_edge = polygon_edges.second;
                if( const auto polygon_adj =
                        non_manifold_polygon_adjacent_edge( surface_mesh_,
                            polygon_edge, polygon_edges.first.vertices() ) )
                {
                    do_unset_polygon_adjacent( polygon_edge );
                    do_unset_polygon_adjacent( polygon_adj.value() );
                }
            }
        }
    }

    template < index_t dimension >
    SurfaceEdgesBuilder< dimension >
        SurfaceMeshBuilder< dimension >::edges_builder()
    {
        return { surface_mesh_.edges( {} ) };
    }

    template < index_t dimension >
    std::vector< index_t > SurfaceMeshBuilder< dimension >::delete_polygons(
        const std::vector< bool >& to_delete )
    {
        const auto old2new = detail::mapping_after_deletion( to_delete );
        if( absl::c_find( to_delete, true ) == to_delete.end() )
        {
            return old2new;
        }
        if( surface_mesh_.are_edges_enabled() )
        {
            auto edges = edges_builder();
            for( const auto p : Range{ surface_mesh_.nb_polygons() } )
            {
                if( to_delete[p] )
                {
                    for( const auto e :
                        LRange{ surface_mesh_.nb_polygon_edges( p ) } )
                    {
                        edges.remove_edge(
                            surface_mesh_.polygon_edge_vertices( { p, e } ) );
                    }
                }
            }
        }
        update_polygon_around( surface_mesh_, *this, old2new );
        update_polygon_adjacencies( surface_mesh_, *this, old2new );
        surface_mesh_.polygon_attribute_manager().delete_elements( to_delete );
        do_delete_polygons( to_delete, old2new );
        return old2new;
    }

    template < index_t dimension >
    std::vector< index_t > SurfaceMeshBuilder< dimension >::permute_polygons(
        absl::Span< const index_t > permutation )
    {
        const auto old2new = old2new_permutation( permutation );
        update_polygon_around( surface_mesh_, *this, old2new );
        update_polygon_adjacencies( surface_mesh_, *this, old2new );
        surface_mesh_.polygon_attribute_manager().permute_elements(
            permutation );
        do_permute_polygons( permutation, old2new );
        return old2new;
    }

    template < index_t dimension >
    std::vector< index_t >
        SurfaceMeshBuilder< dimension >::delete_isolated_vertices()
    {
        std::vector< bool > to_delete( surface_mesh_.nb_vertices(), false );
        for( const auto v : Range{ surface_mesh_.nb_vertices() } )
        {
            to_delete[v] = !surface_mesh_.polygon_around_vertex( v );
        }
        return delete_vertices( to_delete );
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::set_point(
        index_t vertex_id, Point< dimension > point )
    {
        OPENGEODE_ASSERT( vertex_id < surface_mesh_.nb_vertices(),
            "[SurfaceMeshBuilder::set_point] Accessing a vertex that does "
            "not exist" );
        do_set_point( vertex_id, std::move( point ) );
    }

    template < index_t dimension >
    index_t SurfaceMeshBuilder< dimension >::create_point(
        Point< dimension > point )
    {
        const auto added_vertex = surface_mesh_.nb_vertices();
        create_vertex();
        set_point( added_vertex, std::move( point ) );
        return added_vertex;
    }

    template < index_t dimension >
    void SurfaceMeshBuilder< dimension >::copy(
        const SurfaceMesh< dimension >& surface_mesh )
    {
        OPENGEODE_EXCEPTION( surface_mesh_.nb_vertices() == 0
                                 && surface_mesh_.nb_polygons() == 0,
            "[SurfaceMeshBuilder::copy] Cannot copy a mesh into an already "
            "initialized mesh." );
        if( surface_mesh_.are_edges_enabled() )
        {
            OPENGEODE_EXCEPTION( surface_mesh_.edges().nb_edges() == 0,
                "[SurfaceMeshBuilder::copy] Cannot copy a mesh into an already "
                "initialized mesh." );
            surface_mesh_.disable_edges();
        }
        VertexSetBuilder::copy( surface_mesh );
        if( surface_mesh.impl_name() == surface_mesh_.impl_name() )
        {
            do_copy_points( surface_mesh );
            do_copy_polygons( surface_mesh );
        }
        else
        {
            copy_points( surface_mesh, *this );
            copy_polygons( surface_mesh, *this );
        }
        surface_mesh_.polygon_attribute_manager().copy(
            surface_mesh.polygon_attribute_manager() );
        if( surface_mesh.are_edges_enabled() )
        {
            surface_mesh_.copy_edges( surface_mesh, {} );
        }
    }

    template class opengeode_mesh_api SurfaceMeshBuilder< 2 >;
    template class opengeode_mesh_api SurfaceMeshBuilder< 3 >;
} // namespace geode
