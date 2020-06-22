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

#pragma once

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/component_type.h>

namespace geode
{
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
         * Range to iterates on all boundaries of one component
         */
        class opengeode_model_api BoundaryRangeIterator
        {
        public:
            BoundaryRangeIterator(
                const Relationships& relationships, const uuid& id );
            BoundaryRangeIterator( BoundaryRangeIterator&& other ) noexcept;
            BoundaryRangeIterator( const BoundaryRangeIterator& other );
            ~BoundaryRangeIterator();

            bool operator!=( const BoundaryRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api BoundaryRange
            : public BoundaryRangeIterator,
              public BeginEnd< BoundaryRange >
        {
        public:
            BoundaryRange( const Relationships& relationships, const uuid& id )
                : BoundaryRangeIterator( relationships, id ), BeginEnd( *this )
            {
            }
        };

        /*!
         * Range to iterates on all incidences of one component
         */
        class opengeode_model_api IncidenceRangeIterator
        {
        public:
            IncidenceRangeIterator(
                const Relationships& relationships, const uuid& id );
            IncidenceRangeIterator( IncidenceRangeIterator&& other ) noexcept;
            IncidenceRangeIterator( const IncidenceRangeIterator& other );
            ~IncidenceRangeIterator();

            bool operator!=( const IncidenceRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api IncidenceRange
            : public IncidenceRangeIterator,
              public BeginEnd< IncidenceRange >
        {
        public:
            IncidenceRange( const Relationships& relationships, const uuid& id )
                : IncidenceRangeIterator( relationships, id ), BeginEnd( *this )
            {
            }
        };

        /*!
         * Range to iterates on all internal components of one component
         */
        class opengeode_model_api InternalRangeIterator
        {
        public:
            InternalRangeIterator(
                const Relationships& relationships, const uuid& id );
            InternalRangeIterator( InternalRangeIterator&& other ) noexcept;
            InternalRangeIterator( const InternalRangeIterator& other );
            ~InternalRangeIterator();

            bool operator!=( const InternalRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api InternalRange
            : public InternalRangeIterator,
              public BeginEnd< InternalRange >
        {
        public:
            InternalRange( const Relationships& relationships, const uuid& id )
                : InternalRangeIterator( relationships, id ), BeginEnd( *this )
            {
            }
        };

        /*!
         * Range to iterates on all embeddings of one component
         */
        class opengeode_model_api EmbeddingRangeIterator
        {
        public:
            EmbeddingRangeIterator(
                const Relationships& relationships, const uuid& id );
            EmbeddingRangeIterator( EmbeddingRangeIterator&& other ) noexcept;
            EmbeddingRangeIterator( const EmbeddingRangeIterator& other );
            ~EmbeddingRangeIterator();

            bool operator!=( const EmbeddingRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api EmbeddingRange
            : public EmbeddingRangeIterator,
              public BeginEnd< EmbeddingRange >
        {
        public:
            EmbeddingRange( const Relationships& relationships, const uuid& id )
                : EmbeddingRangeIterator( relationships, id ), BeginEnd( *this )
            {
            }
        };

        /*!
         * Range to iterates on all items of one component
         */
        class opengeode_model_api ItemRangeIterator
        {
        public:
            ItemRangeIterator(
                const Relationships& relationships, const uuid& id );
            ItemRangeIterator( ItemRangeIterator&& other ) noexcept;
            ItemRangeIterator( const ItemRangeIterator& other );
            ~ItemRangeIterator();

            bool operator!=( const ItemRangeIterator& /*unused*/ ) const;

            void operator++();

            const ComponentID& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_model_api ItemRange : public ItemRangeIterator,
                                              public BeginEnd< ItemRange >
        {
        public:
            ItemRange( const Relationships& relationships, const uuid& id )
                : ItemRangeIterator( relationships, id ), BeginEnd( *this )
            {
            }
        };

        /*!
         * Range to iterates on all collections of one component
         */
        class opengeode_model_api CollectionRangeIterator
        {
        public:
            CollectionRangeIterator(
                const Relationships& relationships, const uuid& id );
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
            : public CollectionRangeIterator,
              public BeginEnd< CollectionRange >
        {
        public:
            CollectionRange(
                const Relationships& relationships, const uuid& id )
                : CollectionRangeIterator( relationships, id ),
                  BeginEnd( *this )
            {
            }
        };

    public:
        Relationships();
        ~Relationships();

        index_t nb_boundaries( const uuid& id ) const;

        BoundaryRange boundaries( const uuid& id ) const;

        index_t nb_incidences( const uuid& id ) const;

        IncidenceRange incidences( const uuid& id ) const;

        index_t nb_internals( const uuid& id ) const;

        InternalRange internals( const uuid& id ) const;

        index_t nb_embeddings( const uuid& id ) const;

        EmbeddingRange embeddings( const uuid& id ) const;

        index_t nb_items( const uuid& id ) const;

        ItemRange items( const uuid& id ) const;

        index_t nb_collections( const uuid& id ) const;

        CollectionRange collections( const uuid& id ) const;

        bool is_boundary( const uuid& boundary, const uuid& incidence ) const;

        bool is_internal( const uuid& internal, const uuid& embedding ) const;

        bool is_item( const uuid& item, const uuid& collection ) const;

        void save_relationships( absl::string_view directory ) const;

        /*!
         * Add a component in the set of components registered by the
         * Relationships
         * @param[in] id The component identifier to add
         */
        void register_component(
            const ComponentID& id, RelationshipsBuilderKey );

        /*!
         * Remove a component from the set of components registered by the
         * Relationships and all its associated relationships
         * @param[in] id Unique index of the component to remove
         */
        void unregister_component( const uuid& id, RelationshipsBuilderKey );

        /*!
         * Add a new relationship of type boundary-incidence between two
         * components
         * @param[in] boundary Unique index of the boundary component
         * @param[in] incidence Unique index of the incidence component
         */
        void add_boundary_relation( const uuid& boundary,
            const uuid& incidence,
            RelationshipsBuilderKey );

        /*!
         * Add a new relationship of type internal-embedding between two
         * components
         * @param[in] internal Unique index of the internal component
         * @param[in] embedding Unique index of the embedding component
         */
        void add_internal_relation( const uuid& internal,
            const uuid& embedding,
            RelationshipsBuilderKey );

        /*!
         * Add a new relationship of type item-collection between two components
         * @param[in] item Unique index of the item component
         * @param[in] collection Unique index of the collection component
         */
        void add_item_in_collection(
            const uuid& item, const uuid& collection, RelationshipsBuilderKey );

        void load_relationships(
            absl::string_view directory, RelationshipsBuilderKey );

    protected:
        Relationships( Relationships&& );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
