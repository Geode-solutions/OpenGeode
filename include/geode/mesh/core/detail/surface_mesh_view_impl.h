/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/mesh/builder/surface_mesh_builder.h>
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
                const SurfaceMesh< dimension >& surface )
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

            local_index_t get_nb_polygon_vertices( index_t polygon_id ) const
            {
                return surface_.nb_polygon_vertices(
                    viewed_polygon( polygon_id ) );
            }

            absl::optional< PolygonVertex > get_polygon_around_vertex(
                index_t vertex_id ) const
            {
                const auto viewed_vertex = this->viewed_vertex( vertex_id );
                const auto viewed_polygon_vertex =
                    surface_.polygon_around_vertex( viewed_vertex );
                if( !viewed_polygon_vertex )
                {
                    return absl::nullopt;
                }
                const auto it =
                    polygons2view_.find( viewed_polygon_vertex->polygon_id );
                if( it != polygons2view_.end() )
                {
                    return PolygonVertex{ it->second,
                        viewed_polygon_vertex->vertex_id };
                }
                for( const auto& polygon_around_vertex :
                    surface_.polygons_around_vertex( viewed_vertex ) )
                {
                    const auto it2 =
                        polygons2view_.find( polygon_around_vertex.polygon_id );
                    if( it2 != polygons2view_.end() )
                    {
                        return PolygonVertex{ it2->second,
                            polygon_around_vertex.vertex_id };
                    }
                }
                return absl::nullopt;
            }

            absl::optional< index_t > get_polygon_adjacent(
                const PolygonEdge& polygon_edge ) const
            {
                if( const auto adj = surface_.polygon_adjacent(
                        { viewed_polygon( polygon_edge.polygon_id ),
                            polygon_edge.edge_id } ) )
                {
                    const auto it = polygons2view_.find( adj.value() );
                    if( it != polygons2view_.end() )
                    {
                        return it->second;
                    }
                }
                return absl::nullopt;
            }

            index_t viewed_polygon( index_t polygon_id ) const
            {
                return view2polygons_->value( polygon_id );
            }

            index_t add_viewed_polygon( index_t polygon_id )
            {
                const auto polygon_view_id = surface_view_.nb_polygons();
                if( polygons2view_.emplace( polygon_id, polygon_view_id )
                        .second )
                {
                    surface_view_.polygon_attribute_manager().resize(
                        polygon_view_id + 1 );
                    view2polygons_->set_value( polygon_view_id, polygon_id );

                    for( const auto v :
                        LRange{ surface_.nb_polygon_vertices( polygon_id ) } )
                    {
                        this->add_viewed_vertex(
                            surface_.polygon_vertex( { polygon_id, v } ) );
                    }
                }
                return polygons2view_.at( polygon_id );
            }

        private:
            PolygonVertex viewed_polygon_vertex(
                const PolygonVertex& polygon_vertex ) const
            {
                return { viewed_polygon( polygon_vertex.polygon_id ),
                    polygon_vertex.vertex_id };
            }

        private:
            const SurfaceMesh< dimension >& surface_;
            SurfaceMesh< dimension >& surface_view_;
            std::shared_ptr< VariableAttribute< index_t > > view2polygons_;
            absl::flat_hash_map< index_t, index_t > polygons2view_;
        };
    } // namespace detail
} // namespace geode
