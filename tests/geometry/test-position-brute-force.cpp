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

#include <array>
#include <cstdint>
#include <map>
#include <string>

#include <absl/numeric/int128.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/common.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/intersection_detection.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/position.hpp>

#include <geode/tests/common.hpp>

#include "brute-force.hpp"

/*
 * Exact predicates are checked on exact constructions: integer vertices and
 * points defined as exact barycentric combinations (weights over a power of
 * two denominator), so that the expected position is known by construction.
 * Side predicates are checked against exact integer orientations.
 */
namespace
{
    // Portable 128 bits integers (no __int128 with MSVC)
    using int128 = absl::int128;
    constexpr std::int64_t DENOMINATOR{ 1024 };
    constexpr std::int64_t RANGE{ 1 << 20 };

    template < std::size_t dimension >
    using IntPoint = std::array< std::int64_t, dimension >;

    /*
     * Integer point scaled by DENOMINATOR (exact in double)
     */
    template < std::size_t dimension >
    geode::Point< static_cast< geode::index_t >( dimension ) > to_point(
        const IntPoint< dimension >& scaled )
    {
        geode::Point< static_cast< geode::index_t >( dimension ) > point;
        for( const auto d : geode::LRange{ dimension } )
        {
            point.set_value( d, static_cast< double >( scaled[d] )
                                    / static_cast< double >( DENOMINATOR ) );
        }
        return point;
    }

    template < geode::index_t dimension >
    IntPoint< dimension > random_vertex()
    {
        IntPoint< dimension > vertex;
        for( auto& value : vertex )
        {
            value = random_int( -RANGE, RANGE ) * DENOMINATOR;
        }
        return vertex;
    }

    /*
     * Small random vertices make degenerated configurations more frequent
     */
    template < geode::index_t dimension >
    IntPoint< dimension > random_small_vertex()
    {
        IntPoint< dimension > vertex;
        for( auto& value : vertex )
        {
            value = random_int( -4, 4 ) * DENOMINATOR;
        }
        return vertex;
    }

    template < std::size_t dimension, std::size_t nb_vertices >
    IntPoint< dimension > combination(
        const std::array< IntPoint< dimension >, nb_vertices >& vertices,
        const std::array< std::int64_t, nb_vertices >& weights )
    {
        IntPoint< dimension > result{};
        for( const auto v : geode::LRange{ nb_vertices } )
        {
            for( const auto d : geode::LRange{ dimension } )
            {
                result[d] += weights[v] * vertices[v][d] / DENOMINATOR;
            }
        }
        return result;
    }

    template < std::size_t dimension >
    IntPoint< dimension > add( const IntPoint< dimension >& point,
        const IntPoint< dimension >& offset,
        std::int64_t factor = 1 )
    {
        auto result = point;
        for( const auto d : geode::LRange{ dimension } )
        {
            result[d] += factor * offset[d];
        }
        return result;
    }

    template < std::size_t dimension >
    IntPoint< dimension > difference(
        const IntPoint< dimension >& from, const IntPoint< dimension >& to )
    {
        return add( to, from, -1 );
    }

    int128 orient_2d(
        const IntPoint< 2 >& a, const IntPoint< 2 >& b, const IntPoint< 2 >& c )
    {
        return static_cast< int128 >( b[0] - a[0] ) * ( c[1] - a[1] )
               - static_cast< int128 >( b[1] - a[1] ) * ( c[0] - a[0] );
    }

    std::array< int128, 3 > cross(
        const IntPoint< 3 >& u, const IntPoint< 3 >& v )
    {
        return { static_cast< int128 >( u[1] ) * v[2]
                     - static_cast< int128 >( u[2] ) * v[1],
            static_cast< int128 >( u[2] ) * v[0]
                - static_cast< int128 >( u[0] ) * v[2],
            static_cast< int128 >( u[0] ) * v[1]
                - static_cast< int128 >( u[1] ) * v[0] };
    }

    int128 dot( const std::array< int128, 3 >& u, const IntPoint< 3 >& v )
    {
        return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
    }

