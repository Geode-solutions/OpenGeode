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

#include <geode/model/helpers/detail/cut_along_internal_lines.h>

#include <async++.h>

#include <absl/memory/memory.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/helpers/component_mesh_edges.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    namespace detail
    {
        template < typename Model >
        class CutAlongInternalLines< Model >::Impl
        {
            using CMVmapping =
                std::pair< ComponentMeshVertex, ComponentMeshVertex >;
            using CMVmappings = std::vector< CMVmapping >;
            using Task = async::task< CMVmappings >;
            using ModelBuilder = typename Model::Builder;
            static constexpr auto dimension = Model::dim;
            struct SurfaceInfo
            {
                SurfaceInfo( index_t nb_vertices )
                    : polygon_vertices( nb_vertices )
                {
                }
                absl::FixedArray< PolygonsAroundVertex > polygon_vertices;
                std::vector< index_t > vertices_to_check;
            };

        public:
            Impl( Model& model )
                : model_( model ),
                  builder_ptr_{ absl::make_unique< ModelBuilder >( model ) },
                  builder_( *builder_ptr_ )
            {
            }

            Impl( const Model& model, ModelBuilder& builder )
                : model_( model ), builder_( builder )
            {
            }

            CMVmappings cut()
            {
                absl::FixedArray< Task > tasks( model_.nb_surfaces() );
                index_t count{ 0 };
                for( const auto& surface : model_.surfaces() )
                {
                    tasks[count++] = async::spawn( [this, &surface] {
                        return split_points( surface );
                    } );
                }
                CMVmappings mapping;
                async::when_all( tasks )
                    .then( [this, &mapping]( std::vector< Task > all_task ) {
                        for( auto& task : all_task )
                        {
                            auto cmv_mappings = task.get();
                            update_unique_vertices( cmv_mappings );
                            mapping.insert( mapping.end(),
                                std::make_move_iterator( cmv_mappings.begin() ),
                                std::make_move_iterator( cmv_mappings.end() ) );
                        }
                    } )
                    .get();
                return mapping;
            }

            CMVmappings cut_surface( const Surface< dimension >& surface )
            {
                const auto mapping = split_points( surface );
                update_unique_vertices( mapping );
                return mapping;
            }

        private:
            void update_unique_vertices( const CMVmappings& mapping )
            {
                for( const auto& cmv_mapping : mapping )
                {
                    const auto unique_vertex_id =
                        model_.unique_vertex( cmv_mapping.first );
                    builder_.set_unique_vertex(
                        cmv_mapping.second, unique_vertex_id );
                }
            }

            CMVmappings split_points( const Surface< dimension >& surface )
            {
                auto builder = builder_.surface_mesh_builder( surface.id() );
                remove_adjacencies_along_internal_lines( surface, *builder );
                return duplicate_points( surface, *builder );
            }

            CMVmappings duplicate_points( const Surface< dimension >& surface,
                SurfaceMeshBuilder< dimension >& builder )
            {
                const auto& mesh = surface.mesh();
                const auto info = compute_surface_info( mesh );
                CMVmappings mapping;
                mapping.reserve( info.vertices_to_check.size() );
                for( const auto vertex_id : info.vertices_to_check )
                {
                    auto polygons_around =
                        mesh.polygons_around_vertex( vertex_id );
                    const auto& polygon_vertices =
                        info.polygon_vertices[vertex_id];
                    auto nb_polygons_around = polygons_around.size();
                    OPENGEODE_ASSERT(
                        nb_polygons_around <= polygon_vertices.size(),
                        "[CutAlongInternalLines] Wrong size comparison" );
                    PolygonsAroundVertex total_polygons;
                    while( nb_polygons_around != polygon_vertices.size() )
                    {
                        for( auto& polygon : polygons_around )
                        {
                            total_polygons.emplace_back( std::move( polygon ) );
                        }
                        mapping.emplace_back(
                            process_component( surface, mesh, builder,
                                vertex_id, total_polygons, polygon_vertices ) );
                        polygons_around =
                            mesh.polygons_around_vertex( vertex_id );
                        nb_polygons_around += polygons_around.size();
                    }
                }
                return mapping;
            }

            SurfaceInfo compute_surface_info(
                const SurfaceMesh< dimension >& mesh ) const
            {
                SurfaceInfo info{ mesh.nb_vertices() };
                std::vector< bool > vertex_to_check(
                    mesh.nb_vertices(), false );
                for( const auto p : Range{ mesh.nb_polygons() } )
                {
                    for( const auto v :
                        LRange{ mesh.nb_polygon_vertices( p ) } )
                    {
                        PolygonVertex vertex{ p, v };
                        const auto vertex_id = mesh.polygon_vertex( vertex );
                        if( mesh.is_edge_on_border( vertex ) )
                        {
                            vertex_to_check[vertex_id] = true;
                        }
                        info.polygon_vertices[vertex_id].emplace_back(
                            std::move( vertex ) );
                    }
                }
                for( const auto v : Indices{ vertex_to_check } )
                {
                    if( vertex_to_check[v] )
                    {
                        info.vertices_to_check.push_back( v );
                    }
                }
                return info;
            }

            void remove_adjacencies_along_internal_lines(
                const Surface< dimension >& surface,
                SurfaceMeshBuilder< dimension >& builder )
            {
                std::vector< PolygonEdge > edges;
                for( const auto& line : model_.internal_lines( surface ) )
                {
                    const auto& mesh = line.mesh();
                    for( const auto e : Range{ mesh.nb_edges() } )
                    {
                        const auto model_edges =
                            component_mesh_edges( model_, line, e );
                        const auto& surface_edges = model_edges.surface_edges;
                        const auto it = surface_edges.find( surface.id() );
                        if( it == surface_edges.end() )
                        {
                            continue;
                        }
                        for( auto& edge : it->second )
                        {
                            edges.emplace_back( std::move( edge ) );
                        }
                    }
                }
                for( const auto& edge : edges )
                {
                    builder.unset_polygon_adjacent( edge );
                }
            }

            CMVmapping process_component( const Surface< dimension >& surface,
                const SurfaceMesh< dimension >& mesh,
                SurfaceMeshBuilder< dimension >& builder,
                index_t vertex_id,
                const PolygonsAroundVertex& total_polygons,
                const PolygonsAroundVertex& polygon_vertices )
            {
                const auto new_vertex_id =
                    builder.create_point( mesh.point( vertex_id ) );
                builder.replace_vertex( vertex_id, new_vertex_id );
                for( const auto& polygon_vertex : polygon_vertices )
                {
                    if( absl::c_find( total_polygons, polygon_vertex )
                        == total_polygons.end() )
                    {
                        builder.associate_polygon_vertex_to_vertex(
                            polygon_vertex, vertex_id );
                        break;
                    }
                }
                OPENGEODE_ASSERT(
                    !mesh.polygons_around_vertex( vertex_id ).empty(),
                    "[ModelFromMeshBuilder::cut_surface_by_lines] Lost "
                    "polygon around vertex" );
                return {
                    ComponentMeshVertex{ surface.component_id(), vertex_id },
                    ComponentMeshVertex{ surface.component_id(), new_vertex_id }
                };
            }

        private:
            const Model& model_;
            std::unique_ptr< ModelBuilder > builder_ptr_;
            ModelBuilder& builder_;
        };

        template < typename Model >
        CutAlongInternalLines< Model >::CutAlongInternalLines( Model& model )
            : impl_{ model }
        {
        }

        template < typename Model >
        CutAlongInternalLines< Model >::CutAlongInternalLines(
            const Model& model, typename Model::Builder& builder )
            : impl_{ model, builder }
        {
        }

        template < typename Model >
        CutAlongInternalLines< Model >::~CutAlongInternalLines()
        {
        }

        template < typename Model >
        std::vector< std::pair< ComponentMeshVertex, ComponentMeshVertex > >
            CutAlongInternalLines< Model >::cut_all_surfaces()
        {
            return impl_->cut();
        }

        template < typename Model >
        std::vector< std::pair< ComponentMeshVertex, ComponentMeshVertex > >
            CutAlongInternalLines< Model >::cut_surface(
                const Surface< Model::dim >& surface )
        {
            return impl_->cut_surface( surface );
        }

        template class opengeode_model_api CutAlongInternalLines< Section >;
        template class opengeode_model_api CutAlongInternalLines< BRep >;
    } // namespace detail
} // namespace geode