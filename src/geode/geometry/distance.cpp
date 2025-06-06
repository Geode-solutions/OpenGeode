/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <optional>

#include <geode/basic/logger.hpp>
#include <geode/geometry/distance.hpp>

#include <absl/algorithm/container.h>

#include <geode/geometry/barycentric_coordinates.hpp>
#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/ellipse.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/perpendicular.hpp>
#include <geode/geometry/projection.hpp>
#include <geode/geometry/sign.hpp>
#include <geode/geometry/vector.hpp>

namespace
{
    /*
     * This function tests if the point is in the triangle by computing signed
     * areas
     * @warning This is not robust in edge cases!
     */
    bool may_point_be_in_triangle(
        const geode::Point2D& point, const geode::Triangle2D& triangle )
    {
        const auto& vertices = triangle.vertices();
        const auto signed_area_1 = geode::triangle_signed_area(
            geode::Triangle2D{ vertices[0], vertices[1], point } );
        const auto signed_area_2 = geode::triangle_signed_area(
            geode::Triangle2D{ vertices[1], vertices[2], point } );
        const auto signed_area_3 = geode::triangle_signed_area(
            geode::Triangle2D{ vertices[2], vertices[0], point } );

        return ( signed_area_1 <= 0. && signed_area_2 <= 0.
                   && signed_area_3 <= 0 )
               || ( signed_area_1 >= 0. && signed_area_2 >= 0.
                    && signed_area_3 >= 0 );
    }

    std::optional< double > compute_point_line_distance( double segment_length,
        double point_to_v0_length,
        double point_to_v1_length )
    {
        const auto p =
            ( point_to_v0_length + point_to_v1_length + segment_length ) / 2;
        if( p - point_to_v0_length <= geode::GLOBAL_EPSILON )
        {
            return std::nullopt;
        }
        if( p - point_to_v1_length <= geode::GLOBAL_EPSILON )
        {
            return std::nullopt;
        }
        if( p - segment_length <= geode::GLOBAL_EPSILON )
        {
            return std::nullopt;
        }
        const auto area2 = p * ( p - point_to_v0_length )
                           * ( p - point_to_v1_length )
                           * ( p - segment_length );
        return 2 * std::sqrt( area2 ) / segment_length;
    }

    void get_min_edge02( double a11, double b1, std::array< double, 2 >& p )
    {
        p[0] = 0;
        if( b1 >= 0 )
        {
            p[1] = 0;
        }
        else if( a11 + b1 <= 0 )
        {
            p[1] = 1;
        }
        else
        {
            p[1] = -b1 / a11;
        }
    }

    void get_min_edge12( double a01,
        double a11,
        double b1,
        double f10,
        double f01,
        std::array< double, 2 >& p )
    {
        const auto h0 = a01 + b1 - f10;
        if( h0 >= 0 )
        {
            p[1] = 0;
        }
        else
        {
            const auto h1 = a11 + b1 - f01;
            if( h1 <= 0 )
            {
                p[1] = 1;
            }
            else
            {
                p[1] = h0 / ( h0 - h1 );
            }
        }
        p[0] = 1 - p[1];
    }

    void get_min_interior( const std::array< double, 2 >& p0,
        double h0,
        const std::array< double, 2 >& p1,
        double h1,
        std::array< double, 2 >& p )
    {
        const auto z = h0 / ( h0 - h1 );
        const auto omz = 1 - z;
        p[0] = omz * p0[0] + z * p1[0];
        p[1] = omz * p0[1] + z * p1[1];
    }

    template < geode::index_t dimension >
    std::tuple< double, geode::Point< dimension > >
        point_segment_distance_using_projection(
            const geode::Point< dimension >& point,
            const geode::Segment< dimension >& segment )
    {
        const auto nearest_p = point_segment_projection( point, segment );
        return std::make_tuple(
            point_point_distance( point, nearest_p ), nearest_p );
    }

    template < geode::index_t dimension >
    std::tuple< double, geode::Point< dimension > >
        point_line_distance_using_projection(
            const geode::Point< dimension >& point,
            const geode::InfiniteLine< dimension >& line )
    {
        const auto nearest_p = point_line_projection( point, line );
        return std::make_tuple(
            point_point_distance( point, nearest_p ), nearest_p );
    }

    std::tuple< double, geode::Point3D > no_pivot_point_triangle_distance(
        const geode::Point3D& point, const geode::Triangle3D& triangle )
    {
        std::array< std::tuple< double, geode::Point3D >, 3 > edge_distances;
        auto min_distance = std::numeric_limits< double >::max();
        geode::local_index_t selected_edge{ geode::NO_LID };
        const auto& vertices = triangle.vertices();
        for( const auto e : geode::LRange{ 3 } )
        {
            const auto next = e == 2 ? 0 : e + 1;
            edge_distances[e] = point_segment_distance_using_projection(
                point, { vertices[e], vertices[next] } );
            const auto& cur_distance = std::get< 0 >( edge_distances[e] );
            if( cur_distance < min_distance )
            {
                min_distance = cur_distance;
                selected_edge = e;
            }
        }
        return edge_distances[selected_edge];
    }