    int128 orient_3d( const IntPoint< 3 >& a,
        const IntPoint< 3 >& b,
        const IntPoint< 3 >& c,
        const IntPoint< 3 >& d )
    {
        return dot( cross( difference( a, b ), difference( a, c ) ),
            difference( a, d ) );
    }

    template < std::size_t dimension >
    bool is_collinear( const IntPoint< dimension >& a,
        const IntPoint< dimension >& b,
        const IntPoint< dimension >& c )
    {
        if constexpr( dimension == 2 )
        {
            return orient_2d( a, b, c ) == 0;
        }
        else
        {
            const auto product =
                cross( difference( a, b ), difference( a, c ) );
            return product[0] == 0 && product[1] == 0 && product[2] == 0;
        }
    }

    template < std::size_t dimension >
    std::string hex( const IntPoint< dimension >& point )
    {
        return hex( to_point( point ) );
    }

    void check_position( const std::string& name,
        geode::POSITION result,
        geode::POSITION expected,
        const std::string& context )
    {
        if( result != expected )
        {
            record(
                name, context + " got "
                          + std::to_string( static_cast< int >( result ) )
                          + " expected "
                          + std::to_string( static_cast< int >( expected ) ) );
        }
    }

    /*
     * Random weights summing to DENOMINATOR: strictly inside, with zero
     * weights (on a face, edge or vertex) or with a negative weight (outside)
     */
    template < std::size_t nb_vertices >
    std::array< std::int64_t, nb_vertices > random_weights(
        geode::index_t mode )
    {
        std::array< std::int64_t, nb_vertices > weights;
        do
        {
            std::int64_t sum{ 0 };
            for( const auto v : geode::LRange{ nb_vertices - 1 } )
            {
                switch( mode % 3 )
                {
                    case 0:
                        weights[v] = random_int( 1, DENOMINATOR );
                        break;
                    case 1:
                        weights[v] = random_int( 0, 2 ) == 0
                                         ? 0
                                         : random_int( 1, DENOMINATOR );
                        break;
                    default:
                        weights[v] = random_int( -DENOMINATOR, DENOMINATOR );
                }
                sum += weights[v];
            }
            weights[nb_vertices - 1] = DENOMINATOR - sum;
        } while( mode % 3 != 2 && weights[nb_vertices - 1] < 0 );
        if( mode % 3 == 1 && random_int( 0, 1 ) == 0 )
        {
            // Swap the last weight to also get it null
            std::swap( weights[0], weights[nb_vertices - 1] );
        }
        return weights;
    }

    /*
     * Expected position from the weights: indices of the non null weights
     */
    template < std::size_t nb_vertices >
    geode::POSITION expected_position(
        const std::array< std::int64_t, nb_vertices >& weights )
    {
        std::vector< geode::local_index_t > non_null;
        for( const auto v : geode::LRange{ nb_vertices } )
        {
            if( weights[v] < 0 )
            {
                return geode::POSITION::outside;
            }
            if( weights[v] > 0 )
            {
                non_null.push_back( v );
            }
        }
        if( non_null.size() == nb_vertices )
        {
            return geode::POSITION::inside;
        }
        if( non_null.size() == 1 )
        {
            return static_cast< geode::POSITION >(
                static_cast< int >( geode::POSITION::vertex0 ) + non_null[0] );
        }
        if constexpr( nb_vertices == 3 )
        {
            // Edge i is between vertices i and i+1
            const auto first = non_null[0];
            const auto second = non_null[1];
            const auto edge = second == first + 1 ? first : second;
            return static_cast< geode::POSITION >(
                static_cast< int >( geode::POSITION::edge0 ) + edge );
        }
        else
        {
            if( non_null.size() == 3 )
            {
                // Facet opposite to the null weight vertex
                for( const auto v : geode::LRange{ 4 } )
                {
                    if( weights[v] == 0 )
                    {
                        return static_cast< geode::POSITION >(
                            static_cast< int >( geode::POSITION::facet0 ) + v );
                    }
                }
            }
            static const std::map< std::pair< int, int >, geode::POSITION >
                edges{ { { 0, 1 }, geode::POSITION::edge01 },
                    { { 0, 2 }, geode::POSITION::edge02 },
                    { { 0, 3 }, geode::POSITION::edge03 },
                    { { 1, 2 }, geode::POSITION::edge12 },
                    { { 1, 3 }, geode::POSITION::edge13 },
                    { { 2, 3 }, geode::POSITION::edge23 } };
            return edges.at( { non_null[0], non_null[1] } );
        }
    }

