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

#include <geode/mesh/helpers/detail/curve_merger.hpp>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/core/detail/vertex_cycle.hpp>
#include <geode/mesh/core/edged_curve.hpp>

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class EdgedCurveMerger< dimension >::Impl
        {
            using Edge = std::array< index_t, 2 >;
            using TypedVertexCycle = detail::VertexCycle< Edge >;
            using EdgedCurveId = absl::flat_hash_set< index_t >;

        public:
            Impl( absl::Span< const std::reference_wrapper<
                    const EdgedCurve< dimension > > > curves )
                : new_id_( curves.size() )
            {
                index_t nb_edges{ 0 };
                for( const auto s : Indices{ curves } )
                {
                    const auto& curve = curves[s].get();
                    new_id_[s].resize( curve.nb_edges(), NO_ID );
                    nb_edges += curve.nb_edges();
                }
                edges_origins_.reserve( nb_edges );
                curve_id_.reserve( nb_edges );
            }

            std::unique_ptr< EdgedCurve< dimension > > merge(
                EdgedCurveMerger< dimension >& merger )
            {
                merger.create_points();
                create_edges( merger );
                clean_curve( merger );
                curve_id_.clear();
                return merger.steal_mesh();
            }

            index_t edge_in_merged( const EdgedCurveMerger< dimension >& merger,
                index_t curve,
                index_t edge ) const
            {
                geode_unused( merger );
                OPENGEODE_ASSERT( curve < merger.meshes().size(),
                    "[EdgedCurveMerger::edge_in_merged] Wrong curve index" );
                OPENGEODE_ASSERT(
                    edge < merger.meshes()[curve].get().nb_edges(),
                    "[EdgedCurveMerger::edge_in_merged] Wrong curve edge "
                    "index" );
                return new_id_[curve][edge];
            }

            const EdgeOrigins& edge_origins( index_t edge ) const
            {
                return edges_origins_[edge];
            }

        private:
            void clean_curve( EdgedCurveMerger< dimension >& merger )
            {
                bool delete_needed{ false };
                std::vector< bool > to_delete(
                    merger.mesh().nb_edges(), false );
                for( const auto e : Range{ merger.mesh().nb_edges() } )
                {
                    const auto vertices = merger.mesh().edge_vertices( e );
                    if( vertices[0] == vertices[1] )
                    {
                        to_delete[e] = true;
                        delete_needed = true;
                    }
                }
                if( delete_needed )
                {
                    const auto old2new =
                        merger.builder().delete_edges( to_delete );
                    const auto& meshes = merger.meshes();
                    for( const auto curve_id : Indices{ merger.meshes() } )
                    {
                        const auto& curve = meshes[curve_id].get();
                        for( const auto e : Range{ curve.nb_edges() } )
                        {
                            const auto old = new_id_[curve_id][e];
                            new_id_[curve_id][e] = old2new[old];
                        }
                    }
                }
            }

            void create_edges( EdgedCurveMerger< dimension >& merger )
            {
                absl::flat_hash_map< TypedVertexCycle, index_t > edges;
                const auto& meshes = merger.meshes();
                for( const auto s : Indices{ merger.meshes() } )
                {
                    const auto& curve = meshes[s].get();
                    for( const auto e : Range{ curve.nb_edges() } )
                    {
                        Edge vertices;
                        for( const auto v : LRange{ 2 } )
                        {
                            vertices[v] = merger.vertex_in_merged(
                                s, curve.edge_vertex( { e, v } ) );
                        }
                        const auto it =
                            edges.try_emplace( TypedVertexCycle{ vertices },
                                merger.mesh().nb_edges() );
                        if( it.second )
                        {
                            const auto edge_id = merger.builder().create_edge(
                                vertices[0], vertices[1] );
                            OPENGEODE_ASSERT( edge_id == curve_id_.size(),
                                "[EdgedCurveMerger::create_edges] Issue in "
                                "edge database (curve_id_)" );
                            curve_id_.emplace_back();
                            curve_id_.back().insert( s );
                            new_id_[s][e] = edge_id;
                            OPENGEODE_ASSERT( edge_id == edges_origins_.size(),
                                "[EdgedCurveMerger::create_edges] Issue in "
                                "edge database (edges_origins_)" );
                            edges_origins_.emplace_back();
                            edges_origins_.back().emplace_back( s, e );
                        }
                        else
                        {
                            const auto edge_id = it.first->second;
                            OPENGEODE_ASSERT( edge_id < curve_id_.size(),
                                "[EdgedCurveMerger::create_edges] Issue in "
                                "edge database (curve_id_)" );
                            curve_id_[edge_id].insert( s );
                            new_id_[s][e] = edge_id;
                            OPENGEODE_ASSERT( edge_id < edges_origins_.size(),
                                "[EdgedCurveMerger::create_edges] Issue in "
                                "edge database (edges_origins_)" );
                            edges_origins_[edge_id].emplace_back( s, e );
                        }
                    }
                }
            }

        private:
            std::vector< EdgedCurveId > curve_id_;
            absl::FixedArray< std::vector< index_t > > new_id_;
            std::vector< EdgeOrigins > edges_origins_;
        };

        template < index_t dimension >
        EdgedCurveMerger< dimension >::EdgedCurveMerger(
            absl::Span< const std::reference_wrapper<
                const EdgedCurve< dimension > > > curves,
            double epsilon )
            : VertexMerger< EdgedCurve< dimension > >{ curves, epsilon },
              impl_{ curves }
        {
        }

        template < index_t dimension >
        EdgedCurveMerger< dimension >::~EdgedCurveMerger() = default;

        template < index_t dimension >
        std::unique_ptr< EdgedCurve< dimension > >
            EdgedCurveMerger< dimension >::merge()
        {
            return impl_->merge( *this );
        }

        template < index_t dimension >
        index_t EdgedCurveMerger< dimension >::edge_in_merged(
            index_t curve, index_t edge ) const
        {
            return impl_->edge_in_merged( *this, curve, edge );
        }

        template < index_t dimension >
        auto EdgedCurveMerger< dimension >::edge_origins(
            index_t edge ) const -> const EdgeOrigins&
        {
            return impl_->edge_origins( edge );
        }

        template class opengeode_mesh_api EdgedCurveMerger< 2 >;
        template class opengeode_mesh_api EdgedCurveMerger< 3 >;
    } // namespace detail
} // namespace geode