    std::tuple< double, geode::Point3D > pivot_point_triangle_distance(
        const geode::Point3D& point,
        const geode::Triangle3D& triangle,
        geode::local_index_t v0 )
    {
        const auto v1 = v0 == 2 ? 0 : v0 + 1;
        const auto v2 = v1 == 2 ? 0 : v1 + 1;
        const auto& vertices = triangle.vertices();
        const geode::Vector3D edge0{ vertices[v0], vertices[v1] };
        const geode::Vector3D edge1{ vertices[v0], vertices[v2] };
        const auto a00 = edge0.length2();
        const auto a01 = edge0.dot( edge1 );
        const auto a11 = edge1.length2();
        const geode::Vector3D diff{ vertices[v0], point };
        const auto b0 = -diff.dot( edge0 );
        const auto b1 = -diff.dot( edge1 );

        auto f00 = b0;
        auto f10 = b0 + a00;
        auto f01 = b0 + a01;

        std::array< double, 2 > p0, p1, p;
        double dt1, h0, h1;

        if( f00 >= 0 )
        {
            if( f01 >= 0 )
            { // (1) p0 = (0,0), p1 = (0,1), H(z) = G(L(z))
                get_min_edge02( a11, b1, p );
            }
            else
            { // (2) p0 = (0,t10), p1 = (t01,1-t01),
                // H(z) = (t11 - t10)*G(L(z))
                p0[0] = 0;
                p0[1] = f00 / ( f00 - f01 );
                p1[0] = f01 / ( f01 - f10 );
                p1[1] = 1 - p1[0];
                dt1 = p1[1] - p0[1];
                h0 = dt1 * ( a11 * p0[1] + b1 );
                if( h0 >= 0 )
                {
                    get_min_edge02( a11, b1, p );
                }
                else
                {
                    h1 = dt1 * ( a01 * p1[0] + a11 * p1[1] + b1 );
                    if( h1 <= 0 )
                    {
                        get_min_edge12( a01, a11, b1, f10, f01, p );
                    }
                    else
                    {
                        get_min_interior( p0, h0, p1, h1, p );
                    }
                }
            }
        }
        else if( f01 <= 0 )
        {
            if( f10 <= 0 )
            {
                // (3) p0 = (1,0), p1 = (0,1), H(z) = G(L(z)) - F(L(z))
                get_min_edge12( a01, a11, b1, f10, f01, p );
            }
            else
            {
                // (4) p0 = (t00,0), p1 = (t01,1-t01), H(z) = t11*G(L(z))
                p0[0] = f00 / ( f00 - f10 );
                p0[1] = 0;
                p1[0] = f01 / ( f01 - f10 );
                p1[1] = 1 - p1[0];
                h0 = p1[1] * ( a01 * p0[0] + b1 );
                if( h0 >= 0 )
                {
                    p = p0; // GetMinEdge01
                }
                else
                {
                    h1 = p1[1] * ( a01 * p1[0] + a11 * p1[1] + b1 );
                    if( h1 <= 0 )
                    {
                        get_min_edge12( a01, a11, b1, f10, f01, p );
                    }
                    else
                    {
                        get_min_interior( p0, h0, p1, h1, p );
                    }
                }
            }
        }
        else if( f10 <= 0 )
        {
            // (5) p0 = (0,t10), p1 = (t01,1-t01),
            // H(z) = (t11 - t10)*G(L(z))
            p0[0] = 0;
            p0[1] = f00 / ( f00 - f01 );
            p1[0] = f01 / ( f01 - f10 );
            p1[1] = 1 - p1[0];
            dt1 = p1[1] - p0[1];
            h0 = dt1 * ( a11 * p0[1] + b1 );
            if( h0 >= 0 )
            {
                get_min_edge02( a11, b1, p );
            }
            else
            {
                h1 = dt1 * ( a01 * p1[0] + a11 * p1[1] + b1 );
                if( h1 <= 0 )
                {
                    get_min_edge12( a01, a11, b1, f10, f01, p );
                }
                else
                {
                    get_min_interior( p0, h0, p1, h1, p );
                }
            }
        }
        else
        {
            // (6) p0 = (t00,0), p1 = (0,t11), H(z) = t11*G(L(z))
            p0[0] = f00 / ( f00 - f10 );
            p0[1] = 0;
            p1[0] = 0;
            p1[1] = f00 / ( f00 - f01 );
            h0 = p1[1] * ( a01 * p0[0] + b1 );
            if( h0 >= 0 )
            {
                p = p0; // GetMinEdge01
            }
            else
            {
                h1 = p1[1] * ( a11 * p1[1] + b1 );
                if( h1 <= 0 )
                {
                    get_min_edge02( a11, b1, p );
                }
                else
                {
                    get_min_interior( p0, h0, p1, h1, p );
                }
            }
        }

        geode::Point3D closest_point{ vertices[v0].get() + edge0 * p[0]
                                      + edge1 * p[1] };
        const auto distance =
            geode::point_point_distance( point, closest_point );
        return std::make_tuple( distance, std::move( closest_point ) );
    }

    std::pair< std::vector< geode::local_index_t >,
        std::vector< geode::local_index_t > >
        find_non_colocated_triangles_points( const geode::Triangle3D& triangle0,
            const geode::Triangle3D& triangle1 )
    {
        std::pair< std::array< bool, 3 >, std::array< bool, 3 > > colocated{
            { false, false, false }, { false, false, false }
        };
        const auto& vertices0 = triangle0.vertices();
        const auto& vertices1 = triangle1.vertices();
        for( const auto vertex0 : geode::LRange{ 3 } )
        {
            for( const auto vertex1 : geode::LRange{ 3 } )
            {
                if( geode::point_point_distance(
                        vertices0[vertex0].get(), vertices1[vertex1].get() )
                    <= geode::GLOBAL_EPSILON )
                {
                    colocated.first[vertex0] = true;
                    colocated.second[vertex1] = true;
                }
            }
        }

        std::pair< std::vector< geode::local_index_t >,
            std::vector< geode::local_index_t > >
            result;
        for( const auto v : geode::LRange{ 3 } )
        {
            if( !colocated.first[v] )
            {
                result.first.push_back( v );
            }
            if( !colocated.second[v] )
            {
                result.second.push_back( v );
            }
        }
        return result;
    }

