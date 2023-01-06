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

#pragma once

#include <geode/geometry/nn_search.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/mesh_factory.h>

namespace
{
    template < typename Mesh >
    std::unique_ptr< Mesh > create_mesh(
        absl::Span< const std::reference_wrapper< const Mesh > > meshes )
    {
        const auto type = meshes.front().get().type_name();
        for( const auto& mesh : meshes )
        {
            if( mesh.get().type_name() != type )
            {
                return Mesh::create();
            }
        }
        return Mesh::create( geode::MeshFactory::default_impl( type ) );
    }
} // namespace

namespace geode
{
    namespace detail
    {
        template < template < index_t > class Mesh,
            template < index_t >
            class Builder,
            index_t dimension >
        class VertexMerger
        {
            using ColocatedInfo = typename NNSearch< dimension >::ColocatedInfo;

        public:
            VertexMerger( absl::Span< const std::reference_wrapper<
                              const Mesh< dimension > > > meshes,
                double epsilon )
                : meshes_( meshes ),
                  epsilon_( epsilon ),
                  mesh_{ create_mesh( meshes ) },
                  builder_{ Builder< dimension >::create( *mesh_ ) },
                  offset_vertices_( meshes.size() + 1 )
            {
                offset_vertices_[0] = 0;
                for( const auto m : Indices{ meshes_ } )
                {
                    const auto& mesh = meshes_[m].get();
                    offset_vertices_[m + 1] =
                        offset_vertices_[m] + mesh.nb_vertices();
                }
            }

            index_t vertex_in_merged( index_t mesh, index_t vertex ) const
            {
                OPENGEODE_ASSERT( mesh < meshes_.size(),
                    "[Merger::vertex_in_merged] Wrong mesh index" );
                OPENGEODE_ASSERT( vertex < meshes_[mesh].get().nb_vertices(),
                    "[Merger::vertex_in_merged] Wrong mesh vertex "
                    "index" );
                OPENGEODE_ASSERT(
                    offset_vertices_[mesh] + vertex < vertices_.size(),
                    "[Merger::vertex_in_merged] Wrong global vertex "
                    "index" );
                return vertices_[offset_vertices_[mesh] + vertex];
            }

        protected:
            absl::Span<
                const std::reference_wrapper< const Mesh< dimension > > >
                meshes() const
            {
                return meshes_;
            }

            const Mesh< dimension >& mesh() const
            {
                return *mesh_;
            }

            std::unique_ptr< Mesh< dimension > > steal_mesh()
            {
                return std::move( mesh_ );
            }

            Builder< dimension >& builder()
            {
                return *builder_;
            }

            void create_points()
            {
                auto info = create_colocated_index_mapping();
                vertices_ = std::move( info.colocated_mapping );
                for( const auto& point : info.unique_points )
                {
                    builder_->create_point( point );
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
            absl::Span<
                const std::reference_wrapper< const Mesh< dimension > > >
                meshes_;
            double epsilon_;
            std::unique_ptr< Mesh< dimension > > mesh_;
            std::unique_ptr< Builder< dimension > > builder_;
            std::vector< index_t > vertices_;
            absl::FixedArray< index_t > offset_vertices_;
        };
    } // namespace detail
} // namespace geode
