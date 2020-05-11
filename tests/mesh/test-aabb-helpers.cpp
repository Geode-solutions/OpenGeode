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

#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/mesh/core/aabb_helpers.h>

#include <geode/tests/common.h>

using namespace geode;

template < index_t dimension >
Point< dimension > create_vertex( double i, double j )
{
    Point< dimension > point;
    point.set_value( 0, i );
    point.set_value( 1, j );
    return point;
}

template < index_t dimension >
void add_vertices(
    TriangulatedSurfaceBuilder< dimension >& builder, index_t size )
{
    builder.create_vertices( size * size );
    index_t id = 0;
    for( index_t i : Range( size ) )
    {
        for( index_t j : Range( size ) )
        {
            builder.set_point( id++, create_vertex< dimension >( i, j ) );
        }
    }
}

template < index_t dimension >
void add_triangles(
    TriangulatedSurfaceBuilder< dimension >& builder, index_t size )
{
    builder.create_triangles( ( size - 1 ) * ( size - 1 ) * 2 );
    index_t id = 0;
    for( const auto i : Range{ size - 1 } )
    {
        for( index_t j : Range( size - 1 ) )
        {
            builder.set_polygon_vertex( { id, 0 }, i * size + j );
            builder.set_polygon_vertex( { id, 1 }, i * size + j + 1 );
            builder.set_polygon_vertex( { id, 2 }, ( i + 1 ) * size + j );
            id++;
            builder.set_polygon_vertex( { id, 0 }, i * size + j + 1 );
            builder.set_polygon_vertex( { id, 1 }, ( i + 1 ) * size + j + 1 );
            builder.set_polygon_vertex( { id, 2 }, ( i + 1 ) * size + j );
            id++;
        }
    }
}

template < index_t dimension >
void check_tree( const AABBTree< dimension >& tree,
    const DistanceToTriangle< dimension >& distance_action,
    index_t size )
{
    constexpr auto offset = 0.2;
    index_t id = 0;
    for( index_t i : Range( size - 1 ) )
    {
        for( index_t j : Range( size - 1 ) )
        {
            Point< dimension > query1 =
                create_vertex< dimension >( i + offset, j + offset );
            index_t triangle1 = NO_ID;
            Point< dimension > nearest_point1;
            std::tie( triangle1, nearest_point1, std::ignore ) =
                tree.closest_element_box( query1, distance_action );
            OPENGEODE_EXCEPTION(
                triangle1 == id++, "[TEST] Wrong triangle found" );
            OPENGEODE_EXCEPTION(
                nearest_point1 == query1, "[TEST] Wrong nearest point found" );

            Point< dimension > query2 =
                create_vertex< dimension >( i + 1 - offset, j + 1 - offset );
            index_t triangle2 = NO_ID;
            Point< dimension > nearest_point2;
            std::tie( triangle2, nearest_point2, std::ignore ) =
                tree.closest_element_box( query2, distance_action );
            OPENGEODE_EXCEPTION(
                triangle2 == id++, "[TEST] Wrong triangle found" );
            OPENGEODE_EXCEPTION(
                nearest_point2 == query2, "[TEST] Wrong nearest point found" );
        }
    }

    Point< dimension > query;
    index_t triangle = NO_ID;
    Point< dimension > nearest_point;
    std::tie( triangle, nearest_point, std::ignore ) =
        tree.closest_element_box( query, distance_action );
    OPENGEODE_EXCEPTION( triangle == 0, "[TEST] Wrong triangle found" );
    OPENGEODE_EXCEPTION( nearest_point == Point< dimension >(),
        "[TEST] Wrong nearest point found" );
}

template < index_t dimension >
void test_SurfaceAABB()
{
    Logger::info( "TEST", " TriangulatedSurface AABB Helper", dimension, "D" );

    auto t_surf = TriangulatedSurface< dimension >::create();
    auto t_surf_builder =
        TriangulatedSurfaceBuilder< dimension >::create( *t_surf );

    index_t size = 10;
    add_vertices( *t_surf_builder, size );
    add_triangles( *t_surf_builder, size );

    auto aabb_tree = get_aabb( *t_surf );
    DistanceToTriangle< dimension > distance_action( *t_surf );

    check_tree< dimension >( *aabb_tree, distance_action, size );
}
template < index_t dimension >
void do_test()
{
    test_SurfaceAABB< dimension >();
}

void test()
{
    do_test< 2 >();
    do_test< 3 >();
}

OPENGEODE_TEST( "aabb-helpers" )