    template < geode::index_t dimension >
    void test_point_segment_position()
    {
        const std::string name =
            "point_segment_position " + std::to_string( dimension ) + "D";
        for( const auto trial : geode::Range{ 20000 } )
        {
            current_mode = trial % 2 == 0 ? " [large]" : " [small]";
            const std::array< IntPoint< dimension >, 2 > vertices{
                trial % 2 == 0 ? random_vertex< dimension >()
                               : random_small_vertex< dimension >(),
                trial % 2 == 0 ? random_vertex< dimension >()
                               : random_small_vertex< dimension >()
            };
            if( vertices[0] == vertices[1] )
            {
                continue;
            }
            const auto weights = random_weights< 2 >( trial / 2 );
            auto point = combination( vertices, weights );
            auto expected = expected_position( weights );
            if( trial % 5 == 4 )
            {
                // Close to the segment line
                IntPoint< dimension > offset;
                for( auto& value : offset )
                {
                    value = random_int( -3, 3 );
                }
                const auto moved = add( point, offset );
                if( !is_collinear( vertices[0], vertices[1], moved ) )
                {
                    point = moved;
                    expected = geode::POSITION::outside;
                }
            }
            const auto p0 = to_point( vertices[0] );
            const auto p1 = to_point( vertices[1] );
            const geode::Segment< dimension > segment{ p0, p1 };
            check_position( name,
                geode::point_segment_position( to_point( point ), segment ),
                expected,
                "S " + hex( vertices[0] ) + hex( vertices[1] ) + "P "
                    + hex( point ) );
        }
    }

    template < geode::index_t dimension >
    void test_point_triangle_position()
    {
        const std::string name =
            "point_triangle_position " + std::to_string( dimension ) + "D";
        for( const auto trial : geode::Range{ 20000 } )
        {
            current_mode = trial % 2 == 0 ? " [large]" : " [small]";
            std::array< IntPoint< dimension >, 3 > vertices;
            for( auto& vertex : vertices )
            {
                vertex = trial % 2 == 0 ? random_vertex< dimension >()
                                        : random_small_vertex< dimension >();
            }
            if( is_collinear( vertices[0], vertices[1], vertices[2] ) )
            {
                continue;
            }
            const auto weights = random_weights< 3 >( trial / 2 );
            auto point = combination( vertices, weights );
            auto expected = expected_position( weights );
            if constexpr( dimension == 3 )
            {
                if( trial % 5 == 4 )
                {
                    // Close to the triangle plane
                    IntPoint< 3 > offset;
                    for( auto& value : offset )
                    {
                        value = random_int( -3, 3 );
                    }
                    const auto moved = add( point, offset );
                    if( orient_3d(
                            vertices[0], vertices[1], vertices[2], moved )
                        != 0 )
                    {
                        point = moved;
                        expected = geode::POSITION::outside;
                    }
                }
            }
            const auto p0 = to_point( vertices[0] );
            const auto p1 = to_point( vertices[1] );
            const auto p2 = to_point( vertices[2] );
            const geode::Triangle< dimension > triangle{ p0, p1, p2 };
            check_position( name,
                geode::point_triangle_position( to_point( point ), triangle ),
                expected,
                "T " + hex( vertices[0] ) + hex( vertices[1] )
                    + hex( vertices[2] ) + "P " + hex( point ) );
        }
    }

