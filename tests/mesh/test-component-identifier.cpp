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
#include <geode/mesh/helpers/detail/component_identifier.hpp>

void test_components_before_identification()
{
    auto surface = geode::SurfaceMesh3D::create();
    auto builder = geode::SurfaceMeshBuilder3D::create( *surface );
    builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
    builder->create_point( geode::Point3D{ { 1, 0, 0 } } );
    builder->create_point( geode::Point3D{ { 0, 1, 0 } } );
    builder->create_point( geode::Point3D{ { 1, 1, 0 } } );
    builder->create_polygon( { 0, 1, 2 } );
    builder->create_polygon( { 1, 3, 2 } );
    builder->compute_polygon_adjacencies();
    geode::detail::SurfaceIdentifier3D identifier{ *surface };
    const auto no_components = identifier.identified_connected_polygons();
    geode::OpenGeodeMeshException::test( no_components.empty(),
        "No component should be returned before identification" );
    geode::OpenGeodeMeshException::test(
        identifier.identify_polygons() == 1, "Wrong number of components" );
    const auto components = identifier.identified_connected_polygons();
    geode::OpenGeodeMeshException::test(
        components.size() == 1 && components[0].size() == 2,
        "Wrong components after identification" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_components_before_identification();
}

OPENGEODE_TEST( "component-identifier" )
