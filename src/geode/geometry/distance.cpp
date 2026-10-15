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

#include <optional>

#include <geode/basic/logger.hpp>
#include <geode/geometry/distance.hpp>

#include <absl/algorithm/container.h>

#include <geode/geometry/barycentric_coordinates.hpp>
#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/cylinder.hpp>
#include <geode/geometry/basic_objects/ellipse.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/internal/position_from_sides.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/perpendicular.hpp>
#include <geode/geometry/projection.hpp>
#include <geode/geometry/sign.hpp>
#include <geode/geometry/vector.hpp>

namespace
{
    /*
     * This function tests if the point is in the triangle using exact
     * orientation predicates
     */
    bool may_point_be_in_triangle(
        const geode::Point2D& point, const geode::Triangle2D& triangle )
    {
        const auto& vertices = triangle.vertices();
        const auto sign_1 = geode::triangle_area_sign(
            geode::Triangle2D{ vertices[0], vertices[1], point } );
        const auto sign_2 = geode::triangle_area_sign(
            geode::Triangle2D{ vertices[1], vertices[2], point } );
        const auto sign_3 = geode::triangle_area_sign(
            geode::Triangle2D{ vertices[2], vertices[0], point } );
        if( sign_1 == geode::SIGN::zero && sign_2 == geode::SIGN::zero
            && sign_3 == geode::SIGN::zero )
        {
            // Flat triangle and aligned point: decided by the edge distances
            return false;
        }
        return ( sign_1 != geode::SIGN::positive
                   && sign_2 != geode::SIGN::positive
                   && sign_3 != geode::SIGN::positive )
               || ( sign_1 != geode::SIGN::negative
                    && sign_2 != geode::SIGN::negative
                    && sign_3 != geode::SIGN::negative );
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
        return { point_point_distance( point, nearest_p ), nearest_p };
    }

    template < geode::index_t dimension >
    std::tuple< double, geode::Point< dimension > >
        point_line_distance_using_projection(
            const geode::Point< dimension >& point,
            const geode::InfiniteLine< dimension >& line )
    {
        const auto nearest_p = point_line_projection( point, line );
        return { point_point_distance( point, nearest_p ), nearest_p };
    }

