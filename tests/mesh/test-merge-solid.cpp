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

#include <geode/tests/common.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/solid_mesh_builder.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/helpers/convert_solid_mesh.h>

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    std::vector< geode::Point3D > points{
        geode::Point3D{ { -1, 0, 0 } },
        geode::Point3D{ { -1, -1, -1 } },
        geode::Point3D{ { 0, 0, 0 } },
        geode::Point3D{ { 1, 0, 0 } },
        geode::Point3D{ { 0, 1, 0 } },
        geode::Point3D{ { 1, 1, 0 } },
        geode::Point3D{ { 0, 0, 1 } },
        geode::Point3D{ { 1, 0, 1 } },
        geode::Point3D{ { 0, 1, 1 } },
        geode::Point3D{ { 1, 1, 1 } },
    };

    auto mesh0 = geode::SolidMesh3D::create();
    auto builder0 = geode::SolidMeshBuilder3D::create( *mesh0 );
    builder0->create_point( points[0] );
    builder0->create_point( points[1] );
    builder0->create_point( points[2] );
    builder0->create_point( points[4] );
    builder0->create_point( points[8] );
    builder0->create_point( points[6] );
    builder0->create_polyhedron( { 0, 1, 2, 3 },
        { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
    builder0->create_polyhedron( { 0, 2, 5, 3 },
        { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
    builder0->create_polyhedron( { 0, 3, 5, 4 },
        { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
    builder0->compute_polyhedron_adjacencies();

    auto mesh1 = geode::SolidMesh3D::create();
    auto builder1 = geode::SolidMeshBuilder3D::create( *mesh1 );
    builder1->create_point( points[0] );
    builder1->create_point( points[2] );
    builder1->create_point( points[3] );
    builder1->create_point( points[4] );
    builder1->create_point( points[5] );
    builder1->create_point( points[6] );
    builder1->create_point( points[7] );
    builder1->create_point( points[8] );
    builder1->create_point( points[9] );
    builder1->create_polyhedron( { 0, 1, 5, 3 },
        { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
    builder1->create_polyhedron( { 0, 3, 5, 7 },
        { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
    builder1->create_polyhedron( { 1, 2, 3, 4, 5, 6, 7, 8 },
        { { 0, 1, 2, 3 }, { 4, 7, 6, 5 }, { 0, 4, 5, 1 }, { 2, 6, 7, 3 },
            { 1, 5, 6, 2 }, { 0, 3, 7, 4 } } );
    builder1->compute_polyhedron_adjacencies();

    std::vector< std::reference_wrapper< const geode::SolidMesh3D > > meshes{
        *mesh0, *mesh1
    };
    const auto merged = geode::merge_solid_meshes( meshes );
    OPENGEODE_EXCEPTION(
        merged->nb_vertices() == 10, "[Test] Wrong number of vertices" );
    OPENGEODE_EXCEPTION(
        merged->nb_polyhedra() == 4, "[Test] Wrong number of polyhedra" );
}

OPENGEODE_TEST( "merge-solid" )