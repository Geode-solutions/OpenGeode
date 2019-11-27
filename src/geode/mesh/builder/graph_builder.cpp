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

#include <geode/mesh/builder/graph_builder.h>

#include <type_traits>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/detail/mapping_after_deletion.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/graph.h>

namespace
{
    using geode::index_t;

    void update_edge_vertices( const geode::Graph& graph,
        geode::GraphBuilder& builder,
        const std::vector< index_t >& old2new )
    {
        std::vector< bool > edges_to_delete( graph.nb_edges(), false );
        for( const auto e : geode::Range{ graph.nb_edges() } )
        {
            for( const auto v : geode::Range{ 2 } )
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
} // namespace

namespace geode
{
    GraphBuilder::GraphBuilder( Graph& graph )
        : VertexSetBuilder( graph ), graph_( graph )
    {
    }

    std::unique_ptr< GraphBuilder > GraphBuilder::create( Graph& mesh )
    {
        try
        {
            return GraphBuilderFactory::create( mesh.type_name(), mesh );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Could not create Graph builder of data structure: ",
                mesh.type_name().get()
            };
        }
    }

    void GraphBuilder::set_edge_vertex(
        const EdgeVertex& edge_vertex, index_t vertex_id )
    {
        OPENGEODE_ASSERT( edge_vertex.edge_id < graph_.nb_edges(),
            "[GraphBuilder::set_edge_vertex] Accessing an edge that does not "
            "exist" );
        OPENGEODE_ASSERT( edge_vertex.vertex_id < 2,
            "[GraphBuilder::set_edge_vertex] Trying to access an invalid edge "
            "local vertex" );
        OPENGEODE_ASSERT( vertex_id < graph_.nb_vertices(),
            "[GraphBuilder::set_edge_vertex] Accessing a vertex that does not "
            "exist" );
        if( graph_.edge_vertex( edge_vertex ) == vertex_id )
        {
            return;
        }
        graph_.associate_edge_vertex_to_vertex( edge_vertex, vertex_id );
        do_set_edge_vertex( edge_vertex, vertex_id );
    }

    index_t GraphBuilder::create_edge()
    {
        const auto added_edge = graph_.nb_edges();
        graph_.edge_attribute_manager().resize( added_edge + 1 );
        do_create_edge();
        return added_edge;
    }

    index_t GraphBuilder::create_edge( index_t v0_id, index_t v1_id )
    {
        OPENGEODE_ASSERT( v0_id != v1_id, "[GraphBuilder::create_edge] "
                                          "Trying to create an edge with "
                                          "same extremities" );
        const auto added_edge = graph_.nb_edges();
        create_edge();
        set_edge_vertex( { added_edge, 0 }, v0_id );
        set_edge_vertex( { added_edge, 1 }, v1_id );
        return added_edge;
    }

    index_t GraphBuilder::create_edges( index_t nb )
    {
        const auto first_added_edge = graph_.nb_edges();
        graph_.edge_attribute_manager().resize( first_added_edge + nb );
        do_create_edges( nb );
        return first_added_edge;
    }

    void GraphBuilder::do_delete_vertices(
        const std::vector< bool >& to_delete )
    {
        const auto old2new = detail::mapping_after_deletion( to_delete );
        update_edge_vertices( graph_, *this, old2new );
        do_delete_curve_vertices( to_delete );
    }

    void GraphBuilder::delete_edges( const std::vector< bool >& to_delete )
    {
        const auto old2new = detail::mapping_after_deletion( to_delete );
        for( const auto v : Range{ graph_.nb_vertices() } )
        {
            const auto& edges = graph_.edges_around_vertex( v );
            std::vector< EdgeVertex > new_edges;
            new_edges.reserve( edges.size() );
            for( const auto& edge : edges )
            {
                const auto edge_id = old2new[edge.edge_id];
                if( edge_id != NO_ID )
                {
                    new_edges.emplace_back( edge_id, edge.vertex_id );
                }
            }
            graph_.set_edges_around_vertex( v, new_edges );
        }

        graph_.edge_attribute_manager().delete_elements( to_delete );
        do_delete_edges( to_delete );
    }

    void GraphBuilder::delete_isolated_vertices()
    {
        std::vector< bool > to_delete( graph_.nb_vertices(), false );
        for( const auto v : Range{ graph_.nb_vertices() } )
        {
            const auto& edge_vertices = graph_.edges_around_vertex( v );
            if( edge_vertices.empty() )
            {
                to_delete[v] = true;
            }
        }
        delete_vertices( to_delete );
    }

    void GraphBuilder::copy( const Graph& graph )
    {
        VertexSetBuilder::copy( graph );
        create_edges( graph.nb_edges() );
        graph_.edge_attribute_manager().copy( graph.edge_attribute_manager() );
        for( const auto e : Range{ graph.nb_edges() } )
        {
            for( const auto v : Range{ 2 } )
            {
                const EdgeVertex id{ e, v };
                set_edge_vertex( id, graph.edge_vertex( id ) );
            }
        }
    }
} // namespace geode
