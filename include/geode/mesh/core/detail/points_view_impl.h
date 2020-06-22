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

#include <geode/basic/attribute.h>
#include <geode/basic/attribute_manager.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension, typename Mesh >
        class PointsViewImpl
        {
        public:
            PointsViewImpl( VertexSet& mesh_view, Mesh& mesh )
                : mesh_( mesh ),
                  mesh_view_( mesh_view ),
                  view2vertices_(
                      mesh_view.vertex_attribute_manager()
                          .find_or_create_attribute< VariableAttribute,
                              index_t >( "view2vertices", NO_ID ) )
            {
            }

            const Point< dimension >& get_point( index_t vertex_id ) const
            {
                return mesh_.point( viewed_vertex( vertex_id ) );
            }

            index_t viewed_vertex( index_t vertex_id ) const
            {
                return view2vertices_->value( vertex_id );
            }

            index_t vertex_in_view( index_t vertex_id ) const
            {
                return vertices2view_.at( vertex_id );
            }

            index_t add_viewed_vertex( index_t vertex_id )
            {
                const auto id = mesh_view_.nb_vertices();
                if( vertices2view_.emplace( vertex_id, id ).second )
                {
                    mesh_view_.vertex_attribute_manager().resize( id + 1 );
                    view2vertices_->set_value( id, vertex_id );
                }
                return vertex_in_view( vertex_id );
            }

        private:
            Mesh& mesh_;
            VertexSet& mesh_view_;
            std::shared_ptr< VariableAttribute< index_t > > view2vertices_;
            absl::flat_hash_map< index_t, index_t > vertices2view_;
        };
    } // namespace detail
} // namespace geode
