/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geometry/distance.h>

#include <geode/geometry/basic_objects/circle.h>
#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/sphere.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/mensuration.h>
#include <geode/geometry/perpendicular.h>
#include <geode/geometry/projection.h>
#include <geode/geometry/vector.h>

namespace
{
    constexpr double MAX_DOUBLE = std::numeric_limits< double >::max();

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

    absl::optional< double > compute_point_line_distance( double segment_length,
        double point_to_v0_length,
        double point_to_v1_length )
    {
        const auto p =
            ( point_to_v0_length + point_to_v1_length + segment_length ) / 2;
        {
            return absl::nullopt;
        }
        if( p - point_to_v1_length <= geode::global_epsilon )
        {
            return absl::nullopt;
        }
        if( p - segment_length <= geode::global_epsilon )
        {
            return absl::nullopt;
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
        if( length <= global_epsilon )
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
        if( distance < global_epsilon )
        {
            return std::make_tuple(
                distance, closest_on_segment0, closest_on_segment1 );
        }
        const auto distance_to_closest0 =
            point_segment_distance( closest_on_segment0, segment1 );
        if( distance_to_closest0 < global_epsilon )
        {
            return std::make_tuple( distance_to_closest0, closest_on_segment0,
                point_segment_projection( closest_on_segment0, segment1 ) );
        }
        const auto distance_to_closest1 =
            point_segment_distance( closest_on_segment1, segment0 );
        if( distance_to_closest1 < global_epsilon )
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

    std::tuple< double, Point3D > point_triangle_distance(
        const Point3D& point, const Triangle3D& triangle, local_index_t v0 )
    {
        const auto v1 = v0 == 2 ? 0 : v0 + 1;
        const auto v2 = v1 == 2 ? 0 : v1 + 1;
        const auto& vertices = triangle.vertices();
        const Vector3D edge0{ vertices[v0], vertices[v1] };
        const Vector3D edge1{ vertices[v0], vertices[v2] };
        const auto a00 = edge0.length2();
        const auto a01 = edge0.dot( edge1 );
        const auto a11 = edge1.length2();
        const Vector3D diff{ vertices[v0], point };
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

        Point3D closest_point{ vertices[v0].get() + edge0 * p[0]
                               + edge1 * p[1] };
        const auto distance = point_point_distance( point, closest_point );
        return std::make_tuple( distance, std::move( closest_point ) );
    }

    template <>
    std::tuple< double, Point3D > point_triangle_distance(
        const Point3D& point, const Triangle3D& triangle )
    {
        if( const auto pivot = triangle.pivot() )
        {
            return point_triangle_distance( point, triangle, pivot.value() );
        }
        std::array< std::tuple< double, Point3D >, 3 > edge_distances;
        auto min_distance = std::numeric_limits< double >::max();
        local_index_t selected_edge{ NO_LID };
        const auto& vertices = triangle.vertices();
        for( const auto e : LRange{ 3 } )
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

    template <>
    std::tuple< double, Point2D > point_triangle_distance(
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

    std::tuple< double, Point3D > point_tetrahedron_distance(
        const Point3D& point, const Tetrahedron& tetra )
    {
        auto max_distance = MAX_DOUBLE;
        Point3D nearest_p;
        bool inside{ true };
        const auto& vertices = tetra.vertices();
        for( const auto f : Range{ 4 } )
        {
            const auto& facet_vertices =
                Tetrahedron::tetrahedron_facet_vertex[f];
            double distance;
            Point3D cur_p;
            std::tie( distance, cur_p ) = point_triangle_signed_distance(
                point, Triangle3D{ vertices[facet_vertices[0]],
                           vertices[facet_vertices[1]],
                           vertices[facet_vertices[2]] } );
            if( distance > 0 )
            {
                inside = false;
            }
            if( distance < max_distance && distance >= 0 )
            {
                max_distance = distance;
                nearest_p = cur_p;
            }
        }
        if( inside )
        {
            return std::make_tuple( 0.0, point );
        }
        return std::make_tuple( max_distance, nearest_p );
    }

    std::tuple< double, Point3D > point_triangle_signed_distance(
        const Point3D& point, const Triangle3D& triangle )
    {
        Point3D nearest_point;
        double distance;
        std::tie( distance, nearest_point ) =
            point_triangle_distance( point, triangle );
        const Vector3D proj2point{ nearest_point, point };
        // Tetra facet normals point towards inside
        if( const auto normal = triangle.normal() )
        {
            const auto signed_distance =
                proj2point.dot( normal.value() ) <= 0 ? distance : -distance;
            return std::make_tuple( signed_distance, nearest_point );
        }
        return std::make_tuple( distance, nearest_point );
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
        if( center_to_point.length() < global_epsilon )
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
        if( center_to_point.length() < global_epsilon )
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
        if( center_to_projected_point.length() < global_epsilon )
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
                "[point_circle_distance] Problem while getting circle nearest "
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
        if( circle.plane().normal().dot( point ) < 0 )
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

    template double opengeode_geometry_api point_point_distance(
        const Point1D&, const Point1D& );
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
        point_triangle_distance( const Point2D&, const Triangle2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_sphere_distance( const Point2D&, const Sphere2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_sphere_signed_distance( const Point2D&, const Sphere2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_ball_distance( const Point2D&, const Ball2D& );

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
        point_triangle_distance( const Point3D&, const Triangle3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_sphere_distance( const Point3D&, const Sphere3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_sphere_signed_distance( const Point3D&, const Sphere3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_ball_distance( const Point3D&, const Ball3D& );
} // namespace geode
