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

#include <geode/mesh/builder/geode/register_builder.h>

#include <geode/mesh/builder/geode/geode_edged_curve_builder.h>
#include <geode/mesh/builder/geode/geode_graph_builder.h>
#include <geode/mesh/builder/geode/geode_hybrid_solid_builder.h>
#include <geode/mesh/builder/geode/geode_point_set_builder.h>
#include <geode/mesh/builder/geode/geode_polygonal_surface_builder.h>
#include <geode/mesh/builder/geode/geode_polyhedral_solid_builder.h>
#include <geode/mesh/builder/geode/geode_regular_grid_solid_builder.h>
#include <geode/mesh/builder/geode/geode_regular_grid_surface_builder.h>
#include <geode/mesh/builder/geode/geode_tetrahedral_solid_builder.h>
#include <geode/mesh/builder/geode/geode_triangulated_surface_builder.h>
#include <geode/mesh/builder/geode/geode_vertex_set_builder.h>
#include <geode/mesh/builder/mesh_builder_factory.h>
#include <geode/mesh/core/geode/geode_edged_curve.h>
#include <geode/mesh/core/geode/geode_graph.h>
#include <geode/mesh/core/geode/geode_hybrid_solid.h>
#include <geode/mesh/core/geode/geode_point_set.h>
#include <geode/mesh/core/geode/geode_polygonal_surface.h>
#include <geode/mesh/core/geode/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode/geode_regular_grid_solid.h>
#include <geode/mesh/core/geode/geode_regular_grid_surface.h>
#include <geode/mesh/core/geode/geode_tetrahedral_solid.h>
#include <geode/mesh/core/geode/geode_triangulated_surface.h>
#include <geode/mesh/core/geode/geode_vertex_set.h>

namespace geode
{
    void register_geode_builder()
    {
        MeshBuilderFactory::register_mesh_builder< OpenGeodeVertexSetBuilder >(
            OpenGeodeVertexSet::impl_name_static() );

        MeshBuilderFactory::register_mesh_builder< OpenGeodeGraphBuilder >(
            OpenGeodeGraph::impl_name_static() );

        MeshBuilderFactory::register_mesh_builder< OpenGeodePointSetBuilder2D >(
            OpenGeodePointSet2D::impl_name_static() );
        MeshBuilderFactory::register_mesh_builder< OpenGeodePointSetBuilder3D >(
            OpenGeodePointSet3D::impl_name_static() );

        MeshBuilderFactory::register_mesh_builder<
            OpenGeodeEdgedCurveBuilder2D >(
            OpenGeodeEdgedCurve2D::impl_name_static() );
        MeshBuilderFactory::register_mesh_builder<
            OpenGeodeEdgedCurveBuilder3D >(
            OpenGeodeEdgedCurve3D::impl_name_static() );

        MeshBuilderFactory::register_mesh_builder<
            OpenGeodePolygonalSurfaceBuilder2D >(
            OpenGeodePolygonalSurface2D::impl_name_static() );
        MeshBuilderFactory::register_mesh_builder<
            OpenGeodePolygonalSurfaceBuilder3D >(
            OpenGeodePolygonalSurface3D::impl_name_static() );

        MeshBuilderFactory::register_mesh_builder<
            OpenGeodeTriangulatedSurfaceBuilder2D >(
            OpenGeodeTriangulatedSurface2D::impl_name_static() );
        MeshBuilderFactory::register_mesh_builder<
            OpenGeodeTriangulatedSurfaceBuilder3D >(
            OpenGeodeTriangulatedSurface3D::impl_name_static() );

        MeshBuilderFactory::register_mesh_builder<
            OpenGeodeRegularGridBuilder2D >(
            OpenGeodeRegularGrid2D::impl_name_static() );
        MeshBuilderFactory::register_mesh_builder<
            OpenGeodeRegularGridBuilder3D >(
            OpenGeodeRegularGrid3D::impl_name_static() );

        MeshBuilderFactory::register_mesh_builder<
            OpenGeodePolyhedralSolidBuilder3D >(
            OpenGeodePolyhedralSolid3D::impl_name_static() );

        MeshBuilderFactory::register_mesh_builder<
            OpenGeodeTetrahedralSolidBuilder3D >(
            OpenGeodeTetrahedralSolid3D::impl_name_static() );

        MeshBuilderFactory::register_mesh_builder<
            OpenGeodeHybridSolidBuilder3D >(
            OpenGeodeHybridSolid3D::impl_name_static() );
    }
} // namespace geode