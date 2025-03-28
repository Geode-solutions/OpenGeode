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

#include <optional>

#include <absl/types/span.h>

#include <geode/mesh/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( Grid );
    FORWARD_DECLARATION_DIMENSION_CLASS( TetrahedralSolid );
    FORWARD_DECLARATION_DIMENSION_CLASS( HybridSolid );
    ALIAS_3D( SolidMesh );
    ALIAS_3D( Grid );
    ALIAS_3D( TetrahedralSolid );
    ALIAS_3D( HybridSolid );
} // namespace geode

namespace geode
{
    [[nodiscard]] std::optional< std::unique_ptr< TetrahedralSolid3D > >
        opengeode_mesh_api convert_solid_mesh_into_tetrahedral_solid(
            const SolidMesh3D& solid );

    [[nodiscard]] std::unique_ptr< TetrahedralSolid3D > opengeode_mesh_api
        convert_grid_into_tetrahedral_solid( const Grid3D& grid );

    [[nodiscard]] std::unique_ptr< HybridSolid3D >
        opengeode_mesh_api convert_grid_into_hybrid_solid( const Grid3D& grid );

    [[nodiscard]] std::unique_ptr< TetrahedralSolid3D > opengeode_mesh_api
        convert_grid_into_densified_tetrahedral_solid( const Grid3D& grid,
            absl::Span< const geode::index_t > cells_to_densify );

    [[nodiscard]] std::optional< std::unique_ptr< HybridSolid3D > >
        opengeode_mesh_api convert_solid_mesh_into_hybrid_solid(
            const SolidMesh3D& solid );

    [[nodiscard]] std::unique_ptr< SolidMesh3D >
        opengeode_mesh_api merge_solid_meshes(
            absl::Span< const std::reference_wrapper< const SolidMesh3D > >
                solids );
} // namespace geode