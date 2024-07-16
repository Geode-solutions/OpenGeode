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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#include <geode/basic/logger.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/helpers/aabb_edged_curve_helpers.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/point.hpp>

#include <geode/tests/common.hpp>

template < geode::index_t dimension >
geode::Point< dimension > create_vertex( double i, double j )
{
    geode::Point< dimension > point;
    point.set_value( 0, i );
    point.set_value( 1, j );
    return point;
}

template < geode::index_t dimension >
void add_vertices(
    geode::EdgedCurveBuilder< dimension >& builder, geode::index_t size )
{
    builder.create_vertices( size );
    for( const auto i : geode::Range{ size } )
    {
        builder.set_point( i, create_vertex< dimension >( i, i + 1 ) );
    }
}

template < geode::index_t dimension >
void add_edges(
    geode::EdgedCurveBuilder< dimension >& builder, geode::index_t size )
{
    builder.create_edges( size - 1 );
    for( const auto i : geode::Range{ size - 1 } )
    {
        builder.set_edge_vertex( { i, 0 }, i );
        builder.set_edge_vertex( { i, 1 }, i + 1 );
    }
}

template < geode::index_t dimension >
void check_edgedcurve_tree( const geode::EdgedCurve< dimension >& edged_curve )
{
    const auto aabb_tree = geode::create_aabb_tree< dimension >( edged_curve );
    geode::DistanceToEdge< dimension > distance_action( edged_curve );

    for( const auto e : geode::Range{ edged_curve.nb_edges() } )
    {
        const auto barycenter = edged_curve.edge_barycenter( e );
        geode::index_t closest_edge;
        std::tie( closest_edge, std::ignore ) =
            aabb_tree.closest_element_box( barycenter, distance_action );

        OPENGEODE_EXCEPTION( closest_edge == e, "[TEST] Wrong edge found" );
    }
}

template < geode::index_t dimension >
void test_EdgedCurveAABB()
{
    geode::Logger::info( "TEST", " EdgedCurve AABB ", dimension, "D" );
    auto edged_curve = geode::EdgedCurve< dimension >::create();
    auto edged_curve_builder =
        geode::EdgedCurveBuilder< dimension >::create( *edged_curve );

    geode::index_t size = 10;
    add_vertices( *edged_curve_builder, size );
    add_edges( *edged_curve_builder, size );

    check_edgedcurve_tree< dimension >( *edged_curve );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_EdgedCurveAABB< 2 >();
    test_EdgedCurveAABB< 3 >();
}

OPENGEODE_TEST( "aabb-edged-curve-helpers" )
