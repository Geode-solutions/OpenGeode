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

#include <absl/types/optional.h>

#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( PolygonalSurface );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMeshBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurface );
    ALIAS_2D_AND_3D( PolygonalSurface );
    ALIAS_2D_AND_3D( SurfaceMesh );
    ALIAS_2D_AND_3D( TriangulatedSurface );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< PolygonalSurface< dimension > >
        convert_surface_mesh_into_polygonal_surface(
            const SurfaceMesh< dimension >& surface );

    template < index_t dimension >
    absl::optional< std::unique_ptr< TriangulatedSurface< dimension > > >
        convert_surface_mesh_into_triangulated_surface(
            const SurfaceMesh< dimension >& surface );

    template < index_t dimension >
    void triangulate_surface_mesh( SurfaceMesh< dimension >& surface );

    template < index_t dimension >
    void triangulate_surface_mesh( const SurfaceMesh< dimension >& surface,
        SurfaceMeshBuilder< dimension >& builder );

    std::unique_ptr< SurfaceMesh3D >
        opengeode_mesh_api convert_surface_mesh2d_into_3d(
            const SurfaceMesh2D& surface2d, index_t axis_to_add );

    std::unique_ptr< SurfaceMesh2D >
        opengeode_mesh_api convert_surface_mesh3d_into_2d(
            const SurfaceMesh3D& surface3d, index_t axis_to_remove );

    std::unique_ptr< PolygonalSurface3D >
        opengeode_mesh_api convert_polygonal_surface2d_into_3d(
            const PolygonalSurface2D& surface2d, index_t axis_to_add );

    std::unique_ptr< PolygonalSurface2D >
        opengeode_mesh_api convert_polygonal_surface3d_into_2d(
            const PolygonalSurface3D& surface3d, index_t axis_to_remove );

    std::unique_ptr< TriangulatedSurface3D >
        opengeode_mesh_api convert_triangulated_surface2d_into_3d(
            const TriangulatedSurface2D& surface2d, index_t axis_to_add );

    std::unique_ptr< TriangulatedSurface2D >
        opengeode_mesh_api convert_triangulated_surface3d_into_2d(
            const TriangulatedSurface3D& surface3d, index_t axis_to_remove );

    template < index_t dimension >
    std::unique_ptr< SurfaceMesh< dimension > > merge_surface_mesh(
        const SurfaceMesh< dimension >& surface );

    template < index_t dimension >
    std::unique_ptr< SurfaceMesh< dimension > > merge_surface_meshes(
        const std::vector<
            std::reference_wrapper< const SurfaceMesh< dimension > > >&
            surfaces );
} // namespace geode