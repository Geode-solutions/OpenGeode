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

#include <geode/mesh/builder/solid_mesh_builder.h>
#include <geode/mesh/core/detail/edges_view_impl.h>
#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class SolidMeshViewImpl
            : public detail::EdgesViewImpl< dimension, SolidMesh< dimension > >
        {
        public:
            SolidMeshViewImpl( SolidMesh< dimension >& solid_view,
                const SolidMesh< dimension >& solid )
                : detail::EdgesViewImpl< dimension, SolidMesh< dimension > >(
                    solid_view, solid ),
                  solid_( solid ),
                  solid_view_( solid_view ),
                  view2polyhedra_(
                      solid_view.polyhedron_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              index_t >( "view2polyhedra", NO_ID ) ),
                  view2facets_(
                      solid_view.facet_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              index_t >( "view2facets", NO_ID ) ),
                  facet_vertices_(
                      solid_view.facet_attribute_manager()
                          .template find_or_create_attribute< VariableAttribute,
                              PolyhedronFacetVertices >(
                              "facet_vertices", PolyhedronFacetVertices{} ) )
            {
            }

            index_t get_polyhedron_vertex(
                const PolyhedronVertex& polyhedron_vertex ) const
            {
                return this->vertex_in_view( solid_.polyhedron_vertex(
                    viewed_polyhedron_vertex( polyhedron_vertex ) ) );
            }

            index_t get_nb_polyhedron_vertices( index_t polyhedron_id ) const
            {
                return solid_.nb_polyhedron_vertices(
                    viewed_polyhedron( polyhedron_id ) );
            }

            index_t get_nb_polyhedron_facets( index_t polyhedron_id ) const
            {
                return solid_.nb_polyhedron_facets(
                    viewed_polyhedron( polyhedron_id ) );
            }

            index_t get_nb_polyhedron_facet_vertices(
                const PolyhedronFacet& polyhedron_facet ) const
            {
                return solid_.nb_polyhedron_facet_vertices(
                    viewed_polyhedron_facet( polyhedron_facet ) );
            }

            absl::optional< PolyhedronVertex > get_polyhedron_around_vertex(
                index_t vertex_id ) const
            {
                const auto viewed_vertex = this->viewed_vertex( vertex_id );
                const auto viewed_polyhedron_vertex =
                    solid_.polyhedron_around_vertex( viewed_vertex );
                if( !viewed_polyhedron_vertex )
                {
                    return absl::nullopt;
                }
                const auto it = polyhedra2view_.find(
                    viewed_polyhedron_vertex->polyhedron_id );
                if( it != polyhedra2view_.end() )
                {
                    return PolyhedronVertex{ it->second,
                        viewed_polyhedron_vertex->vertex_id };
                }
                for( const auto& polyhedron_around_vertex :
                    solid_.polyhedra_around_vertex( viewed_vertex ) )
                {
                    const auto it2 = polyhedra2view_.find(
                        polyhedron_around_vertex.polyhedron_id );
                    if( it2 != polyhedra2view_.end() )
                    {
                        return PolyhedronVertex{ it2->second,
                            polyhedron_around_vertex.vertex_id };
                    }
                }
                return absl::nullopt;
            }

            bool get_isolated_edge( index_t edge_id ) const
            {
                const auto& vertices = solid_view_.edge_vertices( edge_id );
                for( const auto& polyhedron_vertex :
                    solid_view_.polyhedra_around_vertex( vertices[0] ) )
                {
                    const auto polyhedron = polyhedron_vertex.polyhedron_id;
                    for( const auto f : Range{
                             solid_view_.nb_polyhedron_facets( polyhedron ) } )
                    {
                        PolyhedronFacet facet{ polyhedron, f };
                        const auto nb_vertices =
                            solid_view_.nb_polyhedron_facet_vertices( facet );
                        for( const auto v : Range{ nb_vertices } )
                        {
                            const auto v0 = solid_view_.polyhedron_facet_vertex(
                                { facet, v } );
                            if( v0 == vertices[0] )
                            {
                                const auto v1_next =
                                    solid_view_.polyhedron_facet_vertex(
                                        { facet, ( v + 1 ) % nb_vertices } );
                                if( v1_next == vertices[1] )
                                {
                                    return false;
                                }
                                const auto v1_prev =
                                    solid_view_.polyhedron_facet_vertex(
                                        { facet, ( v + nb_vertices - 1 )
                                                     % nb_vertices } );
                                if( v1_prev == vertices[1] )
                                {
                                    return false;
                                }
                                break;
                            }
                        }
                    }
                }
                return true;
            }

            bool get_isolated_facet( index_t facet_id ) const
            {
                using Cycle = detail::VertexCycle< PolyhedronFacetVertices >;
                const auto& vertices = solid_view_.facet_vertices( facet_id );
                Cycle cycle{ vertices };
                for( const auto& polyhedron_vertex :
                    solid_view_.polyhedra_around_vertex( vertices[0] ) )
                {
                    const auto polyhedron = polyhedron_vertex.polyhedron_id;
                    for( const auto f : Range{
                             solid_view_.nb_polyhedron_facets( polyhedron ) } )
                    {
                        PolyhedronFacet facet{ polyhedron, f };
                        const auto nb_vertices =
                            solid_view_.nb_polyhedron_facet_vertices( facet );
                        PolyhedronFacetVertices facet_vertices;
                        facet_vertices.reserve( nb_vertices );
                        for( const auto v : Range{ nb_vertices } )
                        {
                            facet_vertices.push_back(
                                solid_view_.polyhedron_facet_vertex(
                                    { facet, v } ) );
                        }
                        if( cycle == Cycle{ facet_vertices } )
                        {
                            return false;
                        }
                    }
                }
                return true;
            }

            absl::optional< index_t > get_facet_from_vertices(
                const PolyhedronFacetVertices& vertices ) const
            {
                PolyhedronFacetVertices viewed_vertices;
                for( const auto v : vertices )
                {
                    viewed_vertices.push_back( this->viewed_vertex( v ) );
                }
                if( const auto viewed_facet =
                        solid_.facet_from_vertices( viewed_vertices ) )
                {
                    const auto it = facets2view_.find( viewed_facet.value() );
                    if( it != facets2view_.end() )
                    {
                        return it->second;
                    }
                }
                return absl::nullopt;
            }

            const PolyhedronFacetVertices& get_facet_vertices(
                index_t facet_id ) const
            {
                const auto& viewed_vertices =
                    solid_.facet_vertices( viewed_facet( facet_id ) );
                facet_vertices_->modify_value(
                    facet_id, [&viewed_vertices, this](
                                  PolyhedronFacetVertices& vertices ) {
                        vertices.resize( viewed_vertices.size() );
                        for( const auto v : Range{ vertices.size() } )
                        {
                            vertices[v] =
                                this->vertex_in_view( viewed_vertices[v] );
                        }
                    } );
                return facet_vertices_->value( facet_id );
            }

            index_t viewed_facet( index_t facet_id ) const
            {
                return view2facets_->value( facet_id );
            }

            index_t facet_in_view( index_t facet_id ) const
            {
                return facets2view_.at( facet_id );
            }

            index_t add_viewed_facet( index_t facet_id )
            {
                const auto id = solid_view_.nb_facets();
                if( facets2view_.emplace( facet_id, id ).second )
                {
                    solid_view_.facet_attribute_manager().resize( id + 1 );
                    view2facets_->set_value( id, facet_id );
                }
                return facet_in_view( facet_id );
            }

            absl::optional< index_t > get_polyhedron_adjacent(
                const PolyhedronFacet& polyhedron_facet ) const
            {
                if( const auto adj = solid_.polyhedron_adjacent(
                        viewed_polyhedron_facet( polyhedron_facet ) ) )
                {
                    const auto it = polyhedra2view_.find( adj.value() );
                    if( it != polyhedra2view_.end() )
                    {
                        return it->second;
                    }
                }
                return absl::nullopt;
            }

            index_t get_polyhedron_facet(
                const PolyhedronFacet& polyhedron_facet ) const
            {
                const auto viewed_facet = solid_.polyhedron_facet(
                    viewed_polyhedron_facet( polyhedron_facet ) );
                return this->facet_in_view( viewed_facet );
            }

            PolyhedronVertex get_polyhedron_facet_vertex_id(
                const PolyhedronFacetVertex& polyhedron_facet_vertex ) const
            {
                const auto viewed_facet = viewed_polyhedron_facet(
                    polyhedron_facet_vertex.polyhedron_facet );
                const auto viewed_vertex = solid_.polyhedron_facet_vertex(
                    { viewed_facet, polyhedron_facet_vertex.vertex_id } );
                for( const auto v : Range{ solid_.nb_polyhedron_vertices(
                         viewed_facet.polyhedron_id ) } )
                {
                    if( solid_.polyhedron_vertex(
                            { viewed_facet.polyhedron_id, v } )
                        == viewed_vertex )
                    {
                        return { polyhedron_facet_vertex.polyhedron_facet
                                     .polyhedron_id,
                            v };
                    }
                }

                return {};
            }

            index_t viewed_polyhedron( index_t polyhedron_id ) const
            {
                return view2polyhedra_->value( polyhedron_id );
            }

            index_t add_viewed_polyhedron( index_t polyhedron_id )
            {
                const auto polyhedron_view_id = solid_view_.nb_polyhedra();
                if( polyhedra2view_.emplace( polyhedron_id, polyhedron_view_id )
                        .second )
                {
                    solid_view_.polyhedron_attribute_manager().resize(
                        polyhedron_view_id + 1 );
                    view2polyhedra_->set_value(
                        polyhedron_view_id, polyhedron_id );

                    for( const auto v : Range{
                             solid_.nb_polyhedron_vertices( polyhedron_id ) } )
                    {
                        this->add_viewed_vertex(
                            solid_.polyhedron_vertex( { polyhedron_id, v } ) );
                    }

                    for( const auto f :
                        Range{ solid_.nb_polyhedron_facets( polyhedron_id ) } )
                    {
                        PolyhedronFacet facet{ polyhedron_id, f };
                        this->add_viewed_facet(
                            solid_.polyhedron_facet( facet ) );
                        const auto nb_vertices =
                            solid_.nb_polyhedron_facet_vertices( facet );
                        for( const auto v : Range{ nb_vertices } )
                        {
                            const auto v0 =
                                solid_.polyhedron_facet_vertex( { facet, v } );
                            const auto v1 = solid_.polyhedron_facet_vertex(
                                { facet, ( v + 1 ) % nb_vertices } );
                            this->add_viewed_edge(
                                solid_.edge_from_vertices( { { v0, v1 } } )
                                    .value() );
                        }
                    }
                }
                return polyhedra2view_.at( polyhedron_id );
            }

        private:
            PolyhedronVertex viewed_polyhedron_vertex(
                const PolyhedronVertex& polyhedron_vertex ) const
            {
                return { viewed_polyhedron( polyhedron_vertex.polyhedron_id ),
                    polyhedron_vertex.vertex_id };
            }

            PolyhedronFacet viewed_polyhedron_facet(
                const PolyhedronFacet& polyhedron_facet ) const
            {
                return { viewed_polyhedron( polyhedron_facet.polyhedron_id ),
                    polyhedron_facet.facet_id };
            }

        private:
            const SolidMesh< dimension >& solid_;
            SolidMesh< dimension >& solid_view_;
            std::shared_ptr< VariableAttribute< index_t > > view2polyhedra_;
            absl::flat_hash_map< index_t, index_t > polyhedra2view_;
            std::shared_ptr< VariableAttribute< index_t > > view2facets_;
            mutable std::shared_ptr<
                VariableAttribute< PolyhedronFacetVertices > >
                facet_vertices_;
            absl::flat_hash_map< index_t, index_t > facets2view_;
        };
    } // namespace detail
} // namespace geode
