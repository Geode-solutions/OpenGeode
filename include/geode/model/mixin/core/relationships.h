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

#pragma once

#include <geode/basic/pimpl.h>

#include <geode/model/common.h>

namespace geode
{
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
    public:
        /*!
         * Range to iterates on all boundaries of one component
         */
        class opengeode_model_api BoundaryRange
        {
        public:
            BoundaryRange( const Relationships& manager, const uuid& id );
            BoundaryRange( BoundaryRange&& other ) noexcept;
            BoundaryRange( const BoundaryRange& other );
            ~BoundaryRange();

            const BoundaryRange& begin() const
            {
                return *this;
            }

            const BoundaryRange& end() const
            {
                return *this;
            }

            bool operator!=( const BoundaryRange& /*unused*/ ) const;

            void operator++();

            const uuid& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        /*!
         * Range to iterates on all incidences of one component
         */
        class opengeode_model_api IncidenceRange
        {
        public:
            IncidenceRange( const Relationships& manager, const uuid& id );
            IncidenceRange( IncidenceRange&& other ) noexcept;
            IncidenceRange( const IncidenceRange& other );
            ~IncidenceRange();

            const IncidenceRange& begin() const
            {
                return *this;
            }

            const IncidenceRange& end() const
            {
                return *this;
            }

            bool operator!=( const IncidenceRange& /*unused*/ ) const;

            void operator++();

            const uuid& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        /*!
         * Range to iterates on all items of one component
         */
        class opengeode_model_api ItemRange
        {
        public:
            ItemRange( const Relationships& manager, const uuid& id );
            ItemRange( ItemRange&& other ) noexcept;
            ItemRange( const ItemRange& other );
            ~ItemRange();

            const ItemRange& begin() const
            {
                return *this;
            }

            const ItemRange& end() const
            {
                return *this;
            }

            bool operator!=( const ItemRange& /*unused*/ ) const;

            void operator++();

            const uuid& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        /*!
         * Range to iterates on all collections of one component
         */
        class opengeode_model_api CollectionRange
        {
        public:
            CollectionRange( const Relationships& manager, const uuid& id );
            CollectionRange( CollectionRange&& other ) noexcept;
            CollectionRange( const CollectionRange& other );
            ~CollectionRange();

            const CollectionRange& begin() const
            {
                return *this;
            }

            const CollectionRange& end() const
            {
                return *this;
            }

            bool operator!=( const CollectionRange& /*unused*/ ) const;

            void operator++();

            const uuid& operator*() const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };

    public:
        Relationships();
        ~Relationships();

        index_t nb_boundaries( const uuid& id ) const;

        BoundaryRange boundaries( const uuid& id ) const;

        index_t nb_incidences( const uuid& id ) const;

        IncidenceRange incidences( const uuid& id ) const;

        index_t nb_items( const uuid& id ) const;

        ItemRange items( const uuid& id ) const;

        index_t nb_collections( const uuid& id ) const;

        CollectionRange collections( const uuid& id ) const;

        /*!
         * Add a component in the set of components registered by the
         * Relationships
         * @param[in] id Unique index of the component to add
         */
        void register_component( const uuid& id );

        /*!
         * Remove a component from the set of components registered by the
         * Relationships and all its associated relationships
         * @param[in] id Unique index of the component to remove
         */
        void unregister_component( const uuid& id );

        /*!
         * Add a new relationship of type boundary-incidence between two
         * components
         * @param[in] boundary Unique index of the boundary component
         * @param[in] incidence Unique index of the incidence component
         */
        void add_boundary_relation(
            const uuid& boundary, const uuid& incidence );
        /*!
         * Add a new relationship of type item-collection between two components
         * @param[in] item Unique index of the item component
         * @param[in] collection Unique index of the collection component
         */
        void add_item_in_collection( const uuid& item, const uuid& collection );

        std::string save_relationships( const std::string& directory ) const;

        void load_relationships( const std::string& directory );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
