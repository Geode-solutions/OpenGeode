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

#include <geode/mesh/helpers/detail/vertex_merger.hpp>

#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/nn_search.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/detail/create_mesh.hpp>

namespace geode
{
    namespace detail
    {
        template < typename Mesh >
        class VertexMerger< Mesh >::Impl
        {
            static constexpr auto dimension = Mesh::dim;
            using Builder = typename Mesh::Builder;
            using ColocatedInfo = typename NNSearch< dimension >::ColocatedInfo;

        public:
            Impl(
                absl::Span< const std::reference_wrapper< const Mesh > > meshes,
                double epsilon )
                : meshes_( meshes ),
                  epsilon_( epsilon ),
                  mesh_{ create_mesh( meshes ) },
                  builder_{ Builder::create( *mesh_ ) },
                  offset_vertices_( meshes.size() + 1 )
            {
                offset_vertices_[0] = 0;
                for( const auto m : Indices{ meshes_ } )
                {
                    const auto& mesh = meshes_[m].get();
                    offset_vertices_[m + 1] =
                        offset_vertices_[m] + mesh.nb_vertices();
                }
                vertices_origins_.resize( offset_vertices_.back() );
            }

            index_t vertex_in_merged( index_t mesh, index_t vertex ) const
            {
                OPENGEODE_ASSERT( mesh < meshes_.size(),
                    "[Merger::vertex_in_merged] Wrong mesh index" );
                OPENGEODE_ASSERT( vertex < meshes_[mesh].get().nb_vertices(),
                    "[Merger::vertex_in_merged] Wrong mesh vertex index" );
                OPENGEODE_ASSERT(
                    offset_vertices_[mesh] + vertex < vertices_.size(),
                    "[Merger::vertex_in_merged] Wrong global vertex index" );
                return vertices_[offset_vertices_[mesh] + vertex];
            }

            const VertexOrigins& vertex_origins( index_t vertex ) const
            {
                return vertices_origins_[vertex];
            }

            absl::Span< const std::reference_wrapper< const Mesh > >
                meshes() const
            {
                return meshes_;
            }

            const Mesh& mesh() const
            {
                return *mesh_;
            }

            std::unique_ptr< Mesh > steal_mesh()
            {
                return std::move( mesh_ );
            }

            Builder& builder()
            {
                return *builder_;
            }

            void create_points()
            {
                auto info = create_colocated_index_mapping();
                vertices_ = std::move( info.colocated_mapping );
                builder_->create_vertices( info.nb_unique_points() );
                for( const auto p : Range{ info.nb_unique_points() } )
                {
                    builder_->set_point( p, info.unique_points[p] );
                }
                for( const auto m : Indices{ meshes_ } )
                {
                    const auto& mesh = meshes_[m].get();
                    for( const auto v : Range{ mesh.nb_vertices() } )
                    {
                        vertices_origins_[vertex_in_merged( m, v )]
                            .emplace_back( m, v );
                    }
                }
            }

        private:
            ColocatedInfo create_colocated_index_mapping()
            {
                index_t nb_points{ 0 };
                for( const auto& mesh : meshes_ )
                {
                    nb_points += mesh.get().nb_vertices();
                }
                std::vector< Point< dimension > > points;
                points.reserve( nb_points );
                for( const auto& mesh : meshes_ )
                {
                    for( const auto v : Range{ mesh.get().nb_vertices() } )
                    {
                        points.emplace_back( mesh.get().point( v ) );
                    }
                }
                NNSearch< dimension > nnsearch{ std::move( points ) };
                return nnsearch.colocated_index_mapping( epsilon_ );
            }

        private:
            absl::Span< const std::reference_wrapper< const Mesh > > meshes_;
            double epsilon_;
            std::unique_ptr< Mesh > mesh_;
            std::unique_ptr< Builder > builder_;
            std::vector< index_t > vertices_;
            absl::FixedArray< index_t > offset_vertices_;
            std::vector< VertexOrigins > vertices_origins_;
        };

        template < typename Mesh >
        VertexMerger< Mesh >::VertexMerger(
            absl::Span< const std::reference_wrapper< const Mesh > > meshes,
            double epsilon )
            : impl_{ meshes, epsilon }
        {
        }

        template < typename Mesh >
        VertexMerger< Mesh >::~VertexMerger()
        {
        }

        template < typename Mesh >
        index_t VertexMerger< Mesh >::vertex_in_merged(
            index_t mesh, index_t vertex ) const
        {
            return impl_->vertex_in_merged( mesh, vertex );
        }

        template < typename Mesh >
        auto VertexMerger< Mesh >::vertex_origins(
            index_t vertex ) const -> const VertexOrigins&
        {
            return impl_->vertex_origins( vertex );
        }

        template < typename Mesh >
        absl::Span< const std::reference_wrapper< const Mesh > >
            VertexMerger< Mesh >::meshes() const
        {
            return impl_->meshes();
        }

        template < typename Mesh >
        const Mesh& VertexMerger< Mesh >::mesh() const
        {
            return impl_->mesh();
        }

        template < typename Mesh >
        std::unique_ptr< Mesh > VertexMerger< Mesh >::steal_mesh()
        {
            return impl_->steal_mesh();
        }

        template < typename Mesh >
        auto VertexMerger< Mesh >::builder() -> Builder&
        {
            return impl_->builder();
        }

        template < typename Mesh >
        void VertexMerger< Mesh >::create_points()
        {
            impl_->create_points();
        }

        template class opengeode_mesh_api VertexMerger< PointSet2D >;
        template class opengeode_mesh_api VertexMerger< PointSet3D >;

        template class opengeode_mesh_api VertexMerger< EdgedCurve2D >;
        template class opengeode_mesh_api VertexMerger< EdgedCurve3D >;

        template class opengeode_mesh_api VertexMerger< SurfaceMesh2D >;
        template class opengeode_mesh_api VertexMerger< SurfaceMesh3D >;

        template class opengeode_mesh_api VertexMerger< SolidMesh3D >;
    } // namespace detail
} // namespace geode
