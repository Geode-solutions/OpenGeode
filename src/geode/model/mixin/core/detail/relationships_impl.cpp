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

#include <geode/model/mixin/core/detail/relationships_impl.h>

#include <fstream>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/bitsery_archive.h>

#include <geode/mesh/builder/graph_builder.h>
#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/io/graph_input.h>
#include <geode/mesh/io/graph_output.h>

#include <geode/model/mixin/core/bitsery_archive.h>

namespace geode
{
    namespace detail
    {
        RelationshipsImpl::RelationshipsImpl() : graph_{ Graph::create() }
        {
            initialize_attributes();
        }

        index_t RelationshipsImpl::nb_components_with_relations() const
        {
            return graph_->nb_vertices();
        }

        index_t RelationshipsImpl::nb_relations(
            const uuid& component_id ) const
        {
            if( const auto index = vertex_id( component_id ) )
            {
                return static_cast< index_t >(
                    graph_->edges_around_vertex( index.value() ).size() );
            }
            return 0;
        }

        RelationshipsImpl::Iterator RelationshipsImpl::begin_edge(
            const uuid& component_id ) const
        {
            if( const auto index = vertex_id( component_id ) )
            {
                return graph_->edges_around_vertex( index.value() ).begin();
            }
            return {};
        }

        RelationshipsImpl::Iterator RelationshipsImpl::end_edge(
            const uuid& component_id ) const
        {
            if( const auto index = vertex_id( component_id ) )
            {
                return graph_->edges_around_vertex( index.value() ).end();
            }
            return {};
        }

        const ComponentID& RelationshipsImpl::graph_component_id(
            index_t graph_component ) const
        {
            return ids_->value( graph_component );
        }

        const ComponentID& RelationshipsImpl::graph_component_id(
            const EdgeVertex& graph_edge_vertex ) const
        {
            return graph_component_id(
                graph_->edge_vertex( graph_edge_vertex ) );
        }

        void RelationshipsImpl::remove_component( const uuid& component_id )
        {
            const auto index = vertex_id( component_id );
            if( !index )
            {
                return;
            }
            uuid2index_.erase( component_id );
            std::vector< bool > to_delete( graph_->nb_edges(), false );
            for( const auto& edge :
                graph_->edges_around_vertex( index.value() ) )
            {
                to_delete[edge.edge_id] = true;
                const auto vertex = graph_->edge_vertex( edge.opposite() );
                if( graph_->edges_around_vertex( vertex ).size() == 1 )
                {
                    uuid2index_.erase( component_from_index( vertex ).id() );
                }
            }
            auto builder = GraphBuilder::create( *graph_ );
            builder->delete_edges( to_delete );
            const auto old2new = builder->delete_isolated_vertices();
            uuid2index_.update( old2new );
        }

        index_t RelationshipsImpl::add_relation_edge(
            const ComponentID& from, const ComponentID& to )
        {
            if( const auto component_id =
                    relation_edge_index( from.id(), to.id() ) )
            {
                Logger::warn( "This relation already exists (", from.string(),
                    " and ", to.string(), ")" );
                return component_id.value();
            }
            const auto index = GraphBuilder::create( *graph_ )->create_edge(
                find_or_create_vertex_id( from ),
                find_or_create_vertex_id( to ) );
            return index;
        }

        void RelationshipsImpl::remove_relation(
            const uuid& component_id1, const uuid& component_id2 )
        {
            if( const auto component_id =
                    relation_edge_index( component_id1, component_id2 ) )
            {
                std::vector< bool > to_delete( graph_->nb_edges(), false );
                to_delete[component_id.value()] = true;
                GraphBuilder::create( *graph_ )->delete_edges( to_delete );
            }
        }

        AttributeManager& RelationshipsImpl::component_attribute_manager() const
        {
            return graph_->vertex_attribute_manager();
        }

