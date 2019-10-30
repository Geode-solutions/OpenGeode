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

#include <geode/mesh/core/graph.h>

#include <algorithm>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/geometry/vector.h>

#include <geode/mesh/builder/graph_builder.h>
#include <geode/mesh/core/geode_graph.h>

namespace geode
{
    class Graph::Impl
    {
    public:
        explicit Impl( Graph& graph )
            : edges_around_vertex_(
                graph.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        std::vector< EdgeVertex > >( "edges_around_vertex", [] {
                        std::vector< EdgeVertex > edges;
                        edges.reserve( 2 );
                        return edges;
                    }() ) )
        {
        }

        AttributeManager& edge_attribute_manager() const
        {
            return edge_attribute_manager_;
        }

        const std::vector< EdgeVertex >& edges_around_vertex(
            index_t vertex_id ) const
        {
            return edges_around_vertex_->value( vertex_id );
        }

        std::vector< EdgeVertex >& get_edges_around_vertex( index_t vertex_id )
        {
            return edges_around_vertex_->value( vertex_id );
        }

        void associate_edge_vertex_to_vertex( const Graph& graph,
            const EdgeVertex& edge_vertex,
            index_t vertex_id )
        {
            auto previous_vertex = graph.edge_vertex( edge_vertex );
            if( previous_vertex < graph.nb_vertices() )
            {
                auto& edges = edges_around_vertex_->value( previous_vertex );
                auto it = std::find( edges.begin(), edges.end(), edge_vertex );
                if( it != edges.end() )
                {
                    edges.erase( it );
                }
            }
            auto& edges = edges_around_vertex_->value( vertex_id );
            auto it = std::find( edges.begin(), edges.end(), edge_vertex );
            if( it == edges.end() )
            {
                edges.push_back( edge_vertex );
            }
        }

    private:
        friend class bitsery::Access;
        Impl() = default;

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.object( impl.edge_attribute_manager_ );
                    archive.ext( impl.edges_around_vertex_,
                        bitsery::ext::StdSmartPtr{} );
                } );
        }

    private:
        mutable AttributeManager edge_attribute_manager_;
        std::shared_ptr< VariableAttribute< std::vector< EdgeVertex > > >
            edges_around_vertex_;
    };

    Graph::Graph() : impl_( *this ) {}

    Graph::~Graph() {} // NOLINT

    std::unique_ptr< Graph > Graph::create()
    {
        return std::unique_ptr< Graph >{ new OpenGeodeGraph };
    }

    std::unique_ptr< Graph > Graph::create( const MeshType& type )
    {
        try
        {
            return GraphFactory::create( type );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException(
                "Could not create Graph data structure: ", type.get() );
        }
    }

    index_t Graph::edge_vertex( const EdgeVertex& edge_vertex ) const
    {
        OPENGEODE_EXCEPTION( edge_vertex.edge_id < nb_edges(),
            "[Graph::edge_vertex] Trying to access an invalid edge" );
        OPENGEODE_EXCEPTION( edge_vertex.vertex_id < 2,
            "[Graph::edge_vertex] Trying to access "
            "an invalid edge local vertex" );
        return get_edge_vertex( edge_vertex );
    }

    index_t Graph::nb_edges() const
    {
        return edge_attribute_manager().nb_elements();
    }

    const std::vector< EdgeVertex >& Graph::edges_around_vertex(
        index_t vertex_id ) const
    {
        OPENGEODE_EXCEPTION( vertex_id < this->nb_vertices(),
            "[Graph::edges_around_vertex] Accessing an invalid vertex" );
        return impl_->edges_around_vertex( vertex_id );
    }

    std::vector< EdgeVertex >& Graph::get_edges_around_vertex(
        index_t vertex_id )
    {
        OPENGEODE_EXCEPTION( vertex_id < this->nb_vertices(),
            "[Graph::get_edges_around_vertex] Accessing an invalid vertex" );
        return impl_->get_edges_around_vertex( vertex_id );
    }

    void Graph::associate_edge_vertex_to_vertex(
        const EdgeVertex& edge_vertex, index_t vertex_id )
    {
        impl_->associate_edge_vertex_to_vertex( *this, edge_vertex, vertex_id );
    }

    AttributeManager& Graph::edge_attribute_manager() const
    {
        return impl_->edge_attribute_manager();
    }

    template < typename Archive >
    void Graph::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, Graph >{},
            []( Archive& archive, Graph& graph ) {
                archive.ext( graph, bitsery::ext::BaseClass< VertexSet >{} );
                archive.object( graph.impl_ );
            } );
    }

    std::unique_ptr< Graph > Graph::clone() const
    {
        auto clone = create( type_name() );
        auto builder = GraphBuilder::create( *clone );
        builder->copy( *this );
        return clone;
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, Graph );
} // namespace geode