    std::tuple< double, geode::Point3D > no_pivot_point_triangle_distance(
        const geode::Point3D& point, const geode::Triangle3D& triangle )
    {
        std::array< std::tuple< double, geode::Point3D >, 3 > edge_distances;
        auto min_distance = std::numeric_limits< double >::max();
        geode::local_index_t selected_edge{ 0 };
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

    // Pivot on the vertex opposite to the longest edge: this is cheaper than
    // Triangle::pivot() and its height over the longest edge tells if the
    // triangle is degenerated
    std::optional< geode::local_index_t > longest_edge_opposite_pivot(
        const geode::Triangle3D& triangle )
    {
        const auto& vertices = triangle.vertices();
        std::array< double, 3 > squared_lengths;
        for( const auto e : geode::LRange{ 3 } )
        {
            squared_lengths[e] =
                geode::Vector3D{ vertices[e], vertices[e == 2 ? 0 : e + 1] }
                    .length2();
        }
        geode::local_index_t longest_edge{ 0 };
        for( const auto e : geode::LRange{ 1, 3 } )
        {
            if( squared_lengths[e] > squared_lengths[longest_edge] )
            {
                longest_edge = e;
            }
        }
        const geode::local_index_t pivot =
            longest_edge == 0 ? 2 : longest_edge - 1;
        const auto next = pivot == 2 ? 0 : pivot + 1;
        const auto previous = next == 2 ? 0 : next + 1;
        const auto squared_double_area =
            geode::Vector3D{ vertices[pivot], vertices[next] }
                .cross( geode::Vector3D{ vertices[pivot], vertices[previous] } )
                .length2();
        // height over longest edge <= epsilon
        if( !( squared_double_area > geode::GLOBAL_EPSILON
                                         * geode::GLOBAL_EPSILON
                                         * squared_lengths[longest_edge] ) )
        {
            return std::nullopt;
        }
        return pivot;
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
        std::tuple< double, geode::Point3D > result;
        auto& [distance, closest_point] = result;
        closest_point =
            geode::Point3D{ vertices[v0].get() + edge0 * p[0] + edge1 * p[1] };
        distance = geode::point_point_distance( point, closest_point );
        return result;
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

    std::tuple< double, geode::Point3D, geode::Point3D >
        test_triangle_non_common_edges(
            const geode::local_index_t non_colocated0,
            const geode::local_index_t non_colocated1,
            const geode::Triangle3D& triangle0,
            const geode::Triangle3D& triangle1 )
    {
        const auto& vertices0 = triangle0.vertices();
        const auto& vertices1 = triangle1.vertices();
        double min_distance{ std::numeric_limits< double >::max() };
        geode::Point3D point0;
        geode::Point3D point1;
        for( const auto v0 : geode::LRange{ 3 } )
        {
            if( v0 == non_colocated0 )
            {
                continue;
            }
            const geode::Segment3D edge0{ vertices0[non_colocated0],
                vertices0[v0] };
            for( const auto v1 : geode::LRange{ 3 } )
            {
                if( v1 == non_colocated1 )
                {
                    continue;
                }
                const geode::Segment3D edge1{ vertices1[non_colocated1],
                    vertices1[v1] };
                auto [cur_distance, cur_pt0, cur_pt1] =
                    geode::segment_segment_distance( edge0, edge1 );
                if( cur_pt0.inexact_equal( vertices0[v0] )
                    || cur_pt1.inexact_equal( vertices1[v1] ) )
                {
                    continue;
                }
                if( cur_distance < min_distance )
                {
                    min_distance = cur_distance;
                    point0 = cur_pt0;
                    point1 = cur_pt1;
                }
            }
        }
        return { min_distance, point0, point1 };
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
        for( const auto i : geode::Indices{ non_colocated_points } )
        {
            const auto vertex0 = non_colocated_points[i];
            for( const auto j :
                geode::Range{ i + 1, non_colocated_points.size() } )
            {
                const auto vertex1 = non_colocated_points[j];
                if( geode::point_point_distance( base_vertices[vertex0].get(),
                        base_vertices[vertex1].get() )
                    <= geode::GLOBAL_EPSILON )
                {
                    // Colocated vertices: the edge is degenerated, its
                    // vertices are tested through the other edges
                    continue;
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
        return { min_distance, point0, point1 };
    }

    struct SegmentSegmentTerms
    {
        // det = a*c - b*b, s_numerator = b*e - c*d, t_numerator = a*e - b*d
        // computed with cross products to avoid cancellations for nearly
        // parallel segments (Binet-Cauchy identity)
        double det{ 0 };
        double s_numerator{ 0 };
        double t_numerator{ 0 };
    };

    SegmentSegmentTerms segment_segment_terms( const geode::Vector1D& /*u*/,
        const geode::Vector1D& /*v*/,
        const geode::Vector1D& /*w*/ )
    {
        return {};
    }

    SegmentSegmentTerms segment_segment_terms( const geode::Vector2D& u,
        const geode::Vector2D& v,
        const geode::Vector2D& w )
    {
        const auto uv = geode::dot_perpendicular( u, v );
        return { uv * uv, uv * geode::dot_perpendicular( v, w ),
            uv * geode::dot_perpendicular( u, w ) };
    }

    SegmentSegmentTerms segment_segment_terms( const geode::Vector3D& u,
        const geode::Vector3D& v,
        const geode::Vector3D& w )
    {
        const auto uv = u.cross( v );
        return { uv.length2(), uv.dot( v.cross( w ) ), uv.dot( u.cross( w ) ) };
    }

    /*
     * Parameters (s, t) of the closest points P0 + s*(P1-P0) and
     * Q0 + t*(Q1-Q0) of two non-degenerated segments.
     * Algorithm and code found on
     * https://github.com/davideberly/GeometricTools/blob/master/GTE/Mathematics/DistSegmentSegment.h
     */
    template < geode::index_t dimension >
    std::pair< double, double > segment_segment_parameters(
        const geode::Vector< dimension >& P1mP0,
        const geode::Vector< dimension >& Q1mQ0,
        const geode::Vector< dimension >& P0mQ0 )
    {
        const auto a = P1mP0.dot( P1mP0 );
        const auto b = P1mP0.dot( Q1mQ0 );
        const auto c = Q1mQ0.dot( Q1mQ0 );
        const auto d = P1mP0.dot( P0mQ0 );
        const auto e = Q1mQ0.dot( P0mQ0 );
        const auto terms = segment_segment_terms( P1mP0, Q1mQ0, P0mQ0 );
        double s, t, nd, bmd, bpe;
        // det = a*c*sin^2(angle): segments are parallel when each one deviates
        // from the direction of the other by less than GLOBAL_EPSILON, i.e.
        // max(a,c)*sin^2(angle) < GLOBAL_EPSILON^2
        if( terms.det
            > geode::GLOBAL_EPSILON * geode::GLOBAL_EPSILON * std::min( a, c ) )
        {
            const auto det = terms.det;
            if( terms.s_numerator <= 0 ) // s <= 0
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
                s = terms.s_numerator;
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
                    if( terms.t_numerator <= 0 ) // t <= 0
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
                        t = terms.t_numerator;
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

        return { s, t };
    }

    template < geode::index_t dimension >
    geode::Point< dimension > segment_point(
        const geode::Segment< dimension >& segment,
        const geode::Vector< dimension >& direction,
        double parameter )
    {
        if( parameter <= 0 )
        {
            return segment.vertices()[0].get();
        }
        if( parameter >= 1 )
        {
            return segment.vertices()[1].get();
        }
        return segment.vertices()[0].get() + direction * parameter;
    }

    double clamp_parameter( double numerator, double denominator )
    {
        if( numerator <= 0 )
        {
            return 0;
        }
        if( numerator >= denominator )
        {
            return 1;
        }
        return numerator / denominator;
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
        return point_point_distance(
            point, point_segment_projection( point, segment ) );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension >, Point< dimension > >
        segment_segment_distance( const Segment< dimension >& segment0,
            const Segment< dimension >& segment1 )
    {
        const auto& P0 = segment0.vertices()[0].get();
        const auto& Q0 = segment1.vertices()[0].get();
        const Vector< dimension > P1mP0{ P0, segment0.vertices()[1] };
        const Vector< dimension > Q1mQ0{ Q0, segment1.vertices()[1] };
        const auto a = P1mP0.length2();
        const auto c = Q1mQ0.length2();
        constexpr auto SQUARED_EPSILON = GLOBAL_EPSILON * GLOBAL_EPSILON;
        if( a <= SQUARED_EPSILON )
        {
            if( c <= SQUARED_EPSILON )
            {
                return { point_point_distance( P0, Q0 ), P0, Q0 };
            }
            const auto t = clamp_parameter(
                Vector< dimension >{ Q0, P0 }.dot( Q1mQ0 ), c );
            auto closest1 = segment_point( segment1, Q1mQ0, t );
            const auto distance = point_point_distance( P0, closest1 );
            return { distance, P0, std::move( closest1 ) };
        }
        if( c <= SQUARED_EPSILON )
        {
            const auto s = clamp_parameter(
                Vector< dimension >{ P0, Q0 }.dot( P1mP0 ), a );
            auto closest0 = segment_point( segment0, P1mP0, s );
            const auto distance = point_point_distance( closest0, Q0 );
            return { distance, std::move( closest0 ), Q0 };
        }
        auto [s, t] = segment_segment_parameters< dimension >(
            P1mP0, Q1mQ0, Vector< dimension >{ Q0, P0 } );
        // Polish: closest point on segment1 to P(s), then closest point on
        // segment0 to Q(t) (each step can only decrease the distance)
        t = clamp_parameter(
            Vector< dimension >{ Q0, segment_point( segment0, P1mP0, s ) }.dot(
                Q1mQ0 ),
            c );
        auto closest1 = segment_point( segment1, Q1mQ0, t );
        s = clamp_parameter(
            Vector< dimension >{ P0, closest1 }.dot( P1mP0 ), a );
        auto closest0 = segment_point( segment0, P1mP0, s );
        const auto distance = point_point_distance( closest0, closest1 );
        return { distance, std::move( closest0 ), std::move( closest1 ) };
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension >, Point< dimension > >
        segment_line_distance( const Segment< dimension >& segment,
            const InfiniteLine< dimension >& line )
    {
        const Vector< dimension > segment_direction{ segment.vertices()[0],
            segment.vertices()[1] };
        const auto& line_direction = line.direction();
        const auto terms =
            segment_segment_terms( line_direction, segment_direction,
                Vector< dimension >{ segment.vertices()[0], line.origin() } );
        // det = a*c*sin^2(angle): the segment is parallel to the line when it
        // deviates from the line direction by less than GLOBAL_EPSILON, i.e.
        // c*sin^2(angle) < GLOBAL_EPSILON^2. The squared distance to the line
        // is convex along the segment: clamp its minimum on the segment.
        const auto parameter =
            terms.det
                    > GLOBAL_EPSILON * GLOBAL_EPSILON * line_direction.length2()
                ? clamp_parameter( terms.t_numerator, terms.det )
                : 0.;
        auto closest_on_segment =
            segment_point( segment, segment_direction, parameter );
        auto closest_on_line =
            point_line_projection( closest_on_segment, line );
        const auto distance =
            point_point_distance( closest_on_line, closest_on_segment );
        return { distance, std::move( closest_on_segment ),
            std::move( closest_on_line ) };
    }

    template < index_t dimension >
    double point_line_distance(
        const Point< dimension >& point, const InfiniteLine< dimension >& line )
    {
        return point_point_distance(
            point, point_line_projection( point, line ) );
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
        const auto pivot = longest_edge_opposite_pivot( triangle );
        if( !pivot )
        {
            return no_pivot_point_triangle_distance( point, triangle );
        }
        return pivot_point_triangle_distance( point, triangle, pivot.value() );
    }

    template <>
    std::tuple< double, Point2D >
        opengeode_geometry_api point_triangle_distance(
            const Point2D& point, const Triangle2D& triangle )
    {
        if( !triangle.is_degenerated()
            && may_point_be_in_triangle( point, triangle ) )
        {
            return { 0.0, point };
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
        return { result, closest_point };
    }

    std::tuple< double, Point3D, Point3D > line_triangle_distance(
        const InfiniteLine3D& line, const Triangle3D& triangle )
    {
        // Test whether the line intersects triangle with exact predicates: the
        // line is on the same side of the three triangle edges. If so, the
        // distance is zero at the intersection with the triangle plane.
        const auto& vertices = triangle.vertices();
        const auto& origin = line.origin();
        const Point3D destination = origin + line.direction();
        local_index_t nb_positive{ 0 };
        local_index_t nb_negative{ 0 };
        for( const auto v : LRange{ 3 } )
        {
            const auto next = v == 2 ? 0 : v + 1;
            const auto sign = tetrahedron_volume_sign( Tetrahedron{
                origin, destination, vertices[v], vertices[next] } );
            nb_positive += sign == SIGN::positive ? 1 : 0;
            nb_negative += sign == SIGN::negative ? 1 : 0;
        }
        if( ( nb_positive == 0 ) != ( nb_negative == 0 ) )
        {
            const Vector3D E1{ vertices[0], vertices[1] };
            const Vector3D E2{ vertices[0], vertices[2] };
            const auto N = E1.cross( E2 );
            const auto NdD = N.dot( line.direction() );
            if( NdD != 0 )
            {
                const Vector3D PmV0{ vertices[0], origin };
                const auto Y =
                    origin + line.direction() * ( -N.dot( PmV0 ) / NdD );
                // Y is approximated: far from the triangle for nearly
                // parallel lines, then decided by the edge distances
                auto [distance, closest] =
                    point_triangle_distance( Y, triangle );
                if( distance <= GLOBAL_EPSILON )
                {
                    return { distance, Y, std::move( closest ) };
                }
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

        return { smallest_distance, closest_on_line, closest_on_edge };
    }

    std::tuple< double, Point3D, Point3D > segment_triangle_distance(
        const Segment3D& segment, const Triangle3D& triangle )
    {
        if( segment.is_degenerated() )
        {
            const auto point = segment.barycenter();
            const auto [distance, closest_on_triangle] =
                point_triangle_distance( point, triangle );
            return { distance, point, closest_on_triangle };
        }
        const InfiniteLine3D line{ segment };
        const auto [_, closest_on_line, closest_on_triangle] =
            line_triangle_distance( line, triangle );
        const auto closest_on_segment =
            point_segment_projection( closest_on_line, segment );
        const auto reprojection_on_triangle =
            point_triangle_projection( closest_on_segment, triangle );
        return { point_point_distance(
                     closest_on_segment, reprojection_on_triangle ),
            closest_on_segment, reprojection_on_triangle };
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
            if( min_distance <= GLOBAL_EPSILON )
            {
                return std::tuple{ min_distance, point0, point1 };
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
            if( min_distance <= GLOBAL_EPSILON )
            {
                return std::tuple{ min_distance, point0, point1 };
            }
        }
        if( non_colocated_points0.size() == 1
            && non_colocated_points1.size() == 1 )
        {
            auto [cur_distance, cur_pt0, cur_pt1] =
                test_triangle_non_common_edges( non_colocated_points0.front(),
                    non_colocated_points1.front(), triangle0, triangle1 );
            if( cur_distance < min_distance )
            {
                min_distance = cur_distance;
                point0 = cur_pt0;
                point1 = cur_pt1;
            }
            if( min_distance <= GLOBAL_EPSILON )
            {
                return std::tuple{ min_distance, point0, point1 };
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
            if( min_distance <= GLOBAL_EPSILON )
            {
                return std::tuple{ min_distance, point0, point1 };
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
            if( min_distance <= GLOBAL_EPSILON )
            {
                return std::tuple{ min_distance, point0, point1 };
            }
        }
        return std::tuple{ min_distance, point0, point1 };
    }

    std::tuple< double, Point3D > point_tetrahedron_distance(
        const Point3D& point, const Tetrahedron& tetra )
    {
        // Facets seen from the point (with exact predicates): the closest
        // point is on one of them, and there is none for an inside point. A
        // flat tetrahedron is the union of its facets.
        const auto orientation = internal::tetrahedron_orientation( tetra );
        std::array< bool, 4 > visible;
        for( const auto facet : LRange{ 4 } )
        {
            visible[facet] = orientation == GEO::zero
                             || internal::point_tetrahedron_facet_inner_sign(
                                    point, tetra, facet, orientation )
                                    == GEO::negative;
        }
        if( absl::c_none_of( visible, []( bool seen ) {
                return seen;
            } ) )
        {
            return { 0.0, point };
        }
        const auto& vertices = tetra.vertices();
        std::tuple< double, Point3D > result{
            std::numeric_limits< double >::max(), point
        };
        for( const auto facet : LRange{ 4 } )
        {
            if( !visible[facet] )
            {
                continue;
            }
            const auto& facet_vertices =
                Tetrahedron::tetrahedron_facet_vertex[facet];
            auto output = point_triangle_distance(
                point, Triangle3D{ vertices[facet_vertices[0]],
                           vertices[facet_vertices[1]],
                           vertices[facet_vertices[2]] } );
            if( std::get< 0 >( output ) < std::get< 0 >( result ) )
            {
                result = std::move( output );
            }
        }
        return result;
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
            return { -std::get< 0 >( output ), nearest_point };
        }
        return no_pivot_point_triangle_distance( point, triangle );
    }

    std::tuple< double, Point3D > point_plane_signed_distance(
        const Point3D& point, const Plane& plane )
    {
        const Vector3D v{ plane.origin(), point };
        const auto distance = v.dot( plane.normal() );
        const Point3D projected_p{ point - plane.normal() * distance };
        return { distance, projected_p };
    }

    std::tuple< double, Point3D > point_plane_distance(
        const Point3D& point, const Plane& plane )
    {
        double distance;
        Point3D projected_p;
        std::tie( distance, projected_p ) =
            point_plane_signed_distance( point, plane );
        return { std::fabs( distance ), projected_p };
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_sphere_distance(
        const Point< dimension >& point, const Sphere< dimension >& sphere )
    {
        const Vector< dimension > center_to_point{ sphere.origin(), point };
        if( center_to_point.length2() == 0 )
        {
            // No direction: any sphere point is the closest one
            Vector< dimension > dummy_direction;
            dummy_direction.set_value( 0, 1 );
            return { sphere.radius(),
                sphere.origin() + dummy_direction * sphere.radius() };
        }
        return { std::fabs( center_to_point.length() - sphere.radius() ),
            sphere.origin() + center_to_point.normalize() * sphere.radius() };
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_sphere_signed_distance(
        const Point< dimension >& point, const Sphere< dimension >& sphere )
    {
        const Vector< dimension > center_to_point{ sphere.origin(), point };
        if( center_to_point.length2() == 0 )
        {
            // No direction: any sphere point is the closest one
            Vector< dimension > dummy_direction;
            dummy_direction.set_value( 0, 1 );
            return { -sphere.radius(),
                sphere.origin() + dummy_direction * sphere.radius() };
        }
        return { center_to_point.length() - sphere.radius(),
            sphere.origin() + center_to_point.normalize() * sphere.radius() };
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
        return { 0, point };
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
            OpenGeodeGeometryException::check_assertion(
                !circle.plane().normal().inexact_equal( other_direction ),
                "[point_circle_distance] Problem while getting circle "
                "nearest point" );
            const Vector3D other_projected_on_plane =
                other_direction
                - circle.plane().normal()
                      * other_direction.dot( circle.plane().normal() );
            return { std::sqrt( circle.radius() * circle.radius()
                                + distance_to_plane * distance_to_plane ),
                circle.plane().origin()
                    + other_projected_on_plane.normalize() * circle.radius() };
        }
        const auto nearest_point =
            circle.plane().origin()
            + center_to_projected_point.normalize() * circle.radius();
        return { point_point_distance( point, nearest_point ), nearest_point };
    }

    std::tuple< double, Point3D > point_circle_signed_distance(
        const Point3D& point, const Circle& circle )
    {
        double distance;
        Point3D nearest_point;
        std::tie( distance, nearest_point ) =
            point_circle_distance( point, circle );
        if( circle.plane().normal().dot(
                Vector3D{ circle.plane().origin(), point } )
            < 0 )
        {
            distance = -distance;
        }
        return { distance, nearest_point };
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
            return { std::fabs( distance_to_plane ), projected_on_plane };
        }
        return point_circle_distance( point, disk );
    }

    std::tuple< double, Point3D > point_cylinder_distance(
        const Point3D& point, const Cylinder& cylinder )
    {
        // Cylinder frame: axial coordinate along the unit axis direction,
        // radial vector orthogonal to it, both from the axis center.
        const auto& axis = cylinder.axis();
        const auto axis_direction = axis.direction();
        const auto half_height = 0.5 * axis.length();
        const auto radius = cylinder.radius();
        const auto center = axis.barycenter();
        const Vector3D center_to_point{ center, point };
        const auto axial = center_to_point.dot( axis_direction );
        const auto radial_vector = center_to_point - axis_direction * axial;
        const auto radial = radial_vector.length();
        const auto axial_distance = std::fabs( axial ) - half_height;
        const auto radial_distance = radial - radius;
        const auto disk_center =
            center + axis_direction * std::copysign( half_height, axial );
        const auto to_wall = [&]( const Point3D& origin ) {
            if( radial > 0 )
            {
                return origin + radial_vector * ( radius / radial );
            }
            // Point on the axis: any radial direction is the closest one
            Vector3D other;
            other.set_value(
                std::fabs( axis_direction.value( 0 ) ) < 0.9 ? 0 : 1, 1. );
            return origin + axis_direction.cross( other ).normalize() * radius;
        };
        if( axial_distance > 0 )
        {
            if( radial_distance <= 0 )
            {
                return { axial_distance, disk_center + radial_vector };
            }
            return { std::sqrt( axial_distance * axial_distance
                                + radial_distance * radial_distance ),
                to_wall( disk_center ) };
        }
        if( radial_distance >= 0 )
        {
            return { radial_distance,
                to_wall( center + axis_direction * axial ) };
        }
        if( -radial_distance <= -axial_distance )
        {
            return { -radial_distance,
                to_wall( center + axis_direction * axial ) };
        }
        return { -axial_distance, disk_center + radial_vector };
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
        // The point is at most (sqrt(sum_z_squared) - 1) * emax away from the
        // ellipse (emax = locE[0])
        if( std::fabs( std::sqrt( sum_z_squared ) - 1 ) * locE[0]
            < geode::GLOBAL_EPSILON )
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
            numerator[i] = pSqr[i] * zPos[i];
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
                ePos[numpos] = extents[i];
                yPos[numpos] = query_point_coordinates[i];
                ++numpos;
                continue;
            }
            closest_point_coordinates[i] = 0.;
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
    std::tuple< double, std::array< double, dimension > > SquaredDistance(
        const std::array< double, dimension >& extents,
        const std::array< double, dimension >& query_point_coordinates )
    {
        // Sort the axes by decreasing extent and reflect the query point in
        // the first orthant
        std::array< index_t, dimension > sorted_axes;
        absl::c_iota( sorted_axes, 0 );
        absl::c_stable_sort( sorted_axes, [&extents]( index_t a, index_t b ) {
            return extents[a] > extents[b];
        } );
        std::array< double, dimension > sorted_extents;
        std::array< double, dimension > sorted_coordinates;
        for( const auto i : geode::LRange{ dimension } )
        {
            const auto axis = sorted_axes[i];
            sorted_extents[i] = extents[axis];
            sorted_coordinates[i] = std::fabs( query_point_coordinates[axis] );
        }
        auto result = SqrDistanceSpecial< dimension >(
            sorted_extents, sorted_coordinates );
        // Restore the axis order and the reflections
        const auto sorted_closest_coordinates = std::get< 1 >( result );
        auto& closest_coordinates = std::get< 1 >( result );
        for( const auto i : geode::LRange{ dimension } )
        {
            const auto axis = sorted_axes[i];
            closest_coordinates[axis] = query_point_coordinates[axis] < 0.
                                            ? -sorted_closest_coordinates[i]
                                            : sorted_closest_coordinates[i];
        }
        return result;
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_ellipse_distance(
        const Point< dimension >& point, const Ellipse< dimension >& ellipse )
    {
        std::array< Vector< dimension >, dimension > unit_axes;
        std::array< double, dimension > extents;
        std::array< double, dimension > point_coordinates;
        const Vector< dimension > center_to_point{ ellipse.center(), point };
        for( const auto i : geode::LRange{ dimension } )
        {
            const auto& axis = ellipse.axes().direction( i );
            extents[i] = axis.length();
            unit_axes[i] = axis / extents[i];
            point_coordinates[i] = center_to_point.dot( unit_axes[i] );
        }
        const auto [squared_distance, closest_point_coordinates] =
            SquaredDistance< dimension >( extents, point_coordinates );
        std::tuple< double, Point< dimension > > result{
            std::sqrt( squared_distance ), ellipse.center()
        };
        auto& closest_point = std::get< 1 >( result );
        for( const auto i : geode::LRange{ dimension } )
        {
            closest_point += unit_axes[i] * closest_point_coordinates[i];
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
