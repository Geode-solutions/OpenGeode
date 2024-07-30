/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/mesh/helpers/detail/surface_merger.hpp>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/detail/vertex_cycle.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/repair_polygon_orientations.hpp>

namespace
{
    bool is_polygon_degenerated( absl::Span< const geode::index_t > vertices )
    {
        for( const auto v : geode::LIndices{ vertices } )
        {
            const auto vertex = vertices[v];
            for( const auto other_v : geode::LIndices{ vertices } )
            {
                if( v == other_v )
                {
                    continue;
                }
                if( vertex == vertices[other_v] )
                {
                    return true;
                }
            }
        }
        return false;
    }
} // namespace

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class SurfaceMeshMerger< dimension >::Impl
        {
            using Polygon = absl::FixedArray< index_t >;
            using TypedVertexCycle = detail::VertexCycle< Polygon >;
            using SurfaceId = absl::flat_hash_set< index_t >;

        public:
            Impl( absl::Span< const std::reference_wrapper<
                    const SurfaceMesh< dimension > > > surfaces )
                : new_id_( surfaces.size() )
            {
                index_t nb_polygons{ 0 };
                for( const auto s : Indices{ surfaces } )
                {
                    const auto& surface = surfaces[s].get();
                    new_id_[s].resize( surface.nb_polygons(), NO_ID );
                    nb_polygons += surface.nb_polygons();
                }
                polygons_origins_.reserve( nb_polygons );
                surface_id_.reserve( nb_polygons );
            }

            std::unique_ptr< SurfaceMesh< dimension > > merge(
                SurfaceMeshMerger< dimension >& merger )
            {
                merger.create_points();
                create_polygons( merger );
                create_adjacencies( merger );
                clean_surface( merger );
                surface_id_.clear();
                return merger.steal_mesh();
            }

            index_t polygon_in_merged(
                const SurfaceMeshMerger< dimension >& merger,
                index_t surface,
                index_t polygon ) const
            {
                geode_unused( merger );
                OPENGEODE_ASSERT( surface < merger.meshes().size(),
                    "[SurfaceMerger::polygon_in_merged] Wrong surface index" );
                OPENGEODE_ASSERT(
                    polygon < merger.meshes()[surface].get().nb_polygons(),
                    "[SurfaceMerger::polygon_in_merged] Wrong surface polygon "
                    "index" );
                return new_id_[surface][polygon];
            }

            const PolygonOrigins& polygon_origins( index_t polygon ) const
            {
                return polygons_origins_[polygon];
            }

        private:
            void clean_surface( SurfaceMeshMerger< dimension >& merger )
            {
                separate_surfaces( merger );
                repair_polygon_orientations( merger.mesh(), merger.builder() );
            }

            void create_polygons( SurfaceMeshMerger< dimension >& merger )
            {
                absl::flat_hash_map< TypedVertexCycle, index_t > polygons;
                const auto& meshes = merger.meshes();
                for( const auto s : Indices{ merger.meshes() } )
                {
                    const auto& surface = meshes[s].get();
                    for( const auto p : Range{ surface.nb_polygons() } )
                    {
                        Polygon vertices( surface.nb_polygon_vertices( p ) );
                        for( const auto v :
                            LRange{ surface.nb_polygon_vertices( p ) } )
                        {
                            vertices[v] = merger.vertex_in_merged(
                                s, surface.polygon_vertex( { p, v } ) );
                        }
                        if( is_polygon_degenerated( vertices ) )
                        {
                            continue;
                        }
                        const auto it =
                            polygons.try_emplace( TypedVertexCycle{ vertices },
                                merger.mesh().nb_polygons() );
                        if( it.second )
                        {
                            const auto polygon_id =
                                merger.builder().create_polygon( vertices );
                            OPENGEODE_ASSERT( polygon_id == surface_id_.size(),
                                "[SurfaceMerger::create_polygons] Issue in "
                                "polygon database (surface_id_)" );
                            surface_id_.emplace_back();
                            surface_id_.back().insert( s );
                            new_id_[s][p] = polygon_id;
                            OPENGEODE_ASSERT(
                                polygon_id == polygons_origins_.size(),
                                "[SurfaceMerger::create_polygons] Issue in "
                                "polygon database (polygons_origins_)" );
                            polygons_origins_.emplace_back();
                            polygons_origins_.back().emplace_back( s, p );
                        }
                        else
                        {
                            const auto polygon_id = it.first->second;
                            OPENGEODE_ASSERT( polygon_id < surface_id_.size(),
                                "[SurfaceMerger::create_polygons] Issue in "
                                "polygon database (surface_id_)" );
                            surface_id_[polygon_id].insert( s );
                            new_id_[s][p] = polygon_id;
                            OPENGEODE_ASSERT(
                                polygon_id < polygons_origins_.size(),
                                "[SurfaceMerger::create_polygons] Issue in "
                                "polygon database (polygons_origins_)" );
                            polygons_origins_[polygon_id].emplace_back( s, p );
                        }
                    }
                }
            }

            void create_adjacencies( SurfaceMeshMerger< dimension >& merger )
            {
                merger.builder().compute_polygon_adjacencies();
                for( const auto p : Range{ merger.mesh().nb_polygons() } )
                {
                    for( const auto e :
                        LRange{ merger.mesh().nb_polygon_edges( p ) } )
                    {
                        const PolygonEdge edge{ p, e };
                        const auto adj =
                            merger.mesh().polygon_adjacent_edge( edge );
                        if( !adj )
                        {
                            continue;
                        }
                        const auto edge_vertices =
                            merger.mesh().polygon_edge_vertices( edge );
                        bool keep_adj{ false };
                        for( const auto& origin : polygons_origins_[p] )
                        {
                            const auto edge_origin = find_edge_origin(
                                merger, edge_vertices, origin, e );
                            const auto& surface =
                                merger.meshes()[origin.surface].get();
                            if( !surface.is_edge_on_border( edge_origin ) )
                            {
                                keep_adj = true;
                                break;
                            }
                        }
                        if( !keep_adj )
                        {
                            merger.builder().unset_polygon_adjacent( edge );
                            merger.builder().unset_polygon_adjacent(
                                adj.value() );
                        }
                    }
                }
            }

            PolygonEdge find_edge_origin(
                SurfaceMeshMerger< dimension >& merger,
                const std::array< index_t, 2 >& merged_edge_vertices,
                const PolygonOrigin& origin,
                local_index_t hint ) const
            {
                const auto& surface = merger.meshes()[origin.surface].get();
                const auto is_same_edge = [&merger, &merged_edge_vertices,
                                              &surface,
                                              &origin]( const auto& edge ) {
                    for( const auto vertex :
                        surface.polygon_edge_vertices( edge ) )
                    {
                        const auto merged_vertex =
                            merger.vertex_in_merged( origin.surface, vertex );
                        if( merged_vertex != merged_edge_vertices[0]
                            && merged_vertex != merged_edge_vertices[1] )
                        {
                            return false;
                        }
                    }
                    return true;
                };
                const PolygonEdge hint_edge{ origin.polygon, hint };
                if( is_same_edge( hint_edge ) )
                {
                    return hint_edge;
                }
                for( const auto& e :
                    LRange{ surface.nb_polygon_edges( origin.polygon ) } )
                {
                    if( e == hint )
                    {
                        continue;
                    }
                    const PolygonEdge edge{ origin.polygon, e };
                    if( is_same_edge( edge ) )
                    {
                        return edge;
                    }
                }
                OPENGEODE_ASSERT_NOT_REACHED(
                    "[SurfaceMerger::find_edge_origin] Edge not found" );
                return hint_edge;
            }

            void separate_surfaces( SurfaceMeshMerger< dimension >& merger )
            {
                for( const auto p : Range{ merger.mesh().nb_polygons() } )
                {
                    for( const auto e :
                        LRange{ merger.mesh().nb_polygon_edges( p ) } )
                    {
                        const auto adj =
                            merger.mesh().polygon_adjacent( { p, e } );
                        if( !adj )
                        {
                            continue;
                        }
                        if( surface_id_[p] != surface_id_[adj.value()] )
                        {
                            merger.builder().unset_polygon_adjacent( { p, e } );
                        }
                    }
                }
            }

        private:
            std::vector< SurfaceId > surface_id_;
            absl::FixedArray< std::vector< index_t > > new_id_;
            std::vector< PolygonOrigins > polygons_origins_;
        };

        template < index_t dimension >
        SurfaceMeshMerger< dimension >::SurfaceMeshMerger(
            absl::Span< const std::reference_wrapper<
                const SurfaceMesh< dimension > > > surfaces,
            double epsilon )
            : VertexMerger< SurfaceMesh< dimension > >{ surfaces, epsilon },
              impl_{ surfaces }
        {
        }

        template < index_t dimension >
        SurfaceMeshMerger< dimension >::~SurfaceMeshMerger() = default;

        template < index_t dimension >
        std::unique_ptr< SurfaceMesh< dimension > >
            SurfaceMeshMerger< dimension >::merge()
        {
            return impl_->merge( *this );
        }

        template < index_t dimension >
        index_t SurfaceMeshMerger< dimension >::polygon_in_merged(
            index_t surface, index_t polygon ) const
        {
            return impl_->polygon_in_merged( *this, surface, polygon );
        }

        template < index_t dimension >
        auto SurfaceMeshMerger< dimension >::polygon_origins(
            index_t polygon ) const -> const PolygonOrigins&
        {
            return impl_->polygon_origins( polygon );
        }

        template class opengeode_mesh_api SurfaceMeshMerger< 2 >;
        template class opengeode_mesh_api SurfaceMeshMerger< 3 >;
    } // namespace detail
} // namespace geode
