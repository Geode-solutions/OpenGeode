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

#include <geode/tests/common.hpp>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/detail/mesh_intersection_detection.hpp>

template < geode::index_t dimension >
std::unique_ptr< geode::SurfaceMesh< dimension > > create_surface()
{
    auto surface = geode::SurfaceMesh< dimension >::create();
    auto builder = geode::SurfaceMeshBuilder< dimension >::create( *surface );
    // In 2D, the notch triangle lies inside the notch of the concave quad.
    // In 3D, it is vertical and crosses the quad plane inside the notch.
    std::array< std::array< double, 3 >, 10 > coordinates{ {
        // concave quad (chevron) with a notch below vertex 1
        { 0, 0, 0 },
        { 2, 1, 0 },
        { 4, 0, 0 },
        { 2, 3, 0 },
        // triangle inside the notch: no intersection
        { 1.8, 0.2, 0 },
        { 2.2, 0.2, 0 },
        { 2, -0.5, 0 },
        // triangle crossing edge 2-3 of the quad
        { 2, 2, 0 },
        { 5, 2, 0 },
        { 5, 2.5, 0 },
    } };
    if constexpr( dimension == 3 )
    {
        coordinates[4] = { 1.8, 0.4, 1 };
        coordinates[5] = { 2.2, 0.4, 1 };
        coordinates[6] = { 2, 0.4, -1 };
    }
    for( const auto& xyz : coordinates )
    {
        geode::Point< dimension > point;
        for( const auto d : geode::LRange{ dimension } )
        {
            point.set_value( d, xyz[d] );
        }
        builder->create_point( point );
    }
    builder->create_polygon( { 0, 1, 2, 3 } );
    builder->create_polygon( { 4, 5, 6 } );
    builder->create_polygon( { 7, 8, 9 } );
    builder->create_polygon( { 4, 6, 5 } );
    return surface;
}

template < geode::index_t dimension >
void test_concave_polygons()
{
    const auto surface = create_surface< dimension >();
    const auto quad = surface->polygon_vertices( 0 );
    const auto in_notch = surface->polygon_vertices( 1 );
    const auto crossing = surface->polygon_vertices( 2 );
    geode::OpenGeodeMeshException::test(
        !geode::detail::polygons_intersection_detection(
            *surface, quad, in_notch ),
        "[", dimension,
        "D] Triangle in the notch of a concave quad should not intersect it" );
    geode::OpenGeodeMeshException::test(
        !geode::detail::polygons_intersection_detection(
            *surface, in_notch, quad ),
        "[", dimension,
        "D] Triangle in the notch of a concave quad should not intersect it "
        "(swapped)" );
    geode::OpenGeodeMeshException::test(
        geode::detail::polygons_intersection_detection(
            *surface, quad, crossing ),
        "[", dimension,
        "D] Triangle crossing a concave quad edge should intersect it" );
    geode::OpenGeodeMeshException::test(
        geode::detail::polygons_intersection_detection(
            *surface, in_notch, surface->polygon_vertices( 3 ) ),
        "[", dimension, "D] Identical triangles should intersect" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_concave_polygons< 2 >();
    test_concave_polygons< 3 >();
}

OPENGEODE_TEST( "mesh-intersection-detection" )
