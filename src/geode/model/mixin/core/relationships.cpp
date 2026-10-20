/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/model/mixin/core/relationships.hpp>

#include <fstream>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/detail/count_range_elements.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/bitsery_archive.hpp>

#include <geode/mesh/core/bitsery_archive.hpp>
#include <geode/mesh/core/geode/geode_graph.hpp>
#include <geode/mesh/io/graph_input.hpp>
#include <geode/mesh/io/graph_output.hpp>

#include <geode/model/mixin/core/bitsery_archive.hpp>
#include <geode/model/mixin/core/detail/relationships_impl.hpp>

namespace geode
{
    class Relationships::Impl : public detail::RelationshipsImpl
    {
    public:
        using RelationType = index_t;
        static constexpr index_t NO_RELATION{ NO_ID };
        static constexpr index_t BOUNDARY_RELATION{ 0 };
        static constexpr index_t INTERNAL_RELATION{ 1 };
        static constexpr index_t ITEM_RELATION{ 2 };
        static constexpr local_index_t BOUNDARY_EDGE_VERTEX{ 0 };
        static constexpr local_index_t INCIDENT_EDGE_VERTEX{ 1 };
        static constexpr local_index_t INTERNAL_EDGE_VERTEX{ 0 };
        static constexpr local_index_t EMBEDDING_EDGE_VERTEX{ 1 };
        static constexpr local_index_t ITEM_EDGE_VERTEX{ 0 };
        static constexpr local_index_t COLLECTION_EDGE_VERTEX{ 1 };

        Impl() : RelationshipsImpl()
        {
            initialize_relation_attribute();
        }

        Impl( BITSERY bitsery ) : RelationshipsImpl( bitsery ) {}

        RelationType relation_type( const index_t edge_id ) const
        {
            return relation_type_->value( edge_id );
        }

