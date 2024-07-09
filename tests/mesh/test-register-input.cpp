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
void check_register( const GeodeFactory& factory, absl::string_view extension )
{
    OPENGEODE_EXCEPTION( factory.has_creator( extension.data() ),
        "[Test] No creator for extension ", extension, " is not correct" );
}

void test()
{
    /* To initialize the mesh library */
    geode::register_geode_mesh_input();

    /* Run checks */
    check_register( geode::graph_input_factory,
        geode::OpenGeodeGraph::native_extension_static().data() );
    check_register( geode::point_set_input_factory_2d,
        geode::OpenGeodePointSet2D::native_extension_static().data() );
    check_register( geode::point_set_input_factory_3d,
        geode::OpenGeodePointSet3D::native_extension_static().data() );
    check_register( geode::edged_curve_input_factory_2d,
        geode::OpenGeodeEdgedCurve2D::native_extension_static().data() );
    check_register( geode::edged_curve_input_factory_3d,
        geode::OpenGeodeEdgedCurve3D::native_extension_static().data() );
    check_register( geode::polygonal_surface_input_factory_2d,
        geode::OpenGeodePolygonalSurface2D::native_extension_static().data() );
    check_register( geode::polygonal_surface_input_factory_3d,
        geode::OpenGeodePolygonalSurface3D::native_extension_static().data() );
    check_register( geode::triangulated_surface_input_factory_2d,
        geode::OpenGeodeTriangulatedSurface2D::native_extension_static()
            .data() );
    check_register( geode::triangulated_surface_input_factory_3d,
        geode::OpenGeodeTriangulatedSurface3D::native_extension_static()
            .data() );
    check_register( geode::polyhedral_solid_input_factory_3d,
        geode::OpenGeodePolyhedralSolid3D::native_extension_static().data() );
    check_register( geode::tetrahedral_solid_input_factory_3d,
        geode::OpenGeodeTetrahedralSolid3D::native_extension_static().data() );
}

OPENGEODE_TEST( "register-input" )