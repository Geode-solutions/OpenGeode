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

#include <geode/mesh/core/graph.h>

#include <algorithm>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/vector.h>

#include <geode/mesh/builder/graph_builder.h>
#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/mesh_factory.h>

namespace geode
{
    class Graph::Impl
    {
        static constexpr auto attribute_name = "edges_around_vertex";
        friend class bitsery::Access;

    public:
        explicit Impl( Graph& graph )
            : edges_around_vertex_(
                graph.vertex_attribute_manager()
                    .template find_or_create_attribute< VariableAttribute,
                        EdgesAroundVertex >(
                        attribute_name, EdgesAroundVertex{} ) )
        {
        }

        Impl( Impl&& other ) = default;

        AttributeManager& edge_attribute_manager() const
        {
            return edge_attribute_manager_;
        }

        const EdgesAroundVertex& edges_around_vertex(
            const index_t vertex_id ) const
        {
            return edges_around_vertex_->value( vertex_id );
        }

        void set_edges_around_vertex(
            const index_t vertex_id, EdgesAroundVertex edges ) const
        {
            edges_around_vertex_->set_value( vertex_id, std::move( edges ) );
        }

        void associate_edge_vertex_to_vertex( const Graph& graph,
            const EdgeVertex& edge_vertex,
            const index_t vertex_id )
        {
            disassociate_edge_vertex_to_vertex( graph, edge_vertex );
            const auto& edges = edges_around_vertex_->value( vertex_id );
            const auto it = absl::c_find( edges, edge_vertex );
            if( it == edges.end() )
            {
                edges_around_vertex_->modify_value(
                    vertex_id, [&edge_vertex]( EdgesAroundVertex& value ) {
                        value.push_back( edge_vertex );
                    } );
            }
        }

        void disassociate_edge_vertex_to_vertex(
            const Graph& graph, const EdgeVertex& edge_vertex )
        {
            const auto previous_vertex = graph.edge_vertex( edge_vertex );
            if( previous_vertex < graph.nb_vertices() )
            {
                const auto& edges =
                    edges_around_vertex_->value( previous_vertex );
                const auto it = absl::c_find( edges, edge_vertex );
                if( it != edges.end() )
                {
                    edges_around_vertex_->modify_value(
                        previous_vertex, [&it]( EdgesAroundVertex& value ) {
                            value.erase( it );
                        } );
                }
            }
        }

    private:
        Impl() = default;

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& a, Impl& impl ) {
                    a.object( impl.edge_attribute_manager_ );
                    a.ext( impl.edges_around_vertex_,
                        bitsery::ext::StdSmartPtr{} );
                } );
        }

    private:
        mutable AttributeManager edge_attribute_manager_;
        std::shared_ptr< VariableAttribute< EdgesAroundVertex > >
            edges_around_vertex_;
    };

    template < typename Archive >
    void EdgeVertex::serialize( Archive& archive )
    {
        archive.ext( *this, Growable< Archive, EdgeVertex >{
                                { []( Archive& a, EdgeVertex& edge_vertex ) {
                                     a.value4b( edge_vertex.edge_id );
                                     index_t value{ NO_ID };
                                     a.value4b( value );
                                     edge_vertex.vertex_id = value;
                                 },
                                    []( Archive& a, EdgeVertex& edge_vertex ) {
                                        a.value4b( edge_vertex.edge_id );
                                        a.value1b( edge_vertex.vertex_id );
                                    } } } );
    }

    Graph::Graph() : impl_( *this ) {}

    Graph::Graph( Graph&& other ) : impl_( std::move( other.impl_ ) ) {}

    Graph::~Graph() {} // NOLINT

    std::unique_ptr< Graph > Graph::create()
    {
        return MeshFactory::create_default_mesh< Graph >(
            Graph::type_name_static() );
    }

    std::unique_ptr< Graph > Graph::create( const MeshImpl& impl )
    {
        return MeshFactory::create_mesh< Graph >( impl );
    }

    index_t Graph::edge_vertex( const EdgeVertex& edge_vertex ) const
    {
        OPENGEODE_ASSERT( edge_vertex.edge_id < nb_edges(),
            "[Graph::edge_vertex] Trying to access an invalid edge" );
        OPENGEODE_ASSERT( edge_vertex.vertex_id < 2,
            "[Graph::edge_vertex] Trying to access "
            "an invalid edge local vertex" );
        return get_edge_vertex( edge_vertex );
    }

    std::array< index_t, 2 > Graph::edge_vertices( index_t edge_id ) const
    {
        return { edge_vertex( { edge_id, 0 } ), edge_vertex( { edge_id, 1 } ) };
    }

    index_t Graph::nb_edges() const
    {
        return edge_attribute_manager().nb_elements();
    }

    const EdgesAroundVertex& Graph::edges_around_vertex(
        index_t vertex_id ) const
    {
        OPENGEODE_ASSERT( vertex_id < this->nb_vertices(),
            "[Graph::edges_around_vertex] Accessing an invalid vertex" );
        return impl_->edges_around_vertex( vertex_id );
    }

    bool Graph::is_vertex_isolated( index_t vertex_id ) const
    {
        return edges_around_vertex( vertex_id ).empty();
    }

    absl::optional< index_t > Graph::edge_from_vertices(
        index_t v0, index_t v1 ) const
    {
        for( const auto& edge : edges_around_vertex( v0 ) )
        {
            const local_index_t next = edge.vertex_id == 0 ? 1 : 0;
            if( edge_vertex( { edge.edge_id, next } ) == v1 )
            {
                return edge.edge_id;
            }
        }
        return absl::nullopt;
    }

    void Graph::set_edges_around_vertex(
        index_t vertex_id, EdgesAroundVertex edges, GraphKey )
    {
        OPENGEODE_ASSERT( vertex_id < this->nb_vertices(),
            "[Graph::get_edges_around_vertex] Accessing an invalid vertex" );
        return impl_->set_edges_around_vertex( vertex_id, std::move( edges ) );
    }

    void Graph::associate_edge_vertex_to_vertex(
        const EdgeVertex& edge_vertex, index_t vertex_id, GraphKey )
    {
        impl_->associate_edge_vertex_to_vertex( *this, edge_vertex, vertex_id );
    }

    void Graph::disassociate_edge_vertex_to_vertex(
        const EdgeVertex& edge_vertex, GraphKey )
    {
        impl_->disassociate_edge_vertex_to_vertex( *this, edge_vertex );
    }

    AttributeManager& Graph::edge_attribute_manager() const
    {
        return impl_->edge_attribute_manager();
    }

    template < typename Archive >
    void Graph::serialize( Archive& archive )
    {
        archive.ext( *this, DefaultGrowable< Archive, Graph >{},
            []( Archive& a, Graph& graph ) {
                a.ext( graph, bitsery::ext::BaseClass< VertexSet >{} );
                a.object( graph.impl_ );
            } );
    }

    std::unique_ptr< Graph > Graph::clone() const
    {
        auto clone = create( impl_name() );
        auto builder = GraphBuilder::create( *clone );
        builder->copy( *this );
        return clone;
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, EdgeVertex );

    SERIALIZE_BITSERY_ARCHIVE( opengeode_mesh_api, Graph );
} // namespace geode
