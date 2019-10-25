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

#include <geode/mesh/builder/detail/register_builder.h>

#include <geode/mesh/builder/geode_edged_curve_builder.h>
#include <geode/mesh/builder/geode_graph_builder.h>
#include <geode/mesh/builder/geode_point_set_builder.h>
#include <geode/mesh/builder/geode_polygonal_surface_builder.h>
#include <geode/mesh/builder/geode_polyhedral_solid_builder.h>
#include <geode/mesh/builder/geode_tetrahedral_solid_builder.h>
#include <geode/mesh/builder/geode_triangulated_surface_builder.h>
#include <geode/mesh/builder/geode_vertex_set_builder.h>

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
    void register_geode_builder()
    {
        VertexSetBuilderFactory::register_creator< OpenGeodeVertexSetBuilder >(
            OpenGeodeVertexSet::type_name_static() );

        GraphBuilderFactory::register_creator< OpenGeodeGraphBuilder >(
            OpenGeodeGraph::type_name_static() );

        PointSetBuilderFactory2D::register_creator<
            OpenGeodePointSetBuilder2D >(
            OpenGeodePointSet2D::type_name_static() );
        PointSetBuilderFactory3D::register_creator<
            OpenGeodePointSetBuilder3D >(
            OpenGeodePointSet3D::type_name_static() );

        EdgedCurveBuilderFactory2D::register_creator<
            OpenGeodeEdgedCurveBuilder2D >(
            OpenGeodeEdgedCurve2D::type_name_static() );
        EdgedCurveBuilderFactory3D::register_creator<
            OpenGeodeEdgedCurveBuilder3D >(
            OpenGeodeEdgedCurve3D::type_name_static() );

        PolygonalSurfaceBuilderFactory2D::register_creator<
            OpenGeodePolygonalSurfaceBuilder2D >(
            OpenGeodePolygonalSurface2D::type_name_static() );
        PolygonalSurfaceBuilderFactory3D::register_creator<
            OpenGeodePolygonalSurfaceBuilder3D >(
            OpenGeodePolygonalSurface3D::type_name_static() );

        TriangulatedSurfaceBuilderFactory2D::register_creator<
            OpenGeodeTriangulatedSurfaceBuilder2D >(
            OpenGeodeTriangulatedSurface2D::type_name_static() );
        TriangulatedSurfaceBuilderFactory3D::register_creator<
            OpenGeodeTriangulatedSurfaceBuilder3D >(
            OpenGeodeTriangulatedSurface3D::type_name_static() );

        PolyhedralSolidBuilderFactory3D::register_creator<
            OpenGeodePolyhedralSolidBuilder3D >(
            OpenGeodePolyhedralSolid3D::type_name_static() );

        TetrahedralSolidBuilderFactory3D::register_creator<
            OpenGeodeTetrahedralSolidBuilder3D >(
            OpenGeodeTetrahedralSolid3D::type_name_static() );
    }
} // namespace geode