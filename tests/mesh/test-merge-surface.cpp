/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/surface_mesh.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/io/triangulated_surface_input.h>

#include <geode/mesh/helpers/convert_surface_mesh.h>

void test()
{
    std::vector< geode::Point2D > points{ { { 0, 0 } }, { { 0, 1 } },
        { { 0, 2 } }, { { 1, 0 } }, { { 1, 1 } }, { { 1, 2 } } };

    auto mesh0 = geode::SurfaceMesh2D::create();
    auto builder0 = geode::SurfaceMeshBuilder2D::create( *mesh0 );
    builder0->create_point( points[0] );
    builder0->create_point( points[1] );
    builder0->create_point( points[3] );
    builder0->create_point( points[4] );
    builder0->create_polygon( { 0, 1, 3 } );
    builder0->create_polygon( { 0, 3, 2 } );
    builder0->compute_polygon_adjacencies();

    auto mesh1 = geode::SurfaceMesh2D::create();
    auto builder1 = geode::SurfaceMeshBuilder2D::create( *mesh1 );
    builder1->create_point( points[1] );
    builder1->create_point( points[2] );
    builder1->create_point( points[4] );
    builder1->create_point( points[5] );
    builder1->create_polygon( { 0, 1, 3 } );
    builder1->create_polygon( { 0, 3, 2 } );
    builder1->compute_polygon_adjacencies();

    std::vector< std::reference_wrapper< const geode::SurfaceMesh2D > > meshes{
        *mesh0, *mesh1
    };
    const auto merged = geode::merge_surface_meshes< 2 >( meshes );
    OPENGEODE_EXCEPTION(
        merged->nb_vertices() == 6, "[Test] Wrong number of vertices" );
    OPENGEODE_EXCEPTION(
        merged->nb_polygons() == 4, "[Test] Wrong number of polygons" );

    OPENGEODE_EXCEPTION(
        !merged->polygon_adjacent( { 0, 0 } ), "[Test] Wrong adjacency" );
    OPENGEODE_EXCEPTION(
        !merged->polygon_adjacent( { 2, 0 } ), "[Test] Wrong adjacency" );

    OPENGEODE_EXCEPTION(
        merged->polygon_adjacent( { 0, 2 } ) == 1, "[Test] Wrong adjacency" );
    OPENGEODE_EXCEPTION(
        merged->polygon_adjacent( { 1, 0 } ) == 0, "[Test] Wrong adjacency" );
    OPENGEODE_EXCEPTION(
        merged->polygon_adjacent( { 2, 2 } ) == 3, "[Test] Wrong adjacency" );
    OPENGEODE_EXCEPTION(
        merged->polygon_adjacent( { 3, 0 } ) == 2, "[Test] Wrong adjacency" );

    OPENGEODE_EXCEPTION(
        !merged->polygon_adjacent( { 0, 1 } ), "[Test] Wrong adjacency" );
    OPENGEODE_EXCEPTION(
        !merged->polygon_adjacent( { 3, 2 } ), "[Test] Wrong adjacency" );
}

OPENGEODE_TEST( "merge-surface" )