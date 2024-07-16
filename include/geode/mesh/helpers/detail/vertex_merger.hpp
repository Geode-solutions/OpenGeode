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

#pragma once

#include <absl/container/inlined_vector.h>
#include <absl/types/span.h>

#include <geode/basic/pimpl.hpp>

namespace geode
{
    namespace detail
    {
        template < typename Mesh >
        class VertexMerger
        {
        public:
            using Builder = typename Mesh::Builder;

            struct VertexOrigin
            {
                VertexOrigin( index_t mesh_in, index_t vertex_in )
                    : mesh( mesh_in ), vertex( vertex_in )
                {
                }

                index_t mesh;
                index_t vertex;
            };
            using VertexOrigins = absl::InlinedVector< VertexOrigin, 1 >;

            index_t vertex_in_merged( index_t mesh, index_t vertex ) const;

            const VertexOrigins& vertex_origins( index_t vertex ) const;

        protected:
            VertexMerger(
                absl::Span< const std::reference_wrapper< const Mesh > > meshes,
                double epsilon );
            ~VertexMerger();

            absl::Span< const std::reference_wrapper< const Mesh > >
                meshes() const;

            const Mesh& mesh() const;

            std::unique_ptr< Mesh > steal_mesh();

            Builder& builder();

            void create_points();

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
    } // namespace detail
} // namespace geode
