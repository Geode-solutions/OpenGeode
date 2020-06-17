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
                SurfaceMesh< dimension >& surface )
                : detail::PointsViewImpl< dimension, SurfaceMesh< dimension > >(
                    surface_view, surface ),
                  surface_( surface ),
                  surface_view_( surface_view ),
                  view2polygons_(
                      surface_view.polygon_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              index_t >( "view2polygons", NO_ID ) ),
                  view2edges_(
                      surface_view.edge_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              index_t >( "view2edges", NO_ID ) ),
                  polygon_around_(
                      surface_view.vertex_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              PolygonVertex >(
                              "polygon_around_vertex", PolygonVertex{} ) ),
                  edge_vertices_(
                      surface_view.edge_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              std::array< index_t, 2 > >( "facet_vertices",
                              std::array< index_t, 2 >{ { NO_ID, NO_ID } } ) )
            {
            }

            index_t get_polygon_vertex(
                const PolygonVertex& polygon_vertex ) const
            {
                return this->vertex_in_view( surface_.polygon_vertex(
                    viewed_polygon_vertex( polygon_vertex ) ) );
            }

            index_t get_nb_polygon_vertices( index_t polygon_id ) const
            {
                return surface_.nb_polygon_vertices(
                    viewed_polygon( polygon_id ) );
            }

            const PolygonVertex& get_polygon_around_vertex(
                index_t vertex_id ) const
            {
                const auto viewed_vertex = this->viewed_vertex( vertex_id );
                const auto& viewed_polygon_vertex =
                    SurfaceMeshBuilder< dimension >::create( surface_ )
                        ->polygon_around_vertex( viewed_vertex );
                const auto it =
                    polygons2view_.find( viewed_polygon_vertex.polygon_id );
                if( it != polygons2view_.end() )
                {
                    polygon_around_->modify_value(
                        vertex_id, [&it, &viewed_polygon_vertex](
                                       PolygonVertex& polygon_vertex ) {
                            polygon_vertex.polygon_id = it->second;
                            polygon_vertex.vertex_id =
                                viewed_polygon_vertex.vertex_id;
                        } );
                }
                else
                {
                    for( const auto& polygon_around_vertex :
                        surface_.polygons_around_vertex( viewed_vertex ) )
                    {
                        const auto it2 = polygons2view_.find(
                            polygon_around_vertex.polygon_id );
                        if( it2 != polygons2view_.end() )
                        {
                            polygon_around_->modify_value(
                                vertex_id, [&it, &polygon_around_vertex](
                                               PolygonVertex& polygon_vertex ) {
                                    polygon_vertex.polygon_id = it->second;
                                    polygon_vertex.vertex_id =
                                        polygon_around_vertex.vertex_id;
                                } );
                            break;
                        }
                    }
                }
                return polygon_around_->value( vertex_id );
            }

            bool get_isolated_edge( index_t edge_id ) const
            {
                const auto& vertices = surface_view_.edge_vertices( edge_id );
                for( const auto& polygon_vertex :
                    surface_view_.polygons_around_vertex( vertices[0] ) )
                {
                    const auto polygon = polygon_vertex.polygon_id;
                    for( const auto& v :
                        Range{ surface_view_.nb_polygon_vertices( polygon ) } )
                    {
                        if( surface_view_.polygon_vertex(
                                surface_view_.next_polygon_vertex(
                                    { polygon, v } ) )
                            == vertices[1] )
                        {
                            return false;
                        }
                        if( surface_view_.polygon_vertex(
                                surface_view_.previous_polygon_vertex(
                                    { polygon, v } ) )
                            == vertices[1] )
                        {
                            return false;
                        }
                    }
                }
                return true;
            }

            absl::optional< index_t > get_edge_from_vertices(
                const std::array< index_t, 2 >& vertices ) const
            {
                std::array< index_t, 2 > viewed_vertices{ { this->viewed_vertex(
                                                                vertices[0] ),
                    this->viewed_vertex( vertices[1] ) } };
                if( const auto viewed_edge =
                        surface_.edge_from_vertices( viewed_vertices ) )
                {
                    const auto it = edges2view_.find( viewed_edge.value() );
                    if( it != edges2view_.end() )
                    {
                        return it->second;
                    }
                }
                return absl::nullopt;
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

            index_t get_polygon_edge( const PolygonEdge& polygon_edge ) const
            {
                const auto viewed_edge = surface_.polygon_edge(
                    viewed_polygon_edge( polygon_edge ) );
                return edges2view_.at( viewed_edge );
            }

            const std::array< index_t, 2 >& get_edge_vertices(
                index_t edge_id ) const
            {
                const auto& viewed_vertices =
                    surface_.edge_vertices( viewed_edge( edge_id ) );
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

            index_t viewed_polygon( index_t polygon_id ) const
            {
                return view2polygons_->value( polygon_id );
            }

            void add_viewed_polygon( index_t polygon_id )
            {
                const auto polygon_view_id = surface_view_.nb_polygons();
                if( polygons2view_.emplace( polygon_id, polygon_view_id )
                        .second )
                {
                    surface_view_.polygon_attribute_manager().resize(
                        polygon_view_id + 1 );
                    view2polygons_->set_value( polygon_view_id, polygon_id );

                    for( const auto v :
                        Range{ surface_.nb_polygon_vertices( polygon_id ) } )
                    {
                        this->add_viewed_vertex(
                            surface_.polygon_vertex( { polygon_id, v } ) );
                    }

                    for( const auto e :
                        Range{ surface_.nb_polygon_edges( polygon_id ) } )
                    {
                        const auto edge_view_id = surface_view_.nb_edges();
                        const auto edge_id =
                            surface_.polygon_edge( { polygon_id, e } );
                        if( edges2view_.emplace( edge_id, edge_view_id )
                                .second )
                        {
                            surface_view_.edge_attribute_manager().resize(
                                edge_view_id + 1 );
                            view2edges_->set_value( edge_view_id, edge_id );
                        }
                    }
                }
            }

        private:
            PolygonVertex viewed_polygon_vertex(
                const PolygonVertex polygon_vertex ) const
            {
                return { viewed_polygon( polygon_vertex.polygon_id ),
                    polygon_vertex.vertex_id };
            }

            PolygonEdge viewed_polygon_edge(
                const PolygonEdge polygon_edge ) const
            {
                return { viewed_polygon( polygon_edge.polygon_id ),
                    polygon_edge.edge_id };
            }

        private:
            SurfaceMesh< dimension >& surface_;
            SurfaceMesh< dimension >& surface_view_;
            std::shared_ptr< VariableAttribute< index_t > > view2polygons_;
            absl::flat_hash_map< index_t, index_t > polygons2view_;
            std::shared_ptr< VariableAttribute< index_t > > view2edges_;
            mutable std::shared_ptr< VariableAttribute< PolygonVertex > >
                polygon_around_;
            mutable std::shared_ptr<
                VariableAttribute< std::array< index_t, 2 > > >
                edge_vertices_;
            absl::flat_hash_map< index_t, index_t > edges2view_;
        };
    } // namespace detail
} // namespace geode