    void test_point_tetrahedron_position()
    {
        const std::string name = "point_tetrahedron_position";
        for( const auto trial : geode::Range{ 20000 } )
        {
            current_mode = trial % 2 == 0 ? " [large]" : " [small]";
            std::array< IntPoint< 3 >, 4 > vertices;
            for( auto& vertex : vertices )
            {
                vertex = trial % 2 == 0 ? random_vertex< 3 >()
                                        : random_small_vertex< 3 >();
            }
            if( orient_3d( vertices[0], vertices[1], vertices[2], vertices[3] )
                == 0 )
            {
                continue;
            }
            const auto weights = random_weights< 4 >( trial / 2 );
            const auto point = combination( vertices, weights );
            const auto p0 = to_point( vertices[0] );
            const auto p1 = to_point( vertices[1] );
            const auto p2 = to_point( vertices[2] );
            const auto p3 = to_point( vertices[3] );
            const geode::Tetrahedron tetra{ p0, p1, p2, p3 };
            check_position( name,
                geode::point_tetrahedron_position( to_point( point ), tetra ),
                expected_position( weights ),
                "T " + hex( vertices[0] ) + hex( vertices[1] )
                    + hex( vertices[2] ) + hex( vertices[3] ) + "P "
                    + hex( point ) );
        }
    }

    /*
     * The side must be the sign of the exact orientation, up to a global
     * convention sign
     */
    void check_side( const std::string& name,
        geode::SIDE side,
        int128 orientation,
        std::map< std::string, int >& conventions,
        const std::string& context )
    {
        if( orientation == 0 )
        {
            if( side != geode::SIDE::zero )
            {
                record( name + " not zero", context );
            }
            return;
        }
        if( side == geode::SIDE::zero )
        {
            record( name + " zero", context );
            return;
        }
        const auto sign =
            ( side == geode::SIDE::positive ) == ( orientation > 0 ) ? 1 : -1;
        auto& convention = conventions[name];
        if( convention == 0 )
        {
            convention = sign;
        }
        else if( convention != sign )
        {
            record( name + " wrong sign", context );
        }
    }

    void test_sides()
    {
        std::map< std::string, int > conventions;
        for( const auto trial : geode::Range{ 20000 } )
        {
            current_mode = trial % 2 == 0 ? " [large]" : " [small]";
            const auto random = [&trial]( auto dimension_tag ) {
                constexpr geode::index_t dimension =
                    decltype( dimension_tag )::value;
                return trial % 2 == 0 ? random_vertex< dimension >()
                                      : random_small_vertex< dimension >();
            };
            using Two = std::integral_constant< geode::index_t, 2 >;
            using Three = std::integral_constant< geode::index_t, 3 >;
            // 2D: point side to a segment (collinear points are frequent with
            // small coordinates or exact combinations)
            const auto a = random( Two{} );
            const auto b = random( Two{} );
            if( a != b )
            {
                const auto c = trial % 3 == 0 ? combination< 2, 2 >( { a, b },
                                                    random_weights< 2 >( 2 ) )
                                              : random( Two{} );
                const auto pa = to_point( a );
                const auto pb = to_point( b );
                const auto context =
                    "S " + hex( a ) + hex( b ) + "P " + hex( c );
                check_side( "point_side_to_segment",
                    geode::point_side_to_segment(
                        to_point( c ), geode::Segment2D{ pa, pb } ),
                    orient_2d( a, b, c ), conventions, context );
                check_side( "are_points_aligned 2D",
                    geode::are_points_aligned( pa, pb, to_point( c ) )
                        ? geode::SIDE::zero
                        : geode::SIDE::positive,
                    orient_2d( a, b, c ) == 0 ? 0 : 1, conventions, context );
                // Axis aligned line: exact direction
                const auto axis =
                    static_cast< geode::local_index_t >( random_int( 0, 1 ) );
                IntPoint< 2 > unit{};
                unit[axis] = DENOMINATOR;
                geode::Vector2D direction;
                direction.set_value( axis, 1 );
                check_side( "point_side_to_line",
                    geode::point_side_to_line(
                        to_point( c ), geode::InfiniteLine2D{ direction, pa } ),
                    orient_2d( a, add( a, unit ), c ), conventions,
                    context + " axis " + std::to_string( axis ) );
            }
            // 3D: point side to a triangle
            const std::array< IntPoint< 3 >, 3 > triangle{ random( Three{} ),
                random( Three{} ), random( Three{} ) };
            if( !is_collinear( triangle[0], triangle[1], triangle[2] ) )
            {
                const auto point =
                    trial % 3 == 0
                        ? combination( triangle, random_weights< 3 >( 2 ) )
                        : random( Three{} );
                const auto p0 = to_point( triangle[0] );
                const auto p1 = to_point( triangle[1] );
                const auto p2 = to_point( triangle[2] );
                check_side( "point_side_to_triangle",
                    geode::point_side_to_triangle(
                        to_point( point ), geode::Triangle3D{ p0, p1, p2 } ),
                    orient_3d( triangle[0], triangle[1], triangle[2], point ),
                    conventions,
                    "T " + hex( triangle[0] ) + hex( triangle[1] )
                        + hex( triangle[2] ) + "P " + hex( point ) );
                check_side( "are_points_aligned 3D",
                    geode::are_points_aligned( p0, p1, to_point( point ) )
                        ? geode::SIDE::zero
                        : geode::SIDE::positive,
                    is_collinear( triangle[0], triangle[1], point ) ? 0 : 1,
                    conventions,
                    "P " + hex( triangle[0] ) + hex( triangle[1] )
                        + hex( point ) );
                // Axis aligned plane normal: exact plane
                const auto axis =
                    static_cast< geode::local_index_t >( random_int( 0, 2 ) );
                geode::Vector3D normal;
                normal.set_value( axis, 1 );
                const auto plane_side = geode::point_side_to_plane(
                    to_point( point ), geode::Plane{ normal, p0 } );
                check_side( "point_side_to_plane", plane_side,
                    point[axis] - triangle[0][axis], conventions,
                    "O " + hex( triangle[0] ) + "P " + hex( point ) + " axis "
                        + std::to_string( axis ) );
            }
        }
    }
    void check_pair( const std::string& name,
        const std::pair< geode::POSITION, geode::POSITION >& result,
        const std::pair< geode::POSITION, geode::POSITION >& expected,
        const std::string& context )
    {
        if( result != expected )
        {
            record( name,
                context + " got "
                    + std::to_string( static_cast< int >( result.first ) ) + ","
                    + std::to_string( static_cast< int >( result.second ) )
                    + " expected "
                    + std::to_string( static_cast< int >( expected.first ) )
                    + ","
                    + std::to_string( static_cast< int >( expected.second ) ) );
        }
    }

