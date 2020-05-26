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

#include <absl/container/flat_hash_map.h>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/common.h>
#include <geode/basic/detail/mapping_after_deletion.h>
#include <geode/basic/range.h>

#include <geode/mesh/core/detail/vertex_cycle.h>

namespace geode
{
    namespace detail
    {
        template < typename VertexContainer >
        class FacetStorage
        {
            using TypedVertexCycle = VertexCycle< VertexContainer >;
            static constexpr auto ATTRIBUTE_NAME = "facet_vertices";
            friend class bitsery::Access;

        protected:
            FacetStorage()
                : counter_(
                      facet_attribute_manager_
                          .template find_or_create_attribute< VariableAttribute,
                              index_t >( "counter", 1u ) ),
                  vertices_(
                      facet_attribute_manager_
                          .template find_or_create_attribute< VariableAttribute,
                              VertexContainer >(
                              attribute_name(), VertexContainer{} ) )
            {
            }

            AttributeManager& facet_attribute_manager() const
            {
                return facet_attribute_manager_;
            }

            absl::optional< index_t > find_facet(
                TypedVertexCycle vertices ) const
            {
                const auto itr = facet_indices_.find( vertices );
                if( itr != facet_indices_.end() )
                {
                    return itr->second;
                }
                return absl::nullopt;
            }

            index_t add_facet( TypedVertexCycle vertices )
            {
                const auto id = facet_indices_.size();
                bool inserted;
                typename absl::flat_hash_map< TypedVertexCycle,
                    index_t >::iterator it;
                std::tie( it, inserted ) =
                    facet_indices_.try_emplace( std::move( vertices ), id );
                if( !inserted )
                {
                    const auto old_id = it->second;
                    counter_->set_value(
                        old_id, counter_->value( old_id ) + 1 );
                    return old_id;
                }
                facet_attribute_manager_.resize( id + 1 );
                vertices_->set_value( id, it->first.vertices() );
                return id;
            }

            void remove_facet( TypedVertexCycle vertices )
            {
                const auto it = facet_indices_.find( vertices );
                if( it == facet_indices_.end() )
                {
                    return;
                }
                const auto id = it->second;
                OPENGEODE_ASSERT( id != NO_ID,
                    "[FacetStorage::remove_facet] Cannot "
                    "find facet from given vertices" );
                const auto old_count = counter_->value( id );
                const auto new_count = std::max( 1u, old_count ) - 1;
                counter_->set_value( id, new_count );
            }

            std::vector< index_t > clean_facets()
            {
                std::vector< bool > to_delete(
                    facet_attribute_manager_.nb_elements(), false );
                for( const auto e :
                    Range{ facet_attribute_manager_.nb_elements() } )
                {
                    to_delete[e] = !counter_->value( e );
                }
                return delete_facets( to_delete );
            }

            std::vector< index_t > delete_facets(
                const std::vector< bool >& to_delete )
            {
                const auto old2new =
                    detail::mapping_after_deletion( to_delete );
                std::vector< TypedVertexCycle > key_to_erase;
                key_to_erase.reserve( old2new.size() );
                for( auto& cycle : facet_indices_ )
                {
                    if( old2new[cycle.second] == NO_ID )
                    {
                        key_to_erase.emplace_back( cycle.first );
                    }
                    cycle.second = old2new[cycle.second];
                }
                for( const auto& key : key_to_erase )
                {
                    facet_indices_.erase( key );
                }
                facet_attribute_manager_.delete_elements( to_delete );
                return old2new;
            }

            void update_facet_vertices( absl::Span< const index_t > old2new )
            {
                const auto old_facet_indices = facet_indices_;
                facet_indices_.clear();
                facet_indices_.reserve( old_facet_indices.size() );
                for( const auto& cycle : old_facet_indices )
                {
                    auto updated_vertices = cycle.first.vertices();
                    for( auto& v : updated_vertices )
                    {
                        v = old2new[v];
                    }
                    auto it = std::get< 0 >( facet_indices_.emplace(
                        TypedVertexCycle{ std::move( updated_vertices ) },
                        cycle.second ) );
                    vertices_->set_value( cycle.second, it->first.vertices() );
                }
            }

            const VertexContainer& get_facet_vertices( index_t facet_id ) const
            {
                return vertices_->value( facet_id );
            }

        protected:
            static constexpr absl::string_view attribute_name()
            {
                return ATTRIBUTE_NAME;
            }

            void update_attribute()
            {
                vertices_ =
                    facet_attribute_manager_.template find_or_create_attribute<
                        VariableAttribute, VertexContainer >(
                        attribute_name(), VertexContainer{} );
            }

            index_t get_counter( index_t facet_id ) const
            {
                return counter_->value( facet_id );
            }

            void overwrite( const FacetStorage< VertexContainer >& from )
            {
                facet_attribute_manager_.copy( from.facet_attribute_manager() );
                facet_indices_ = from.facet_indices_;
                counter_ =
                    facet_attribute_manager_
                        .find_or_create_attribute< VariableAttribute, index_t >(
                            "counter", 1u );
                vertices_ = facet_attribute_manager_.find_or_create_attribute<
                    VariableAttribute, VertexContainer >(
                    attribute_name(), VertexContainer{} );
            }

        private:
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this,
                    DefaultGrowable< Archive,
                        FacetStorage< VertexContainer > >{},
                    []( Archive& archive,
                        FacetStorage< VertexContainer >& storage ) {
                        archive.object( storage.facet_attribute_manager_ );
                        archive.ext( storage.facet_indices_,
                            bitsery::ext::StdMap{
                                storage.facet_indices_.max_size() },
                            []( Archive& archive, TypedVertexCycle& cycle,
                                index_t& attribute ) {
                                archive.object( cycle );
                                archive.value4b( attribute );
                            } );
                        archive.ext(
                            storage.counter_, bitsery::ext::StdSmartPtr{} );
                        archive.ext(
                            storage.vertices_, bitsery::ext::StdSmartPtr{} );
                    } );
            }

        private:
            mutable AttributeManager facet_attribute_manager_;
            absl::flat_hash_map< TypedVertexCycle, index_t > facet_indices_;
            std::shared_ptr< VariableAttribute< index_t > > counter_;
            std::shared_ptr< VariableAttribute< VertexContainer > > vertices_;
        };
    } // namespace detail
} // namespace geode
