/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/attribute.h>
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

        Impl()
            : relation_type_(
                graph_.edge_attribute_manager()
                    .find_or_create_attribute< VariableAttribute,
                        RelationType >( "relation_type", NO_ID ) ),
              ids_( graph_.vertex_attribute_manager()
                        .find_or_create_attribute< VariableAttribute,
                            ComponentID >( "id" ) )
        {
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
            return graph_.edges_around_vertex( id ).begin();
        }

        Iterator end_edge( const index_t id ) const
        {
            return graph_.edges_around_vertex( id ).end();
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
            return ids_->value( graph_.edge_vertex( edge_vertex ) );
        }

        void register_component( const ComponentID& id )
        {
            const auto index = OpenGeodeGraphBuilder{ graph_ }.create_vertex();
            uuid2index_.set_new_mapping( id.id(), index );
            ids_->set_value( index, id );
        }

        void unregister_component( const uuid& id )
        {
            const auto index = vertex_id( id );
            std::vector< bool > to_delete( graph_.nb_vertices(), false );
            to_delete[index] = true;
            OpenGeodeGraphBuilder{ graph_ }.delete_vertices( to_delete );
            uuid2index_.erase( id );
            uuid2index_.decrement_indices_larger_than( index );
        }

        bool check_relation_exists(
            const uuid& from, const uuid& to, const RelationType type )
        {
            const auto& edges_around =
                graph_.edges_around_vertex( vertex_id( from ) );
            for( const auto& edge_vertex : edges_around )
            {
                const auto& other =
                    this->vertex_component_id( { edge_vertex.edge_id,
                        ( edge_vertex.vertex_id + 1 ) % 2 } );
                if( to == other.id()
                    && relation_type( edge_vertex.edge_id ) == type )
                {
                    return true;
                }
            }
            return false;
        }

        void add_relation(
            const uuid& from, const uuid& to, const RelationType type )
        {
            if( check_relation_exists( from, to, type ) )
            {
                Logger::warn( "This relation already exists (",
                    ids_->value( vertex_id( from ) ).type().get(), " and ",
                    ids_->value( vertex_id( to ) ).type().get(), ")" );
                return;
            }
            const auto index = OpenGeodeGraphBuilder{ graph_ }.create_edge(
                vertex_id( from ), vertex_id( to ) );
            relation_type_->set_value( index, type );
        }

        std::string save( const std::string& directory ) const
        {
            const auto filename = directory + "/relationships";
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
                "[Relationships::save] Error while writing file: " + filename );
            return filename;
        }

        void load( const std::string& directory )
        {
            const auto filename = directory + "/relationships";
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
                "[Relationships::load] Error while reading file: " + filename );
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, DefaultGrowable< Archive, Impl >{},
                []( Archive& archive, Impl& impl ) {
                    archive.object( impl.graph_ );
                    archive.object( impl.uuid2index_ );
                    archive.ext(
                        impl.relation_type_, bitsery::ext::StdSmartPtr{} );
                    archive.ext( impl.ids_, bitsery::ext::StdSmartPtr{} );
                } );
        }

        index_t vertex_id( const uuid& id ) const
        {
            return uuid2index_.index( id );
        }

    private:
        OpenGeodeGraph graph_;
        detail::UuidToIndex uuid2index_;
        std::shared_ptr< VariableAttribute< RelationType > > relation_type_;
        std::shared_ptr< VariableAttribute< ComponentID > > ids_;
    };

    Relationships::Relationships() {} // NOLINT
    Relationships::~Relationships() {} // NOLINT

    void Relationships::register_component( const ComponentID& id )
    {
        impl_->register_component( id );
    }

    void Relationships::unregister_component( const uuid& id )
    {
        impl_->unregister_component( id );
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

    void Relationships::add_boundary_relation(
        const uuid& boundary, const uuid& incidence )
    {
        impl_->add_relation(
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

    void Relationships::add_internal_relation(
        const uuid& internal, const uuid& embedding )
    {
        impl_->add_relation(
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

    void Relationships::add_item_in_collection(
        const uuid& item, const uuid& collection )
    {
        impl_->add_relation(
            item, collection, Relationships::Impl::ITEM_RELATION );
    }

    void Relationships::save_relationships( const std::string& directory ) const
    {
        impl_->save( directory );
    }

    void Relationships::load_relationships( const std::string& directory )
    {
        return impl_->load( directory );
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
                { iterator->edge_id, ( iterator->vertex_id + 1 ) % 2 } );
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
                { iterator->edge_id, ( iterator->vertex_id + 1 ) % 2 } );
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
                { iterator->edge_id, ( iterator->vertex_id + 1 ) % 2 } );
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
                { iterator->edge_id, ( iterator->vertex_id + 1 ) % 2 } );
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
                { iterator->edge_id, ( iterator->vertex_id + 1 ) % 2 } );
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
                { iterator->edge_id, ( iterator->vertex_id + 1 ) % 2 } );
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