        const ComponentID& RelationshipsImpl::component_from_index(
            index_t component_id ) const
        {
            return ids_->value( component_id );
        }

        AttributeManager& RelationshipsImpl::relation_attribute_manager() const
        {
            return graph_->edge_attribute_manager();
        }

        absl::optional< index_t > RelationshipsImpl::relation_edge_index(
            const uuid& component_id1, const uuid& component_id2 ) const
        {
            const auto index1 = vertex_id( component_id1 );
            if( !index1 )
            {
                return absl::nullopt;
            }
            const auto index2 = vertex_id( component_id2 );
            if( !index2 )
            {
                return absl::nullopt;
            }
            return graph_->edge_from_vertices( index1.value(), index2.value() );
        }

        std::tuple< ComponentID, ComponentID >
            RelationshipsImpl::relation_components_from_index(
                index_t component_id ) const
        {
            const auto id0 = graph_->edge_vertex( { component_id, 0 } );
            const auto id1 = graph_->edge_vertex( { component_id, 1 } );
            return std::make_tuple(
                component_from_index( id0 ), component_from_index( id1 ) );
        }

        void RelationshipsImpl::copy(
            const RelationshipsImpl& impl, const ModelCopyMapping& mapping )
        {
            graph_ = impl.graph_->clone();
            initialize_attributes();
            std::vector< index_t > vertices_to_delete;
            for( const auto v : Range{ graph_->nb_vertices() } )
            {
                const auto& component_id = component_from_index( v );
                if( mapping.has_mapping_type( component_id.type() )
                    && mapping.at( component_id.type() )
                           .has_mapping_input( component_id.id() ) )
                {
                    const auto& new_uuid = mapping.at( component_id.type() )
                                               .in2out( component_id.id() );
                    ids_->set_value( v, { component_id.type(), new_uuid } );
                    uuid2index_.set_new_mapping( new_uuid, v );
                }
                else
                {
                    vertices_to_delete.push_back( v );
                }
            }
            if( vertices_to_delete.empty() )
            {
                return;
            }
            std::vector< bool > to_delete( graph_->nb_edges(), false );
            for( const auto v : vertices_to_delete )
            {
                for( const auto& edge : graph_->edges_around_vertex( v ) )
                {
                    to_delete[edge.edge_id] = true;
                }
            }
            auto builder = GraphBuilder::create( *graph_ );
            builder->delete_edges( to_delete );
            const auto old2new = builder->delete_isolated_vertices();
            uuid2index_.update( old2new );
        }

        void RelationshipsImpl::initialize_attributes()
        {
            ids_ =
                graph_->vertex_attribute_manager()
                    .find_or_create_attribute< VariableAttribute, ComponentID >(
                        "id", ComponentID{} );
        }

        absl::optional< index_t > RelationshipsImpl::vertex_id(
            const uuid& component_id ) const
        {
            return uuid2index_.index( component_id );
        }

        const Graph& RelationshipsImpl::graph() const
        {
            return *graph_;
        }

        index_t RelationshipsImpl::register_component(
            const ComponentID& component_id )
        {
            const auto index = GraphBuilder::create( *graph_ )->create_vertex();
            uuid2index_.set_new_mapping( component_id.id(), index );
            ids_->set_value( index, component_id );
            return index;
        }

        index_t RelationshipsImpl::find_or_create_vertex_id(
            const ComponentID& component_id )
        {
            if( const auto index = vertex_id( component_id.id() ) )
            {
                return index.value();
            }
            return register_component( component_id );
        }

        void RelationshipsImpl::delete_isolated_vertices()
        {
            auto builder = GraphBuilder::create( *graph_ );
            const auto old2new = builder->delete_isolated_vertices();
            for( const auto v : Indices{ old2new } )
            {
                if( old2new[v] == NO_ID )
                {
                    uuid2index_.erase( v );
                }
            }
            uuid2index_.update( old2new );
        }
    } // namespace detail
} // namespace geode
