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

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/growable.hpp>

#include <geode/mesh/core/graph.hpp>

namespace geode
{
    namespace internal
    {
        class EdgesImpl
        {
        public:
            static constexpr auto EDGES_NAME = "edges";

            explicit EdgesImpl( Graph& graph ) : edges_()
            {
                DEBUG( "EdgesImpl" );
                const auto edge_attribute_id =
                    graph.edge_attribute_manager()
                        .template create_attribute< VariableAttribute,
                            std::array< index_t, 2 > >( "edges",
                            std::array< index_t, 2 >{ NO_ID, NO_ID },
                            { false, false, false } );
                edges_ =
                    graph.edge_attribute_manager()
                        .template find_attribute< VariableAttribute,
                            std::array< index_t, 2 > >( edge_attribute_id );
            }

            [[nodiscard]] index_t get_edge_vertex(
                const EdgeVertex& edge_vertex ) const
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

            [[nodiscard]] const uuid& edges_attribute_id() const
            {
                return edges_->id();
            }

        protected:
            EdgesImpl() = default;

        private:
            friend class bitsery::Access;
            template < typename Archive >
            void serialize( Archive& serializer )
            {
                serializer.ext( *this,
                    Growable< Archive, EdgesImpl >{
                        { []( Archive& archive, EdgesImpl& impl ) {
                             DEBUG( "serialize EdgesIMPL" );
                             archive.ext(
                                 impl.edges_, bitsery::ext::StdSmartPtr{} );
                             const auto& old_edges_properties =
                                 impl.edges_->properties();
                             impl.edges_->set_properties(
                                 { old_edges_properties.assignable,
                                     old_edges_properties.interpolable,
                                     false } );
                             DEBUG( "serialize EdgesIMPL end" );
                         },
                            []( Archive& archive, EdgesImpl& impl ) {
                                DEBUG( "serialize EdgesIMPL" );
                                archive.ext(
                                    impl.edges_, bitsery::ext::StdSmartPtr{} );
                                DEBUG( "serialize EdgesIMPL end" );
                            } } } );
            }

        private:
            std::shared_ptr< VariableAttribute< std::array< index_t, 2 > > >
                edges_;
        };
    } // namespace internal
} // namespace geode
