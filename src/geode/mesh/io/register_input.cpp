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

#include <geode/mesh/io/geode/register_input.h>

#include <geode/mesh/core/geode/geode_edged_curve.h>
#include <geode/mesh/core/geode/geode_graph.h>
#include <geode/mesh/core/geode/geode_hybrid_solid.h>
#include <geode/mesh/core/geode/geode_point_set.h>
#include <geode/mesh/core/geode/geode_polygonal_surface.h>
#include <geode/mesh/core/geode/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode/geode_tetrahedral_solid.h>
#include <geode/mesh/core/geode/geode_triangulated_surface.h>
#include <geode/mesh/core/geode/geode_vertex_set.h>
#include <geode/mesh/core/light_regular_grid.h>
#include <geode/mesh/io/geode/geode_edged_curve_input.h>
#include <geode/mesh/io/geode/geode_graph_input.h>
#include <geode/mesh/io/geode/geode_hybrid_solid_input.h>
#include <geode/mesh/io/geode/geode_point_set_input.h>
#include <geode/mesh/io/geode/geode_polygonal_surface_input.h>
#include <geode/mesh/io/geode/geode_polyhedral_solid_input.h>
#include <geode/mesh/io/geode/geode_regular_grid_input.h>
#include <geode/mesh/io/geode/geode_tetrahedral_solid_input.h>
#include <geode/mesh/io/geode/geode_triangulated_surface_input.h>
#include <geode/mesh/io/geode/geode_vertex_set_input.h>
#include <geode/mesh/io/light_regular_grid_input.h>

namespace geode
{
    void register_geode_mesh_input()
    {
        vertex_set_input_factory.register_creator< OpenGeodeVertexSetInput >(
            OpenGeodeVertexSet::native_extension_static().data() );

        graph_input_factory.register_creator< OpenGeodeGraphInput >(
            OpenGeodeGraph::native_extension_static().data() );

        edged_curve_input_factory_2d
            .register_creator< OpenGeodeEdgedCurveInput2D >(
                OpenGeodeEdgedCurve2D::native_extension_static().data() );
        edged_curve_input_factory_3d
            .register_creator< OpenGeodeEdgedCurveInput3D >(
                OpenGeodeEdgedCurve3D::native_extension_static().data() );

        point_set_input_factory_2d.register_creator< OpenGeodePointSetInput2D >(
            OpenGeodePointSet2D::native_extension_static().data() );
        point_set_input_factory_3d.register_creator< OpenGeodePointSetInput3D >(
            OpenGeodePointSet3D::native_extension_static().data() );

        polygonal_surface_input_factory_2d
            .register_creator< OpenGeodePolygonalSurfaceInput2D >(
                OpenGeodePolygonalSurface2D::native_extension_static().data() );
        polygonal_surface_input_factory_3d
            .register_creator< OpenGeodePolygonalSurfaceInput3D >(
                OpenGeodePolygonalSurface3D::native_extension_static().data() );

        regular_grid_input_factory_2d
            .register_creator< OpenGeodeRegularGridInput2D >(
                OpenGeodeRegularGrid2D::native_extension_static().data() );
        regular_grid_input_factory_3d
            .register_creator< OpenGeodeRegularGridInput3D >(
                OpenGeodeRegularGrid3D::native_extension_static().data() );

        triangulated_surface_input_factory_2d.register_creator<
            OpenGeodeTriangulatedSurfaceInput2D >(
            OpenGeodeTriangulatedSurface2D::native_extension_static().data() );
        triangulated_surface_input_factory_3d.register_creator<
            OpenGeodeTriangulatedSurfaceInput3D >(
            OpenGeodeTriangulatedSurface3D::native_extension_static().data() );

        hybrid_solid_input_factory_3d
            .register_creator< OpenGeodeHybridSolidInput3D >(
                OpenGeodeHybridSolid3D::native_extension_static().data() );

        polyhedral_solid_input_factory_3d
            .register_creator< OpenGeodePolyhedralSolidInput3D >(
                OpenGeodePolyhedralSolid3D::native_extension_static().data() );

        tetrahedral_solid_input_factory_3d
            .register_creator< OpenGeodeTetrahedralSolidInput3D >(
                OpenGeodeTetrahedralSolid3D::native_extension_static().data() );

        light_regular_grid_input_factory_2d
            .register_creator< LightRegularGridInput2D >(
                LightRegularGrid2D::native_extension_static().data() );
        light_regular_grid_input_factory_3d
            .register_creator< LightRegularGridInput3D >(
                LightRegularGrid3D::native_extension_static().data() );
    }
} // namespace geode