    /*
     * Position on a segment X - k0*u, X + k1*u of the point X
     */
    geode::POSITION segment_position_from_steps(
        std::int64_t step0, std::int64_t step1 )
    {
        if( step0 == 0 )
        {
            return geode::POSITION::vertex0;
        }
        if( step1 == 0 )
        {
            return geode::POSITION::vertex1;
        }
        return geode::POSITION::inside;
    }

    template < std::size_t dimension >
    IntPoint< dimension > random_direction( bool small )
    {
        IntPoint< dimension > direction;
        for( auto& value : direction )
        {
            value = small ? random_int( -3, 3 ) * DENOMINATOR
                          : random_int( -RANGE, RANGE ) * DENOMINATOR / 64;
        }
        return direction;
    }

    template < std::size_t dimension >
    void test_segment_segment_detection()
    {
        constexpr auto geode_dimension =
            static_cast< geode::index_t >( dimension );
        const std::string name = "segment_segment_intersection_detection "
                                 + std::to_string( dimension ) + "D";
        for( const auto trial : geode::Range{ 20000 } )
        {
            const auto small = trial % 2 == 1;
            const std::array< IntPoint< dimension >, 2 > vertices{
                small ? random_small_vertex< geode_dimension >()
                      : random_vertex< geode_dimension >(),
                small ? random_small_vertex< geode_dimension >()
                      : random_vertex< geode_dimension >()
            };
            if( vertices[0] == vertices[1] )
            {
                continue;
            }
            const auto weights = random_weights< 2 >( trial / 2 );
            const auto crossing = combination( vertices, weights );
            const auto direction = random_direction< dimension >( small );
            const auto step0 = random_int( 0, 2 );
            const auto step1 =
                step0 == 0 ? random_int( 1, 2 ) : random_int( 0, 2 );
            auto q0 = add( crossing, direction, -step0 );
            auto q1 = add( crossing, direction, step1 );
            const auto mode = trial / 2 % 4;
            std::pair< geode::POSITION, geode::POSITION > expected;
            if( mode == 3 )
            {
                // Collinear segments
                current_mode = " [collinear]";
                q0 = combination( vertices, random_weights< 2 >( 2 ) );
                q1 = combination( vertices, random_weights< 2 >( 2 ) );
                if( q0 == q1 )
                {
                    continue;
                }
                expected = { geode::POSITION::parallel,
                    geode::POSITION::parallel };
            }
            else
            {
                current_mode = small ? " [small]" : " [large]";
                if( is_collinear( vertices[0], vertices[1],
                        add( vertices[0], direction ) ) )
                {
                    continue;
                }
                const auto position0 = expected_position( weights );
                expected =
                    position0 == geode::POSITION::outside
                        ? std::make_pair( geode::POSITION::outside,
                              geode::POSITION::outside )
                        : std::make_pair( position0,
                              segment_position_from_steps( step0, step1 ) );
                if constexpr( dimension == 3 )
                {
                    if( mode == 2 )
                    {
                        // Skew segments
                        current_mode += " [skew]";
                        IntPoint< 3 > offset;
                        for( auto& value : offset )
                        {
                            value = random_int( -3, 3 );
                        }
                        if( orient_3d( vertices[0], vertices[1], q0,
                                add( q0, offset ) )
                            == 0 )
                        {
                            continue;
                        }
                        q0 = add( q0, offset );
                        q1 = add( q1, offset );
                        expected = { geode::POSITION::outside,
                            geode::POSITION::outside };
                    }
                }
            }
            const auto p0 = to_point( vertices[0] );
            const auto p1 = to_point( vertices[1] );
            const auto pq0 = to_point( q0 );
            const auto pq1 = to_point( q1 );
            check_pair( name,
                geode::segment_segment_intersection_detection(
                    geode::Segment< geode_dimension >{ p0, p1 },
                    geode::Segment< geode_dimension >{ pq0, pq1 } ),
                expected,
                "S0 " + hex( vertices[0] ) + hex( vertices[1] ) + "S1 "
                    + hex( q0 ) + hex( q1 ) );
        }
    }

