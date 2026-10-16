/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_map.h>

#include <bitsery/ext/std_map.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/common.hpp>
#include <geode/basic/mapping.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/mesh/core/detail/legacy_attributes.hpp>
#include <geode/mesh/core/detail/vertex_cycle.hpp>

namespace geode
{
    namespace detail
    {
        template < typename VertexContainer >
        class FacetStorage
        {
            using TypedVertexCycle = VertexCycle< VertexContainer >;
            friend class bitsery::Access;

        public:
            [[nodiscard]] bool is_opposite(
                index_t facet_id, const VertexContainer& vertices ) const
            {
                const OrientedVertexCycle other_oriented_cycle{ vertices };
                const OrientedVertexCycle oriented_cycle{ vertices_[facet_id] };
                return oriented_cycle.is_opposite( other_oriented_cycle );
            }

        protected:
            FacetStorage( BITSERY ) {}

            FacetStorage() = default;

            [[nodiscard]] AttributeManager& facet_attribute_manager() const
            {
                return facet_attribute_manager_;
            }

            /*!
             * Reserve memory for the given total number of facets
             */
            void reserve( index_t capacity )
            {
                facet_indices_.reserve( capacity );
                counters_.reserve( capacity );
                vertices_.reserve( capacity );
                facet_attribute_manager_.reserve( capacity );
            }

            [[nodiscard]] std::optional< index_t > find_facet(
                TypedVertexCycle vertices ) const
            {
                const auto itr = facet_indices_.find( vertices );
                if( itr != facet_indices_.end() )
                {
                    return itr->second;
                }
                return std::nullopt;
            }

            index_t add_facet( TypedVertexCycle vertices )
            {
                const auto id = static_cast< index_t >( counters_.size() );
                const auto output =
                    facet_indices_.try_emplace( std::move( vertices ), id );
                const auto it = std::get< 0 >( output );
                if( !std::get< 1 >( output ) )
                {
                    const auto old_id = it->second;
                    counters_[old_id]++;
                    return old_id;
                }
                facet_attribute_manager_.resize( id + 1 );
                counters_.push_back( 1 );
                vertices_.push_back( it->first.vertices() );
                return id;
            }

            index_t remove_facet( TypedVertexCycle vertices )
            {
                const auto it = facet_indices_.find( vertices );
                if( it == facet_indices_.end() )
                {
                    return NO_ID;
                }
                const auto id = it->second;
                OpenGeodeMeshException::check_assertion( id != NO_ID,
                    "[FacetStorage::remove_facet] Cannot find "
                    "facet from given vertices" );
                counters_[id] = std::max( 1u, counters_[id] ) - 1;
                return id;
            }

            std::vector< index_t > clean_facets()
            {
                std::vector< bool > to_delete( counters_.size(), false );
                bool has_isolated_facet{ false };
                for( const auto e : Indices{ counters_ } )
                {
                    if( counters_[e] == 0 )
                    {
                        to_delete[e] = true;
                        has_isolated_facet = true;
                    }
                }
                if( !has_isolated_facet )
                {
                    return identity_mapping();
                }
                return delete_facets( to_delete );
            }

            std::vector< index_t > delete_facets(
                const std::vector< bool >& to_delete )
            {
                if( absl::c_find( to_delete, true ) == to_delete.end() )
                {
                    return identity_mapping();
                }
                const auto old2new = mapping_after_deletion( to_delete );
                for( auto it = facet_indices_.begin();
                    it != facet_indices_.end(); )
                {
                    const auto new_id = old2new[it->second];
                    if( new_id == NO_ID )
                    {
                        facet_indices_.erase( it++ );
                    }
                    else
                    {
                        it->second = new_id;
                        ++it;
                    }
                }
                delete_elements( to_delete );
                return old2new;
            }

            geode::BijectiveMapping< index_t > update_facet_vertex(
                VertexContainer facet_vertices,
                const index_t facet_vertex_id,
                const index_t new_vertex_id )
            {
                geode::BijectiveMapping< index_t > mapping;
                auto updated_facet_vertices = facet_vertices;
                updated_facet_vertices[facet_vertex_id] = new_vertex_id;
                const auto new_facet_id = this->add_facet(
                    TypedVertexCycle{ std::move( updated_facet_vertices ) } );
                const auto old_facet_id = this->remove_facet(
                    TypedVertexCycle{ std::move( facet_vertices ) } );
                mapping.map( old_facet_id, new_facet_id );
                return mapping;
            }

