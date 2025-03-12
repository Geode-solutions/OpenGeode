/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/mesh/helpers/detail/solid_merger.hpp>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/core/detail/vertex_cycle.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

namespace
{
    bool is_polyhedron_degenerated(
        absl::Span< const geode::index_t > vertices )
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
        class SolidMeshMerger< dimension >::Impl
        {
            using Polyhedron = absl::FixedArray< index_t >;
            using TypedVertexCycle = detail::VertexCycle< Polyhedron >;
            using SolidId = absl::flat_hash_set< index_t >;

        public:
            Impl( absl::Span< const std::reference_wrapper<
                    const SolidMesh< dimension > > > solids )
                : new_id_( solids.size() )
            {
                index_t nb_polyhedra{ 0 };
                for( const auto s : Indices{ solids } )
                {
                    const auto& solid = solids[s].get();
                    new_id_[s].resize( solid.nb_polyhedra(), NO_ID );
                    nb_polyhedra += solid.nb_polyhedra();
                }
                polyhedra_origins_.reserve( nb_polyhedra );
                solid_id_.reserve( nb_polyhedra );
            }

            std::unique_ptr< SolidMesh< dimension > > merge(
                SolidMeshMerger< dimension >& merger, const double epsilon )
            {
                merger.create_points( epsilon );
                create_polyhedra( merger );
                create_adjacencies( merger );
                clean_solid( merger );
                solid_id_.clear();
                return merger.steal_mesh();
            }

            std::unique_ptr< SolidMesh< dimension > > merge(
                SolidMeshMerger< dimension >& merger,
                const Frame3D& epsilons_frame )
            {
                merger.create_points( epsilons_frame );
                create_polyhedra( merger );
                create_adjacencies( merger );
                clean_solid( merger );
                solid_id_.clear();
                return merger.steal_mesh();
            }

            index_t polyhedron_in_merged(
                const SolidMeshMerger< dimension >& merger,
                index_t solid,
                index_t polyhedron ) const
            {
                geode_unused( merger );
                OPENGEODE_ASSERT( solid < merger.meshes().size(),
                    "[SolidMerger::polyhedron_in_merged] Wrong solid index" );
                OPENGEODE_ASSERT(
                    polyhedron < merger.meshes()[solid].get().nb_polyhedra(),
                    "[SolidMerger::polyhedron_in_merged] Wrong solid "
                    "polyhedron "
                    "index" );
                return new_id_[solid][polyhedron];
            }

            const PolyhedronOrigins& polyhedron_origins(
                index_t polyhedron ) const
            {
                return polyhedra_origins_[polyhedron];
            }

        private:
            void clean_solid( SolidMeshMerger< dimension >& merger )
            {
                separate_solids( merger );
            }

            void create_polyhedra( SolidMeshMerger< dimension >& merger )
            {
                absl::flat_hash_map< TypedVertexCycle, index_t > polyhedra;
                const auto& meshes = merger.meshes();
                for( const auto s : Indices{ merger.meshes() } )
                {
                    const auto& solid = meshes[s].get();
                    for( const auto p : Range{ solid.nb_polyhedra() } )
                    {
                        Polyhedron vertices(
                            solid.nb_polyhedron_vertices( p ) );
                        for( const auto v : LIndices{ vertices } )
                        {
                            vertices[v] = merger.vertex_in_merged(
                                s, solid.polyhedron_vertex( { p, v } ) );
                        }
                        if( is_polyhedron_degenerated( vertices ) )
                        {
                            continue;
                        }
                        const auto it =
                            polyhedra.try_emplace( TypedVertexCycle{ vertices },
                                merger.mesh().nb_polyhedra() );
                        if( it.second )
                        {
                            absl::FixedArray< std::vector< local_index_t > >
                                facets( solid.nb_polyhedron_facets( p ) );
                            for( const auto f : LIndices{ facets } )
                            {
                                const PolyhedronFacet facet{ p, f };
                                auto& facet_vertices = facets[f];
                                facet_vertices.resize(
                                    solid.nb_polyhedron_facet_vertices(
                                        facet ) );
                                for( const auto v : LIndices{ facet_vertices } )
                                {
                                    facet_vertices[v] =
                                        solid
                                            .polyhedron_facet_vertex_id(
                                                { facet, v } )
                                            .vertex_id;
                                }
                            }
                            const auto polyhedron_id =
                                merger.builder().create_polyhedron(
                                    vertices, facets );
                            OPENGEODE_ASSERT( polyhedron_id == solid_id_.size(),
                                "[SolidMerger::create_polyhedra] Issue in "
                                "polyhedron database (solid_id_)" );
                            solid_id_.emplace_back();
                            solid_id_.back().insert( s );
                            new_id_[s][p] = polyhedron_id;
                            OPENGEODE_ASSERT(
                                polyhedron_id == polyhedra_origins_.size(),
                                "[SolidMerger::create_polyhedra] Issue in "
                                "polyhedron database (polyhedra_origins_)" );
                            polyhedra_origins_.emplace_back();
                            polyhedra_origins_.back().emplace_back( s, p );
                        }
                        else
                        {
                            const auto polyhedron_id = it.first->second;
                            OPENGEODE_ASSERT( polyhedron_id < solid_id_.size(),
                                "[SolidMerger::create_polyhedra] Issue in "
                                "polyhedron database (solid_id_)" );
                            solid_id_[polyhedron_id].insert( s );
                            new_id_[s][p] = polyhedron_id;
                            OPENGEODE_ASSERT(
                                polyhedron_id < polyhedra_origins_.size(),
                                "[SolidMerger::create_polyhedra] Issue in "
                                "polyhedron database (polyhedra_origins_)" );
                            polyhedra_origins_[polyhedron_id].emplace_back(
                                s, p );
                        }
                    }
                }
            }

