/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/geometrical_operations_on_mesh.h>

#include <geode/tests/common.h>

std::unique_ptr< geode::TriangulatedSurface3D > create_surface()
{
    auto surface = geode::TriangulatedSurface3D::create();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_point( { { 0.1, 0.2, 0.3 } } );
    builder->create_point( { { 2.1, 9.4, 6.7 } } );
    builder->create_point( { { 7.5, 5.2, 6.3 } } );
    builder->create_point( { { 8.1, 1.4, 4.7 } } );
    builder->create_point( { { 4.7, 2.1, 1.3 } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 1, 3, 2 } );
    builder->create_triangle( { 3, 4, 2 } );
    builder->compute_polygon_adjacencies();
    return surface;
}

void test_rescale( geode::TriangulatedSurface3D& surface )
{
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( surface );
    geode::rescale_mesh( surface, *builder, { 2, -2, 0.1 } );
    OPENGEODE_EXCEPTION(
        surface.point( 0 ).inexact_equal( { { 0.2, -0.4, 0.03 } } ),
        "[Test] Wrong rescale of vertex 0" );
    OPENGEODE_EXCEPTION(
        surface.point( 1 ).inexact_equal( { { 4.2, -18.8, 0.67 } } ),
        "[Test] Wrong rescale of vertex 1" );
    OPENGEODE_EXCEPTION(
        surface.point( 2 ).inexact_equal( { { 15.0, -10.4, 0.63 } } ),
        "[Test] Wrong rescale of vertex 2" );
    OPENGEODE_EXCEPTION(
        surface.point( 3 ).inexact_equal( { { 16.2, -2.8, 0.47 } } ),
        "[Test] Wrong rescale of vertex 3" );
    OPENGEODE_EXCEPTION(
        surface.point( 4 ).inexact_equal( { { 9.4, -4.2, 0.13 } } ),
        "[Test] Wrong rescale of vertex 4" );
}

void test_translate( geode::TriangulatedSurface3D& surface )
{
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( surface );
    geode::translate_mesh( surface, *builder, { { 2, -2, 1 } } );
    OPENGEODE_EXCEPTION(
        surface.point( 0 ).inexact_equal( { { 2.1, -1.8, 1.3 } } ),
        "[Test] Wrong translation of vertex 0" );
    OPENGEODE_EXCEPTION(
        surface.point( 1 ).inexact_equal( { { 4.1, 7.4, 7.7 } } ),
        "[Test] Wrong translation of vertex 1" );
    OPENGEODE_EXCEPTION(
        surface.point( 2 ).inexact_equal( { { 9.5, 3.2, 7.3 } } ),
        "[Test] Wrong translation of vertex 2" );
    OPENGEODE_EXCEPTION(
        surface.point( 3 ).inexact_equal( { { 10.1, -0.6, 5.7 } } ),
        "[Test] Wrong translation of vertex 3" );
    OPENGEODE_EXCEPTION(
        surface.point( 4 ).inexact_equal( { { 6.7, 0.1, 2.3 } } ),
        "[Test] Wrong translation of vertex 4" );
}

void test()
{
    geode::OpenGeodeMesh::initialize();
    auto surf = create_surface();
    test_rescale( *surf->clone() );
    test_translate( *surf->clone() );
}

OPENGEODE_TEST( "nnsearch-point-set" )
