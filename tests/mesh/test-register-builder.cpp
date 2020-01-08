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

#include <geode/basic/logger.h>

#include <geode/mesh/builder/detail/register_builder.h>
#include <geode/mesh/builder/geode_edged_curve_builder.h>
#include <geode/mesh/builder/geode_graph_builder.h>
#include <geode/mesh/builder/geode_point_set_builder.h>
#include <geode/mesh/builder/geode_polygonal_surface_builder.h>
#include <geode/mesh/builder/geode_polyhedral_solid_builder.h>
#include <geode/mesh/builder/geode_tetrahedral_solid_builder.h>
#include <geode/mesh/builder/geode_triangulated_surface_builder.h>
#include <geode/mesh/core/geode_edged_curve.h>
#include <geode/mesh/core/geode_graph.h>
#include <geode/mesh/core/geode_point_set.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode_tetrahedral_solid.h>
#include <geode/mesh/core/geode_triangulated_surface.h>

#include <geode/tests/common.h>

template < typename GeodeFactory >
void check_register( const geode::MeshType& key )
{
    OPENGEODE_EXCEPTION( GeodeFactory::has_creator( key ),
        "[Test] No creator for ", key.get(), " is not correct" );
}

void test()
{
    /* To initialize the mesh library */
    geode::register_geode_builder();

    /* Run checks */
    check_register< geode::GraphBuilderFactory >(
        geode::OpenGeodeGraph::type_name_static() );
    check_register< geode::PointSetBuilderFactory2D >(
        geode::OpenGeodePointSet2D::type_name_static() );
    check_register< geode::PointSetBuilderFactory3D >(
        geode::OpenGeodePointSet3D::type_name_static() );
    check_register< geode::EdgedCurveBuilderFactory2D >(
        geode::OpenGeodeEdgedCurve2D::type_name_static() );
    check_register< geode::EdgedCurveBuilderFactory3D >(
        geode::OpenGeodeEdgedCurve3D::type_name_static() );
    check_register< geode::PolygonalSurfaceBuilderFactory2D >(
        geode::OpenGeodePolygonalSurface2D::type_name_static() );
    check_register< geode::PolygonalSurfaceBuilderFactory3D >(
        geode::OpenGeodePolygonalSurface3D::type_name_static() );
    check_register< geode::TriangulatedSurfaceBuilderFactory2D >(
        geode::OpenGeodeTriangulatedSurface2D::type_name_static() );
    check_register< geode::TriangulatedSurfaceBuilderFactory3D >(
        geode::OpenGeodeTriangulatedSurface3D::type_name_static() );
    check_register< geode::PolyhedralSolidBuilderFactory3D >(
        geode::OpenGeodePolyhedralSolid3D::type_name_static() );
    check_register< geode::TetrahedralSolidBuilderFactory3D >(
        geode::OpenGeodeTetrahedralSolid3D::type_name_static() );
}

OPENGEODE_TEST( "register-builder" )