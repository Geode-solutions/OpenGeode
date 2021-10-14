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

#include <geode/model/mixin/core/relationships.h>

#include <fstream>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/bitsery_archive.h>

#include <geode/mesh/builder/geode_graph_builder.h>
#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/core/geode_graph.h>
#include <geode/mesh/io/graph_input.h>
#include <geode/mesh/io/graph_output.h>

#include <geode/model/mixin/core/bitsery_archive.h>
#include <geode/model/mixin/core/detail/count_relationships.h>
#include <geode/model/mixin/core/detail/uuid_to_index.h>
#include <geode/model/representation/builder/copy_mapping.h>

namespace geode
{
    class Relationships::Impl
    {
    public:
        using Iterator = typename EdgesAroundVertex::const_iterator;
        using RelationType = index_t;
        static constexpr index_t BOUNDARY_RELATION = index_t( 0 );
        static constexpr index_t INTERNAL_RELATION = index_t( 1 );
        static constexpr index_t ITEM_RELATION = index_t( 2 );
        static constexpr index_t BOUNDARY_EDGE_VERTEX = index_t( 0 );
        static constexpr index_t INCIDENT_EDGE_VERTEX = index_t( 1 );
        static constexpr index_t ITEM_EDGE_VERTEX = index_t( 0 );
        static constexpr index_t COLLECTION_EDGE_VERTEX = index_t( 1 );
        static constexpr index_t INTERNAL_EDGE_VERTEX = index_t( 0 );
        static constexpr index_t EMBEDDING_EDGE_VERTEX = index_t( 1 );

        Impl() : graph_{ Graph::create() }
        {
            initialize_attributes();
        }

        index_t nb_relations( const uuid& id ) const
        {
            return static_cast< index_t >(
                graph_->edges_around_vertex( vertex_id( id ) ).size() );
        }

        RelationType relation_type( const index_t edge_id ) const
        {
            return relation_type_->value( edge_id );
        }

        bool is_boundary_relation( const index_t edge_id ) const
        {
            return relation_type_->value( edge_id ) == BOUNDARY_RELATION;
        }

        bool is_internal_relation( const index_t edge_id ) const
        {
            return relation_type_->value( edge_id ) == INTERNAL_RELATION;
        }

        bool is_item_relation( const index_t edge_id ) const
        {
            return relation_type_->value( edge_id ) == ITEM_RELATION;
        }

        Iterator begin_edge( const index_t id ) const
        {
            return graph_->edges_around_vertex( id ).begin();
        }

        Iterator end_edge( const index_t id ) const
        {
            return graph_->edges_around_vertex( id ).end();
        }

        Iterator begin_edge( const uuid& id ) const
        {
            return begin_edge( vertex_id( id ) );
        }

        Iterator end_edge( const uuid& id ) const
        {
            return end_edge( vertex_id( id ) );
        }

        const ComponentID& vertex_component_id(
            const EdgeVertex& edge_vertex ) const
        {
            return ids_->value( graph_->edge_vertex( edge_vertex ) );
        }

        void register_component( const ComponentID& id )
        {
            const auto index = GraphBuilder::create( *graph_ )->create_vertex();
            uuid2index_.set_new_mapping( id.id(), index );
            ids_->set_value( index, id );
        }

        void unregister_component( const uuid& id )
        {
            const auto index = vertex_id( id );
            std::vector< bool > to_delete( graph_->nb_vertices(), false );
            to_delete[index] = true;
            GraphBuilder::create( *graph_ )->delete_vertices( to_delete );
            uuid2index_.erase( id );
            uuid2index_.decrement_indices_larger_than( index );
        }

        absl::optional< index_t > check_relation_exists(
            const uuid& from, const uuid& to, const RelationType type ) const
        {
            for( const auto& edge_vertex :
                graph_->edges_around_vertex( vertex_id( from ) ) )
            {
                if( relation_type( edge_vertex.edge_id ) != type )
                {
                    continue;
                }
                const auto& other =
                    this->vertex_component_id( { edge_vertex.edge_id,
                        static_cast< local_index_t >(
                            ( edge_vertex.vertex_id + 1 ) % 2 ) } );
                if( to == other.id() )
                {
                    return edge_vertex.edge_id;
                }
            }
            return absl::nullopt;
        }

