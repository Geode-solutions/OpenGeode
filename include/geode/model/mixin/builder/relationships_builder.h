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

#pragma once

#include <geode/model/common.h>
#include <geode/model/mixin/core/relationships.h>

namespace geode
{
    /*!
     * Class managing modification of Relationships
     */
    class opengeode_model_api RelationshipsBuilder
    {
    public:
        RelationshipsBuilder( Relationships& relationships );

        /*!
         * Add a component in the set of components registered by the
         * Relationships
         * @param[in] id The component identifier to add
         */
        void register_component( const ComponentID& id );

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
        index_t add_boundary_relation(
            const uuid& boundary, const uuid& incidence );

        /*!
         * Add a new relationship of type internal-embedding between two
         * components
         * @param[in] internal Unique index of the internal component
         * @param[in] embedding Unique index of the embedding component
         */
        index_t add_internal_relation(
            const uuid& internal, const uuid& embedding );

        /*!
         * Add a new relationship of type item-collection between two components
         * @param[in] item Unique index of the item component
         * @param[in] collection Unique index of the collection component
         */
        index_t add_item_in_collection(
            const uuid& item, const uuid& collection );

        /*!
         * Add a new relationship between two components
         */
        index_t add_relation( const uuid& id1, const uuid& id2 );

        /*!
         * Remove a relationship between two components
         */
        void remove_relation( const uuid& id1, const uuid& id2 );

        void copy_relationships( const ModelCopyMapping& mapping,
            const Relationships& relationships );

        void load_relationships( absl::string_view directory );

    private:
        Relationships& relationships_;
    };
} // namespace geode