            void create_adjacencies( SolidMeshMerger< dimension >& merger )
            {
                merger.builder().compute_polyhedron_adjacencies();
                for( const auto p : Range{ merger.mesh().nb_polyhedra() } )
                {
                    for( const auto f :
                        LRange{ merger.mesh().nb_polyhedron_facets( p ) } )
                    {
                        const PolyhedronFacet facet{ p, f };
                        const auto adj =
                            merger.mesh().polyhedron_adjacent_facet( facet );
                        if( !adj )
                        {
                            continue;
                        }
                        const auto facet_vertices =
                            merger.mesh().polyhedron_facet_vertices( facet );
                        bool keep_adj{ false };
                        for( const auto& origin : polyhedra_origins_[p] )
                        {
                            const auto facet_origin = find_facet_origin(
                                merger, facet_vertices, origin, f );
                            const auto& meshes = merger.meshes();
                            const auto& solid = meshes[origin.solid].get();
                            if( !solid.is_polyhedron_facet_on_border(
                                    facet_origin ) )
                            {
                                keep_adj = true;
                                break;
                            }
                        }
                        if( !keep_adj )
                        {
                            merger.builder().unset_polyhedron_adjacent( facet );
                            merger.builder().unset_polyhedron_adjacent(
                                adj.value() );
                        }
                    }
                }
            }

            PolyhedronFacet find_facet_origin(
                SolidMeshMerger< dimension >& merger,
                const PolyhedronFacetVertices& merged_facet_vertices,
                const PolyhedronOrigin& origin,
                local_index_t hint ) const
            {
                using Facet = VertexCycle< PolyhedronFacetVertices >;
                const Facet merged_cycle{ merged_facet_vertices };
                const auto& meshes = merger.meshes();
                const auto& solid = meshes[origin.solid].get();
                const auto is_same_facet = [&merger, &merged_cycle, &solid,
                                               &origin]( const auto& facet ) {
                    PolyhedronFacetVertices updated_vertices;
                    for( const auto vertex :
                        solid.polyhedron_facet_vertices( facet ) )
                    {
                        updated_vertices.push_back(
                            merger.vertex_in_merged( origin.solid, vertex ) );
                    }
                    return Facet{ updated_vertices } == merged_cycle;
                };
                const PolyhedronFacet hint_facet{ origin.polyhedron, hint };
                if( is_same_facet( hint_facet ) )
                {
                    return hint_facet;
                }
                for( const auto& f :
                    LRange{ solid.nb_polyhedron_facets( origin.polyhedron ) } )
                {
                    if( f == hint )
                    {
                        continue;
                    }
                    const PolyhedronFacet facet{ origin.polyhedron, f };
                    if( is_same_facet( facet ) )
                    {
                        return facet;
                    }
                }
                OPENGEODE_ASSERT_NOT_REACHED(
                    "[SolidMerger::find_facet_origin] Facet not found" );
                return hint_facet;
            }

            void separate_solids( SolidMeshMerger< dimension >& merger )
            {
                for( const auto p : Range{ merger.mesh().nb_polyhedra() } )
                {
                    for( const auto f :
                        LRange{ merger.mesh().nb_polyhedron_facets( p ) } )
                    {
                        if( const auto adj =
                                merger.mesh().polyhedron_adjacent( { p, f } ) )
                        {
                            if( solid_id_[p] != solid_id_[adj.value()] )
                            {
                                merger.builder().unset_polyhedron_adjacent(
                                    { p, f } );
                            }
                        }
                    }
                }
            }

        private:
            std::vector< SolidId > solid_id_;
            absl::FixedArray< std::vector< index_t > > new_id_;
            std::vector< PolyhedronOrigins > polyhedra_origins_;
        };

        template < index_t dimension >
        SolidMeshMerger< dimension >::SolidMeshMerger( absl::Span<
            const std::reference_wrapper< const SolidMesh< dimension > > >
                solids )
            : VertexMerger< SolidMesh< dimension > >{ solids }, impl_{ solids }
        {
        }

        template < index_t dimension >
        SolidMeshMerger< dimension >::SolidMeshMerger(
            SolidMeshMerger&& ) noexcept = default;

        template < index_t dimension >
        SolidMeshMerger< dimension >::~SolidMeshMerger() = default;

        template < index_t dimension >
        std::unique_ptr< SolidMesh< dimension > >
            SolidMeshMerger< dimension >::merge( const double epsilon )
        {
            return impl_->merge( *this, epsilon );
        }

        template < index_t dimension >
        std::unique_ptr< SolidMesh< dimension > >
            SolidMeshMerger< dimension >::merge(
                const Frame< dimension >& epsilons_frame )
        {
            return impl_->merge( *this, epsilons_frame );
        }

        template < index_t dimension >
        index_t SolidMeshMerger< dimension >::polyhedron_in_merged(
            index_t solid, index_t polyhedron ) const
        {
            return impl_->polyhedron_in_merged( *this, solid, polyhedron );
        }

        template < index_t dimension >
        auto SolidMeshMerger< dimension >::polyhedron_origins(
            index_t polyhedron ) const -> const PolyhedronOrigins&
        {
            return impl_->polyhedron_origins( polyhedron );
        }

        template class opengeode_mesh_api SolidMeshMerger< 3 >;
    } // namespace detail
} // namespace geode
