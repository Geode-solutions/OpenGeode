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

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/growable.hpp>
#include <geode/basic/permutation.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/mesh/core/graph.hpp>

namespace geode
{
    namespace internal
    {
        class EdgesImpl
        {
            using Edge = std::array< index_t, 2 >;
            static constexpr Edge NO_EDGE{ NO_ID, NO_ID };

        public:
            EdgesImpl() = default;

            [[nodiscard]] index_t get_edge_vertex(
                const EdgeVertex& edge_vertex ) const
            {
                return edges_[edge_vertex.edge_id][edge_vertex.vertex_id];
            }

            void set_edge_vertex(
                const EdgeVertex& edge_vertex, const index_t vertex_id )
            {
                edges_[edge_vertex.edge_id][edge_vertex.vertex_id] = vertex_id;
            }

            void add_edge()
            {
                edges_.push_back( NO_EDGE );
            }

            void add_edges( const index_t nb )
            {
                edges_.resize( edges_.size() + nb, NO_EDGE );
            }

            void remove_edges( const std::vector< bool >& to_delete )
            {
                delete_vector_elements( to_delete, edges_ );
            }

            void permute_edges( absl::Span< const index_t > permutation )
            {
                permute( edges_, permutation );
            }

            void copy_edges( const EdgesImpl& impl )
            {
                edges_ = impl.edges_;
            }

            void remove_legacy_attributes( AttributeManager& manager )
            {
                for( const auto& attribute_id : legacy_attributes_ )
                {
                    manager.delete_attribute( attribute_id );
                }
                legacy_attributes_.clear();
            }

        private:
            template < typename Archive >
            void deserialize_legacy_storage( Archive& archive )
            {
                std::shared_ptr< VariableAttribute< Edge > > edges;
                archive.ext( edges, bitsery::ext::StdSmartPtr{} );
                edges_.resize( edges->size() );
                for( const auto e : Indices{ edges_ } )
                {
                    edges_[e] = edges->value( e );
                }
                legacy_attributes_ = { edges->id() };
            }

            friend class bitsery::Access;
            template < typename Archive >
            void serialize( Archive& serializer )
            {
                serializer.ext( *this,
                    Growable< Archive, EdgesImpl >{
                        { []( Archive& archive, EdgesImpl& impl ) {
                             impl.deserialize_legacy_storage( archive );
                         },
                            []( Archive& archive, EdgesImpl& impl ) {
                                impl.deserialize_legacy_storage( archive );
                            },
                            []( Archive& archive, EdgesImpl& impl ) {
                                archive.container( impl.edges_,
                                    impl.edges_.max_size(),
                                    []( Archive& archive2, Edge& edge ) {
                                        archive2.container4b( edge );
                                    } );
                            } } } );
            }

        private:
            std::vector< Edge > edges_;
            std::vector< uuid > legacy_attributes_;
        };
    } // namespace internal
} // namespace geode
