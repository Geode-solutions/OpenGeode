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

#pragma once

#include <geode/basic/pimpl.hpp>

#include <absl/container/inlined_vector.h>
#include <absl/types/span.h>

#include <geode/mesh/common.hpp>
#include <geode/mesh/helpers/detail/vertex_merger.hpp>

#include <geode/geometry/frame.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
} // namespace geode

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class SolidMeshMerger
            : public VertexMerger< SolidMesh< dimension >, dimension >
        {
        public:
            struct PolyhedronOrigin
            {
                PolyhedronOrigin( index_t solid_in, index_t polyhedron_in )
                    : solid( solid_in ), polyhedron( polyhedron_in )
                {
                }

                [[nodiscard]] bool operator==(
                    const PolyhedronOrigin& other ) const
                {
                    return solid == other.solid
                           && polyhedron == other.polyhedron;
                }

                index_t solid;
                index_t polyhedron;
            };
            using PolyhedronOrigins =
                absl::InlinedVector< PolyhedronOrigin, 1 >;

            SolidMeshMerger( absl::Span< const std::reference_wrapper<
                    const SolidMesh< dimension > > > solids );
            SolidMeshMerger( SolidMeshMerger&& ) noexcept;
            ~SolidMeshMerger();

            [[nodiscard]] std::unique_ptr< SolidMesh< dimension > > merge(
                double epsilon );

            [[nodiscard]] std::unique_ptr< SolidMesh< dimension > > merge(
                const Frame< dimension >& epsilons_frame );

            [[nodiscard]] index_t polyhedron_in_merged(
                index_t solid, index_t polyhedron ) const;

            [[nodiscard]] const PolyhedronOrigins& polyhedron_origins(
                index_t polyhedron ) const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
        ALIAS_3D( SolidMeshMerger );
    } // namespace detail
} // namespace geode
