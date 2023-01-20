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

#include <geode/mesh/helpers/detail/solid_merger.h>

#include <absl/container/flat_hash_set.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/builder/solid_mesh_builder.h>
#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/helpers/private/vertex_merger.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class SolidMeshMerger< dimension >::Impl
            : public VertexMerger< SolidMesh, SolidMeshBuilder, dimension >
        {
            using ColocatedInfo = typename NNSearch< dimension >::ColocatedInfo;
            using Polyhedron = absl::FixedArray< index_t >;
            using TypedVertexCycle = detail::VertexCycle< Polyhedron >;
            using SolidId = absl::flat_hash_set< index_t >;

        public:
            Impl( absl::Span< const std::reference_wrapper<
                      const SolidMesh< dimension > > > solids,
                double epsilon )
                : VertexMerger< SolidMesh,
                    SolidMeshBuilder,
                    dimension >{ solids, epsilon },
                  new_id_( solids.size() )
            {
                index_t nb_polyhedra{ 0 };
                for( const auto s : Indices{ this->meshes() } )
                {
                    const auto& solid = this->meshes()[s].get();
                    new_id_[s].resize( solid.nb_polyhedra(), NO_ID );
                    nb_polyhedra += solid.nb_polyhedra();
                }
                polyhedra_origins_.reserve( nb_polyhedra );
                solid_id_.reserve( nb_polyhedra );
            }

            std::unique_ptr< SolidMesh< dimension > > merge()
            {
                create_solid_step();
                return this->steal_mesh();
            }

            index_t polyhedron_in_merged(
                index_t solid, index_t polyhedron ) const
            {
                OPENGEODE_ASSERT( solid < this->meshes().size(),
                    "[SolidMerger::polyhedron_in_merged] Wrong solid index" );
                OPENGEODE_ASSERT(
                    polyhedron < this->meshes()[solid].get().nb_polyhedra(),
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
            void create_solid_step()
            {
                this->create_points();
                create_polyhedra();
                create_adjacencies();
                clean_solid();
                solid_id_.clear();
            }

            void clean_solid()
            {
                bool delete_needed{ false };
                std::vector< bool > to_delete(
                    this->mesh().nb_polyhedra(), false );
                for( const auto p : Range{ this->mesh().nb_polyhedra() } )
                {
                    const auto vertices = this->mesh().polyhedron_vertices( p );
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
                    const auto old2new =
                        this->builder().delete_polyhedra( to_delete );
                    for( const auto solid_id : Indices{ this->meshes() } )
                    {
                        const auto& solid = this->meshes()[solid_id].get();
                        for( const auto p : Range{ solid.nb_polyhedra() } )
                        {
                            const auto old = new_id_[solid_id][p];
                            new_id_[solid_id][p] = old2new[old];
                        }
                    }
                }
                separate_solids();
            }

            void create_polyhedra()
            {
                absl::flat_hash_map< TypedVertexCycle, index_t > polyhedra;
                for( const auto s : Indices{ this->meshes() } )
                {
                    const auto& solid = this->meshes()[s].get();
                    for( const auto p : Range{ solid.nb_polyhedra() } )
                    {
                        Polyhedron vertices(
                            solid.nb_polyhedron_vertices( p ) );
                        for( const auto v : LIndices{ vertices } )
                        {
                            vertices[v] = this->vertex_in_merged(
                                s, solid.polyhedron_vertex( { p, v } ) );
                        }
                        const auto it = polyhedra.try_emplace(
                            vertices, this->mesh().nb_polyhedra() );
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
                                this->builder().create_polyhedron(
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

            void create_adjacencies()
            {
                for( const auto s : Indices{ this->meshes() } )
                {
                    const auto& solid = this->meshes()[s].get();
                    for( const auto p : Range{ solid.nb_polyhedra() } )
                    {
                        for( const auto f :
                            LRange{ solid.nb_polyhedron_facets( p ) } )
                        {
                            if( const auto adj =
                                    solid.polyhedron_adjacent( { p, f } ) )
                            {
                                const auto new_id = new_id_[s][p];
                                const auto new_adj_id = new_id_[s][adj.value()];
                                this->builder().set_polyhedron_adjacent(
                                    { new_id, f }, new_adj_id );
                            }
                        }
                    }
                }
            }

            void separate_solids()
            {
                for( const auto p : Range{ this->mesh().nb_polyhedra() } )
                {
                    for( const auto f :
                        LRange{ this->mesh().nb_polyhedron_facets( p ) } )
                    {
                        if( const auto adj =
                                this->mesh().polyhedron_adjacent( { p, f } ) )
                        {
                            if( solid_id_[p] != solid_id_[adj.value()] )
                            {
                                this->builder().unset_polyhedron_adjacent(
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
        SolidMeshMerger< dimension >::SolidMeshMerger(
            absl::Span< const std::reference_wrapper<
                const SolidMesh< dimension > > > solids,
            double epsilon )
            : impl_( solids, epsilon )
        {
        }

        template < index_t dimension >
        SolidMeshMerger< dimension >::~SolidMeshMerger()
        {
        }

        template < index_t dimension >
        std::unique_ptr< SolidMesh< dimension > >
            SolidMeshMerger< dimension >::merge()
        {
            return impl_->merge();
        }

        template < index_t dimension >
        index_t SolidMeshMerger< dimension >::vertex_in_merged(
            index_t solid, index_t vertex ) const
        {
            return impl_->vertex_in_merged( solid, vertex );
        }

        template < index_t dimension >
        index_t SolidMeshMerger< dimension >::polyhedron_in_merged(
            index_t solid, index_t polyhedron ) const
        {
            return impl_->polyhedron_in_merged( solid, polyhedron );
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
