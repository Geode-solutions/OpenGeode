/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#pragma once

#include <optional>

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/component_type.h>
#include <geode/model/representation/core/mapping.h>

namespace geode
{
    class AttributeManager;
    class RelationshipsBuilder;
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * This class stores all the relations between a set of components.
     * Each relationship links two components.
     * Relationship types are boundary and its dual incidence, and
     * item and its dual collection.
     * Components should be added through their uuids.
     */
    class opengeode_model_api Relationships
    {
        PASSKEY( RelationshipsBuilder, RelationshipsBuilderKey );

    public:
        /*!
         * Range to iterate on all relations of one component
         */
        class opengeode_model_api RelationRangeIterator
        {
        public:
            RelationRangeIterator(
                const Relationships& relationships, const uuid& component_id );
            RelationRangeIterator( RelationRangeIterator&& other ) noexcept;
            RelationRangeIterator( const RelationRangeIterator& other );
            ~RelationRangeIterator();

            bool operator!=( const RelationRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api RelationRange : public RelationRangeIterator
        {
        public:
            RelationRange(
                const Relationships& relationships, const uuid& component_id )
                : RelationRangeIterator( relationships, component_id )
            {
            }

            const RelationRange& begin() const
            {
                return *this;
            }

            const RelationRange& end() const
            {
                return *this;
            }
        };

        /*!
         * Range to iterate on all boundaries of one component
         */
        class opengeode_model_api BoundaryRangeIterator
        {
        public:
            BoundaryRangeIterator(
                const Relationships& relationships, const uuid& component_id );
            BoundaryRangeIterator( BoundaryRangeIterator&& other ) noexcept;
            BoundaryRangeIterator( const BoundaryRangeIterator& other );
            ~BoundaryRangeIterator();

            bool operator!=( const BoundaryRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api BoundaryRange : public BoundaryRangeIterator
        {
        public:
            BoundaryRange(
                const Relationships& relationships, const uuid& component_id )
                : BoundaryRangeIterator( relationships, component_id )
            {
            }

            const BoundaryRange& begin() const
            {
                return *this;
            }

            const BoundaryRange& end() const
            {
                return *this;
            }
        };

        /*!
         * Range to iterate on all incidences of one component
         */
        class opengeode_model_api IncidenceRangeIterator
        {
        public:
            IncidenceRangeIterator(
                const Relationships& relationships, const uuid& component_id );
            IncidenceRangeIterator( IncidenceRangeIterator&& other ) noexcept;
            IncidenceRangeIterator( const IncidenceRangeIterator& other );
            ~IncidenceRangeIterator();

            bool operator!=( const IncidenceRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api IncidenceRange : public IncidenceRangeIterator
        {
        public:
            IncidenceRange(
                const Relationships& relationships, const uuid& component_id )
                : IncidenceRangeIterator( relationships, component_id )
            {
            }

            const IncidenceRange& begin() const
            {
                return *this;
            }

            const IncidenceRange& end() const
            {
                return *this;
            }
        };

        /*!
         * Range to iterate on all internal components of one component
         */
        class opengeode_model_api InternalRangeIterator
        {
        public:
            InternalRangeIterator(
                const Relationships& relationships, const uuid& component_id );
            InternalRangeIterator( InternalRangeIterator&& other ) noexcept;
            InternalRangeIterator( const InternalRangeIterator& other );
            ~InternalRangeIterator();

            bool operator!=( const InternalRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api InternalRange : public InternalRangeIterator
        {
        public:
            InternalRange(
                const Relationships& relationships, const uuid& component_id )
                : InternalRangeIterator( relationships, component_id )
            {
            }

            const InternalRange& begin() const
            {
                return *this;
            }

            const InternalRange& end() const
            {
                return *this;
            }
        };

        /*!
         * Range to iterate on all embeddings of one component
         */
        class opengeode_model_api EmbeddingRangeIterator
        {
        public:
            EmbeddingRangeIterator(
                const Relationships& relationships, const uuid& component_id );
            EmbeddingRangeIterator( EmbeddingRangeIterator&& other ) noexcept;
            EmbeddingRangeIterator( const EmbeddingRangeIterator& other );
            ~EmbeddingRangeIterator();

            bool operator!=( const EmbeddingRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api EmbeddingRange : public EmbeddingRangeIterator
        {
        public:
            EmbeddingRange(
                const Relationships& relationships, const uuid& component_id )
                : EmbeddingRangeIterator( relationships, component_id )
            {
            }

            const EmbeddingRange& begin() const
            {
                return *this;
            }

            const EmbeddingRange& end() const
            {
                return *this;
            }
        };

        /*!
         * Range to iterate on all items of one component
         */
        class opengeode_model_api ItemRangeIterator
        {
        public:
            ItemRangeIterator(
                const Relationships& relationships, const uuid& component_id );
            ItemRangeIterator( ItemRangeIterator&& other ) noexcept;
            ItemRangeIterator( const ItemRangeIterator& other );
            ~ItemRangeIterator();

            bool operator!=( const ItemRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api ItemRange : public ItemRangeIterator
        {
        public:
            ItemRange(
                const Relationships& relationships, const uuid& component_id )
                : ItemRangeIterator( relationships, component_id )
            {
            }

            const ItemRange& begin() const
            {
                return *this;
            }

            const ItemRange& end() const
            {
                return *this;
            }
        };

        /*!
         * Range to iterate on all collections of one component
         */
        class opengeode_model_api CollectionRangeIterator
        {
        public:
            CollectionRangeIterator(
                const Relationships& relationships, const uuid& component_id );
            CollectionRangeIterator( CollectionRangeIterator&& other ) noexcept;
            CollectionRangeIterator( const CollectionRangeIterator& other );
            ~CollectionRangeIterator();

            bool operator!=( const CollectionRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api CollectionRange
            : public CollectionRangeIterator
        {
        public:
            CollectionRange(
                const Relationships& relationships, const uuid& component_id )
                : CollectionRangeIterator( relationships, component_id )
            {
            }

            const CollectionRange& begin() const
            {
                return *this;
            }

            const CollectionRange& end() const
            {
                return *this;
            }
        };

    public:
        Relationships();
        ~Relationships();

        index_t nb_components_with_relations() const;

        const ComponentID& component_with_relation(
            index_t relation_component_id ) const;

        index_t nb_relations( const uuid& component_id ) const;

        RelationRange relations( const uuid& component_id ) const;

        index_t nb_boundaries( const uuid& component_id ) const;

        BoundaryRange boundaries( const uuid& component_id ) const;

        index_t nb_incidences( const uuid& component_id ) const;

        IncidenceRange incidences( const uuid& component_id ) const;

        index_t nb_internals( const uuid& component_id ) const;

        InternalRange internals( const uuid& component_id ) const;

        index_t nb_embeddings( const uuid& component_id ) const;

        EmbeddingRange embeddings( const uuid& component_id ) const;

        index_t nb_items( const uuid& component_id ) const;

        ItemRange items( const uuid& component_id ) const;

        index_t nb_collections( const uuid& component_id ) const;

        CollectionRange collections( const uuid& component_id ) const;

        bool is_boundary( const uuid& boundary, const uuid& incidence ) const;

        bool is_internal( const uuid& internal, const uuid& embedding ) const;

        bool is_item( const uuid& item, const uuid& collection ) const;

        AttributeManager& relation_attribute_manager() const;

        std::optional< index_t > relation_index(
            const uuid& component_id1, const uuid& component_id2 ) const;

        std::tuple< ComponentID, ComponentID > relation_from_index(
            index_t component_id ) const;

        void save_relationships( std::string_view directory ) const;

    public:
        /*!
         * Remove a component from the set of components registered by the
         * Relationships and all its associated relationships
         * @param[in] component_id Unique index of the component to remove
         */
        void remove_component(
            const uuid& component_id, RelationshipsBuilderKey );

        /*!
         * Add a new relationship of type boundary-incidence between two
         * components
         */
        index_t add_boundary_relation( const ComponentID& boundary,
            const ComponentID& incidence,
            RelationshipsBuilderKey );

        /*!
         * Add a new relationship of type internal-embedding between two
         * components
         */
        index_t add_internal_relation( const ComponentID& internal,
            const ComponentID& embedding,
            RelationshipsBuilderKey );

        /*!
         * Add a new relationship of type item-collection between two components
         */
        index_t add_item_in_collection( const ComponentID& item,
            const ComponentID& collection,
            RelationshipsBuilderKey );

        /*!
         * Remove a relationship between two components
         */
        void remove_relation( const uuid& component_id1,
            const uuid& component_id2,
            RelationshipsBuilderKey );

        void copy_relationships( const ModelCopyMapping& mapping,
            const Relationships& relationships,
            RelationshipsBuilderKey );

        void load_relationships(
            std::string_view directory, RelationshipsBuilderKey );

    protected:
        Relationships( Relationships&& other ) noexcept;
        Relationships& operator=( Relationships&& other ) noexcept;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
