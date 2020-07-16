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

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/core/detail/points_view_impl.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension, typename Mesh >
        class EdgesViewImpl : public detail::PointsViewImpl< dimension, Mesh >
        {
        public:
            EdgesViewImpl( Mesh& mesh_view, const Mesh& mesh )
                : detail::PointsViewImpl< dimension, Mesh >( mesh_view, mesh ),
                  mesh_( mesh ),
                  mesh_view_( mesh_view ),
                  view2edges_(
                      mesh_view.edge_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              index_t >( "view2edges", NO_ID ) ),
                  edge_vertices_(
                      mesh_view.edge_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              std::array< index_t, 2 > >( "facet_vertices",
                              std::array< index_t, 2 >{ { NO_ID, NO_ID } } ) )
            {
            }

            absl::optional< index_t > get_edge_from_vertices(
                const std::array< index_t, 2 >& vertices ) const
            {
                std::array< index_t, 2 > viewed_vertices{ { this->viewed_vertex(
                                                                vertices[0] ),
                    this->viewed_vertex( vertices[1] ) } };
                if( const auto viewed_edge =
                        mesh_.edge_from_vertices( viewed_vertices ) )
                {
                    const auto it = edges2view_.find( viewed_edge.value() );
                    if( it != edges2view_.end() )
                    {
                        return it->second;
                    }
                }
                return absl::nullopt;
            }

            const std::array< index_t, 2 >& get_edge_vertices(
                index_t edge_id ) const
            {
                const auto& viewed_vertices =
                    mesh_.edge_vertices( viewed_edge( edge_id ) );
                edge_vertices_->modify_value(
                    edge_id, [&viewed_vertices, this](
                                 std::array< index_t, 2 >& vertices ) {
                        vertices[0] =
                            this->vertex_in_view( viewed_vertices[0] );
                        vertices[1] =
                            this->vertex_in_view( viewed_vertices[1] );
                    } );
                return edge_vertices_->value( edge_id );
            }

            index_t viewed_edge( index_t edge_id ) const
            {
                return view2edges_->value( edge_id );
            }

            index_t edge_in_view( index_t edge_id ) const
            {
                return edges2view_.at( edge_id );
            }

            void add_viewed_edge( index_t edge_id )
            {
                const auto id = mesh_view_.nb_edges();
                if( edges2view_.emplace( edge_id, id ).second )
                {
                    mesh_view_.edge_attribute_manager().resize( id + 1 );
                    view2edges_->set_value( id, edge_id );
                }
            }

        private:
            const Mesh& mesh_;
            Mesh& mesh_view_;
            std::shared_ptr< VariableAttribute< index_t > > view2edges_;
            mutable std::shared_ptr<
                VariableAttribute< std::array< index_t, 2 > > >
                edge_vertices_;
            absl::flat_hash_map< index_t, index_t > edges2view_;
        };
    } // namespace detail
} // namespace geode
