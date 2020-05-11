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

/*
 * Modified from RINGMesh https://github.com/ringmesh/RINGMesh
 * Copyright (c) 2012-2018, Association Scientifique pour la Geologie et ses
 * Applications (ASGA)
 */

#include <geode/basic/logger.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/point.h>
#include <geode/mesh/core/aabb_helpers.h>

#include <geode/tests/common.h>

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
void check_edgecurve_tree( const geode::EdgedCurve< dimension >& edge_curve )
{
    auto aabb_tree = geode::create_aabb_tree< dimension >( edge_curve );
    geode::DistanceToEdge< dimension > distance_action( edge_curve );

    for( const auto e : geode::Range{ edge_curve.nb_edges() } )
    {
        const auto barycenter = edge_curve.edge_barycenter( e );
        geode::index_t closest_edge;
        std::tie( closest_edge, std::ignore, std::ignore ) =
            aabb_tree.closest_element_box( barycenter, distance_action );

        OPENGEODE_EXCEPTION( closest_edge == e, "[TEST] Wrong edge found" );
    }
}

template < geode::index_t dimension >
void test_EdgeCurveAABB()
{
    geode::Logger::info( "TEST", " EdgeCurve AABB ", dimension, "D" );
    auto edge_curve = geode::EdgedCurve< dimension >::create();
    auto edge_curve_builder =
        geode::EdgedCurveBuilder< dimension >::create( *edge_curve );

    geode::index_t size = 10;
    add_vertices( *edge_curve_builder, size );
    add_edges( *edge_curve_builder, size );

    check_edgecurve_tree< dimension >( *edge_curve );
}

template < geode::index_t dimension >
void add_vertices( geode::TriangulatedSurfaceBuilder< dimension >& builder,
    geode::index_t size )
{
    builder.create_vertices( size * size );
    geode::index_t id = 0;
    for( const auto i : geode::Range{ size } )
    {
        for( const auto j : geode::Range{ size } )
        {
            builder.set_point( id++, create_vertex< dimension >( i, j ) );
        }
    }
}

template < geode::index_t dimension >
void add_triangles( geode::TriangulatedSurfaceBuilder< dimension >& builder,
    geode::index_t size )
{
    builder.reserve_triangles( ( size - 1 ) * ( size - 1 ) * 2 );
    geode::index_t id = 0;
    for( const auto i : geode::Range{ size - 1 } )
    {
        for( const auto j : geode::Range{ size - 1 } )
        {
            builder.create_triangle(
                { i * size + j, i * size + j + 1, ( i + 1 ) * size + j } );
            id++;
            builder.create_triangle( { i * size + j + 1,
                ( i + 1 ) * size + j + 1, ( i + 1 ) * size + j } );
            id++;
        }
    }
}

template < geode::index_t dimension >
void check_surface_tree( const geode::AABBTree< dimension >& tree,
    const geode::DistanceToTriangle< dimension >& distance_action,
    geode::index_t size )
{
    constexpr auto offset = 0.2;
    geode::index_t id = 0;
    for( const auto i : geode::Range{ size - 1 } )
    {
        for( const auto j : geode::Range{ size - 1 } )
        {
            const auto query1 =
                create_vertex< dimension >( i + offset, j + offset );
            geode::index_t triangle1;
            geode::Point< dimension > nearest_point1;
            std::tie( triangle1, nearest_point1, std::ignore ) =
                tree.closest_element_box( query1, distance_action );
            OPENGEODE_EXCEPTION(
                triangle1 == id++, "[TEST] Wrong triangle found" );
            OPENGEODE_EXCEPTION(
                nearest_point1 == query1, "[TEST] Wrong nearest point found" );

            const auto query2 =
                create_vertex< dimension >( i + 1 - offset, j + 1 - offset );
            geode::index_t triangle2;
            geode::Point< dimension > nearest_point2;
            std::tie( triangle2, nearest_point2, std::ignore ) =
                tree.closest_element_box( query2, distance_action );
            OPENGEODE_EXCEPTION(
                triangle2 == id++, "[TEST] Wrong triangle found" );
            OPENGEODE_EXCEPTION(
                nearest_point2 == query2, "[TEST] Wrong nearest point found" );
        }
    }

    geode::Point< dimension > query;
    geode::index_t triangle;
    geode::Point< dimension > nearest_point;
    std::tie( triangle, nearest_point, std::ignore ) =
        tree.closest_element_box( query, distance_action );
    OPENGEODE_EXCEPTION( triangle == 0, "[TEST] Wrong triangle found" );
    OPENGEODE_EXCEPTION( nearest_point == geode::Point< dimension >(),
        "[TEST] Wrong nearest point found" );
}

template < geode::index_t dimension >
void test_SurfaceAABB()
{
    geode::Logger::info(
        "TEST", " TriangulatedSurface AABB Helper", dimension, "D" );

    auto t_surf = geode::TriangulatedSurface< dimension >::create();
    auto t_surf_builder =
        geode::TriangulatedSurfaceBuilder< dimension >::create( *t_surf );

    geode::index_t size = 10;
    add_vertices( *t_surf_builder, size );
    add_triangles( *t_surf_builder, size );

    auto aabb_tree = create_aabb_tree( *t_surf );
    geode::DistanceToTriangle< dimension > distance_action( *t_surf );

    check_surface_tree< dimension >( aabb_tree, distance_action, size );
}

template < geode::index_t dimension >
void do_test()
{
    test_SurfaceAABB< dimension >();
    test_EdgeCurveAABB< dimension >();
}

void test()
{
    do_test< 2 >();
    do_test< 3 >();
}

OPENGEODE_TEST( "aabb-helpers" )
