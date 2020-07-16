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

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>

#include <geode/mesh/core/graph.h>

namespace geode
{
    namespace detail
    {
        class EdgesImpl
        {
        public:
            EdgesImpl( Graph& graph )
                : edges_(
                      graph.edge_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              std::array< index_t, 2 > >( "edges",
                              std::array< index_t, 2 >{ NO_ID, NO_ID } ) )
            {
            }

            index_t get_edge_vertex( const EdgeVertex& edge_vertex ) const
            {
                return edges_->value(
                    edge_vertex.edge_id )[edge_vertex.vertex_id];
            }

            void set_edge_vertex(
                const EdgeVertex& edge_vertex, const index_t vertex_id )
            {
                edges_->modify_value(
                    edge_vertex.edge_id, [&edge_vertex, vertex_id](
                                             std::array< index_t, 2 >& array ) {
                        array.at( edge_vertex.vertex_id ) = vertex_id;
                    } );
            }

            void register_attributes( Graph& graph )
            {
                edges_ =
                    graph.edge_attribute_manager()
                        .template find_or_create_attribute< VariableAttribute,
                            std::array< index_t, 2 > >(
                            "edges", std::array< index_t, 2 >{ NO_ID, NO_ID } );
            }

        protected:
            EdgesImpl() = default;

        private:
            friend class bitsery::Access;
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this, DefaultGrowable< Archive, EdgesImpl >{},
                    []( Archive& archive, EdgesImpl& impl ) {
                        archive.ext( impl.edges_, bitsery::ext::StdSmartPtr{} );
                    } );
            }

        private:
            std::shared_ptr< VariableAttribute< std::array< index_t, 2 > > >
                edges_;
        };
    } // namespace detail
} // namespace geode
