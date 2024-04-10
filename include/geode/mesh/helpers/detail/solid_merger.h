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

#include <geode/basic/pimpl.h>

#include <absl/container/inlined_vector.h>
#include <absl/types/span.h>

#include <geode/mesh/common.h>
#include <geode/mesh/helpers/detail/vertex_merger.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
} // namespace geode

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class SolidMeshMerger : public VertexMerger< SolidMesh< dimension > >
        {
        public:
            struct PolyhedronOrigin
            {
                PolyhedronOrigin( index_t solid_in, index_t polyhedron_in )
                    : solid( solid_in ), polyhedron( polyhedron_in )
                {
                }

                bool operator==( const PolyhedronOrigin& other ) const
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
                                 const SolidMesh< dimension > > > solids,
                double epsilon );
            ~SolidMeshMerger();

            std::unique_ptr< SolidMesh< dimension > > merge();

            index_t polyhedron_in_merged(
                index_t solid, index_t polyhedron ) const;

            const PolyhedronOrigins& polyhedron_origins(
                index_t polyhedron ) const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
        ALIAS_3D( SolidMeshMerger );
    } // namespace detail
} // namespace geode
