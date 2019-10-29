/*
 * Copyright (c) 2019 Geode-solutions
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

namespace geode
{
    void register_geode_mesh()
    {
        VertexSetFactory::register_creator< OpenGeodeVertexSet >(
            OpenGeodeVertexSet::type_name_static() );

        GraphFactory::register_creator< OpenGeodeGraph >(
            OpenGeodeGraph::type_name_static() );

        PointSetFactory2D::register_creator< OpenGeodePointSet2D >(
            OpenGeodePointSet2D::type_name_static() );
        PointSetFactory3D::register_creator< OpenGeodePointSet3D >(
            OpenGeodePointSet3D::type_name_static() );

        EdgedCurveFactory2D::register_creator< OpenGeodeEdgedCurve2D >(
            OpenGeodeEdgedCurve2D::type_name_static() );
        EdgedCurveFactory3D::register_creator< OpenGeodeEdgedCurve3D >(
            OpenGeodeEdgedCurve3D::type_name_static() );

        PolygonalSurfaceFactory2D::register_creator<
            OpenGeodePolygonalSurface2D >(
            OpenGeodePolygonalSurface2D::type_name_static() );
        PolygonalSurfaceFactory3D::register_creator<
            OpenGeodePolygonalSurface3D >(
            OpenGeodePolygonalSurface3D::type_name_static() );

        TriangulatedSurfaceFactory2D::register_creator<
            OpenGeodeTriangulatedSurface2D >(
            OpenGeodeTriangulatedSurface2D::type_name_static() );
        TriangulatedSurfaceFactory3D::register_creator<
            OpenGeodeTriangulatedSurface3D >(
            OpenGeodeTriangulatedSurface3D::type_name_static() );

        PolyhedralSolidFactory3D::register_creator<
            OpenGeodePolyhedralSolid3D >(
            OpenGeodePolyhedralSolid3D::type_name_static() );

        TetrahedralSolidFactory3D::register_creator<
            OpenGeodeTetrahedralSolid3D >(
            OpenGeodeTetrahedralSolid3D::type_name_static() );
    }
} // namespace geode