        index_t add_relation(
            const uuid& from, const uuid& to, const RelationType type )
        {
            if( const auto id = check_relation_exists( from, to, type ) )
            {
                Logger::warn( "This relation already exists (",
                    ids_->value( vertex_id( from ) ).type().get(), " and ",
                    ids_->value( vertex_id( to ) ).type().get(), ")" );
                return id.value();
            }
            const auto index = do_add_relation( from, to );
            relation_type_->set_value( index, type );
            return index;
        }

        index_t add_relation( const uuid& from, const uuid& to )
        {
            if( const auto id = relation_index( from, to ) )
            {
                Logger::warn( "This relation already exists (",
                    ids_->value( vertex_id( from ) ).type().get(), " and ",
                    ids_->value( vertex_id( to ) ).type().get(), ")" );
                return id.value();
            }
            return do_add_relation( from, to );
        }

        void remove_relation( const uuid& id1, const uuid& id2 )
        {
            if( const auto id = relation_index( id1, id2 ) )
            {
                std::vector< bool > to_delete( graph_->nb_edges(), false );
                to_delete[id.value()] = true;
                GraphBuilder::create( *graph_ )->delete_edges( to_delete );
            }
        }

        void save( absl::string_view directory ) const
        {
            const auto filename = absl::StrCat( directory, "/relationships" );
            std::ofstream file{ filename, std::ofstream::binary };
            TContext context{};
            register_basic_serialize_pcontext( std::get< 0 >( context ) );
            register_geometry_serialize_pcontext( std::get< 0 >( context ) );
            register_mesh_serialize_pcontext( std::get< 0 >( context ) );
            register_model_serialize_pcontext( std::get< 0 >( context ) );
            Serializer archive{ context, file };
            archive.object( *this );
            archive.adapter().flush();
            OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
                "[Relationships::save] Error while writing file: ", filename );
        }

        void load( absl::string_view directory )
        {
            const auto filename = absl::StrCat( directory, "/relationships" );
            std::ifstream file{ filename, std::ifstream::binary };
            TContext context{};
            register_basic_deserialize_pcontext( std::get< 0 >( context ) );
            register_geometry_deserialize_pcontext( std::get< 0 >( context ) );
            register_mesh_deserialize_pcontext( std::get< 0 >( context ) );
            register_model_deserialize_pcontext( std::get< 0 >( context ) );
            Deserializer archive{ context, file };
            archive.object( *this );
            const auto& adapter = archive.adapter();
            OPENGEODE_EXCEPTION(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                "[Relationships::load] Error while reading file: ", filename );
        }

        AttributeManager& component_attribute_manager() const
        {
            return graph_->vertex_attribute_manager();
        }

        index_t component_index( const uuid& id ) const
        {
            return vertex_id( id );
        }

        const ComponentID& component_from_index( index_t id ) const
        {
            return ids_->value( id );
        }

        AttributeManager& relation_attribute_manager() const
        {
            return graph_->edge_attribute_manager();
        }

        absl::optional< index_t > relation_index(
            const uuid& id1, const uuid& id2 ) const
        {
            const auto v0 = component_index( id1 );
            const auto v1 = component_index( id2 );
            return graph_->edge_from_vertices( v0, v1 );
        }

        std::tuple< ComponentID, ComponentID > relation_from_index(
            index_t id ) const
        {
            const auto id0 = graph_->edge_vertex( { id, 0 } );
            const auto id1 = graph_->edge_vertex( { id, 1 } );
            return std::make_tuple(
                component_from_index( id0 ), component_from_index( id1 ) );
        }

