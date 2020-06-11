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

#include <geode/mesh/core/detail/points_view_impl.h>
#include <geode/mesh/core/surface_mesh.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class SurfaceMeshViewImpl : public detail::PointsViewImpl< dimension,
                                        SurfaceMesh< dimension > >
        {
        public:
            SurfaceMeshViewImpl( SurfaceMesh< dimension >& surface_view,
                SurfaceMesh< dimension >& surface )
                : detail::PointsViewImpl< dimension, SurfaceMesh< dimension > >(
                    surface_view, surface ),
                  surface_( surface ),
                  surface_view_( surface_view ),
                  view2polygons_(
                      surface_view.polygon_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              index_t >( "view2polygons", NO_ID ) )
            {
            }

            index_t get_polygon_vertex(
                const PolygonVertex& polygon_vertex ) const
            {
                return this->vertex_in_view( surface_.polygon_vertex(
                    viewed_polygon_vertex( polygon_vertex ) ) );
            }

            absl::optional< index_t > get_polygon_adjacent(
                const PolygonEdge& polygon_edge ) const
            {
                if( const auto adj = surface_.polygon_adjacent(
                        viewed_polygon_edge( polygon_edge ) ) )
                {
                    const auto it = polygons2view_.find( adj.value() );
                    if( it != polygons2view_.end() )
                    {
                        return it->second;
                    }
                }
                return absl::nullopt;
            }

            index_t viewed_triangle( index_t triangle_id ) const
            {
                return view2polygons_->value( triangle_id );
            }

            void add_viewed_triangle( index_t triangle_id )
            {
                const auto id = surface_view_.nb_polygons();
                surface_view_.polygon_attribute_manager().resize( id + 1 );
                view2polygons_->set_value( id, triangle_id );
                polygons2view_.emplace( triangle_id, id );
            }

        private:
            PolygonVertex viewed_polygon_vertex(
                const PolygonVertex polygon_vertex ) const
            {
                return { viewed_triangle( polygon_vertex.polygon_id ),
                    polygon_vertex.vertex_id };
            }

            PolygonEdge viewed_polygon_edge(
                const PolygonEdge polygon_edge ) const
            {
                return { viewed_triangle( polygon_edge.polygon_id ),
                    polygon_edge.edge_id };
            }

        private:
            SurfaceMesh< dimension >& surface_;
            SurfaceMesh< dimension >& surface_view_;
            std::shared_ptr< VariableAttribute< index_t > > view2polygons_;
            absl::flat_hash_map< index_t, index_t > polygons2view_;
        };
    } // namespace detail
} // namespace geode
