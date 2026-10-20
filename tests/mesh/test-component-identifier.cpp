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

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/graph_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/graph.hpp>
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

void test_star_curve()
{
    constexpr geode::index_t nb_branches{ 5000 };
    auto curve = geode::EdgedCurve3D::create();
    auto builder = geode::EdgedCurveBuilder3D::create( *curve );
    builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
    for( const auto e : geode::Range{ nb_branches } )
    {
        const auto vertex = builder->create_point(
            geode::Point3D{ { static_cast< double >( e ), 1, 0 } } );
        builder->create_edge( 0, vertex );
    }
    const auto v0 = builder->create_point( geode::Point3D{ { 0, 5, 0 } } );
    const auto v1 = builder->create_point( geode::Point3D{ { 0, 6, 0 } } );
    builder->create_edge( v0, v1 );
    geode::detail::EdgedCurveIdentifier3D identifier{ *curve };
    geode::OpenGeodeMeshException::test(
        identifier.identify_edges() == 2, "Wrong number of curve components" );
    const auto components = identifier.identified_connected_edges();
    geode::OpenGeodeMeshException::test(
        components.size() == 2 && components[0].size() == nb_branches
            && components[1].size() == 1,
        "Wrong curve components" );
    for( const auto e : geode::Range{ nb_branches } )
    {
        geode::OpenGeodeMeshException::test(
            identifier.edge_identifier( e ) == 0, "Wrong edge identifier" );
    }
    geode::OpenGeodeMeshException::test(
        identifier.edge_identifier( nb_branches ) == 1,
        "Wrong isolated edge identifier" );
}

void test_graph_max_elements()
{
    auto graph = geode::Graph::create();
    auto builder = geode::GraphBuilder::create( *graph );
    builder->create_vertices( 5 );
    for( const auto v : geode::Range{ 4 } )
    {
        builder->create_edge( v, v + 1 );
    }
    geode::detail::GraphIdentifier identifier{ *graph };
    geode::OpenGeodeMeshException::test( identifier.identify_vertices( 2 ) == 3,
        "Wrong number of graph components with a size limit" );
    const auto components = identifier.identified_connected_vertices();
    geode::OpenGeodeMeshException::test(
        components[0] == std::vector< geode::index_t >{ 0, 1 }
            && components[1] == std::vector< geode::index_t >{ 2, 3 }
            && components[2] == std::vector< geode::index_t >{ 4 },
        "Wrong graph components with a size limit" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_components_before_identification();
    test_star_curve();
    test_graph_max_elements();
}

OPENGEODE_TEST( "component-identifier" )
