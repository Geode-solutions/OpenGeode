/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/core/detail/register_mesh.h>

#include <geode/mesh/core/geode_edged_curve.h>
#include <geode/mesh/core/geode_graph.h>
#include <geode/mesh/core/geode_point_set.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode_tetrahedral_solid.h>
#include <geode/mesh/core/geode_triangulated_surface.h>
#include <geode/mesh/core/geode_vertex_set.h>
#include <geode/mesh/core/mesh_factory.h>

namespace geode
{
    void register_geode_mesh()
    {
        MeshFactory::register_default_mesh< OpenGeodeVertexSet >(
            VertexSet::kind_name_static(),
            OpenGeodeVertexSet::type_name_static() );

        MeshFactory::register_default_mesh< OpenGeodeGraph >(
            Graph::kind_name_static(), OpenGeodeGraph::type_name_static() );

        MeshFactory::register_default_mesh< OpenGeodePointSet2D >(
            PointSet2D::kind_name_static(),
            OpenGeodePointSet2D::type_name_static() );
        MeshFactory::register_default_mesh< OpenGeodePointSet3D >(
            PointSet3D::kind_name_static(),
            OpenGeodePointSet3D::type_name_static() );

        MeshFactory::register_default_mesh< OpenGeodeEdgedCurve2D >(
            EdgedCurve2D::kind_name_static(),
            OpenGeodeEdgedCurve2D::type_name_static() );
        MeshFactory::register_default_mesh< OpenGeodeEdgedCurve3D >(
            EdgedCurve3D::kind_name_static(),
            OpenGeodeEdgedCurve3D::type_name_static() );

        MeshFactory::register_default_mesh< OpenGeodePolygonalSurface2D >(
            PolygonalSurface2D::kind_name_static(),
            OpenGeodePolygonalSurface2D::type_name_static() );
        MeshFactory::register_default_mesh< OpenGeodePolygonalSurface3D >(
            PolygonalSurface3D::kind_name_static(),
            OpenGeodePolygonalSurface3D::type_name_static() );

        MeshFactory::register_default_mesh< OpenGeodeTriangulatedSurface2D >(
            TriangulatedSurface2D::kind_name_static(),
            OpenGeodeTriangulatedSurface2D::type_name_static() );
        MeshFactory::register_default_mesh< OpenGeodeTriangulatedSurface3D >(
            TriangulatedSurface3D::kind_name_static(),
            OpenGeodeTriangulatedSurface3D::type_name_static() );

        MeshFactory::register_default_mesh< OpenGeodePolyhedralSolid3D >(
            PolyhedralSolid3D::kind_name_static(),
            OpenGeodePolyhedralSolid3D::type_name_static() );

        MeshFactory::register_default_mesh< OpenGeodeTetrahedralSolid3D >(
            TetrahedralSolid3D::kind_name_static(),
            OpenGeodeTetrahedralSolid3D::type_name_static() );
    }
} // namespace geode
