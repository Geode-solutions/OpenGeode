/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/common.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/detail/mesh_intersection_detection.hpp>

#include <geode/tests/common.hpp>

void test_nested_triangles_2d()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    // Big triangle
    builder->create_point( geode::Point2D{ { 0, 0 } } );
    builder->create_point( geode::Point2D{ { 10, 0 } } );
    builder->create_point( geode::Point2D{ { 0, 10 } } );
    // Small triangle strictly inside, no common vertex
    builder->create_point( geode::Point2D{ { 1, 1 } } );
    builder->create_point( geode::Point2D{ { 2, 1 } } );
    builder->create_point( geode::Point2D{ { 1, 2 } } );
    // Small triangle nested in the corner of the big one at vertex 0
    builder->create_point( geode::Point2D{ { 3, 1 } } );
    builder->create_point( geode::Point2D{ { 1, 3 } } );
    // Triangle outside the big one sharing vertex 0
    builder->create_point( geode::Point2D{ { -3, -1 } } );
    builder->create_point( geode::Point2D{ { -1, -3 } } );

    const geode::SurfaceMesh2D& mesh = *surface;
    const geode::PolygonVertices big{ 0, 1, 2 };
    const geode::PolygonVertices inside{ 3, 4, 5 };
    const geode::PolygonVertices corner{ 0, 6, 7 };
    const geode::PolygonVertices outside{ 0, 8, 9 };
    geode::OpenGeodeMeshException::test(
        geode::detail::polygons_intersection_detection( mesh, big, inside )
            && geode::detail::polygons_intersection_detection(
                mesh, inside, big ),
        "[Test] Triangle inside another should intersect" );
    geode::OpenGeodeMeshException::test(
        geode::detail::polygons_intersection_detection( mesh, big, corner )
            && geode::detail::polygons_intersection_detection(
                mesh, corner, big ),
        "[Test] Triangle nested in a corner should intersect" );
    geode::OpenGeodeMeshException::test(
        !geode::detail::polygons_intersection_detection( mesh, big, outside )
            && !geode::detail::polygons_intersection_detection(
                mesh, outside, big ),
        "[Test] Triangles only sharing a vertex should not intersect" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_nested_triangles_2d();
}

OPENGEODE_TEST( "mesh-intersection-detection" )
