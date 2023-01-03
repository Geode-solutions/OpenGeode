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

#include <geode/mesh/helpers/detail/surface_merger.h>

#include <absl/container/flat_hash_set.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/nn_search.h>
#include <geode/geometry/point.h>

#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/surface_mesh.h>
#include <geode/mesh/helpers/repair_polygon_orientations.h>

namespace
{
    template < geode::index_t dimension >
    std::unique_ptr< geode::SurfaceMesh< dimension > > create_mesh( absl::Span<
        const std::reference_wrapper< const geode::SurfaceMesh< dimension > > >
            surfaces )
    {
        const auto type = surfaces.front().get().type_name();
        for( const auto& surface : surfaces )
        {
            if( surface.get().type_name() != type )
            {
                return geode::SurfaceMesh< dimension >::create();
            }
        }
        return geode::SurfaceMesh< dimension >::create(
            geode::MeshFactory::default_impl( type ) );
    }
} // namespace

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class SurfaceMeshMerger< dimension >::Impl
        {
            using ColocatedInfo = typename NNSearch< dimension >::ColocatedInfo;
            using Polygon = absl::FixedArray< index_t >;
            using TypedVertexCycle = detail::VertexCycle< Polygon >;
            using SurfaceId = absl::flat_hash_set< index_t >;

        public:
            Impl( absl::Span< const std::reference_wrapper<
                      const SurfaceMesh< dimension > > > surfaces,
                double epsilon )
                : surfaces_( surfaces ),
                  epsilon_( epsilon ),
                  surface_{ create_mesh( surfaces ) },
                  builder_{ SurfaceMeshBuilder< dimension >::create(
                      *surface_ ) },
                  new_id_( surfaces.size() ),
                  offset_vertices_( surfaces.size() + 1 )
            {
                offset_vertices_[0] = 0;
                index_t nb_polygons{ 0 };
                for( const auto s : Indices{ surfaces_ } )
                {
                    const auto& surface = surfaces_[s].get();
                    new_id_[s].resize( surface.nb_polygons(), NO_ID );
                    offset_vertices_[s + 1] =
                        offset_vertices_[s] + surface.nb_vertices();
                    nb_polygons += surface.nb_polygons();
                }
                polygons_origins_.reserve( nb_polygons );
                surface_id_.reserve( nb_polygons );
            }

            std::unique_ptr< SurfaceMesh< dimension > > merge()
            {
                create_surface_step();
                return std::move( surface_ );
            }

            void create_surface_step()
            {
                auto info = create_colocated_index_mapping();
                create_points( info );
                vertices_ = std::move( info.colocated_mapping );
                create_polygons();
                create_adjacencies();
                clean_surface();
                surface_id_.clear();
            }

            const SurfaceMesh< dimension >& merged() const
            {
                return *surface_;
            }

            index_t vertex_in_merged( index_t surface, index_t vertex ) const
            {
                OPENGEODE_ASSERT( surface < surfaces_.size(),
                    "[SurfaceMerger::vertex_in_merged] Wrong surface index" );
                OPENGEODE_ASSERT(
                    vertex < surfaces_[surface].get().nb_vertices(),
                    "[SurfaceMerger::vertex_in_merged] Wrong surface vertex "
                    "index" );
                OPENGEODE_ASSERT(
                    offset_vertices_[surface] + vertex < vertices_.size(),
                    "[SurfaceMerger::vertex_in_merged] Wrong global vertex "
                    "index" );
                return vertices_[offset_vertices_[surface] + vertex];
            }

            index_t polygon_in_merged( index_t surface, index_t polygon ) const
            {
                OPENGEODE_ASSERT( surface < surfaces_.size(),
                    "[SurfaceMerger::polygon_in_merged] Wrong surface index" );
                OPENGEODE_ASSERT(
                    polygon < surfaces_[surface].get().nb_polygons(),
                    "[SurfaceMerger::polygon_in_merged] Wrong surface polygon "
                    "index" );
                return new_id_[surface][polygon];
            }

            const PolygonOrigins& polygon_origins( index_t polygon ) const
            {
                OPENGEODE_ASSERT( polygon < surface_->nb_polygons(),
                    "[SurfaceMerger::polygon_origins] Wrong surface polygon "
                    "index" );
                return polygons_origins_[polygon];
            }

        private:
            void clean_surface()
            {
                bool delete_needed{ false };
                std::vector< bool > to_delete( surface_->nb_polygons(), false );
                for( const auto p : Range{ surface_->nb_polygons() } )
                {
                    const auto vertices = surface_->polygon_vertices( p );
                    for( const auto v : LIndices{ vertices } )
                    {
                        if( vertices[v]
                            == vertices[v == vertices.size() - 1 ? 0 : v + 1] )
                        {
                            to_delete[p] = true;
                            delete_needed = true;
                        }
                    }
                }
                if( delete_needed )
                {
                    const auto old2new = builder_->delete_polygons( to_delete );
                    for( const auto s : Indices{ surfaces_ } )
                    {
                        const auto& surface = surfaces_[s].get();
                        for( const auto p : Range{ surface.nb_polygons() } )
                        {
                            const auto old = new_id_[s][p];
                            new_id_[s][p] = old2new[old];
                        }
                    }
                }
                separate_surfaces();
                repair_polygon_orientations( *surface_ );
            }

            void create_points( const ColocatedInfo& info )
            {
                for( const auto& point : info.unique_points )
                {
                    builder_->create_point( point );
                }
            }

            void create_polygons()
            {
                absl::flat_hash_map< TypedVertexCycle, index_t > polygons;
                for( const auto s : Indices{ surfaces_ } )
                {
                    const auto& surface = surfaces_[s].get();
                    for( const auto p : Range{ surface.nb_polygons() } )
                    {
                        absl::FixedArray< index_t > vertices(
                            surface.nb_polygon_vertices( p ) );
                        for( const auto v :
                            LRange{ surface.nb_polygon_vertices( p ) } )
                        {
                            vertices[v] = vertex_in_merged(
                                s, surface.polygon_vertex( { p, v } ) );
                        }
                        const auto it = polygons.try_emplace(
                            vertices, surface_->nb_polygons() );
                        if( it.second )
                        {
                            const auto polygon_id =
                                builder_->create_polygon( vertices );
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

            void create_adjacencies()
            {
                for( const auto s : Indices{ surfaces_ } )
                {
                    const auto& surface = surfaces_[s].get();
                    for( const auto p : Range{ surface.nb_polygons() } )
                    {
                        for( const auto e :
                            LRange{ surface.nb_polygon_edges( p ) } )
                        {
                            if( const auto adj =
                                    surface.polygon_adjacent( { p, e } ) )
                            {
                                const auto new_id = new_id_[s][p];
                                const auto new_adj_id = new_id_[s][adj.value()];
                                builder_->set_polygon_adjacent(
                                    { new_id, e }, new_adj_id );
                            }
                        }
                    }
                }
            }

            void separate_surfaces()
            {
                for( const auto p : Range{ surface_->nb_polygons() } )
                {
                    for( const auto e :
                        LRange{ surface_->nb_polygon_edges( p ) } )
                    {
                        if( const auto adj =
                                surface_->polygon_adjacent( { p, e } ) )
                        {
                            if( surface_id_[p] != surface_id_[adj.value()] )
                            {
                                builder_->unset_polygon_adjacent( { p, e } );
                            }
                        }
                    }
                }
            }

            ColocatedInfo create_colocated_index_mapping()
            {
                index_t nb_points{ 0 };
                for( const auto& surface : surfaces_ )
                {
                    nb_points += surface.get().nb_vertices();
                }
                std::vector< Point< dimension > > points;
                points.reserve( nb_points );
                for( const auto& surface : surfaces_ )
                {
                    for( const auto v : Range{ surface.get().nb_vertices() } )
                    {
                        points.emplace_back( surface.get().point( v ) );
                    }
                }
                NNSearch< dimension > nnsearch{ std::move( points ) };
                return nnsearch.colocated_index_mapping( epsilon_ );
            }

        private:
            absl::Span<
                const std::reference_wrapper< const SurfaceMesh< dimension > > >
                surfaces_;
            double epsilon_;
            std::unique_ptr< SurfaceMesh< dimension > > surface_;
            std::unique_ptr< SurfaceMeshBuilder< dimension > > builder_;
            std::vector< SurfaceId > surface_id_;
            absl::FixedArray< std::vector< index_t > > new_id_;
            std::vector< index_t > vertices_;
            absl::FixedArray< index_t > offset_vertices_;
            std::vector< PolygonOrigins > polygons_origins_;
        };

        template < index_t dimension >
        SurfaceMeshMerger< dimension >::SurfaceMeshMerger(
            absl::Span< const std::reference_wrapper<
                const SurfaceMesh< dimension > > > surfaces,
            double epsilon )
            : impl_( surfaces, epsilon )
        {
        }

        template < index_t dimension >
        SurfaceMeshMerger< dimension >::~SurfaceMeshMerger()
        {
        }

        template < index_t dimension >
        std::unique_ptr< SurfaceMesh< dimension > >
            SurfaceMeshMerger< dimension >::merge()
        {
            return impl_->merge();
        }

        template < index_t dimension >
        void SurfaceMeshMerger< dimension >::create_surface_step()
        {
            impl_->create_surface_step();
        }

        template < index_t dimension >
        const SurfaceMesh< dimension >&
            SurfaceMeshMerger< dimension >::merged() const
        {
            return impl_->merged();
        }

        template < index_t dimension >
        index_t SurfaceMeshMerger< dimension >::vertex_in_merged(
            index_t surface, index_t vertex ) const
        {
            return impl_->vertex_in_merged( surface, vertex );
        }

        template < index_t dimension >
        index_t SurfaceMeshMerger< dimension >::polygon_in_merged(
            index_t surface, index_t polygon ) const
        {
            return impl_->polygon_in_merged( surface, polygon );
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
