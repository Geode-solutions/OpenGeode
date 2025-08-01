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
    FORWARD_DECLARATION_DIMENSION_CLASS( Grid );
    FORWARD_DECLARATION_DIMENSION_CLASS( PolygonalSurface );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMeshBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurface );
    ALIAS_2D_AND_3D( Grid );
    ALIAS_2D_AND_3D( PolygonalSurface );
    ALIAS_2D_AND_3D( SurfaceMesh );
    ALIAS_2D_AND_3D( TriangulatedSurface );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    [[nodiscard]] std::unique_ptr< PolygonalSurface< dimension > >
        convert_surface_mesh_into_polygonal_surface(
            const SurfaceMesh< dimension >& surface );

    template < index_t dimension >
    [[nodiscard]] std::optional<
        std::unique_ptr< TriangulatedSurface< dimension > > >
        convert_surface_mesh_into_triangulated_surface(
            const SurfaceMesh< dimension >& surface );

    [[nodiscard]] std::unique_ptr< TriangulatedSurface2D > opengeode_mesh_api
        convert_grid_into_triangulated_surface( const Grid2D& grid );

    [[nodiscard]] std::unique_ptr< PolygonalSurface2D > opengeode_mesh_api
        convert_grid_into_polygonal_surface( const Grid2D& grid );

    [[nodiscard]] std::unique_ptr< TriangulatedSurface2D > opengeode_mesh_api
        convert_grid_into_densified_triangulated_surface( const Grid2D& grid,
            absl::Span< const geode::index_t > cells_to_densify );

    template < index_t dimension >
    void triangulate_surface_mesh( SurfaceMesh< dimension >& surface );

    template < index_t dimension >
    void triangulate_surface_mesh( const SurfaceMesh< dimension >& surface,
        SurfaceMeshBuilder< dimension >& builder );

    [[nodiscard]] std::unique_ptr< SurfaceMesh3D > opengeode_mesh_api
        convert_surface_mesh2d_into_3d( const SurfaceMesh2D& surface2d,
            local_index_t axis_to_add,
            double axis_coordinate );

    [[nodiscard]] std::unique_ptr< SurfaceMesh2D >
        opengeode_mesh_api convert_surface_mesh3d_into_2d(
            const SurfaceMesh3D& surface3d, local_index_t axis_to_remove );

    [[nodiscard]] std::unique_ptr< PolygonalSurface3D >
        opengeode_mesh_api convert_polygonal_surface2d_into_3d(
            const PolygonalSurface2D& surface2d,
            local_index_t axis_to_add,
            double axis_coordinate );

    [[nodiscard]] std::unique_ptr< PolygonalSurface2D >
        opengeode_mesh_api convert_polygonal_surface3d_into_2d(
            const PolygonalSurface3D& surface3d, local_index_t axis_to_remove );

    [[nodiscard]] std::unique_ptr< TriangulatedSurface3D >
        opengeode_mesh_api convert_triangulated_surface2d_into_3d(
            const TriangulatedSurface2D& surface2d,
            local_index_t axis_to_add,
            double axis_coordinate );

    [[nodiscard]] std::unique_ptr< TriangulatedSurface2D >
        opengeode_mesh_api convert_triangulated_surface3d_into_2d(
            const TriangulatedSurface3D& surface3d,
            local_index_t axis_to_remove );

    template < index_t dimension >
    [[nodiscard]] std::unique_ptr< SurfaceMesh< dimension > >
        merge_surface_meshes( absl::Span< const std::reference_wrapper<
                const SurfaceMesh< dimension > > > surfaces );
} // namespace geode