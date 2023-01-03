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

#include <bitsery/brief_syntax/array.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>

#include <geode/mesh/core/detail/facet_storage.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class FacetEdgesImpl : public FacetStorage< std::array< index_t, 2 > >
        {
            friend class bitsery::Access;

        public:
            FacetEdgesImpl() = default;

            absl::optional< index_t > find_edge(
                const std::array< index_t, 2 >& edge_vertices ) const
            {
                return this->find_facet( edge_vertices );
            }

            index_t find_or_create_edge(
                std::array< index_t, 2 > edge_vertices )
            {
                return this->add_facet( std::move( edge_vertices ) );
            }

            const std::array< index_t, 2 >& edge_vertices(
                const index_t edge_id ) const
            {
                return this->get_facet_vertices( edge_id );
            }

            void update_edge_vertex( std::array< index_t, 2 > edge_vertices,
                const index_t edge_vertex_id,
                const index_t new_vertex_id )
            {
                this->update_facet_vertex(
                    std::move( edge_vertices ), edge_vertex_id, new_vertex_id );
            }

            std::vector< index_t > update_edge_vertices(
                absl::Span< const index_t > old2new )
            {
                return this->update_facet_vertices( old2new );
            }

            void remove_edge( std::array< index_t, 2 > edge_vertices )
            {
                this->remove_facet( std::move( edge_vertices ) );
            }

            std::vector< index_t > delete_edges(
                const std::vector< bool >& to_delete )
            {
                return this->delete_facets( to_delete );
            }

            std::vector< index_t > remove_isolated_edges()
            {
                return this->clean_facets();
            }

            bool is_edge_isolated( index_t edge_id ) const
            {
                return this->get_counter( edge_id ) == 0;
            }

            AttributeManager& edge_attribute_manager() const
            {
                return facet_attribute_manager();
            }

            void overwrite_edges(
                const detail::FacetStorage< std::array< index_t, 2 > >& from )
            {
                this->overwrite( from );
            }

        private:
            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this,
                    DefaultGrowable< Archive, FacetEdgesImpl >{},
                    []( Archive& a, FacetEdgesImpl& impl ) {
                        a.ext(
                            impl, bitsery::ext::BaseClass< detail::FacetStorage<
                                      std::array< index_t, 2 > > >{} );
                    } );
            }
        };
    } // namespace detail
} // namespace geode
