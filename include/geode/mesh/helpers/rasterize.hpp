/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/grid.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Grid );
    FORWARD_DECLARATION_DIMENSION_CLASS( Segment );
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurface );
    ALIAS_3D( Grid );
    ALIAS_3D( TriangulatedSurface );
    class Tetrahedron;
} // namespace geode

namespace geode
{
    /*!
     * Rasterize the segment in the grid cells. The segment may extend outside
     * the grid: only its part inside the grid is rasterized, and the result is
     * empty if the segment is fully outside.
     */
    template < index_t dimension >
    [[nodiscard]] std::vector< typename Grid< dimension >::CellIndices >
        rasterize_segment( const Grid< dimension >& grid,
            const Segment< dimension >& segment );

    /*!
     * Conservatively rasterize the segment in the grid cells. The segment may
     * extend outside the grid: only its part inside the grid is rasterized, and
     * the result is empty if the segment is fully outside.
     */
    template < index_t dimension >
    [[nodiscard]] std::vector< typename Grid< dimension >::CellIndices >
        conservative_rasterize_segment( const Grid< dimension >& grid,
            const Segment< dimension >& segment );

    /*!
     * Conservatively rasterize the triangle in the grid cells. The triangle
     * may extend outside the grid: only its part inside the grid is
     * rasterized, and the result is empty if the triangle is fully outside.
     */
    template < index_t dimension >
    [[nodiscard]] std::vector< typename Grid< dimension >::CellIndices >
        conservative_rasterize_triangle( const Grid< dimension >& grid,
            const Triangle< dimension >& triangle );

    [[nodiscard]] std::vector< typename Grid3D::CellIndices >
        opengeode_mesh_api rasterize_tetrahedron(
            const Grid3D& grid, const Tetrahedron& tetrahedron );

    [[nodiscard]] std::vector< Grid3D::CellIndices >
        opengeode_mesh_api rasterize_closed_surface(
            const Grid3D& grid, const TriangulatedSurface3D& closed_surface );
} // namespace geode