        void skip_relation_edges( const RelationType type,
            const local_index_t excluded_edge_vertex,
            const EdgeVertex*& current,
            const EdgeVertex* end ) const
        {
            while( current != end
                   && ( relation_type( current->edge_id ) != type
                        || current->vertex_id == excluded_edge_vertex ) )
            {
                ++current;
            }
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

        std::optional< index_t > check_relation_exists(
            const uuid& from, const uuid& to, const RelationType type ) const
        {
            if( const auto edge_id = relation_edge_index( from, to ) )
            {
                if( relation_type( edge_id.value() ) == type )
                {
                    return edge_id;
                }
            }
            return std::nullopt;
        }

        bool is_boundary( const uuid& from, const uuid& to ) const
        {
            const auto edge_id =
                check_relation_exists( from, to, BOUNDARY_RELATION );
            if( !edge_id )
            {
                return false;
            }
            return graph_component_id(
                       { edge_id.value(), BOUNDARY_EDGE_VERTEX } )
                       .id
                   == from;
        }

        bool is_internal( const uuid& from, const uuid& to ) const
        {
            const auto edge_id =
                check_relation_exists( from, to, INTERNAL_RELATION );
            if( !edge_id )
            {
                return false;
            }
            return graph_component_id(
                       { edge_id.value(), INTERNAL_EDGE_VERTEX } )
                       .id
                   == from;
        }

        bool is_item( const uuid& from, const uuid& to ) const
        {
            const auto edge_id =
                check_relation_exists( from, to, ITEM_RELATION );
            if( !edge_id )
            {
                return false;
            }
            return graph_component_id( { edge_id.value(), ITEM_EDGE_VERTEX } )
                       .id
                   == from;
        }

        index_t add_relation( const ComponentID& from,
            const ComponentID& to,
            const RelationType type )
        {
            if( const auto component_id =
                    relation_edge_index( from.id, to.id ) )
            {
                const auto relation_type =
                    relation_type_->value( component_id.value() );
                Logger::warning( "There is already a ",
                    relation_to_string( relation_type ), " between (",
                    from.string(), " and ", to.string(), ")" );
                return component_id.value();
            }
            const auto index = add_relation_edge( from, to );
            relation_type_->set_value( index, type );
            return index;
        }

        void copy( const Impl& impl, const ModelCopyMapping& mapping )
        {
            detail::RelationshipsImpl::copy( impl, mapping );
            initialize_relation_attribute();
        }

        void save( std::string_view directory ) const
        {
            const auto filename = absl::StrCat( directory, "/relationships" );
            std::ofstream file{ filename, std::ofstream::binary };
            TContext context{};
            BitseryExtensions::register_serialize_pcontext(
                std::get< 0 >( context ) );
            Serializer archive{ context, file };
            archive.object( *this );
            archive.adapter().flush();
            OpenGeodeModelException::check_exception(
                std::get< 1 >( context ).isValid(), nullptr,
                OpenGeodeException::TYPE::internal,
                "[Relationships::save] Error while writing file: ", filename );
        }

        void load( std::string_view directory )
        {
            const auto filename = absl::StrCat( directory, "/relationships" );
            std::ifstream file{ filename, std::ifstream::binary };
            TContext context{};
            BitseryExtensions::register_deserialize_pcontext(
                std::get< 0 >( context ) );
            Deserializer archive{ context, file };
            archive.object( *this );
            const auto& adapter = archive.adapter();
            OpenGeodeModelException::check_exception(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                nullptr, OpenGeodeException::TYPE::internal,
                "[Relationships::load] Error while reading file: ", filename );
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& serializer )
        {
            serializer.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive, Impl& impl ) {
                         OpenGeodeGraph graph{ BITSERY::constructor };
                         archive.object( graph );
                         archive.object( impl.uuid2index_ );
                         archive.ext(
                             impl.relation_type_, bitsery::ext::StdSmartPtr{} );
                         archive.ext( impl.ids_, bitsery::ext::StdSmartPtr{} );
                         impl.graph_ = std::make_unique< OpenGeodeGraph >(
                             std::move( graph ) );
                         impl.initialize_attributes();
                         impl.initialize_relation_attribute();
                         impl.delete_isolated_vertices();
                     },
                        []( Archive& archive, Impl& impl ) {
                            impl.graph_ = std::make_unique< OpenGeodeGraph >(
                                BITSERY::constructor );
                            archive.ext(
                                impl.graph_, bitsery::ext::StdSmartPtr{} );
                            archive.object( impl.uuid2index_ );
                            archive.ext( impl.relation_type_,
                                bitsery::ext::StdSmartPtr{} );
                            archive.ext(
                                impl.ids_, bitsery::ext::StdSmartPtr{} );
                            impl.delete_isolated_vertices();
                        },
                        []( Archive& archive, Impl& impl ) {
                            archive.ext(
                                impl, bitsery::ext::BaseClass<
                                          detail::RelationshipsImpl >{} );
                            archive.ext( impl.relation_type_,
                                bitsery::ext::StdSmartPtr{} );
                        } } } );
        }

        void initialize_relation_attribute()
        {
            const auto ids =
                relation_attribute_manager().attribute_ids_matching_name(
                    "relation_type" );
            if( ids.has_value() )
            {
                relation_type_ =
                    relation_attribute_manager()
                        .find_attribute< VariableAttribute, RelationType >(
                            ids.value()[0] );
                return;
            }
            AttributeProperties attribute_properties;
            attribute_properties.assignable = false;
            attribute_properties.interpolable = false;
            attribute_properties.transferable = true;
            AttributeValues< RelationType > ids_attribute_values;
            ids_attribute_values.default_value = NO_ID;
            ids_attribute_values.no_value = NO_ID;
            const auto id =
                relation_attribute_manager()
                    .create_attribute< VariableAttribute, RelationType >(
                        "relation_type", ids_attribute_values,
                        attribute_properties );
            relation_type_ =
                relation_attribute_manager()
                    .find_attribute< VariableAttribute, RelationType >( id );
        }

        std::string relation_to_string( index_t relation_type ) const
        {
            if( relation_type == NO_RELATION )
            {
                return "No relation";
            }
            if( relation_type == BOUNDARY_RELATION )
            {
                return "Boundary relation";
            }
            if( relation_type == INTERNAL_RELATION )
            {
                return "Internal relation";
            }
            if( relation_type == ITEM_RELATION )
            {
                return "Item relation";
            }
            return "Undefined relation";
        }

    private:
        std::shared_ptr< VariableAttribute< RelationType > > relation_type_;
    };

    Relationships::Relationships() = default;
    Relationships::Relationships( BITSERY bitsery ) : impl_{ bitsery } {}
    Relationships::Relationships( Relationships&& ) noexcept = default;
    Relationships& Relationships::operator=(
        Relationships&& ) noexcept = default;
    Relationships::~Relationships() = default;

    void Relationships::remove_component(
        const uuid& component_id, RelationshipsBuilderKey /*unused*/ )
    {
        impl_->remove_component( component_id );
    }

    index_t Relationships::nb_components_with_relations() const
    {
        return impl_->nb_components_with_relations();
    }

    const ComponentID& Relationships::component_with_relation(
        index_t relation_component_id ) const
    {
        return impl_->graph_component_id( relation_component_id );
    }

    index_t Relationships::nb_relations( const uuid& component_id ) const
    {
        return impl_->nb_relations( component_id );
    }

    Relationships::RelationRange Relationships::relations(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::nb_boundaries( const uuid& component_id ) const
    {
        return detail::count_range_elements( boundaries( component_id ) );
    }

    Relationships::BoundaryRange Relationships::boundaries(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::nb_incidences( const uuid& component_id ) const
    {
        return detail::count_range_elements( incidences( component_id ) );
    }

    Relationships::IncidenceRange Relationships::incidences(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::add_boundary_relation( const ComponentID& boundary,
        const ComponentID& incidence,
        RelationshipsBuilderKey /*key*/ )
    {
        return impl_->add_relation(
            boundary, incidence, Relationships::Impl::BOUNDARY_RELATION );
    }

    index_t Relationships::nb_internals( const uuid& component_id ) const
    {
        return detail::count_range_elements( internals( component_id ) );
    }

    Relationships::InternalRange Relationships::internals(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::nb_embeddings( const uuid& component_id ) const
    {
        return detail::count_range_elements( embeddings( component_id ) );
    }

    Relationships::EmbeddingRange Relationships::embeddings(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::add_internal_relation( const ComponentID& internal,
        const ComponentID& embedding,
        RelationshipsBuilderKey /*key*/ )
    {
        return impl_->add_relation(
            internal, embedding, Relationships::Impl::INTERNAL_RELATION );
    }

    index_t Relationships::nb_items( const uuid& component_id ) const
    {
        return detail::count_range_elements( items( component_id ) );
    }

    Relationships::ItemRange Relationships::items(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::nb_collections( const uuid& component_id ) const
    {
        return detail::count_range_elements( collections( component_id ) );
    }

    Relationships::CollectionRange Relationships::collections(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::add_item_in_collection( const ComponentID& item,
        const ComponentID& collection,
        RelationshipsBuilderKey /*key*/ )
    {
        return impl_->add_relation(
            item, collection, Relationships::Impl::ITEM_RELATION );
    }

    void Relationships::remove_relation( const uuid& component_id1,
        const uuid& component_id2,
        RelationshipsBuilderKey /*unused*/ )
    {
        impl_->remove_relation( component_id1, component_id2 );
    }

    bool Relationships::is_boundary(
        const uuid& boundary, const uuid& incidence ) const
    {
        return impl_->is_boundary( boundary, incidence );
    }

    bool Relationships::is_internal(
        const uuid& internal, const uuid& embedding ) const
    {
        return impl_->is_internal( internal, embedding );
    }

    bool Relationships::is_item(
        const uuid& item, const uuid& collection ) const
    {
        return impl_->is_item( item, collection );
    }

    void Relationships::save_relationships( std::string_view directory ) const
    {
        impl_->save( directory );
    }

    void Relationships::copy_relationships( const ModelCopyMapping& mapping,
        const Relationships& relationships,
        RelationshipsBuilderKey /*key*/ )
    {
        impl_->copy( *relationships.impl_, mapping );
    }

    void Relationships::load_relationships(
        std::string_view directory, RelationshipsBuilderKey /*key*/ )
    {
        return impl_->load( directory );
    }

    AttributeManager& Relationships::relation_attribute_manager() const
    {
        return impl_->relation_attribute_manager();
    }

    std::optional< index_t > Relationships::relation_index(
        const uuid& component_id1, const uuid& component_id2 ) const
    {
        return impl_->relation_edge_index( component_id1, component_id2 );
    }

    std::tuple< ComponentID, ComponentID > Relationships::relation_from_index(
        index_t component_id ) const
    {
        return impl_->relation_components_from_index( component_id );
    }

    Relationships::RelationRangeIterator::RelationRangeIterator(
        const Relationships& relationships, const uuid& component_id )
        : relationships_( &*relationships.impl_ )
    {
        const auto edges =
            relationships_->edges_around_component( component_id );
        current_ = edges.data();
        end_ = edges.data() + edges.size();
    }

    Relationships::RelationRangeIterator::RelationRangeIterator(
        RelationRangeIterator&& ) noexcept = default;

    Relationships::RelationRangeIterator::RelationRangeIterator(
        const RelationRangeIterator& ) = default;

    Relationships::RelationRangeIterator::~RelationRangeIterator() = default;

    bool Relationships::RelationRangeIterator::operator!=(
        const RelationRangeIterator& /*unused*/ ) const
    {
        return current_ != end_;
    }

    void Relationships::RelationRangeIterator::operator++()
    {
        ++current_;
    }

    const ComponentID& Relationships::RelationRangeIterator::operator*() const
    {
        return relationships_->graph_component_id( current_->opposite() );
    }

    Relationships::BoundaryRangeIterator::BoundaryRangeIterator(
        const Relationships& relationships, const uuid& component_id )
        : relationships_( &*relationships.impl_ )
    {
        const auto edges =
            relationships_->edges_around_component( component_id );
        current_ = edges.data();
        end_ = edges.data() + edges.size();
        relationships_->skip_relation_edges(
            Relationships::Impl::BOUNDARY_RELATION,
            Relationships::Impl::BOUNDARY_EDGE_VERTEX, current_, end_ );
    }

    Relationships::BoundaryRangeIterator::BoundaryRangeIterator(
        BoundaryRangeIterator&& ) noexcept = default;

    Relationships::BoundaryRangeIterator::BoundaryRangeIterator(
        const BoundaryRangeIterator& ) = default;

    Relationships::BoundaryRangeIterator::~BoundaryRangeIterator() = default;

    bool Relationships::BoundaryRangeIterator::operator!=(
        const BoundaryRangeIterator& /*unused*/ ) const
    {
        return current_ != end_;
    }

    void Relationships::BoundaryRangeIterator::operator++()
    {
        ++current_;
        relationships_->skip_relation_edges(
            Relationships::Impl::BOUNDARY_RELATION,
            Relationships::Impl::BOUNDARY_EDGE_VERTEX, current_, end_ );
    }

    const ComponentID& Relationships::BoundaryRangeIterator::operator*() const
    {
        return relationships_->graph_component_id( current_->opposite() );
    }

    Relationships::IncidenceRangeIterator::IncidenceRangeIterator(
        const Relationships& relationships, const uuid& component_id )
        : relationships_( &*relationships.impl_ )
    {
        const auto edges =
            relationships_->edges_around_component( component_id );
        current_ = edges.data();
        end_ = edges.data() + edges.size();
        relationships_->skip_relation_edges(
            Relationships::Impl::BOUNDARY_RELATION,
            Relationships::Impl::INCIDENT_EDGE_VERTEX, current_, end_ );
    }

    Relationships::IncidenceRangeIterator::IncidenceRangeIterator(
        IncidenceRangeIterator&& ) noexcept = default;

    Relationships::IncidenceRangeIterator::IncidenceRangeIterator(
        const IncidenceRangeIterator& ) = default;

    Relationships::IncidenceRangeIterator::~IncidenceRangeIterator() = default;

    bool Relationships::IncidenceRangeIterator::operator!=(
        const IncidenceRangeIterator& /*unused*/ ) const
    {
        return current_ != end_;
    }

    void Relationships::IncidenceRangeIterator::operator++()
    {
        ++current_;
        relationships_->skip_relation_edges(
            Relationships::Impl::BOUNDARY_RELATION,
            Relationships::Impl::INCIDENT_EDGE_VERTEX, current_, end_ );
    }

    const ComponentID& Relationships::IncidenceRangeIterator::operator*() const
    {
        return relationships_->graph_component_id( current_->opposite() );
    }

    Relationships::InternalRangeIterator::InternalRangeIterator(
        const Relationships& relationships, const uuid& component_id )
        : relationships_( &*relationships.impl_ )
    {
        const auto edges =
            relationships_->edges_around_component( component_id );
        current_ = edges.data();
        end_ = edges.data() + edges.size();
        relationships_->skip_relation_edges(
            Relationships::Impl::INTERNAL_RELATION,
            Relationships::Impl::INTERNAL_EDGE_VERTEX, current_, end_ );
    }

    Relationships::InternalRangeIterator::InternalRangeIterator(
        InternalRangeIterator&& ) noexcept = default;

    Relationships::InternalRangeIterator::InternalRangeIterator(
        const InternalRangeIterator& ) = default;

    Relationships::InternalRangeIterator::~InternalRangeIterator() = default;

    bool Relationships::InternalRangeIterator::operator!=(
        const InternalRangeIterator& /*unused*/ ) const
    {
        return current_ != end_;
    }

    void Relationships::InternalRangeIterator::operator++()
    {
        ++current_;
        relationships_->skip_relation_edges(
            Relationships::Impl::INTERNAL_RELATION,
            Relationships::Impl::INTERNAL_EDGE_VERTEX, current_, end_ );
    }

    const ComponentID& Relationships::InternalRangeIterator::operator*() const
    {
        return relationships_->graph_component_id( current_->opposite() );
    }

    Relationships::EmbeddingRangeIterator::EmbeddingRangeIterator(
        const Relationships& relationships, const uuid& component_id )
        : relationships_( &*relationships.impl_ )
    {
        const auto edges =
            relationships_->edges_around_component( component_id );
        current_ = edges.data();
        end_ = edges.data() + edges.size();
        relationships_->skip_relation_edges(
            Relationships::Impl::INTERNAL_RELATION,
            Relationships::Impl::EMBEDDING_EDGE_VERTEX, current_, end_ );
    }

    Relationships::EmbeddingRangeIterator::EmbeddingRangeIterator(
        EmbeddingRangeIterator&& ) noexcept = default;

    Relationships::EmbeddingRangeIterator::EmbeddingRangeIterator(
        const EmbeddingRangeIterator& ) = default;

    Relationships::EmbeddingRangeIterator::~EmbeddingRangeIterator() = default;

    bool Relationships::EmbeddingRangeIterator::operator!=(
        const EmbeddingRangeIterator& /*unused*/ ) const
    {
        return current_ != end_;
    }

    void Relationships::EmbeddingRangeIterator::operator++()
    {
        ++current_;
        relationships_->skip_relation_edges(
            Relationships::Impl::INTERNAL_RELATION,
            Relationships::Impl::EMBEDDING_EDGE_VERTEX, current_, end_ );
    }

    const ComponentID& Relationships::EmbeddingRangeIterator::operator*() const
    {
        return relationships_->graph_component_id( current_->opposite() );
    }

    Relationships::ItemRangeIterator::ItemRangeIterator(
        const Relationships& relationships, const uuid& component_id )
        : relationships_( &*relationships.impl_ )
    {
        const auto edges =
            relationships_->edges_around_component( component_id );
        current_ = edges.data();
        end_ = edges.data() + edges.size();
        relationships_->skip_relation_edges( Relationships::Impl::ITEM_RELATION,
            Relationships::Impl::ITEM_EDGE_VERTEX, current_, end_ );
    }

    Relationships::ItemRangeIterator::ItemRangeIterator(
        ItemRangeIterator&& ) noexcept = default;

    Relationships::ItemRangeIterator::ItemRangeIterator(
        const ItemRangeIterator& ) = default;

    Relationships::ItemRangeIterator::~ItemRangeIterator() = default;

    bool Relationships::ItemRangeIterator::operator!=(
        const ItemRangeIterator& /*unused*/ ) const
    {
        return current_ != end_;
    }

    void Relationships::ItemRangeIterator::operator++()
    {
        ++current_;
        relationships_->skip_relation_edges( Relationships::Impl::ITEM_RELATION,
            Relationships::Impl::ITEM_EDGE_VERTEX, current_, end_ );
    }

    const ComponentID& Relationships::ItemRangeIterator::operator*() const
    {
        return relationships_->graph_component_id( current_->opposite() );
    }

    Relationships::CollectionRangeIterator::CollectionRangeIterator(
        const Relationships& relationships, const uuid& component_id )
        : relationships_( &*relationships.impl_ )
    {
        const auto edges =
            relationships_->edges_around_component( component_id );
        current_ = edges.data();
        end_ = edges.data() + edges.size();
        relationships_->skip_relation_edges( Relationships::Impl::ITEM_RELATION,
            Relationships::Impl::COLLECTION_EDGE_VERTEX, current_, end_ );
    }

    Relationships::CollectionRangeIterator::CollectionRangeIterator(
        CollectionRangeIterator&& ) noexcept = default;

    Relationships::CollectionRangeIterator::CollectionRangeIterator(
        const CollectionRangeIterator& ) = default;

    Relationships::CollectionRangeIterator::~CollectionRangeIterator() =
        default;

    bool Relationships::CollectionRangeIterator::operator!=(
        const CollectionRangeIterator& /*unused*/ ) const
    {
        return current_ != end_;
    }

    void Relationships::CollectionRangeIterator::operator++()
    {
        ++current_;
        relationships_->skip_relation_edges( Relationships::Impl::ITEM_RELATION,
            Relationships::Impl::COLLECTION_EDGE_VERTEX, current_, end_ );
    }

    const ComponentID& Relationships::CollectionRangeIterator::operator*() const
    {
        return relationships_->graph_component_id( current_->opposite() );
    }
} // namespace geode