            /*!
             * Update facet vertices with the given vertex mapping.
             * Facets using a vertex mapped to NO_ID are deleted.
             * Facets becoming identical are merged into the one with the
             * smallest index: their counters are summed up and the other
             * facets are deleted. In the returned mapping, a merged facet is
             * mapped to the new index of the facet it was merged into.
             */
            std::vector< index_t > update_facet_vertices(
                absl::Span< const index_t > old2new )
            {
                const auto old_facet_indices = std::move( facet_indices_ );
                facet_indices_ = {};
                facet_indices_.reserve( old_facet_indices.size() );
                std::vector< bool > to_delete( counters_.size(), false );
                std::vector< std::pair< index_t, index_t > > merged_facets;
                for( const auto& [cycle, facet_id] : old_facet_indices )
                {
                    auto updated_vertices = cycle.vertices();
                    bool removed{ false };
                    for( auto& v : updated_vertices )
                    {
                        v = old2new[v];
                        if( v == NO_ID )
                        {
                            removed = true;
                            break;
                        }
                    }
                    if( removed )
                    {
                        to_delete[facet_id] = true;
                        continue;
                    }
                    const auto [it, inserted] = facet_indices_.try_emplace(
                        TypedVertexCycle{ std::move( updated_vertices ) },
                        facet_id );
                    if( !inserted )
                    {
                        const auto kept = std::min( it->second, facet_id );
                        const auto merged = std::max( it->second, facet_id );
                        counters_[kept] += counters_[merged];
                        to_delete[merged] = true;
                        merged_facets.emplace_back( merged, kept );
                        it->second = kept;
                    }
                    vertices_[it->second] = it->first.vertices();
                }
                delete_elements( to_delete );
                auto cycle_old2new = mapping_after_deletion( to_delete );
                for( auto& cycle : facet_indices_ )
                {
                    cycle.second = cycle_old2new[cycle.second];
                }
                // Reverse order: a kept facet may be merged later on
                for( auto merged_it = merged_facets.rbegin();
                    merged_it != merged_facets.rend(); ++merged_it )
                {
                    cycle_old2new[merged_it->first] =
                        cycle_old2new[merged_it->second];
                }
                return cycle_old2new;
            }

            [[nodiscard]] const VertexContainer& get_facet_vertices(
                index_t facet_id ) const
            {
                return vertices_[facet_id];
            }

        protected:
            [[nodiscard]] index_t get_counter( index_t facet_id ) const
            {
                return counters_[facet_id];
            }

            void overwrite( const FacetStorage< VertexContainer >& from )
            {
                facet_attribute_manager_.copy( from.facet_attribute_manager() );
                facet_indices_ = from.facet_indices_;
                counters_ = from.counters_;
                vertices_ = from.vertices_;
            }

        private:
            [[nodiscard]] std::vector< index_t > identity_mapping() const
            {
                std::vector< index_t > old2new( counters_.size() );
                absl::c_iota( old2new, 0 );
                return old2new;
            }

            void delete_elements( const std::vector< bool >& to_delete )
            {
                facet_attribute_manager_.delete_elements( to_delete );
                delete_vector_elements( to_delete, counters_ );
                delete_vector_elements( to_delete, vertices_ );
            }

            template < typename Archive >
            void serialize_facet_indices( Archive& archive )
            {
                archive.ext( facet_indices_,
                    bitsery::ext::StdMap{ facet_indices_.max_size() },
                    []( Archive& archive2, TypedVertexCycle& cycle,
                        index_t& attribute ) {
                        archive2.object( cycle );
                        archive2.value4b( attribute );
                    } );
            }

            template < typename Archive >
            void deserialize_legacy_storage( Archive& archive )
            {
                LegacyAttributes legacy_attributes;
                legacy_attributes.deserialize( archive, counters_ );
                legacy_attributes.skip< VertexContainer >( archive );
                legacy_attributes.remove_from( facet_attribute_manager_ );
                initialize_vertices();
            }

            void initialize_vertices()
            {
                vertices_.resize( counters_.size() );
                for( const auto& [cycle, facet_id] : facet_indices_ )
                {
                    vertices_[facet_id] = cycle.vertices();
                }
            }

            template < typename Archive >
            void serialize( Archive& serializer )
            {
                serializer.ext( *this,
                    Growable< Archive, FacetStorage< VertexContainer > >{
                        { []( Archive& archive,
                              FacetStorage< VertexContainer >& storage ) {
                             archive.object( storage.facet_attribute_manager_ );
                             storage.serialize_facet_indices( archive );
                             storage.deserialize_legacy_storage( archive );
                         },
                            []( Archive& archive,
                                FacetStorage< VertexContainer >& storage ) {
                                archive.object(
                                    storage.facet_attribute_manager_ );
                                storage.serialize_facet_indices( archive );
                                storage.deserialize_legacy_storage( archive );
                            },
                            []( Archive& archive,
                                FacetStorage< VertexContainer >& storage ) {
                                archive.object(
                                    storage.facet_attribute_manager_ );
                                storage.serialize_facet_indices( archive );
                                archive.container4b( storage.counters_,
                                    storage.counters_.max_size() );
                                storage.initialize_vertices();
                            } } } );
            }

        private:
            mutable AttributeManager facet_attribute_manager_;
            absl::flat_hash_map< TypedVertexCycle, index_t > facet_indices_;
            std::vector< index_t > counters_;
            std::vector< VertexContainer > vertices_;
        };
    } // namespace detail
} // namespace geode
