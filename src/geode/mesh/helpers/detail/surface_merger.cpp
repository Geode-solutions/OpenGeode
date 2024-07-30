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
    using Edge = geode::detail::VertexCycle< std::array< geode::index_t, 2 > >;

    template < geode::index_t dimension >
    absl::flat_hash_map< Edge, std::vector< geode::PolygonEdge > >
        edge_to_polygons_around( const geode::SurfaceMesh< dimension >& mesh )
    {
        absl::flat_hash_map< Edge, std::vector< geode::PolygonEdge > >
            polygons_around_edges;
        for( const auto polygon_id : geode::Range{ mesh.nb_polygons() } )
        {
            const auto& vertices = mesh.polygon_vertices( polygon_id );
            for( const auto polygon_edge_id :
                geode::LRange{ mesh.nb_polygon_edges( polygon_id ) } )
            {
                const auto next_vertex = polygon_edge_id == vertices.size() - 1
                                             ? vertices[0]
                                             : vertices[polygon_edge_id + 1];
                const Edge polygon_edge_vertex_cycle{
                    { vertices[polygon_edge_id], next_vertex }
                };
                polygons_around_edges[polygon_edge_vertex_cycle].emplace_back(
                    polygon_id, polygon_edge_id );
            }
        }
        return polygons_around_edges;
    }

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
                merger.builder().compute_polygon_adjacencies();
            }

            void separate_surfaces( SurfaceMeshMerger< dimension >& merger )
            {
                for( const auto p : Range{ merger.mesh().nb_polygons() } )
                {
                    for( const auto e :
                        LRange{ merger.mesh().nb_polygon_edges( p ) } )
                    {
                        if( const auto adj =
                                merger.mesh().polygon_adjacent( { p, e } ) )
                        {
                            if( surface_id_[p] != surface_id_[adj.value()] )
                            {
                                merger.builder().unset_polygon_adjacent(
                                    { p, e } );
                            }
                        }
                    }
                }
                for( const auto& edge :
                    ::edge_to_polygons_around( merger.mesh() ) )
                {
                    if( edge.second.size() < 3 )
                    {
                        continue;
                    }
                    for( const auto& polygon_edge : edge.second )
                    {
                        merger.builder().unset_polygon_adjacent( polygon_edge );
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
