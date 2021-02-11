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

#include <geode/mesh/builder/graph_builder.h>

#include <type_traits>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/detail/mapping_after_deletion.h>

#include <geode/mesh/builder/mesh_builder_factory.h>

namespace
{
    using geode::index_t;

    void update_edge_vertices( const geode::Graph& graph,
        geode::GraphBuilder& builder,
        absl::Span< const index_t > old2new )
    {
        std::vector< bool > edges_to_delete( graph.nb_edges(), false );
        for( const auto e : geode::Range{ graph.nb_edges() } )
        {
            for( const auto v : geode::LRange{ 2 } )
            {
                const geode::EdgeVertex id{ e, v };
                const auto new_vertex = old2new[graph.edge_vertex( id )];
                if( new_vertex == geode::NO_ID )
                {
                    edges_to_delete[e] = true;
                }
                else
                {
                    builder.set_edge_vertex( id, new_vertex );
                }
            }
        }
        builder.delete_edges( edges_to_delete );
    }

    void update_edges_around( const geode::Graph& graph,
        geode::GraphBuilder& builder,
        absl::Span< const index_t > old2new )
    {
        for( const auto v : geode::Range{ graph.nb_vertices() } )
        {
            const auto& edges = graph.edges_around_vertex( v );
            geode::EdgesAroundVertex new_edges;
            new_edges.reserve( edges.size() );
            for( const auto& edge : edges )
            {
                const auto edge_id = old2new[edge.edge_id];
                if( edge_id != geode::NO_ID )
                {
                    new_edges.emplace_back( edge_id, edge.vertex_id );
                }
            }
            builder.set_edges_around_vertex( v, std::move( new_edges ) );
        }
    }
} // namespace

namespace geode
{
    GraphBuilder::GraphBuilder( VertexSet& vertex_set )
        : VertexSetBuilder( vertex_set )
    {
    }

    void GraphBuilder::set_mesh( Graph& mesh, MeshBuilderFactoryKey key )
    {
        graph_ = &mesh;
        VertexSetBuilder::set_mesh( mesh, key );
    }

    std::unique_ptr< GraphBuilder > GraphBuilder::create( Graph& mesh )
    {
        return MeshBuilderFactory::create_mesh_builder< GraphBuilder >( mesh );
    }

    void GraphBuilder::set_edge_vertex(
        const EdgeVertex& edge_vertex, index_t vertex_id )
    {
        OPENGEODE_ASSERT( edge_vertex.edge_id < graph_->nb_edges(),
            "[GraphBuilder::set_edge_vertex] Accessing an edge that does not "
            "exist" );
        OPENGEODE_ASSERT( edge_vertex.vertex_id < 2,
            "[GraphBuilder::set_edge_vertex] Trying to access an invalid edge "
            "local vertex" );
        OPENGEODE_ASSERT( vertex_id < graph_->nb_vertices(),
            "[GraphBuilder::set_edge_vertex] Accessing a vertex that does not "
            "exist" );
        if( graph_->edge_vertex( edge_vertex ) == vertex_id )
        {
            return;
        }
        associate_edge_vertex_to_vertex( edge_vertex, vertex_id );
        do_set_edge_vertex( edge_vertex, vertex_id );
    }

    void GraphBuilder::associate_edge_vertex_to_vertex(
        const EdgeVertex& edge_vertex, index_t vertex_id )
    {
        graph_->associate_edge_vertex_to_vertex( edge_vertex, vertex_id, {} );
    }

    void GraphBuilder::disassociate_edge_vertex_to_vertex(
        const EdgeVertex& edge_vertex )
    {
        graph_->disassociate_edge_vertex_to_vertex( edge_vertex, {} );
    }

    index_t GraphBuilder::create_edge()
    {
        const auto added_edge = graph_->nb_edges();
        graph_->edge_attribute_manager().resize( added_edge + 1 );
        do_create_edge();
        return added_edge;
    }

    index_t GraphBuilder::create_edge( index_t v0_id, index_t v1_id )
    {
        OPENGEODE_ASSERT( v0_id != v1_id, "[GraphBuilder::create_edge] "
                                          "Trying to create an edge with "
                                          "same extremities" );
        const auto added_edge = graph_->nb_edges();
        create_edge();
        set_edge_vertex( { added_edge, 0 }, v0_id );
        set_edge_vertex( { added_edge, 1 }, v1_id );
        return added_edge;
    }

    index_t GraphBuilder::create_edges( index_t nb )
    {
        const auto first_added_edge = graph_->nb_edges();
        graph_->edge_attribute_manager().resize( first_added_edge + nb );
        do_create_edges( nb );
        return first_added_edge;
    }

    void GraphBuilder::do_delete_vertices( const std::vector< bool >& to_delete,
        absl::Span< const index_t > old2new )
    {
        update_edge_vertices( *graph_, *this, old2new );
        do_delete_curve_vertices( to_delete, old2new );
    }

    void GraphBuilder::do_permute_vertices(
        absl::Span< const index_t > permutation,
        absl::Span< const index_t > old2new )
    {
        update_edge_vertices( *graph_, *this, old2new );
        do_permute_curve_vertices( permutation, old2new );
    }

    void GraphBuilder::set_edges_around_vertex(
        index_t vertex_id, EdgesAroundVertex edges )
    {
        graph_->set_edges_around_vertex( vertex_id, std::move( edges ), {} );
    }

    std::vector< index_t > GraphBuilder::delete_edges(
        const std::vector< bool >& to_delete )
    {
        const auto old2new = detail::mapping_after_deletion( to_delete );
        if( absl::c_find( to_delete, true ) == to_delete.end() )
        {
            return old2new;
        }
        update_edges_around( *graph_, *this, old2new );
        graph_->edge_attribute_manager().delete_elements( to_delete );
        do_delete_edges( to_delete, old2new );
        return old2new;
    }

    std::vector< index_t > GraphBuilder::permute_edges(
        absl::Span< const index_t > permutation )
    {
        const auto old2new = old2new_permutation( permutation );
        update_edges_around( *graph_, *this, old2new );
        graph_->edge_attribute_manager().permute_elements( permutation );
        do_permute_edges( permutation, old2new );
        return old2new;
    }

    std::vector< index_t > GraphBuilder::delete_isolated_vertices()
    {
        std::vector< bool > to_delete( graph_->nb_vertices(), false );
        for( const auto v : Range{ graph_->nb_vertices() } )
        {
            const auto& edge_vertices = graph_->edges_around_vertex( v );
            if( edge_vertices.empty() )
            {
                to_delete[v] = true;
            }
        }
        return delete_vertices( to_delete );
    }

    void GraphBuilder::copy( const Graph& graph )
    {
        VertexSetBuilder::copy( graph );
        create_edges( graph.nb_edges() );
        graph_->edge_attribute_manager().copy( graph.edge_attribute_manager() );
        for( const auto e : Range{ graph.nb_edges() } )
        {
            for( const auto v : LRange{ 2 } )
            {
                const EdgeVertex id{ e, v };
                set_edge_vertex( id, graph.edge_vertex( id ) );
            }
        }
    }
} // namespace geode
