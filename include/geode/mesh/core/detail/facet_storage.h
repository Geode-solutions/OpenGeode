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

#include <geode/basic/common.h>
#include <unordered_map>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/range.h>
#include <geode/mesh/builder/detail/mapping_after_deletion.h>
#include <geode/mesh/core/detail/vertex_cycle.h>

namespace geode
{
    namespace detail
    {
        class FacetStorage
        {
        protected:
            FacetStorage()
                : counter_(
                    facet_attribute_manager_
                        .template find_or_create_attribute< VariableAttribute,
                            index_t >( "counter", 1 ) )
            {
            }

            AttributeManager& facet_attribute_manager() const
            {
                return facet_attribute_manager_;
            }

            index_t find_facet( const VertexCycle& vertices ) const
            {
                auto itr = facet_indices_.find( vertices );
                if( itr != facet_indices_.end() )
                {
                    return facet_indices_.at( vertices );
                }
                return NO_ID;
            }

            index_t add_facet( const VertexCycle& vertices )
            {
                auto id = find_facet( vertices );
                if( id != NO_ID )
                {
                    counter_->value( id )++;
                    return id;
                }
                auto size = facet_indices_.size();
                facet_indices_[vertices] = size;
                facet_attribute_manager_.resize( size + 1 );
                return size;
            }

            void remove_facet( const VertexCycle& vertices )
            {
                auto id = find_facet( vertices );
                OPENGEODE_ASSERT( id != NO_ID,
                    "[FacetStorage::remove_facet] Cannot "
                    "find facet from given vertices" );
                if( counter_ > 0 )
                {
                    counter_->value( id )--;
                }
            }

            void replace_facet( const VertexCycle& from, const VertexCycle& to )
            {
                auto facet_id = find_facet( from );
                OPENGEODE_ASSERT( facet_id != NO_ID,
                    "[FacetStorage::replace_facet] Cannot "
                    "find facet from given vertices" );
                facet_indices_.erase( from );
                facet_indices_[to] = facet_id;
            }

            void clean_facets()
            {
                std::vector< bool > to_delete(
                    facet_attribute_manager_.nb_elements(), false );
                for( auto e :
                    geode::Range{ facet_attribute_manager_.nb_elements() } )
                {
                    if( counter_->value( e ) == 0 )
                    {
                        to_delete[e] = true;
                    }
                }
                delete_facets( to_delete );
            }

            void delete_facets( const std::vector< bool >& to_delete )
            {
                auto old2new = mapping_after_deletion( to_delete );
                std::vector< detail::VertexCycle > key_to_erase;
                key_to_erase.reserve( old2new.size() );
                for( const auto& cycle : facet_indices_ )
                {
                    if( old2new[cycle.second] == NO_ID )
                    {
                        key_to_erase.emplace_back( cycle.first );
                    }
                }
                for( const auto& key : key_to_erase )
                {
                    facet_indices_.erase( key );
                }
                for( auto& cycle : facet_indices_ )
                {
                    cycle.second = old2new[cycle.second];
                }
                facet_attribute_manager_.delete_elements( to_delete );
            }

            void update_facet_vertices( const std::vector< index_t >& old2new )
            {
                auto old_facet_indices = facet_indices_;
                facet_indices_.clear();
                facet_indices_.reserve( old_facet_indices.size() );
                for( const auto& cycle : old_facet_indices )
                {
                    auto updated_vertices = cycle.first.vertices();
                    for( auto& v : updated_vertices )
                    {
                        v = old2new[v];
                    }
                    detail::VertexCycle updated_cycle{ updated_vertices };
                    facet_indices_[updated_cycle] = cycle.second;
                }
            }

        private:
            mutable AttributeManager facet_attribute_manager_;
            std::unordered_map< detail::VertexCycle, index_t > facet_indices_;
            std::shared_ptr< VariableAttribute< index_t > > counter_;
        };
    } // namespace detail
} // namespace geode
