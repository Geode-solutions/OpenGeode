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

#include <geode/basic/pimpl.hpp>

#include <absl/container/inlined_vector.h>
#include <absl/types/span.h>

#include <geode/mesh/common.hpp>
#include <geode/mesh/helpers/detail/vertex_merger.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
} // namespace geode

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        class SurfaceMeshMerger
            : public VertexMerger< SurfaceMesh< dimension > >
        {
        public:
            struct PolygonOrigin
            {
                PolygonOrigin( index_t surface_in, index_t polygon_in )
                    : surface( surface_in ), polygon( polygon_in )
                {
                }

                bool operator==( const PolygonOrigin& other ) const
                {
                    return surface == other.surface && polygon == other.polygon;
                }

                index_t surface;
                index_t polygon;
            };
            using PolygonOrigins = absl::InlinedVector< PolygonOrigin, 1 >;

            SurfaceMeshMerger( absl::Span< const std::reference_wrapper<
                                   const SurfaceMesh< dimension > > > surfaces,
                double epsilon );
            ~SurfaceMeshMerger();

            std::unique_ptr< SurfaceMesh< dimension > > merge();

            index_t polygon_in_merged( index_t surface, index_t polygon ) const;

            const PolygonOrigins& polygon_origins( index_t polygon ) const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
        ALIAS_2D_AND_3D( SurfaceMeshMerger );
    } // namespace detail
} // namespace geode
