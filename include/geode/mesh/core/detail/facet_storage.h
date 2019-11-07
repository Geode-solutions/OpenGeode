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

#include <unordered_map>

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/common.h>
#include <geode/basic/range.h>

#include <geode/basic/detail/mapping_after_deletion.h>
#include <geode/mesh/core/detail/vertex_cycle.h>

namespace geode
{
    namespace detail
    {
        struct SetValue
        {
            SetValue( const std::vector< index_t >& values ) : values_( values )
            {
            }

            void operator()( std::vector< index_t >& to )
            {
                to = values_;
            }

            void operator()( std::array< index_t, 2 >& to )
            {
                OPENGEODE_ASSERT( values_.size() == 2,
                    "[SetValue::operator()] Size of "
                    "vector values should be 2" );
                to[0] = values_[0];
                to[1] = values_[1];
            }

            const std::vector< index_t >& values_;
        };

        template < typename VertexContainer >
        class FacetStorage
        {
        protected:
            friend class bitsery::Access;
            FacetStorage()
                : counter_(
                      facet_attribute_manager_
                          .template find_or_create_attribute< VariableAttribute,
                              index_t >( "counter", 1 ) ),
                  vertices_(
                      facet_attribute_manager_
                          .template find_or_create_attribute< VariableAttribute,
                              VertexContainer >( "facet_vertices" ) )
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
                    counter_->set_value( id, counter_->value( id ) + 1 );
                    return id;
                }
                auto size = facet_indices_.size();
                facet_indices_[vertices] = size;
                facet_attribute_manager_.resize( size + 1 );
                vertices_->modify_value(
                    size, SetValue{ vertices.vertices() } );
                return size;
            }

            void remove_facet( const VertexCycle& vertices )
            {
                auto id = find_facet( vertices );
                OPENGEODE_ASSERT( id != NO_ID,
                    "[FacetStorage::remove_facet] Cannot "
                    "find facet from given vertices" );
                auto old_count = counter_->value( id );
                auto new_count = std::max( 1u, old_count ) - 1;
                counter_->set_value( id, new_count );
            }

            void clean_facets()
            {
                std::vector< bool > to_delete(
                    facet_attribute_manager_.nb_elements(), false );
                for( auto e : Range{ facet_attribute_manager_.nb_elements() } )
                {
                    to_delete[e] = !counter_->value( e );
                }
                delete_facets( to_delete );
            }

            void delete_facets( const std::vector< bool >& to_delete )
            {
                auto old2new = detail::mapping_after_deletion( to_delete );
                std::vector< VertexCycle > key_to_erase;
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
                // for( auto& cycle : facet_indices_ )
                // {
                // }
                DEBUG( "call facet att manag -> delete_el" );
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
                    VertexCycle updated_cycle{ updated_vertices };
                    facet_indices_[updated_cycle] = cycle.second;
                    vertices_->modify_value(
                        cycle.second, SetValue{ updated_vertices } );
                }
            }

            const VertexContainer& get_facet_vertices( index_t facet_id ) const
            {
                return vertices_->value( facet_id );
            }

        private:
            friend class bitsery::Access;
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
                            []( Archive& archive, VertexCycle& cycle,
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
            std::unordered_map< VertexCycle, index_t > facet_indices_;
            std::shared_ptr< VariableAttribute< index_t > > counter_;
            std::shared_ptr< VariableAttribute< VertexContainer > > vertices_;
        };
    } // namespace detail
} // namespace geode
