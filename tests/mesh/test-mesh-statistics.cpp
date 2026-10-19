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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/mesh_statistics.hpp>

#include <geode/tests/common.hpp>

namespace
{
    void test_degenerate_curve()
    {
        auto curve = geode::EdgedCurve3D::create();
        auto builder = geode::EdgedCurveBuilder3D::create( *curve );
        builder->create_point( geode::Point3D{ { 1, 2, 3 } } );
        builder->create_point( geode::Point3D{ { 1, 2, 3 } } );
        builder->create_edge( 0, 1 );
        const auto statistics = geode::compute_curve_statistics( *curve );
        geode::OpenGeodeMeshException::test( statistics.max_edge_size == 0.,
            "Wrong max edge size on degenerate curve: ",
            statistics.max_edge_size );
        geode::OpenGeodeMeshException::test( statistics.min_edge_size == 0.,
            "Wrong min edge size on degenerate curve" );
        geode::OpenGeodeMeshException::test( statistics.mean_edge_size == 0.,
            "Wrong mean edge size on degenerate curve" );
    }

    void test_curve()
    {
        auto curve = geode::EdgedCurve2D::create();
        auto builder = geode::EdgedCurveBuilder2D::create( *curve );
        builder->create_point( geode::Point2D{ { 0, 0 } } );
        builder->create_point( geode::Point2D{ { 1, 0 } } );
        builder->create_point( geode::Point2D{ { 1, 3 } } );
        builder->create_edge( 0, 1 );
        builder->create_edge( 1, 2 );
        const auto statistics = geode::compute_curve_statistics( *curve );
        geode::OpenGeodeMeshException::test(
            statistics.max_edge_size == 3., "Wrong max edge size on curve" );
        geode::OpenGeodeMeshException::test(
            statistics.min_edge_size == 1., "Wrong min edge size on curve" );
        geode::OpenGeodeMeshException::test(
            statistics.mean_edge_size == 2., "Wrong mean edge size on curve" );
    }

    void test_degenerate_surface()
    {
        auto surface = geode::SurfaceMesh3D::create();
        auto builder = geode::SurfaceMeshBuilder3D::create( *surface );
        builder->create_point( geode::Point3D{ { 1, 1, 1 } } );
        builder->create_point( geode::Point3D{ { 1, 1, 1 } } );
        builder->create_point( geode::Point3D{ { 1, 1, 1 } } );
        builder->create_polygon( { 0, 1, 2 } );
        builder->compute_polygon_adjacencies();
        const auto statistics = geode::compute_surface_statistics( *surface );
        geode::OpenGeodeMeshException::test( statistics.max_edge_size == 0.,
            "Wrong max edge size on degenerate surface: ",
            statistics.max_edge_size );
    }
} // namespace

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_degenerate_curve();
    test_curve();
    test_degenerate_surface();
}

OPENGEODE_TEST( "mesh-statistics" )
