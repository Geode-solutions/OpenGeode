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

#include <absl/types/span.h>

#include <geode/basic/pimpl.hpp>

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/meshes_mapping.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMeshBuilder );
    ALIAS_3D( SolidMeshBuilder );
} // namespace geode

namespace geode
{
    namespace detail
    {
        class opengeode_mesh_api SplitAlongSolidFacets
        {
        public:
            SplitAlongSolidFacets(
                const SolidMesh3D& mesh, SolidMeshBuilder3D& builder );
            ~SplitAlongSolidFacets();

            /*
             * Splits the solid along given facets, and returns the mapping on
             * vertices and facets.
             */
            MeshesElementsMapping split_solid_along_facets(
                absl::Span< const PolyhedronFacet > facets_list );

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
    } // namespace detail
} // namespace geode