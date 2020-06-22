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

#include <geode/mesh/core/detail/register_mesh.h>
#include <geode/mesh/core/geode_edged_curve.h>
#include <geode/mesh/core/geode_graph.h>
#include <geode/mesh/core/geode_point_set.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode_tetrahedral_solid.h>
#include <geode/mesh/core/geode_triangulated_surface.h>
#include <geode/mesh/core/mesh_factory.h>

#include <geode/tests/common.h>

void check_register( const geode::MeshImpl& key )
{
    OPENGEODE_EXCEPTION( geode::MeshFactory::has_creator( key ),
        "[Test] No creator for ", key.get(), " is not correct" );
}

void test()
{
    /* To initialize the mesh library */
    geode::register_geode_mesh();

    /* Run checks */
    check_register( geode::OpenGeodeGraph::impl_name_static() );
    check_register( geode::OpenGeodePointSet2D::impl_name_static() );
    check_register( geode::OpenGeodePointSet3D::impl_name_static() );
    check_register( geode::OpenGeodeEdgedCurve2D::impl_name_static() );
    check_register( geode::OpenGeodeEdgedCurve3D::impl_name_static() );
    check_register( geode::OpenGeodePolygonalSurface2D::impl_name_static() );
    check_register( geode::OpenGeodePolygonalSurface3D::impl_name_static() );
    check_register( geode::OpenGeodeTriangulatedSurface2D::impl_name_static() );
    check_register( geode::OpenGeodeTriangulatedSurface3D::impl_name_static() );
    check_register( geode::OpenGeodePolyhedralSolid3D::impl_name_static() );
    check_register( geode::OpenGeodeTetrahedralSolid3D::impl_name_static() );
}

OPENGEODE_TEST( "register-mesh" )