    void test_segment_triangle_detection()
    {
        const std::string name = "segment_triangle_intersection_detection";
        for( const auto trial : geode::Range{ 20000 } )
        {
            const auto small = trial % 2 == 1;
            current_mode = small ? " [small]" : " [large]";
            std::array< IntPoint< 3 >, 3 > vertices;
            for( auto& vertex : vertices )
            {
                vertex =
                    small ? random_small_vertex< 3 >() : random_vertex< 3 >();
            }
            if( is_collinear( vertices[0], vertices[1], vertices[2] ) )
            {
                continue;
            }
            const auto weights = random_weights< 3 >( trial / 2 );
            const auto crossing = combination( vertices, weights );
            const auto direction = random_direction< 3 >( small );
            if( orient_3d( vertices[0], vertices[1], vertices[2],
                    add( vertices[0], direction ) )
                == 0 )
            {
                continue;
            }
            const auto step0 = random_int( 0, 2 );
            const auto step1 =
                step0 == 0 ? random_int( 1, 2 ) : random_int( 0, 2 );
            const auto q0 = add( crossing, direction, -step0 );
            const auto q1 = add( crossing, direction, step1 );
            const auto triangle_position = expected_position( weights );
            const auto expected =
                triangle_position == geode::POSITION::outside
                    ? std::make_pair(
                          geode::POSITION::outside, geode::POSITION::outside )
                    : std::make_pair(
                          segment_position_from_steps( step0, step1 ),
                          triangle_position );
            const auto p0 = to_point( vertices[0] );
            const auto p1 = to_point( vertices[1] );
            const auto p2 = to_point( vertices[2] );
            const auto pq0 = to_point( q0 );
            const auto pq1 = to_point( q1 );
            const geode::Triangle3D triangle{ p0, p1, p2 };
            const auto context = "T " + hex( vertices[0] ) + hex( vertices[1] )
                                 + hex( vertices[2] ) + "S " + hex( q0 )
                                 + hex( q1 );
            check_pair( name,
                geode::segment_triangle_intersection_detection(
                    geode::Segment3D{ pq0, pq1 }, triangle ),
                expected, context );

            // Axis aligned line through the crossing point
            const auto axis =
                static_cast< geode::local_index_t >( random_int( 0, 2 ) );
            IntPoint< 3 > unit{};
            unit[axis] = DENOMINATOR;
            if( orient_3d( vertices[0], vertices[1], vertices[2],
                    add( vertices[0], unit ) )
                == 0 )
            {
                continue;
            }
            geode::Vector3D line_direction;
            line_direction.set_value( axis, 1 );
            const auto origin =
                to_point( add( crossing, unit, -random_int( -2, 2 ) ) );
            check_position( "line_triangle_intersection_detection",
                geode::line_triangle_intersection_detection(
                    geode::InfiniteLine3D{ line_direction, origin }, triangle ),
                triangle_position,
                context + " axis " + std::to_string( axis ) );

            // Axis aligned plane through the crossing point
            geode::Point3D plane_location;
            plane_location.set_value(
                axis, static_cast< double >( crossing[axis] )
                          / static_cast< double >( DENOMINATOR ) );
            geode::Vector3D normal;
            normal.set_value( axis, 1 );
            const auto height0 = q0[axis] - crossing[axis];
            const auto height1 = q1[axis] - crossing[axis];
            geode::POSITION plane_expected;
            if( height0 == 0 && height1 == 0 )
            {
                plane_expected = geode::POSITION::parallel;
            }
            else if( height0 == 0 )
            {
                plane_expected = geode::POSITION::vertex0;
            }
            else if( height1 == 0 )
            {
                plane_expected = geode::POSITION::vertex1;
            }
            else
            {
                plane_expected = ( height0 > 0 ) != ( height1 > 0 )
                                     ? geode::POSITION::inside
                                     : geode::POSITION::outside;
            }
            if( plane_expected != geode::POSITION::parallel )
            {
                check_position( "segment_plane_intersection_detection",
                    geode::segment_plane_intersection_detection(
                        geode::Segment3D{ pq0, pq1 },
                        geode::Plane{ normal, plane_location } ),
                    plane_expected,
                    context + " axis " + std::to_string( axis ) );
            }
        }
    }

