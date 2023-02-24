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

#pragma once

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/core/geode_graph.h>

#include <geode/model/common.h>
#include <geode/model/mixin/core/component_type.h>
#include <geode/model/mixin/core/detail/uuid_to_index.h>
#include <geode/model/representation/core/mapping.h>

namespace geode
{
    namespace detail
    {
        /*!
         * This class stores all the relations between a set of components.
         * Each relationship links two components.
         */
        class opengeode_model_api RelationshipsImpl
        {
        public:
            using Iterator = typename EdgesAroundVertex::const_iterator;

            index_t nb_relations( const uuid& id ) const;

            Iterator begin_edge( const uuid& id ) const;

            Iterator end_edge( const uuid& id ) const;

            const ComponentID& vertex_component_id(
                const EdgeVertex& edge_vertex ) const;

            void remove_component( const uuid& id );

            index_t add_relation_edge(
                const ComponentID& from, const ComponentID& to );

            void remove_relation( const uuid& id1, const uuid& id2 );

            AttributeManager& component_attribute_manager() const;

            AttributeManager& relation_attribute_manager() const;

            absl::optional< index_t > relation_edge_index(
                const uuid& id1, const uuid& id2 ) const;

            std::tuple< ComponentID, ComponentID >
                relation_components_from_index( index_t id ) const;

            void copy( const RelationshipsImpl& impl,
                const ModelCopyMapping& mapping );

        protected:
            RelationshipsImpl();

            void initialize_attributes();

            absl::optional< index_t > vertex_id( const uuid& id ) const;

            const Graph& graph() const;

            void delete_isolated_vertices();

        private:
            friend class bitsery::Access;
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this,
                    DefaultGrowable< Archive, RelationshipsImpl >{},
                    []( Archive& a, RelationshipsImpl& impl ) {
                        a.ext( impl.graph_, bitsery::ext::StdSmartPtr{} );
                        a.object( impl.uuid2index_ );
                        a.ext( impl.ids_, bitsery::ext::StdSmartPtr{} );
                        impl.delete_isolated_vertices();
                    } );
            }

            index_t register_component( const ComponentID& id );

            const ComponentID& component_from_index( index_t id ) const;

            index_t do_add_relation_edge(
                const ComponentID& from, const ComponentID& to );

            index_t find_or_create_vertex_id( const ComponentID& id );

        protected:
            std::unique_ptr< Graph > graph_;
            detail::UuidToIndex uuid2index_;
            std::shared_ptr< VariableAttribute< ComponentID > > ids_;
        };
    } // namespace detail
} // namespace geode