    std::tuple< double, geode::Point3D, geode::Point3D > test_close_triangles(
        const std::vector< geode::local_index_t >& non_colocated_points,
        const geode::Triangle3D& base_triangle,
        const geode::Triangle3D& other_triangle )
    {
        const auto& base_vertices = base_triangle.vertices();
        double min_distance{ std::numeric_limits< double >::max() };
        geode::Point3D point0;
        geode::Point3D point1;
        for( const auto vertex0 : non_colocated_points )
        {
            for( const auto vertex1 : non_colocated_points )
            {
                if( vertex0 == vertex1 )
                {
                    continue;
                }
                if( geode::point_point_distance( base_vertices[vertex0].get(),
                        base_vertices[vertex1].get() )
                    <= geode::GLOBAL_EPSILON )
                {
                    DEBUG( vertex0 );
                    DEBUG( vertex1 );
                    SDEBUG( base_vertices[0].get() );
                    SDEBUG( base_vertices[1].get() );
                    SDEBUG( base_vertices[2].get() );
                    SDEBUG( other_triangle.vertices()[0].get() );
                    SDEBUG( other_triangle.vertices()[1].get() );
                    SDEBUG( other_triangle.vertices()[2].get() );
                }
                const geode::Segment3D edge{ base_vertices[vertex0],
                    base_vertices[vertex1] };
                auto [cur_distance, cur_pt0, cur_pt1] =
                    geode::segment_triangle_distance( edge, other_triangle );
                if( cur_distance < min_distance )
                {
                    min_distance = cur_distance;
                    point0 = cur_pt0;
                    point1 = cur_pt1;
                }
            }
        }
        return std::make_tuple( min_distance, point0, point1 );
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    double point_point_distance(
        const Point< dimension >& point0, const Point< dimension >& point1 )
    {
        double diff2{ 0 };
        for( const auto d : LRange{ dimension } )
        {
            const auto diff = point0.value( d ) - point1.value( d );
            diff2 += diff * diff;
        }
        return std::sqrt( diff2 );
    }

    template < index_t dimension >
    double point_segment_distance(
        const Point< dimension >& point, const Segment< dimension >& segment )
    {
        const auto length = segment.length();
        const auto length0 =
            point_point_distance( segment.vertices()[0].get(), point );
        if( length <= GLOBAL_EPSILON )
        {
            return length0;
        }
        const auto length1 =
            point_point_distance( segment.vertices()[1].get(), point );
        const auto sqr_length = length * length;
        const auto sqr_length0 = length0 * length0;
        const auto sqr_length1 = length1 * length1;
        if( length0 >= length && length0 >= length1
            && sqr_length + sqr_length1 <= sqr_length0 )
        { // obtuse by vertex 1
            return length1;
        }
        if( length1 >= length && length1 >= length0
            && sqr_length + sqr_length0 <= sqr_length1 )
        { // obtuse by vertex 0
            return length0;
        }
        // acute angles
        if( const auto distance =
                compute_point_line_distance( length, length0, length1 ) )
        {
            return distance.value();
        }
        return std::get< 0 >(
            point_segment_distance_using_projection( point, segment ) );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension >, Point< dimension > >
        segment_segment_distance( const Segment< dimension >& segment0,
            const Segment< dimension >& segment1 )
    {
        /* Algorithm and code found on
         * https://github.com/davideberly/GeometricTools/blob/master/GTE/Mathematics/DistSegmentSegment.h
         */
        const auto P1mP0 = segment0.direction();
        const auto Q1mQ0 = segment1.direction();
        const Vector< dimension > P0mQ0{ segment1.vertices()[0],
            segment0.vertices()[0] };
        const auto a = P1mP0.dot( P1mP0 );
        const auto b = P1mP0.dot( Q1mQ0 );
        const auto c = Q1mQ0.dot( Q1mQ0 );
        const auto d = P1mP0.dot( P0mQ0 );
        const auto e = Q1mQ0.dot( P0mQ0 );
        const auto det = a * c - b * b;
        double s, t, nd, bmd, bte, ctd, bpe, ate, btd;

        if( det > 0 )
        {
            bte = b * e;
            ctd = c * d;
            if( bte <= ctd ) // s <= 0
            {
                s = 0;
                if( e <= 0 ) // t <= 0
                {
                    // region 6
                    t = 0;
                    nd = -d;
                    if( nd >= a )
                    {
                        s = 1;
                    }
                    else if( nd > 0 )
                    {
                        s = nd / a;
                    }
                    // else: s is already 0
                }
                else if( e < c ) // 0 < t < 1
                {
                    // region 5
                    t = e / c;
                }
                else // t >= 1
                {
                    // region 4
                    t = 1;
                    bmd = b - d;
                    if( bmd >= a )
                    {
                        s = 1;
                    }
                    else if( bmd > 0 )
                    {
                        s = bmd / a;
                    }
                    // else:  s is already 0
                }
            }
            else // s > 0
            {
                s = bte - ctd;
                if( s >= det ) // s >= 1
                {
                    // s = 1
                    s = 1;
                    bpe = b + e;
                    if( bpe <= 0 ) // t <= 0
                    {
                        // region 8
                        t = 0;
                        nd = -d;
                        if( nd <= 0 )
                        {
                            s = 0;
                        }
                        else if( nd < a )
                        {
                            s = nd / a;
                        }
                        // else: s is already 1
                    }
                    else if( bpe < c ) // 0 < t < 1
                    {
                        // region 1
                        t = bpe / c;
                    }
                    else // t >= 1
                    {
                        // region 2
                        t = 1;
                        bmd = b - d;
                        if( bmd <= 0 )
                        {
                            s = 0;
                        }
                        else if( bmd < a )
                        {
                            s = bmd / a;
                        }
                        // else:  s is already 1
                    }
                }
                else // 0 < s < 1
                {
                    ate = a * e;
                    btd = b * d;
                    if( ate <= btd ) // t <= 0
                    {
                        // region 7
                        t = 0;
                        nd = -d;
                        if( nd <= 0 )
                        {
                            s = 0;
                        }
                        else if( nd >= a )
                        {
                            s = 1;
                        }
                        else
                        {
                            s = nd / a;
                        }
                    }
                    else // t > 0
                    {
                        t = ate - btd;
                        if( t >= det ) // t >= 1
                        {
                            // region 3
                            t = 1;
                            bmd = b - d;
                            if( bmd <= 0 )
                            {
                                s = 0;
                            }
                            else if( bmd >= a )
                            {
                                s = 1;
                            }
                            else
                            {
                                s = bmd / a;
                            }
                        }
                        else // 0 < t < 1
                        {
                            // region 0
                            s /= det;
                            t /= det;
                        }
                    }
                }
            }
        }
        else
        {
            // The segments are parallel. The quadratic factors to
            //   R(s,t) = a*(s-(b/a)*t)^2 + 2*d*(s - (b/a)*t) + f
            // where a*c = b^2, e = b*d/a, f = |P0-Q0|^2, and b is not
            // 0. R is constant along lines of the form s-(b/a)*t = k
            // and its occurs on the line a*s - b*t + d = 0. This line
            // must intersect both the s-axis and the t-axis because 'a'
            // and 'b' are not 0. Because of parallelism, the line is
            // also represented by -b*s + c*t - e = 0.
            //
            // The code determines an edge of the domain [0,1]^2 that
            // intersects the minimum line, or if n1 of the edges
            // intersect, it determines the closest corner to the minimum
            // line. The conditionals are designed to test first for
            // intersection with the t-axis (s = 0) using
            // -b*s + c*t - e = 0 and then with the s-axis (t = 0) using
            // a*s - b*t + d = 0.

            // When s = 0, solve c*t - e = 0 (t = e/c).
            if( e <= 0 ) // t <= 0
            {
                // Now solve a*s - b*t + d = 0 for t = 0 (s = -d/a).
                t = 0;
                nd = -d;
                if( nd <= 0 ) // s <= 0
                {
                    // region 6
                    s = 0;
                }
                else if( nd >= a ) // s >= 1
                {
                    // region 8
                    s = 1;
                }
                else // 0 < s < 1
                {
                    // region 7
                    s = nd / a;
                }
            }
            else if( e >= c ) // t >= 1
            {
                // Now solve a*s - b*t + d = 0 for t = 1 (s = (b-d)/a).
                t = 1;
                bmd = b - d;
                if( bmd <= 0 ) // s <= 0
                {
                    // region 4
                    s = 0;
                }
                else if( bmd >= a ) // s >= 1
                {
                    // region 2
                    s = 1;
                }
                else // 0 < s < 1
                {
                    // region 3
                    s = bmd / a;
                }
            }
            else // 0 < t < 1
            {
                // The point (0,e/c) is on the line and domain, so we have
                // 1 point at which R is a minimum.
                s = 0;
                t = e / c;
            }
        }

        const auto closest_on_segment0 =
            segment0.vertices()[0].get() + P1mP0 * s;
        const auto closest_on_segment1 =
            segment1.vertices()[0].get() + Q1mQ0 * t;
        const auto distance =
            point_point_distance( closest_on_segment0, closest_on_segment1 );
        if( distance < GLOBAL_EPSILON )
        {
            return std::make_tuple(
                distance, closest_on_segment0, closest_on_segment1 );
        }
        const auto distance_to_closest0 =
            point_segment_distance( closest_on_segment0, segment1 );
        if( distance_to_closest0 < GLOBAL_EPSILON )
        {
            return std::make_tuple( distance_to_closest0, closest_on_segment0,
                point_segment_projection( closest_on_segment0, segment1 ) );
        }
        const auto distance_to_closest1 =
            point_segment_distance( closest_on_segment1, segment0 );
        if( distance_to_closest1 < GLOBAL_EPSILON )
        {
            return std::make_tuple( distance_to_closest1,
                point_segment_projection( closest_on_segment1, segment0 ),
                closest_on_segment1 );
        }
        if( distance_to_closest0 < distance )
        {
            if( distance_to_closest1 < distance_to_closest0 )
            {
                return std::make_tuple( distance_to_closest1,
                    point_segment_projection( closest_on_segment1, segment0 ),
                    closest_on_segment1 );
            }
            return std::make_tuple( distance_to_closest0, closest_on_segment0,
                point_segment_projection( closest_on_segment0, segment1 ) );
        }
        if( distance_to_closest1 < distance )
        {
            return std::make_tuple( distance_to_closest1,
                point_segment_projection( closest_on_segment1, segment0 ),
                closest_on_segment1 );
        }
        return std::make_tuple(
            distance, closest_on_segment0, closest_on_segment1 );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension >, Point< dimension > >
        segment_line_distance( const Segment< dimension >& segment,
            const InfiniteLine< dimension >& line )
    {
        const auto segDirection = segment.direction();
        const Vector< dimension > diff{ segment.vertices()[0], line.origin() };
        const auto a00 = line.direction().dot( line.direction() );
        const auto a01 = -line.direction().dot( segDirection );
        const auto a11 = segDirection.dot( segDirection );
        const auto b0 = line.direction().dot( diff );
        const auto det = std::max( a00 * a11 - a01 * a01, 0. );
        double s0, s1;

        if( det > 0 )
        {
            // The line and segment are not parallel.
            const auto b1 = -segDirection.dot( diff );
            s1 = a01 * b0 - a00 * b1;

            if( s1 >= 0 )
            {
                if( s1 <= det )
                {
                    // Two interior points are closest, one on the line
                    // and one on the segment.
                    s0 = ( a01 * b1 - a11 * b0 ) / det;
                    s1 /= det;
                }
                else
                {
                    // The endpoint Q1 of the segment and an interior
                    // point of the line are closest.
                    s0 = -( a01 + b0 ) / a00;
                    s1 = 1;
                }
            }
            else
            {
                // The endpoint Q0 of the segment and an interior point
                // of the line are closest.
                s0 = -b0 / a00;
                s1 = 0;
            }
        }
        else
        {
            // The line and segment are parallel. Select the pair of
            // closest points where the closest segment point is the
            // endpoint Q0.
            s0 = -b0 / a00;
            s1 = 0;
        }

        auto closest_on_line = line.origin() + line.direction() * s0;
        auto closest_on_segment =
            segment.vertices()[0].get() + segDirection * s1;
        return std::make_tuple(
            point_point_distance( closest_on_line, closest_on_segment ),
            std::move( closest_on_segment ), std::move( closest_on_line ) );
    }

    template < index_t dimension >
    double point_line_distance(
        const Point< dimension >& point, const InfiniteLine< dimension >& line )
    {
        const auto length0 = point_point_distance( line.origin(), point );
        const auto length1 =
            point_point_distance( line.origin() + line.direction(), point );
        if( const auto distance =
                compute_point_line_distance( 1, length0, length1 ) )
        {
            return distance.value();
        }
        return std::get< 0 >(
            point_line_distance_using_projection( point, line ) );
    }

    double point_line_signed_distance(
        const Point2D& point, const InfiniteLine2D& line )
    {
        const auto distance = point_line_distance< 2 >( point, line );
        const Vector2D origin2point{ line.origin(), point };
        return dot_perpendicular( origin2point, line.direction() ) <= 0
                   ? distance
                   : -distance;
    }

    template <>
    std::tuple< double, Point3D >
        opengeode_geometry_api point_triangle_distance(
            const Point3D& point, const Triangle3D& triangle )
    {
        if( const auto pivot = triangle.pivot() )
        {
            return pivot_point_triangle_distance(
                point, triangle, pivot.value() );
        }
        return no_pivot_point_triangle_distance( point, triangle );
    }

    template <>
    std::tuple< double, Point2D >
        opengeode_geometry_api point_triangle_distance(
            const Point2D& point, const Triangle2D& triangle )
    {
        if( may_point_be_in_triangle( point, triangle ) )
        {
            return std::make_tuple( 0.0, point );
        }
        const auto& vertices = triangle.vertices();
        std::array< Point2D, 3 > closest;
        std::array< double, 3 > distance;
        std::tie( distance[0], closest[0] ) =
            point_segment_distance_using_projection(
                point, Segment2D{ vertices[0], vertices[1] } );
        std::tie( distance[1], closest[1] ) =
            point_segment_distance_using_projection(
                point, Segment2D{ vertices[1], vertices[2] } );
        std::tie( distance[2], closest[2] ) =
            point_segment_distance_using_projection(
                point, Segment2D{ vertices[2], vertices[0] } );
        double result;
        Point2D closest_point;
        if( distance[0] < distance[1] )
        {
            if( distance[0] < distance[2] )
            {
                result = distance[0];
                closest_point = closest[0];
            }
            else
            {
                result = distance[2];
                closest_point = closest[2];
            }
        }
        else
        {
            if( distance[1] < distance[2] )
            {
                result = distance[1];
                closest_point = closest[1];
            }
            else
            {
                result = distance[2];
                closest_point = closest[2];
            }
        }
        return std::make_tuple( result, closest_point );
    }

    std::tuple< double, Point3D, Point3D > line_triangle_distance(
        const InfiniteLine3D& line, const Triangle3D& triangle )
    {
        // The line points are X = P + t * D and the triangle points
        // are Y = b[0] * V[0] + b[1] * V[1] + b[2] * V[2], where the
        // barycentric coordinates satisfy b[i] in [0,1] and
        // b[0] + b[1] + b[2 = 1. Define the triangle edge directions by
        // E[1] = V[1] - V[0] and E[2] = V[2] - V[0]; then
        // Y = V[0] + b1 * E[1] + b2 * E[2]. If Y is specified the
        // barycentric coordinates are the solution to
        //
        // +-                        -+ +-    -+   +-                 -+
        // | Dot(E1, E1)  Dot(E1, E2) | | b[1] | = | Dot(E1, Y - V[0]) |
        // | Dot(E1, E2)  Dot(E2, E2) | | b[2] |   | Dot(E2, Y - V[0]) |
        // +-                        -+ +-    -+   +-                 -+
        //
        // and b[0] = 1 - b[1] - b[2].

        // Test whether the line intersects triangle. If so, the squared
        // distance is zero. The normal of the plane of the triangle does
        // not have to be normalized to unit length.
        const auto& vertices = triangle.vertices();
        const Vector3D E1{ vertices[0], vertices[1] };
        const Vector3D E2{ vertices[0], vertices[2] };
        const auto N = E1.cross( E2 );
        const auto NdD = N.dot( line.direction() );
        if( std::fabs( NdD ) > 0 )
        {
            // The line and triangle are not parallel, so the line
            // intersects the plane of the triangle at a point Y.
            // Determine whether Y is contained by the triangle.
            const Vector3D PmV0{ vertices[0], line.origin() };
            const auto NdDiff = N.dot( PmV0 );
            const auto tIntersect = -NdDiff / NdD;
            const auto Y = line.origin() + line.direction() * tIntersect;
            const Vector3D YmV0{ vertices[0], Y };

            // Compute the barycentric coordinates of the intersection.
            const auto E1dE1 = E1.dot( E1 );
            const auto E1dE2 = E1.dot( E2 );
            const auto E2dE2 = E2.dot( E2 );
            const auto E1dYmV0 = E1.dot( YmV0 );
            const auto E2dYmV0 = E2.dot( YmV0 );
            const auto det = E1dE1 * E2dE2 - E1dE2 * E1dE2;
            const auto b1 = ( E2dE2 * E1dYmV0 - E1dE2 * E2dYmV0 ) / det;
            const auto b2 = ( E1dE1 * E2dYmV0 - E1dE2 * E1dYmV0 ) / det;
            const auto b0 = 1 - b1 - b2;

            if( b0 >= 0 && b1 >= 0 && b2 >= 0 )
            {
                // The point Y is contained by the triangle.
                return std::make_tuple( 0, Y, Y );
            }
        }

        // Either (1) the line is not parallel to the triangle and the
        // point of intersection of the line and the plane of the triangle
        // is outside the triangle or (2) the line and triangle are
        // parallel. Regardless, the closest point on the triangle is on
        // an edge of the triangle. Compare the line to all three edges
        // of the triangle. To allow for arbitrary precision arithmetic,
        // the initial distance and sqrDistance are initialized to a
        // negative number rather than a floating-point maximum value.
        // Tracking the minimum requires a small amount of extra logic.

        double smallest_distance = std::numeric_limits< double >::max();
        Point3D closest_on_edge;
        Point3D closest_on_line;
        for( const auto v : LRange{ 3 } )
        {
            const auto next = v == 2 ? 0 : v + 1;
            const Segment3D edge{ vertices[v], vertices[next] };
            const auto result = segment_line_distance( edge, line );
            const auto cur_distance = std::get< 0 >( result );

            if( cur_distance < smallest_distance )
            {
                smallest_distance = cur_distance;
                closest_on_edge = std::get< 1 >( result );
                closest_on_line = std::get< 2 >( result );
            }
        }

        return std::make_tuple(
            smallest_distance, closest_on_line, closest_on_edge );
    }

    std::tuple< double, Point3D, Point3D > segment_triangle_distance(
        const Segment3D& segment, const Triangle3D& triangle )
    {
        const InfiniteLine3D line{ segment };
        const auto line_triangle_result =
            line_triangle_distance( line, triangle );
        const auto& closest_on_line = std::get< 1 >( line_triangle_result );
        const auto& closest_on_triangle = std::get< 2 >( line_triangle_result );
        const auto closest_on_segment =
            point_segment_projection( closest_on_line, segment );
        return std::make_tuple(
            point_point_distance( closest_on_segment, closest_on_triangle ),
            closest_on_segment, closest_on_triangle );
    }

    std::tuple< double, Point3D, Point3D >
        opengeode_geometry_api triangle_triangle_distance(
            const Triangle3D& triangle0, const Triangle3D& triangle1 )
    {
        const auto non_colocated_points =
            find_non_colocated_triangles_points( triangle0, triangle1 );
        if( non_colocated_points.first.size() < 3 )
        {
            for( const auto v : LRange{ 3 } )
            {
                if( !absl::c_contains( non_colocated_points.first, v ) )
                {
                    auto& pt0 = triangle0.vertices()[v].get();
                    auto [distance, pt1] =
                        point_triangle_distance( pt0, triangle1 );
                    return std::tuple{ distance, pt0, pt1 };
                }
            }
        }
        if( non_colocated_points.second.size() < 3 )
        {
            for( const auto v : LRange{ 3 } )
            {
                if( !absl::c_contains( non_colocated_points.second, v ) )
                {
                    auto& pt1 = triangle1.vertices()[v].get();
                    auto [distance, pt0] =
                        point_triangle_distance( pt1, triangle0 );
                    return std::tuple{ distance, pt0, pt1 };
                }
            }
        }
        auto [cur_distance0, cur_pt00, cur_pt01] = test_close_triangles(
            non_colocated_points.first, triangle0, triangle1 );
        auto [cur_distance1, cur_pt11, cur_pt10] = test_close_triangles(
            non_colocated_points.second, triangle1, triangle0 );
        if( cur_distance0 < cur_distance1 )
        {
            return std::tuple{ cur_distance0, cur_pt00, cur_pt01 };
        }
        return std::tuple{ cur_distance1, cur_pt10, cur_pt11 };
    }

    std::optional< std::tuple< double, Point3D, Point3D > >
        triangle_triangle_distance_between_non_conformal_parts(
            const Triangle3D& triangle0, const Triangle3D& triangle1 )
    {
        const auto [non_colocated_points0, non_colocated_points1] =
            find_non_colocated_triangles_points( triangle0, triangle1 );
        if( non_colocated_points0.size() == 0
            || non_colocated_points1.size() == 0 )
        {
            return std::nullopt;
        }
        const auto& vertices0 = triangle0.vertices();
        const auto& vertices1 = triangle1.vertices();
        double min_distance{ std::numeric_limits< double >::max() };
        Point3D point0;
        Point3D point1;
        if( non_colocated_points0.size() == 1 )
        {
            auto [cur_distance, cur_pt] = point_triangle_distance(
                vertices0[non_colocated_points0[0]].get(), triangle1 );
            if( cur_distance < min_distance )
            {
                min_distance = cur_distance;
                point0 = vertices0[non_colocated_points0[0]].get();
                point1 = cur_pt;
            }
        }
        if( non_colocated_points1.size() == 1 )
        {
            auto [cur_distance, cur_pt] = point_triangle_distance(
                vertices1[non_colocated_points1[0]].get(), triangle0 );
            if( cur_distance < min_distance )
            {
                min_distance = cur_distance;
                point0 = cur_pt;
                point1 = vertices1[non_colocated_points1[0]].get();
            }
        }
        if( non_colocated_points0.size() > 1 )
        {
            auto [cur_distance, cur_pt0, cur_pt1] = test_close_triangles(
                non_colocated_points0, triangle0, triangle1 );
            if( cur_distance < min_distance )
            {
                min_distance = cur_distance;
                point0 = cur_pt0;
                point1 = cur_pt1;
            }
        }
        if( non_colocated_points1.size() > 1 )
        {
            auto [cur_distance, cur_pt1, cur_pt0] = test_close_triangles(
                non_colocated_points1, triangle1, triangle0 );
            if( cur_distance < min_distance )
            {
                min_distance = cur_distance;
                point0 = cur_pt0;
                point1 = cur_pt1;
            }
        }
        return std::tuple{ min_distance, point0, point1 };
    }

    std::tuple< double, Point3D > point_tetrahedron_distance(
        const Point3D& point, const Tetrahedron& tetra )
    {
        const auto lambdas =
            safe_tetrahedron_barycentric_coordinates( point, tetra );
        const auto facet = static_cast< local_index_t >(
            std::distance( lambdas.begin(), absl::c_min_element( lambdas ) ) );
        if( lambdas[facet] >= 0 )
        {
            return std::make_tuple( 0.0, point );
        }
        const auto& facet_vertices =
            Tetrahedron::tetrahedron_facet_vertex[facet];
        const auto& vertices = tetra.vertices();
        const auto output = point_triangle_distance( point,
            Triangle3D{ vertices[facet_vertices[0]],
                vertices[facet_vertices[1]], vertices[facet_vertices[2]] } );
        return output;
    }

    std::tuple< double, Point3D > point_triangle_signed_distance(
        const Point3D& point, const Triangle3D& triangle )
    {
        if( const auto pivot_and_normal = triangle.pivot_and_normal() )
        {
            const auto output = pivot_point_triangle_distance(
                point, triangle, pivot_and_normal->first );
            const auto& nearest_point = std::get< 1 >( output );
            const Vector3D proj2point{ nearest_point, point };
            if( proj2point.dot( pivot_and_normal->second ) >= 0 )
            {
                return output;
            }
            return std::make_tuple( -std::get< 0 >( output ), nearest_point );
        }
        return no_pivot_point_triangle_distance( point, triangle );
    }

    std::tuple< double, Point3D > point_plane_signed_distance(
        const Point3D& point, const Plane& plane )
    {
        const Vector3D v{ plane.origin(), point };
        const auto distance = v.dot( plane.normal() );
        const Point3D projected_p{ point - plane.normal() * distance };
        return std::make_tuple( distance, projected_p );
    }

    std::tuple< double, Point3D > point_plane_distance(
        const Point3D& point, const Plane& plane )
    {
        double distance;
        Point3D projected_p;
        std::tie( distance, projected_p ) =
            point_plane_signed_distance( point, plane );
        return std::make_tuple( std::fabs( distance ), projected_p );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_sphere_distance(
        const Point< dimension >& point, const Sphere< dimension >& sphere )
    {
        const Vector< dimension > center_to_point{ sphere.origin(), point };
        if( center_to_point.length() < GLOBAL_EPSILON )
        {
            Vector< dimension > dummy_direction;
            dummy_direction.set_value( 0, 1 );
            return std::make_tuple( sphere.radius(),
                sphere.origin() + dummy_direction * sphere.radius() );
        }
        return std::make_tuple(
            std::fabs( center_to_point.length() - sphere.radius() ),
            sphere.origin() + center_to_point.normalize() * sphere.radius() );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_sphere_signed_distance(
        const Point< dimension >& point, const Sphere< dimension >& sphere )
    {
        const Vector< dimension > center_to_point{ sphere.origin(), point };
        if( center_to_point.length() < GLOBAL_EPSILON )
        {
            Vector< dimension > dummy_direction;
            dummy_direction.set_value( 0, 1 );
            return std::make_tuple( -sphere.radius(),
                sphere.origin() + dummy_direction * sphere.radius() );
        }
        return std::make_tuple( center_to_point.length() - sphere.radius(),
            sphere.origin() + center_to_point.normalize() * sphere.radius() );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_ball_distance(
        const Point< dimension >& point, const Ball< dimension >& ball )
    {
        const auto signed_distance =
            point_sphere_signed_distance( point, ball );
        if( std::get< 0 >( signed_distance ) > 0 )
        {
            return signed_distance;
        }
        return std::make_tuple( 0, point );
    }

    std::tuple< double, Point3D > point_circle_distance(
        const Point3D& point, const Circle& circle )
    {
        const Vector3D center_to_point{ circle.plane().origin(), point };
        const auto distance_to_plane =
            center_to_point.dot( circle.plane().normal() );
        const auto projected_on_plane =
            point - circle.plane().normal() * distance_to_plane;
        const Vector3D center_to_projected_point{ circle.plane().origin(),
            projected_on_plane };
        if( center_to_projected_point.length() < GLOBAL_EPSILON )
        {
            Vector3D other_direction{ { 1.0, 0.0, 0.0 } };
            if( circle.plane().normal().inexact_equal( other_direction )
                || circle.plane().normal().inexact_equal(
                    other_direction * -1. ) ) // dummy_direction is aligned
                                              // along plane normal
            {
                other_direction.set_value( 1, 1.0 );
            }
            OPENGEODE_ASSERT(
                !circle.plane().normal().inexact_equal( other_direction ),
                "[point_circle_distance] Problem while getting circle "
                "nearest "
                "point" );
            const Vector3D other_projected_on_plane =
                other_direction
                - circle.plane().normal()
                      * other_direction.dot( circle.plane().normal() );
            return std::make_tuple(
                std::sqrt( circle.radius() * circle.radius()
                           + distance_to_plane * distance_to_plane ),
                circle.plane().origin()
                    + other_projected_on_plane.normalize() * circle.radius() );
        }
        const auto nearest_point =
            circle.plane().origin()
            + center_to_projected_point.normalize() * circle.radius();
        return std::make_tuple(
            point_point_distance( point, nearest_point ), nearest_point );
    }

    std::tuple< double, Point3D > point_circle_signed_distance(
        const Point3D& point, const Circle& circle )
    {
        double distance;
        Point3D nearest_point;
        std::tie( distance, nearest_point ) =
            point_circle_distance( point, circle );
        if( circle.plane().normal().dot( Vector3D{ point } ) < 0 )
        {
            distance = -distance;
        }
        return std::make_tuple( distance, nearest_point );
    }

    std::tuple< double, Point3D > point_disk_distance(
        const Point3D& point, const Disk& disk )
    {
        const Vector3D center_to_point{ disk.plane().origin(), point };
        const auto distance_to_plane =
            center_to_point.dot( disk.plane().normal() );
        const auto projected_on_plane =
            point - disk.plane().normal() * distance_to_plane;
        if( point_point_distance( disk.plane().origin(), projected_on_plane )
            <= disk.radius() )
        {
            return std::make_tuple(
                std::fabs( distance_to_plane ), projected_on_plane );
        }
        return point_circle_distance( point, disk );
    }

    template < index_t dimension >
    double Bisector( const geode::index_t number_of_components,
        const std::array< double, dimension >& locE,
        const std::array< double, dimension >& locY,
        std::array< double, dimension >& locX )
    {
        double sum_z_squared{ 0 };
        std::array< double, dimension > zPos;
        for( const auto i : geode::LRange{ number_of_components } )
        {
            zPos[i] = locY[i] / locE[i];
            sum_z_squared += zPos[i] * zPos[i];
        }
        if( std::fabs( sum_z_squared - 1 ) < geode::GLOBAL_EPSILON )
        {
            for( const auto i : geode::LRange{ number_of_components } )
            {
                locX[i] = locY[i];
            }
            return 0;
        }
        const auto emin = locE[number_of_components - 1];
        std::array< double, dimension > numerator;
        std::array< double, dimension > pSqr;
        pSqr.fill( 0 );
        numerator.fill( 0 );
        for( const auto i : geode::LRange{ number_of_components } )
        {
            const auto p = locE[i] / emin;
            pSqr[i] = p * p;
            numerator[i] = p * zPos[i];
        }
        double s{ 0 };
        auto smin = zPos[number_of_components - 1] - 1;
        double smax{ 0 };
        if( sum_z_squared >= 1 )
        {
            geode::Vector< dimension > v{ numerator };
            smax = v.length() - 1;
        }
        const geode::index_t jmax = 2048;
        geode::index_t j{ 0 };
        while( j < jmax )
        {
            s = ( smin + smax ) * 0.5;
            if( s == smin || s == smax )
            {
                break;
            }
            double g = -1;
            for( const auto i : geode::LRange{ number_of_components } )
            {
                const auto ratio = numerator[i] / ( s + pSqr[i] );
                g += ratio * ratio;
            }
            if( g > 0 )
            {
                smin = s;
                j++;
            }
            else if( g < 0 )
            {
                smax = s;
                j++;
            }
            else
            {
                break;
            }
        }
        auto squared_distance = 0.;
        for( const auto i : geode::LRange{ number_of_components } )
        {
            locX[i] = pSqr[i] * locY[i] / ( s + pSqr[i] );
            const auto diff = locX[i] - locY[i];
            squared_distance += diff * diff;
        }
        return squared_distance;
    }

    template < index_t dimension >
    std::tuple< double, std::array< double, dimension > > SqrDistanceSpecial(
        const std::array< double, dimension >& extents,
        const std::array< double, dimension >& query_point_coordinates )
    {
        std::tuple< double, std::array< double, dimension > > result;
        auto& [squared_distance, closest_point_coordinates] = result;
        std::array< double, dimension > ePos;
        std::array< double, dimension > yPos;
        std::array< double, dimension > xPos;
        index_t numpos{ 0 };
        for( const auto i : geode::LRange{ dimension } )
        {
            if( query_point_coordinates[i] > 0 )
            {
                ePos.at( numpos ) = extents[i];
                yPos.at( numpos ) = query_point_coordinates[i];
                ++numpos;
                continue;
            }
            closest_point_coordinates.at( numpos ) = 0.;
        }
        if( query_point_coordinates[dimension - 1] > 0 )
        {
            squared_distance =
                Bisector< dimension >( numpos, ePos, yPos, xPos );
        }
        else
        {
            std::array< double, dimension - 1 > numer;
            std::array< double, dimension - 1 > denom;
            const auto extent_squared =
                extents[dimension - 1] * extents[dimension - 1];
            for( const auto i : geode::LRange{ numpos } )
            {
                numer[i] = ePos[i] * yPos[i];
                denom[i] = ePos[i] * ePos[i] - extent_squared;
            }
            bool inSubbox{ true };
            for( const auto i : geode::LRange{ numpos } )
            {
                if( numer[i] >= denom[i] )
                {
                    inSubbox = false;
                    break;
                }
            }
            bool inSubellipsoid{ false };
            double discr{ 1 };
            if( inSubbox )
            {
                std::array< double, dimension - 1 > xde;
                for( const auto i : geode::LRange{ numpos } )
                {
                    xde[i] = numer[i] / denom[i];
                    discr -= xde[i] * xde[i];
                }
                if( discr > 0 )
                {
                    squared_distance = 0;
                    for( const auto i : geode::LRange{ numpos } )
                    {
                        xPos[i] = ePos[i] * xde[i];
                        const auto diff = xPos[i] - yPos[i];
                        squared_distance += diff * diff;
                    }
                    closest_point_coordinates[dimension - 1] =
                        extents[dimension - 1] * std::sqrt( discr );
                    squared_distance +=
                        closest_point_coordinates[dimension - 1]
                        * closest_point_coordinates[dimension - 1];
                    inSubellipsoid = true;
                }
            }
            if( !inSubellipsoid )
            {
                closest_point_coordinates[dimension - 1] = 0;
                squared_distance =
                    Bisector< dimension >( numpos, ePos, yPos, xPos );
            }
        }
        numpos = 0;
        for( const auto i : geode::LRange{ dimension } )
        {
            if( query_point_coordinates[i] > 0 )
            {
                closest_point_coordinates[i] = xPos[numpos];
                ++numpos;
            }
        }
        return result;
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > > SquaredDistance(
        const Ellipse< dimension >& ellipse,
        const std::array< double, dimension >& query_point_coordinates )
    {
        std::array< bool, dimension > is_query_points_coordinates_negative;
        std::tuple< double, Point< dimension > > result;
        auto& [squared_distance, closest_point] = result;
        for( const auto i : geode::LRange{ dimension } )
        {
            is_query_points_coordinates_negative[i] =
                ( query_point_coordinates[i] < 0. );
        }
        std::array< std::pair< double, index_t >, dimension >
            axis_sorted_by_decreasing_extent;
        for( const auto i : geode::LRange{ dimension } )
        {
            auto& [extent, axis] = axis_sorted_by_decreasing_extent.at( i );
            extent = ellipse.axes().direction( i ).length();
            axis = i;
        };
        absl::c_sort( axis_sorted_by_decreasing_extent,
            []( const std::pair< double, index_t >& a,
                const std::pair< double, index_t >& b ) {
                return a.second < b.second;
            } );
        std::array< index_t, dimension > reverse_permutation;
        for( const auto i : geode::LRange{ dimension } )
        {
            reverse_permutation.at(
                axis_sorted_by_decreasing_extent[i].second ) = i;
        }
        std::array< double, dimension > extents;
        std::array< double, dimension > locY;
        for( const auto i : geode::LRange{ dimension } )
        {
            const auto j = axis_sorted_by_decreasing_extent[i].second;
            extents[i] = ellipse.axes().direction( j ).length();
            locY[i] = std::fabs( query_point_coordinates[j] );
        }
        auto [squared_distance_special, closest_point_coordinates] =
            SqrDistanceSpecial< dimension >( extents, locY );
        squared_distance = squared_distance_special;
        for( const auto i : geode::LRange{ dimension } )
        {
            const auto j = reverse_permutation[i];
            if( is_query_points_coordinates_negative[i] )
            {
                closest_point_coordinates[j] = -closest_point_coordinates[j];
            }
        }
        for( const auto i : geode::LRange{ dimension } )
        {
            closest_point.set_value( i, closest_point_coordinates[i] );
        }
        return result;
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_ellipse_distance(
        const Point< dimension >& point, const Ellipse< dimension >& ellipse )
    {
        std::tuple< double, Point< dimension > > result;
        std::array< double, dimension > point_coordinates;
        const Vector< dimension > center_to_point{ ellipse.center(), point };
        for( const auto i : geode::LRange{ dimension } )
        {
            point_coordinates[i] = center_to_point.dot(
                ellipse.axes().direction( i ).normalize() );
        }
        auto& [distance, closest_point] = result;
        const auto [squared_distance, closest_point_result] =
            SquaredDistance< dimension >( ellipse, point_coordinates );
        distance = std::sqrt( squared_distance );
        closest_point = ellipse.center();
        for( const auto i : geode::LRange{ dimension } )
        {
            closest_point += ellipse.axes().direction( i ).normalize()
                             * closest_point_result.value( i );
        }
        return result;
    }

    template double opengeode_geometry_api point_point_distance(
        const Point1D&, const Point1D& );
    template double opengeode_geometry_api point_segment_distance(
        const Point1D&, const Segment1D& );
    template std::tuple< double, Point1D, Point1D > opengeode_geometry_api
        segment_segment_distance( const Segment1D&, const Segment1D& );

    template double opengeode_geometry_api point_point_distance(
        const Point2D&, const Point2D& );
    template double opengeode_geometry_api point_segment_distance(
        const Point2D&, const Segment2D& );
    template std::tuple< double, Point2D, Point2D > opengeode_geometry_api
        segment_segment_distance( const Segment2D&, const Segment2D& );
    template std::tuple< double, Point2D, Point2D > opengeode_geometry_api
        segment_line_distance( const Segment2D&, const InfiniteLine2D& );
    template double opengeode_geometry_api point_line_distance(
        const Point2D&, const InfiniteLine2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_sphere_distance( const Point2D&, const Sphere2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_sphere_signed_distance( const Point2D&, const Sphere2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_ball_distance( const Point2D&, const Ball2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_ellipse_distance( const Point2D&, const Ellipse2D& ellipse );

    template double opengeode_geometry_api point_point_distance(
        const Point3D&, const Point3D& );
    template double opengeode_geometry_api point_segment_distance(
        const Point3D&, const Segment3D& );
    template std::tuple< double, Point3D, Point3D > opengeode_geometry_api
        segment_segment_distance( const Segment3D&, const Segment3D& );
    template std::tuple< double, Point3D, Point3D > opengeode_geometry_api
        segment_line_distance( const Segment3D&, const InfiniteLine3D& );
    template double opengeode_geometry_api point_line_distance(
        const Point3D&, const InfiniteLine3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_sphere_distance( const Point3D&, const Sphere3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_sphere_signed_distance( const Point3D&, const Sphere3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_ball_distance( const Point3D&, const Ball3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_ellipse_distance( const Point3D&, const Ellipse3D& ellipse );
} // namespace geode