    void test_segment_line_detection()
    {
        const std::string name = "segment_line_intersection_detection";
        for( const auto trial : geode::Range{ 20000 } )
        {
            const auto small = trial % 2 == 1;
            current_mode = small ? " [small]" : " [large]";
            const std::array< IntPoint< 2 >, 2 > vertices{
                small ? random_small_vertex< 2 >() : random_vertex< 2 >(),
                small ? random_small_vertex< 2 >() : random_vertex< 2 >()
            };
            const auto axis =
                static_cast< geode::local_index_t >( random_int( 0, 1 ) );
            IntPoint< 2 > unit{};
            unit[axis] = DENOMINATOR;
            if( vertices[0] == vertices[1]
                || is_collinear(
                    vertices[0], vertices[1], add( vertices[0], unit ) ) )
            {
                continue;
            }
            const auto weights = random_weights< 2 >( trial / 2 );
            const auto crossing = combination( vertices, weights );
            geode::Vector2D direction;
            direction.set_value( axis, 1 );
            const auto origin =
                to_point( add( crossing, unit, random_int( -2, 2 ) ) );
            const auto p0 = to_point( vertices[0] );
            const auto p1 = to_point( vertices[1] );
            check_position( name,
                geode::segment_line_intersection_detection(
                    geode::Segment2D{ p0, p1 },
                    geode::InfiniteLine2D{ direction, origin } ),
                expected_position( weights ),
                "S " + hex( vertices[0] ) + hex( vertices[1] ) + "X "
                    + hex( crossing ) + " axis " + std::to_string( axis ) );
        }
    }
} // namespace

void test()
{
    // Exact predicates need the library initialization
    geode::OpenGeodeGeometryLibrary::initialize();
    test_point_segment_position< 2 >();
    test_point_segment_position< 3 >();
    test_point_triangle_position< 2 >();
    test_point_triangle_position< 3 >();
    test_point_tetrahedron_position();
    test_sides();
    test_segment_segment_detection< 2 >();
    test_segment_segment_detection< 3 >();
    test_segment_triangle_detection();
    test_segment_line_detection();
    report_issues( "Position" );
}

OPENGEODE_TEST( "position-brute-force" )
