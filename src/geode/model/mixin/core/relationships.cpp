/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/bitsery_archive.hpp>

#include <geode/mesh/core/bitsery_archive.hpp>
#include <geode/mesh/core/geode/geode_graph.hpp>
#include <geode/mesh/io/graph_input.hpp>
#include <geode/mesh/io/graph_output.hpp>

#include <geode/model/mixin/core/bitsery_archive.hpp>
#include <geode/model/mixin/core/detail/count_relationships.hpp>
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
                       .id()
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
                       .id()
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
                       .id()
                   == from;
        }

        index_t add_relation( const ComponentID& from,
            const ComponentID& to,
            const RelationType type )
        {
            if( const auto component_id =
                    relation_edge_index( from.id(), to.id() ) )
            {
                const auto relation_type =
                    relation_type_->value( component_id.value() );
                Logger::warn( "There is already a ",
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
            OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
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
            OPENGEODE_EXCEPTION(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                "[Relationships::load] Error while reading file: ", filename );
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
                         impl.initialize_relation_attribute();
                         impl.delete_isolated_vertices();
                     },
                        []( Archive& a, Impl& impl ) {
                            a.ext( impl.graph_, bitsery::ext::StdSmartPtr{} );
                            a.object( impl.uuid2index_ );
                            a.ext( impl.relation_type_,
                                bitsery::ext::StdSmartPtr{} );
                            a.ext( impl.ids_, bitsery::ext::StdSmartPtr{} );
                            impl.delete_isolated_vertices();
                        },
                        []( Archive& a, Impl& impl ) {
                            a.ext( impl, bitsery::ext::BaseClass<
                                             detail::RelationshipsImpl >{} );
                            a.ext( impl.relation_type_,
                                bitsery::ext::StdSmartPtr{} );
                        } } } );
        }

        void initialize_relation_attribute()
        {
            relation_type_ = relation_attribute_manager()
                                 .find_or_create_attribute< VariableAttribute,
                                     RelationType >( "relation_type", NO_ID );
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
        return detail::count_relationships( boundaries( component_id ) );
    }

    Relationships::BoundaryRange Relationships::boundaries(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::nb_incidences( const uuid& component_id ) const
    {
        return detail::count_relationships( incidences( component_id ) );
    }

    Relationships::IncidenceRange Relationships::incidences(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::add_boundary_relation( const ComponentID& boundary,
        const ComponentID& incidence,
        RelationshipsBuilderKey )
    {
        return impl_->add_relation(
            boundary, incidence, Relationships::Impl::BOUNDARY_RELATION );
    }

    index_t Relationships::nb_internals( const uuid& component_id ) const
    {
        return detail::count_relationships( internals( component_id ) );
    }

    Relationships::InternalRange Relationships::internals(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::nb_embeddings( const uuid& component_id ) const
    {
        return detail::count_relationships( embeddings( component_id ) );
    }

    Relationships::EmbeddingRange Relationships::embeddings(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::add_internal_relation( const ComponentID& internal,
        const ComponentID& embedding,
        RelationshipsBuilderKey )
    {
        return impl_->add_relation(
            internal, embedding, Relationships::Impl::INTERNAL_RELATION );
    }

    index_t Relationships::nb_items( const uuid& component_id ) const
    {
        return detail::count_relationships( items( component_id ) );
    }

    Relationships::ItemRange Relationships::items(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::nb_collections( const uuid& component_id ) const
    {
        return detail::count_relationships( collections( component_id ) );
    }

    Relationships::CollectionRange Relationships::collections(
        const uuid& component_id ) const
    {
        return { *this, component_id };
    }

    index_t Relationships::add_item_in_collection( const ComponentID& item,
        const ComponentID& collection,
        RelationshipsBuilderKey )
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
        RelationshipsBuilderKey )
    {
        impl_->copy( *relationships.impl_, mapping );
    }

    void Relationships::load_relationships(
        std::string_view directory, RelationshipsBuilderKey )
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

        const ComponentID& graph_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.graph_component_id( iterator->opposite() );
        }

    private:
        const Relationships::Impl& relationships_;
    };

    Relationships::RelationRangeIterator::RelationRangeIterator(
        const Relationships& relationships, const uuid& component_id )
        : impl_( *relationships.impl_,
              relationships.impl_->begin_edge( component_id ),
              relationships.impl_->end_edge( component_id ) )
    {
    }

    Relationships::RelationRangeIterator::RelationRangeIterator(
        RelationRangeIterator&& ) noexcept = default;

    Relationships::RelationRangeIterator::RelationRangeIterator(
        const RelationRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::RelationRangeIterator::~RelationRangeIterator() = default;

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
        return impl_->graph_component_id();
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

        const ComponentID& graph_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.graph_component_id( iterator->opposite() );
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
        const Relationships& relationships, const uuid& component_id )
        : impl_( *relationships.impl_,
              relationships.impl_->begin_edge( component_id ),
              relationships.impl_->end_edge( component_id ) )
    {
    }

    Relationships::BoundaryRangeIterator::BoundaryRangeIterator(
        BoundaryRangeIterator&& ) noexcept = default;

    Relationships::BoundaryRangeIterator::BoundaryRangeIterator(
        const BoundaryRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::BoundaryRangeIterator::~BoundaryRangeIterator() = default;

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
        return impl_->graph_component_id();
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

        const ComponentID& graph_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.graph_component_id( iterator->opposite() );
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
        const Relationships& relationships, const uuid& component_id )
        : impl_( *relationships.impl_,
              relationships.impl_->begin_edge( component_id ),
              relationships.impl_->end_edge( component_id ) )
    {
    }

    Relationships::IncidenceRangeIterator::IncidenceRangeIterator(
        IncidenceRangeIterator&& ) noexcept = default;

    Relationships::IncidenceRangeIterator::IncidenceRangeIterator(
        const IncidenceRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::IncidenceRangeIterator::~IncidenceRangeIterator() = default;

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
        return impl_->graph_component_id();
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

        const ComponentID& graph_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.graph_component_id( iterator->opposite() );
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
        const Relationships& relationships, const uuid& component_id )
        : impl_( *relationships.impl_,
              relationships.impl_->begin_edge( component_id ),
              relationships.impl_->end_edge( component_id ) )
    {
    }

    Relationships::InternalRangeIterator::InternalRangeIterator(
        InternalRangeIterator&& ) noexcept = default;

    Relationships::InternalRangeIterator::InternalRangeIterator(
        const InternalRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::InternalRangeIterator::~InternalRangeIterator() = default;

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
        return impl_->graph_component_id();
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

        const ComponentID& graph_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.graph_component_id( iterator->opposite() );
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
        const Relationships& relationships, const uuid& component_id )
        : impl_( *relationships.impl_,
              relationships.impl_->begin_edge( component_id ),
              relationships.impl_->end_edge( component_id ) )
    {
    }

    Relationships::EmbeddingRangeIterator::EmbeddingRangeIterator(
        EmbeddingRangeIterator&& ) noexcept = default;

    Relationships::EmbeddingRangeIterator::EmbeddingRangeIterator(
        const EmbeddingRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::EmbeddingRangeIterator::~EmbeddingRangeIterator() = default;

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
        return impl_->graph_component_id();
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

        const ComponentID& graph_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.graph_component_id( iterator->opposite() );
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
        const Relationships& relationships, const uuid& component_id )
        : impl_( *relationships.impl_,
              relationships.impl_->begin_edge( component_id ),
              relationships.impl_->end_edge( component_id ) )
    {
    }

    Relationships::ItemRangeIterator::ItemRangeIterator(
        ItemRangeIterator&& ) noexcept = default;

    Relationships::ItemRangeIterator::ItemRangeIterator(
        const ItemRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::ItemRangeIterator::~ItemRangeIterator() = default;

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
        return impl_->graph_component_id();
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

        const ComponentID& graph_component_id() const
        {
            const auto iterator = this->current();
            return relationships_.graph_component_id( iterator->opposite() );
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
        const Relationships& relationships, const uuid& component_id )
        : impl_( *relationships.impl_,
              relationships.impl_->begin_edge( component_id ),
              relationships.impl_->end_edge( component_id ) )
    {
    }

    Relationships::CollectionRangeIterator::CollectionRangeIterator(
        CollectionRangeIterator&& ) noexcept = default;

    Relationships::CollectionRangeIterator::CollectionRangeIterator(
        const CollectionRangeIterator& other )
        : impl_( *other.impl_ )
    {
    }

    Relationships::CollectionRangeIterator::
        ~CollectionRangeIterator() = default;

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
        return impl_->graph_component_id();
    }
} // namespace geode