        void copy( const Impl& impl, const ModelCopyMapping& mapping )
        {
            graph_ = impl.graph_->clone();
            initialize_attributes();
            uuid2index_ = impl.uuid2index_;
            bool delete_required{ false };
            index_t offset{ 0 };
            std::vector< bool > to_delete( graph_->nb_vertices(), false );
            for( const auto v : Range{ graph_->nb_vertices() } )
            {
                const auto& id = component_from_index( v );
                uuid2index_.erase( id.id() );
                if( mapping.has_mapping_type( id.type() )
                    && mapping.at( id.type() ).has_mapping_input( id.id() ) )
                {
                    const auto& new_uuid =
                        mapping.at( id.type() ).in2out( id.id() );
                    ids_->set_value( v, { id.type(), new_uuid } );
                    uuid2index_.set_new_mapping( new_uuid, v - offset );
                }
                else
                {
                    offset++;
                    delete_required = true;
                    to_delete[v] = true;
                }
            }
            if( delete_required )
            {
                GraphBuilder::create( *graph_ )->delete_vertices( to_delete );
            }
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& a, Impl& impl ) {
                         OpenGeodeGraph graph;
                         a.object( graph );
                         a.object( impl.uuid2index_ );
                         a.ext(
                             impl.relation_type_, bitsery::ext::StdSmartPtr{} );
                         a.ext( impl.ids_, bitsery::ext::StdSmartPtr{} );
                         impl.graph_ = graph.clone();
                         impl.initialize_attributes();
                     },
                        []( Archive& a, Impl& impl ) {
                            a.ext( impl.graph_, bitsery::ext::StdSmartPtr{} );
                            a.object( impl.uuid2index_ );
                            a.ext( impl.relation_type_,
                                bitsery::ext::StdSmartPtr{} );
                            a.ext( impl.ids_, bitsery::ext::StdSmartPtr{} );
                        } } } );
        }

        index_t do_add_relation( const uuid& from, const uuid& to )
        {
            const auto index = GraphBuilder::create( *graph_ )->create_edge(
                vertex_id( from ), vertex_id( to ) );
            return index;
        }

        index_t vertex_id( const uuid& id ) const
        {
            return uuid2index_.index( id );
        }

        void initialize_attributes()
        {
            relation_type_ = graph_->edge_attribute_manager()
                                 .find_or_create_attribute< VariableAttribute,
                                     RelationType >( "relation_type", NO_ID );
            ids_ =
                graph_->vertex_attribute_manager()
                    .find_or_create_attribute< VariableAttribute, ComponentID >(
                        "id", ComponentID{} );
        }

    private:
        std::unique_ptr< Graph > graph_;
        detail::UuidToIndex uuid2index_;
        std::shared_ptr< VariableAttribute< RelationType > > relation_type_;
        std::shared_ptr< VariableAttribute< ComponentID > > ids_;
    };

    Relationships::Relationships() {} // NOLINT
    Relationships::Relationships( Relationships&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }
    Relationships::~Relationships() {} // NOLINT

    void Relationships::register_component(
        const ComponentID& id, RelationshipsBuilderKey )
    {
        impl_->register_component( id );
    }

    void Relationships::unregister_component(
        const uuid& id, RelationshipsBuilderKey )
    {
        impl_->unregister_component( id );
    }

    index_t Relationships::nb_relations( const uuid& id ) const
    {
        return impl_->nb_relations( id );
    }

    Relationships::RelationRange Relationships::relations(
        const uuid& id ) const
    {
        return { *this, id };
    }

    index_t Relationships::nb_boundaries( const uuid& id ) const
    {
        return detail::count_relationships( boundaries( id ) );
    }

    Relationships::BoundaryRange Relationships::boundaries(
        const uuid& id ) const
    {
        return { *this, id };
    }

    index_t Relationships::nb_incidences( const uuid& id ) const
    {
        return detail::count_relationships( incidences( id ) );
    }

    Relationships::IncidenceRange Relationships::incidences(
        const uuid& id ) const
    {
        return { *this, id };
    }

    index_t Relationships::add_boundary_relation(
        const uuid& boundary, const uuid& incidence, RelationshipsBuilderKey )
    {
        return impl_->add_relation(
            boundary, incidence, Relationships::Impl::BOUNDARY_RELATION );
    }

    index_t Relationships::nb_internals( const uuid& id ) const
    {
        return detail::count_relationships( internals( id ) );
    }

    Relationships::InternalRange Relationships::internals(
        const uuid& id ) const
    {
        return { *this, id };
    }

    index_t Relationships::nb_embeddings( const uuid& id ) const
    {
        return detail::count_relationships( embeddings( id ) );
    }

    Relationships::EmbeddingRange Relationships::embeddings(
        const uuid& id ) const
    {
        return { *this, id };
    }

    index_t Relationships::add_internal_relation(
        const uuid& internal, const uuid& embedding, RelationshipsBuilderKey )
    {
        return impl_->add_relation(
            internal, embedding, Relationships::Impl::INTERNAL_RELATION );
    }

    index_t Relationships::nb_items( const uuid& id ) const
    {
        return detail::count_relationships( items( id ) );
    }

    Relationships::ItemRange Relationships::items( const uuid& id ) const
    {
        return { *this, id };
    }

    index_t Relationships::nb_collections( const uuid& id ) const
    {
        return detail::count_relationships( collections( id ) );
    }

    Relationships::CollectionRange Relationships::collections(
        const uuid& id ) const
    {
        return { *this, id };
    }

    index_t Relationships::add_item_in_collection(
        const uuid& item, const uuid& collection, RelationshipsBuilderKey )
    {
        return impl_->add_relation(
            item, collection, Relationships::Impl::ITEM_RELATION );
    }

    index_t Relationships::add_relation(
        const uuid& id1, const uuid& id2, RelationshipsBuilderKey )
    {
        return impl_->add_relation( id1, id2 );
    }

    void Relationships::remove_relation(
        const uuid& id1, const uuid& id2, RelationshipsBuilderKey )
    {
        return impl_->remove_relation( id1, id2 );
    }

    bool Relationships::is_boundary(
        const uuid& boundary, const uuid& incidence ) const
    {
        return impl_
            ->check_relation_exists(
                boundary, incidence, Relationships::Impl::BOUNDARY_RELATION )
            .has_value();
    }

    bool Relationships::is_internal(
        const uuid& internal, const uuid& embedding ) const
    {
        return impl_
            ->check_relation_exists(
                internal, embedding, Relationships::Impl::INTERNAL_RELATION )
            .has_value();
    }

    bool Relationships::is_item(
        const uuid& item, const uuid& collection ) const
    {
        return impl_
            ->check_relation_exists(
                item, collection, Relationships::Impl::ITEM_RELATION )
            .has_value();
    }

    void Relationships::save_relationships( absl::string_view directory ) const
    {
        impl_->save( directory );
    }

    void Relationships::copy_relationships( const ModelCopyMapping& mapping,
        const Relationships& relationships,
        RelationshipsBuilderKey )
    {
        impl_->copy( *relationships.impl_, mapping );
    }

    void Relationships::load_relationships(
        absl::string_view directory, RelationshipsBuilderKey )
    {
        return impl_->load( directory );
    }

    AttributeManager& Relationships::component_attribute_manager() const
    {
        return impl_->component_attribute_manager();
    }

    index_t Relationships::component_index( const uuid& id ) const
    {
        return impl_->component_index( id );
    }

    const ComponentID& Relationships::component_from_index( index_t id ) const
    {
        return impl_->component_from_index( id );
    }

    AttributeManager& Relationships::relation_attribute_manager() const
    {
        return impl_->relation_attribute_manager();
    }

    absl::optional< index_t > Relationships::relation_index(
        const uuid& id1, const uuid& id2 ) const
    {
        return impl_->relation_index( id1, id2 );
    }

    std::tuple< ComponentID, ComponentID > Relationships::relation_from_index(
        index_t id ) const
    {
        return impl_->relation_from_index( id );
    }

    class Relationships::RelationRangeIterator::Impl
        : public BaseRange< typename Relationships::Impl::Iterator >
    {
        using Iterator = typename Relationships::Impl::Iterator;

    public:
        Impl( const Relationships::Impl& relationships,
            Iterator begin,
            Iterator end )
            : BaseRange< Iterator >( begin, end ),
              relationships_( relationships )
        {
        }

        void next()
        {
            this->operator++();
        }

        const ComponentID& vertex_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.vertex_component_id(
                { iterator->edge_id, static_cast< local_index_t >(
                                         ( iterator->vertex_id + 1 ) % 2 ) } );
        }

    private:
        const Relationships::Impl& relationships_;
    };

    Relationships::RelationRangeIterator::RelationRangeIterator(
        const Relationships& relationships, const uuid& id )
        : impl_( *relationships.impl_,
            relationships.impl_->begin_edge( id ),
            relationships.impl_->end_edge( id ) )
    {
    }

    Relationships::RelationRangeIterator::RelationRangeIterator(
        RelationRangeIterator&& other ) noexcept
        : impl_( *other.impl_ )
    {
    }

    Relationships::RelationRangeIterator::RelationRangeIterator(
        const RelationRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::RelationRangeIterator::~RelationRangeIterator() {} // NOLINT

    bool Relationships::RelationRangeIterator::operator!=(
        const RelationRangeIterator& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    void Relationships::RelationRangeIterator::operator++()
    {
        impl_->next();
    }

    const ComponentID& Relationships::RelationRangeIterator::operator*() const
    {
        return impl_->vertex_component_id();
    }

    class Relationships::BoundaryRangeIterator::Impl
        : public BaseRange< typename Relationships::Impl::Iterator >
    {
        using Iterator = typename Relationships::Impl::Iterator;

    public:
        Impl( const Relationships::Impl& relationships,
            Iterator begin,
            Iterator end )
            : BaseRange< Iterator >( begin, end ),
              relationships_( relationships )
        {
            next_boundary_iterator();
        }

        void next()
        {
            this->operator++();
            next_boundary_iterator();
        }

        const ComponentID& vertex_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.vertex_component_id(
                { iterator->edge_id, static_cast< local_index_t >(
                                         ( iterator->vertex_id + 1 ) % 2 ) } );
        }

    private:
        bool is_boundary_edge_vertex()
        {
            return this->current()->vertex_id
                   == Relationships::Impl::BOUNDARY_EDGE_VERTEX;
        }

        void next_boundary_iterator()
        {
            while( this->operator!=( *this )
                   && ( !relationships_.is_boundary_relation(
                            this->current()->edge_id )
                        || is_boundary_edge_vertex() ) )
            {
                this->operator++();
            }
        }

    private:
        const Relationships::Impl& relationships_;
    };

    Relationships::BoundaryRangeIterator::BoundaryRangeIterator(
        const Relationships& relationships, const uuid& id )
        : impl_( *relationships.impl_,
            relationships.impl_->begin_edge( id ),
            relationships.impl_->end_edge( id ) )
    {
    }

    Relationships::BoundaryRangeIterator::BoundaryRangeIterator(
        BoundaryRangeIterator&& other ) noexcept
        : impl_( *other.impl_ )
    {
    }

    Relationships::BoundaryRangeIterator::BoundaryRangeIterator(
        const BoundaryRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::BoundaryRangeIterator::~BoundaryRangeIterator() {} // NOLINT

    bool Relationships::BoundaryRangeIterator::operator!=(
        const BoundaryRangeIterator& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    void Relationships::BoundaryRangeIterator::operator++()
    {
        impl_->next();
    }

    const ComponentID& Relationships::BoundaryRangeIterator::operator*() const
    {
        return impl_->vertex_component_id();
    }

    class Relationships::IncidenceRangeIterator::Impl
        : public BaseRange< typename Relationships::Impl::Iterator >
    {
        using Iterator = typename Relationships::Impl::Iterator;

    public:
        Impl( const Relationships::Impl& relationships,
            Iterator begin,
            Iterator end )
            : BaseRange< Iterator >( begin, end ),
              relationships_( relationships )
        {
            next_incidence_iterator();
        }

        void next()
        {
            this->operator++();
            next_incidence_iterator();
        }

        const ComponentID& vertex_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.vertex_component_id(
                { iterator->edge_id, static_cast< local_index_t >(
                                         ( iterator->vertex_id + 1 ) % 2 ) } );
        }

    private:
        bool is_incident_edge_vertex()
        {
            return this->current()->vertex_id
                   == Relationships::Impl::INCIDENT_EDGE_VERTEX;
        }

        void next_incidence_iterator()
        {
            while( this->operator!=( *this )
                   && ( !relationships_.is_boundary_relation(
                            this->current()->edge_id )
                        || is_incident_edge_vertex() ) )
            {
                this->operator++();
            }
        }

    private:
        const Relationships::Impl& relationships_;
    };

    Relationships::IncidenceRangeIterator::IncidenceRangeIterator(
        const Relationships& relationships, const uuid& id )
        : impl_( *relationships.impl_,
            relationships.impl_->begin_edge( id ),
            relationships.impl_->end_edge( id ) )
    {
    }

    Relationships::IncidenceRangeIterator::IncidenceRangeIterator(
        IncidenceRangeIterator&& other ) noexcept
        : impl_( *other.impl_ )
    {
    }

    Relationships::IncidenceRangeIterator::IncidenceRangeIterator(
        const IncidenceRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::IncidenceRangeIterator::~IncidenceRangeIterator() {
    } // NOLINT

    bool Relationships::IncidenceRangeIterator::operator!=(
        const IncidenceRangeIterator& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    void Relationships::IncidenceRangeIterator::operator++()
    {
        impl_->next();
    }

    const ComponentID& Relationships::IncidenceRangeIterator::operator*() const
    {
        return impl_->vertex_component_id();
    }

    class Relationships::InternalRangeIterator::Impl
        : public BaseRange< typename Relationships::Impl::Iterator >
    {
        using Iterator = typename Relationships::Impl::Iterator;

    public:
        Impl( const Relationships::Impl& relationships,
            Iterator begin,
            Iterator end )
            : BaseRange< Iterator >( begin, end ),
              relationships_( relationships )
        {
            next_internal_iterator();
        }

        void next()
        {
            this->operator++();
            next_internal_iterator();
        }

        const ComponentID& vertex_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.vertex_component_id(
                { iterator->edge_id, static_cast< local_index_t >(
                                         ( iterator->vertex_id + 1 ) % 2 ) } );
        }

    private:
        bool is_internal_edge_vertex()
        {
            return this->current()->vertex_id
                   == Relationships::Impl::INTERNAL_EDGE_VERTEX;
        }

        void next_internal_iterator()
        {
            while( this->operator!=( *this )
                   && ( !relationships_.is_internal_relation(
                            this->current()->edge_id )
                        || is_internal_edge_vertex() ) )
            {
                this->operator++();
            }
        }

    private:
        const Relationships::Impl& relationships_;
    };

    Relationships::InternalRangeIterator::InternalRangeIterator(
        const Relationships& relationships, const uuid& id )
        : impl_( *relationships.impl_,
            relationships.impl_->begin_edge( id ),
            relationships.impl_->end_edge( id ) )
    {
    }

    Relationships::InternalRangeIterator::InternalRangeIterator(
        InternalRangeIterator&& other ) noexcept
        : impl_( *other.impl_ )
    {
    }

    Relationships::InternalRangeIterator::InternalRangeIterator(
        const InternalRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::InternalRangeIterator::~InternalRangeIterator() {} // NOLINT

    bool Relationships::InternalRangeIterator::operator!=(
        const InternalRangeIterator& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    void Relationships::InternalRangeIterator::operator++()
    {
        impl_->next();
    }

    const ComponentID& Relationships::InternalRangeIterator::operator*() const
    {
        return impl_->vertex_component_id();
    }

    class Relationships::EmbeddingRangeIterator::Impl
        : public BaseRange< typename Relationships::Impl::Iterator >
    {
        using Iterator = typename Relationships::Impl::Iterator;

    public:
        Impl( const Relationships::Impl& relationships,
            Iterator begin,
            Iterator end )
            : BaseRange< Iterator >( begin, end ),
              relationships_( relationships )
        {
            next_embedding_iterator();
        }

        void next()
        {
            this->operator++();
            next_embedding_iterator();
        }

        const ComponentID& vertex_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.vertex_component_id(
                { iterator->edge_id, static_cast< local_index_t >(
                                         ( iterator->vertex_id + 1 ) % 2 ) } );
        }

    private:
        bool is_embedding_edge_vertex()
        {
            return this->current()->vertex_id
                   == Relationships::Impl::EMBEDDING_EDGE_VERTEX;
        }

        void next_embedding_iterator()
        {
            while( this->operator!=( *this )
                   && ( !relationships_.is_internal_relation(
                            this->current()->edge_id )
                        || is_embedding_edge_vertex() ) )
            {
                this->operator++();
            }
        }

    private:
        const Relationships::Impl& relationships_;
    };

    Relationships::EmbeddingRangeIterator::EmbeddingRangeIterator(
        const Relationships& relationships, const uuid& id )
        : impl_( *relationships.impl_,
            relationships.impl_->begin_edge( id ),
            relationships.impl_->end_edge( id ) )
    {
    }

    Relationships::EmbeddingRangeIterator::EmbeddingRangeIterator(
        EmbeddingRangeIterator&& other ) noexcept
        : impl_( *other.impl_ )
    {
    }

    Relationships::EmbeddingRangeIterator::EmbeddingRangeIterator(
        const EmbeddingRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::EmbeddingRangeIterator::~EmbeddingRangeIterator() {
    } // NOLINT

    bool Relationships::EmbeddingRangeIterator::operator!=(
        const EmbeddingRangeIterator& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    void Relationships::EmbeddingRangeIterator::operator++()
    {
        impl_->next();
    }

    const ComponentID& Relationships::EmbeddingRangeIterator::operator*() const
    {
        return impl_->vertex_component_id();
    }

    class Relationships::ItemRangeIterator::Impl
        : public BaseRange< typename Relationships::Impl::Iterator >
    {
        using Iterator = typename Relationships::Impl::Iterator;

    public:
        Impl( const Relationships::Impl& relationships,
            Iterator begin,
            Iterator end )
            : BaseRange< Iterator >( begin, end ),
              relationships_( relationships )
        {
            next_item_iterator();
        }

        void next()
        {
            this->operator++();
            next_item_iterator();
        }

        const ComponentID& vertex_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.vertex_component_id(
                { iterator->edge_id, static_cast< local_index_t >(
                                         ( iterator->vertex_id + 1 ) % 2 ) } );
        }

    private:
        bool is_item_edge_vertex()
        {
            return this->current()->vertex_id
                   == Relationships::Impl::ITEM_EDGE_VERTEX;
        }

        void next_item_iterator()
        {
            while( this->operator!=( *this )
                   && ( !relationships_.is_item_relation(
                            this->current()->edge_id )
                        || is_item_edge_vertex() ) )
            {
                this->operator++();
            }
        }

    private:
        const Relationships::Impl& relationships_;
    };

    Relationships::ItemRangeIterator::ItemRangeIterator(
        const Relationships& relationships, const uuid& id )
        : impl_( *relationships.impl_,
            relationships.impl_->begin_edge( id ),
            relationships.impl_->end_edge( id ) )
    {
    }

    Relationships::ItemRangeIterator::ItemRangeIterator(
        ItemRangeIterator&& other ) noexcept
        : impl_( *other.impl_ )
    {
    }

    Relationships::ItemRangeIterator::ItemRangeIterator(
        const ItemRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::ItemRangeIterator::~ItemRangeIterator() {} // NOLINT

    bool Relationships::ItemRangeIterator::operator!=(
        const ItemRangeIterator& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    void Relationships::ItemRangeIterator::operator++()
    {
        impl_->next();
    }

    const ComponentID& Relationships::ItemRangeIterator::operator*() const
    {
        return impl_->vertex_component_id();
    }

    class Relationships::CollectionRangeIterator::Impl
        : public BaseRange< typename Relationships::Impl::Iterator >
    {
        using Iterator = typename Relationships::Impl::Iterator;

    public:
        Impl( const Relationships::Impl& relationships,
            Iterator begin,
            Iterator end )
            : BaseRange< Iterator >( begin, end ),
              relationships_( relationships )
        {
            next_collection_iterator();
        }

        void next()
        {
            this->operator++();
            next_collection_iterator();
        }

        const ComponentID& vertex_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.vertex_component_id(
                { iterator->edge_id, static_cast< local_index_t >(
                                         ( iterator->vertex_id + 1 ) % 2 ) } );
        }

    private:
        bool is_collection_edge_vertex()
        {
            return this->current()->vertex_id
                   == Relationships::Impl::COLLECTION_EDGE_VERTEX;
        }

        void next_collection_iterator()
        {
            while( this->operator!=( *this )
                   && ( !relationships_.is_item_relation(
                            this->current()->edge_id )
                        || is_collection_edge_vertex() ) )
            {
                this->operator++();
            }
        }

    private:
        const Relationships::Impl& relationships_;
    };

    Relationships::CollectionRangeIterator::CollectionRangeIterator(
        const Relationships& relationships, const uuid& id )
        : impl_( *relationships.impl_,
            relationships.impl_->begin_edge( id ),
            relationships.impl_->end_edge( id ) )
    {
    }

    Relationships::CollectionRangeIterator::CollectionRangeIterator(
        CollectionRangeIterator&& other ) noexcept
        : impl_( *other.impl_ )
    {
    }

    Relationships::CollectionRangeIterator::CollectionRangeIterator(
        const CollectionRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::CollectionRangeIterator::~CollectionRangeIterator() {
    } // NOLINT

    bool Relationships::CollectionRangeIterator::operator!=(
        const CollectionRangeIterator& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    void Relationships::CollectionRangeIterator::operator++()
    {
        impl_->next();
    }

    const ComponentID& Relationships::CollectionRangeIterator::operator*() const
    {
        return impl_->vertex_component_id();
    }
} // namespace geode
