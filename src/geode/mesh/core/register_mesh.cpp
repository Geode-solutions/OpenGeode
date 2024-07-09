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

#include <geode/mesh/core/geode/register_mesh.h>

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
#include <geode/mesh/core/mesh_factory.h>

namespace geode
{
    void register_geode_mesh()
    {
        mesh_factory.register_default_mesh< OpenGeodeVertexSet >(
            VertexSet::type_name_static(),
            OpenGeodeVertexSet::impl_name_static() );

        mesh_factory.register_default_mesh< OpenGeodeGraph >(
            Graph::type_name_static(), OpenGeodeGraph::impl_name_static() );

        mesh_factory.register_default_mesh< OpenGeodePointSet2D >(
            PointSet2D::type_name_static(),
            OpenGeodePointSet2D::impl_name_static() );
        mesh_factory.register_default_mesh< OpenGeodePointSet3D >(
            PointSet3D::type_name_static(),
            OpenGeodePointSet3D::impl_name_static() );

        mesh_factory.register_default_mesh< OpenGeodeEdgedCurve2D >(
            EdgedCurve2D::type_name_static(),
            OpenGeodeEdgedCurve2D::impl_name_static() );
        mesh_factory.register_default_mesh< OpenGeodeEdgedCurve3D >(
            EdgedCurve3D::type_name_static(),
            OpenGeodeEdgedCurve3D::impl_name_static() );

        mesh_factory.register_default_mesh< OpenGeodePolygonalSurface2D >(
            PolygonalSurface2D::type_name_static(),
            OpenGeodePolygonalSurface2D::impl_name_static() );
        mesh_factory.register_default_mesh< OpenGeodePolygonalSurface3D >(
            PolygonalSurface3D::type_name_static(),
            OpenGeodePolygonalSurface3D::impl_name_static() );

        mesh_factory.register_default_mesh< OpenGeodeTriangulatedSurface2D >(
            TriangulatedSurface2D::type_name_static(),
            OpenGeodeTriangulatedSurface2D::impl_name_static() );
        mesh_factory.register_default_mesh< OpenGeodeTriangulatedSurface3D >(
            TriangulatedSurface3D::type_name_static(),
            OpenGeodeTriangulatedSurface3D::impl_name_static() );

        mesh_factory.register_default_mesh< OpenGeodePolyhedralSolid3D >(
            PolyhedralSolid3D::type_name_static(),
            OpenGeodePolyhedralSolid3D::impl_name_static() );

        mesh_factory.register_default_mesh< OpenGeodeTetrahedralSolid3D >(
            TetrahedralSolid3D::type_name_static(),
            OpenGeodeTetrahedralSolid3D::impl_name_static() );

        mesh_factory.register_default_mesh< OpenGeodeHybridSolid3D >(
            HybridSolid3D::type_name_static(),
            OpenGeodeHybridSolid3D::impl_name_static() );

        mesh_factory.register_default_mesh< OpenGeodeRegularGrid2D >(
            RegularGrid2D::type_name_static(),
            OpenGeodeRegularGrid2D::impl_name_static() );
        mesh_factory.register_default_mesh< OpenGeodeRegularGrid3D >(
            RegularGrid3D::type_name_static(),
            OpenGeodeRegularGrid3D::impl_name_static() );
    }
} // namespace geode
