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

#include <geode/basic/logger.h>

#include <geode/mesh/core/geode/geode_edged_curve.h>
#include <geode/mesh/core/geode/geode_graph.h>
#include <geode/mesh/core/geode/geode_point_set.h>
#include <geode/mesh/core/geode/geode_polygonal_surface.h>
#include <geode/mesh/core/geode/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode/geode_tetrahedral_solid.h>
#include <geode/mesh/core/geode/geode_triangulated_surface.h>
#include <geode/mesh/io/geode/geode_edged_curve_input.h>
#include <geode/mesh/io/geode/geode_graph_input.h>
#include <geode/mesh/io/geode/geode_point_set_input.h>
#include <geode/mesh/io/geode/geode_polygonal_surface_input.h>
#include <geode/mesh/io/geode/geode_polyhedral_solid_input.h>
#include <geode/mesh/io/geode/geode_tetrahedral_solid_input.h>
#include <geode/mesh/io/geode/geode_triangulated_surface_input.h>
#include <geode/mesh/io/geode/register_input.h>

#include <geode/tests/common.h>

template < typename GeodeFactory >
void check_register( absl::string_view extension )
{
    OPENGEODE_EXCEPTION( GeodeFactory::has_creator( extension.data() ),
        "[Test] No creator for extension ", extension, " is not correct" );
}

void test()
{
    /* To initialize the mesh library */
    geode::register_geode_mesh_input();

    /* Run checks */
    check_register< geode::GraphInputFactory >(
        geode::OpenGeodeGraph::native_extension_static().data() );
    check_register< geode::PointSetInputFactory2D >(
        geode::OpenGeodePointSet2D::native_extension_static().data() );
    check_register< geode::PointSetInputFactory3D >(
        geode::OpenGeodePointSet3D::native_extension_static().data() );
    check_register< geode::EdgedCurveInputFactory2D >(
        geode::OpenGeodeEdgedCurve2D::native_extension_static().data() );
    check_register< geode::EdgedCurveInputFactory3D >(
        geode::OpenGeodeEdgedCurve3D::native_extension_static().data() );
    check_register< geode::PolygonalSurfaceInputFactory2D >(
        geode::OpenGeodePolygonalSurface2D::native_extension_static().data() );
    check_register< geode::PolygonalSurfaceInputFactory3D >(
        geode::OpenGeodePolygonalSurface3D::native_extension_static().data() );
    check_register< geode::TriangulatedSurfaceInputFactory2D >(
        geode::OpenGeodeTriangulatedSurface2D::native_extension_static()
            .data() );
    check_register< geode::TriangulatedSurfaceInputFactory3D >(
        geode::OpenGeodeTriangulatedSurface3D::native_extension_static()
            .data() );
    check_register< geode::PolyhedralSolidInputFactory3D >(
        geode::OpenGeodePolyhedralSolid3D::native_extension_static().data() );
    check_register< geode::TetrahedralSolidInputFactory3D >(
        geode::OpenGeodeTetrahedralSolid3D::native_extension_static().data() );
}

OPENGEODE_TEST( "register-input" )