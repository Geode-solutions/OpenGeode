/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

    double compute_point_segment_distance( double segment_length,
        double point_to_v0_length,
        double point_to_v1_length )
    {
        if( std::fabs(
                point_to_v0_length + point_to_v1_length - segment_length )
            < geode::global_epsilon )
        {
            return 0;
        }
        const auto p =
            ( point_to_v0_length + point_to_v1_length + segment_length ) / 2;
        auto area2 = p * ( p - point_to_v0_length ) * ( p - point_to_v1_length )
                     * ( p - segment_length );
        if( area2 < 0 )
        {
            return 0;
        }
        return 2 * std::sqrt( area2 ) / segment_length;
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
    std::tuple< double, Point< dimension > > point_segment_distance(
        const Point< dimension >& point, const Segment< dimension >& segment )
    {
        const auto nearest_p = point_segment_projection( point, segment );
        return std::make_tuple(
            point_point_distance( point, nearest_p ), nearest_p );
    }

    template < index_t dimension >
    double new_point_segment_distance(
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
        return compute_point_segment_distance( length, length0, length1 );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension >, Point< dimension > >
        compute_segment_segment_distance( const Segment< dimension >& segment0,
            const Segment< dimension >& segment1 )
    {
        /* Algorithm and code found on
         * http://geomalgorithms.com/a07-_distance.html
         */
        const auto& vertices0 = segment0.vertices();
        const auto& vertices1 = segment1.vertices();
        const Vector< dimension > u{ vertices0[0], vertices0[1] };
        const Vector< dimension > v{ vertices1[0], vertices1[1] };
        const Vector< dimension > w{ vertices1[0], vertices0[0] };
        const auto a = u.dot( u ); // always >= 0
        const auto b = u.dot( v );
        const auto c = v.dot( v ); // always >= 0
        const auto D = a * c - b * b;
        auto sc = D;
        auto sN = D;
        auto sD = D; // sc = sN / sD, default sD = D >= 0
        auto tc = D;
        auto tN = D;
        auto tD = D; // tc = tN / tD, default tD = D >= 0
        const auto d = u.dot( w );
        const auto e = v.dot( w );

        // compute the line parameters of the two closest points
        if( D <= 0.0 )
        { // the lines are almost parallel
            sN = 0.0; // force using point P0 on segment0
            sD = 1.0; // to prevent possible division by 0.0 later
            tN = e;
            tD = c;
        }
        else
        { // get the closest points on the infinite lines
            sN = ( b * e - c * d );
            tN = ( a * e - b * d );
            if( sN < 0.0 )
            { // sc < 0 => the s=0 edge is visible
                sN = 0.0;
                tN = e;
                tD = c;
            }
            else if( sN > sD )
            { // sc > 1  => the s=1 edge is visible
                sN = sD;
                tN = e + b;
                tD = c;
            }
        }

        if( tN < 0.0 )
        { // tc < 0 => the t=0 edge is visible
            tN = 0.0;
            // recompute sc for this edge
            if( -d < 0.0 )
                sN = 0.0;
            else if( -d > a )
                sN = sD;
            else
            {
                sN = -d;
                sD = a;
            }
        }
        else if( tN > tD )
        { // tc > 1  => the t=1 edge is visible
            tN = tD;
            // recompute sc for this edge
            if( ( -d + b ) < 0.0 )
                sN = 0;
            else if( ( -d + b ) > a )
                sN = sD;
            else
            {
                sN = ( -d + b );
                sD = a;
            }
        }
        // finally do the division to get sc and tc
        sc = ( std::fabs( sN ) <= 0.0 ? 0.0 : sN / sD );
        tc = ( std::fabs( tN ) <= 0.0 ? 0.0 : tN / tD );

        // get the difference of the two closest points
        const Point< dimension > closest0 = vertices0[0].get() + u * sc;
        const Point< dimension > closest1 = vertices1[0].get() + v * tc;
        const Vector< dimension > dP{ closest0,
            closest1 }; // =  segment0(sc) - segment1(tc)

        return std::make_tuple( dP.length(), closest0, closest1 );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension >, Point< dimension > >
        segment_segment_distance( const Segment< dimension >& segment0,
            const Segment< dimension >& segment1 )
    {
        auto min_distance = std::numeric_limits< double >::max();
        local_index_t min_p0{ 0 };
        local_index_t min_p1{ 0 };
        const auto& vertices0 = segment0.vertices();
        const auto& vertices1 = segment1.vertices();
        for( const auto p0 : LRange{ 2 } )
        {
            for( const auto p1 : LRange{ 2 } )
            {
                const auto cur_dist = point_point_distance(
                    vertices0[p0].get(), vertices1[p1].get() );
                if( cur_dist < min_distance )
                {
                    min_distance = cur_dist;
                    min_p0 = p0;
                    min_p1 = p1;
                }
            }
        }
        if( min_p0 == 1 )
        {
            if( min_p1 == 1 )
            {
                return compute_segment_segment_distance< dimension >(
                    { vertices0[1], vertices0[0] },
                    { vertices1[1], vertices1[0] } );
            }
            return compute_segment_segment_distance(
                { vertices0[1], vertices0[0] }, segment1 );
        }
        if( min_p1 == 1 )
        {
            return compute_segment_segment_distance(
                segment0, { vertices1[1], vertices1[0] } );
        }
        return compute_segment_segment_distance( segment0, segment1 );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_line_distance(
        const Point< dimension >& point, const InfiniteLine< dimension >& line )
    {
        const auto nearest_p = point_line_projection( point, line );
        return std::make_tuple(
            point_point_distance( point, nearest_p ), nearest_p );
    }

    template < index_t dimension >
    double new_point_line_distance(
        const Point< dimension >& point, const InfiniteLine< dimension >& line )
    {
        const auto length0 = point_point_distance( line.origin(), point );
        const auto length1 =
            point_point_distance( line.origin() + line.direction(), point );
        return compute_point_segment_distance( 1, length0, length1 );
    }

    std::tuple< double, Point2D > point_line_signed_distance(
        const Point2D& point, const InfiniteLine2D& line )
    {
        Point2D nearest_point;
        double distance;
        std::tie( distance, nearest_point ) =
            point_line_distance< 2 >( point, line );
        const Vector2D proj2point{ nearest_point, point };
        const auto signed_distance =
            dot_perpendicular( proj2point, line.direction() ) <= 0 ? distance
                                                                   : -distance;
        return std::make_tuple( signed_distance, nearest_point );
    }

    double new_point_line_signed_distance(
        const Point2D& point, const InfiniteLine2D& line )
    {
        const auto distance = new_point_line_distance< 2 >( point, line );
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
        const auto det = std::fabs( a00 * a11 - a01 * a01 );
        const Vector3D diff{ point, vertices[v0] };
        const auto b0 = diff.dot( edge0 );
        const auto b1 = diff.dot( edge1 );
        auto s = a01 * b1 - a11 * b0;
        auto t = a01 * b0 - a00 * b1;

        if( s + t <= det )
        {
            if( s < 0.0 )
            {
                if( t < 0.0 )
                { // region 4
                    if( b0 < 0.0 )
                    {
                        t = 0.0;
                        if( -b0 >= a00 )
                        {
                            s = 1.0;
                        }
                        else
                        {
                            s = -b0 / a00;
                        }
                    }
                    else
                    {
                        s = 0.0;
                        if( b1 >= 0.0 )
                        {
                            t = 0.0;
                        }
                        else if( -b1 >= a11 )
                        {
                            t = 1.0;
                        }
                        else
                        {
                            t = -b1 / a11;
                        }
                    }
                }
                else
                { // region 3
                    s = 0.0;
                    if( b1 >= 0.0 )
                    {
                        t = 0.0;
                    }
                    else if( -b1 >= a11 )
                    {
                        t = 1.0;
                    }
                    else
                    {
                        t = -b1 / a11;
                    }
                }
            }
            else if( t < 0.0 )
            { // region 5
                t = 0.0;
                if( b0 >= 0.0 )
                {
                    s = 0.0;
                }
                else if( -b0 >= a00 )
                {
                    s = 1.0;
                }
                else
                {
                    s = -b0 / a00;
                }
            }
            else
            { // region 0
              // minimum at interior point
                const auto invDet = 1.0 / det;
                s *= invDet;
                t *= invDet;
            }
        }
        else
        {
            if( s < 0.0 )
            { // region 2
                const auto tmp0 = a01 + b0;
                const auto tmp1 = a11 + b1;
                if( tmp1 > tmp0 )
                {
                    const auto numer = tmp1 - tmp0;
                    const auto denom = a00 - 2.0 * a01 + a11;
                    if( numer >= denom )
                    {
                        s = 1.0;
                        t = 0.0;
                    }
                    else
                    {
                        s = numer / denom;
                        t = 1.0 - s;
                    }
                }
                else
                {
                    s = 0.0;
                    if( tmp1 <= 0.0 )
                    {
                        t = 1.0;
                    }
                    else if( b1 >= 0.0 )
                    {
                        t = 0.0;
                    }
                    else
                    {
                        t = -b1 / a11;
                    }
                }
            }
            else if( t < 0.0 )
            { // region 6
                const auto tmp0 = a01 + b1;
                const auto tmp1 = a00 + b0;
                if( tmp1 > tmp0 )
                {
                    const auto numer = tmp1 - tmp0;
                    const auto denom = a00 - 2.0 * a01 + a11;
                    if( numer >= denom )
                    {
                        t = 1.0;
                        s = 0.0;
                    }
                    else
                    {
                        t = numer / denom;
                        s = 1.0 - t;
                    }
                }
                else
                {
                    t = 0.0;
                    if( tmp1 <= 0.0 )
                    {
                        s = 1.0;
                    }
                    else if( b0 >= 0.0 )
                    {
                        s = 0.0;
                    }
                    else
                    {
                        s = -b0 / a00;
                    }
                }
            }
            else
            { // region 1
                const auto numer = a11 + b1 - a01 - b0;
                if( numer <= 0.0 )
                {
                    s = 0.0;
                    t = 1.0;
                }
                else
                {
                    const auto denom = a00 - 2.0 * a01 + a11;
                    if( numer >= denom )
                    {
                        s = 1.0;
                        t = 0.0;
                    }
                    else
                    {
                        s = numer / denom;
                        t = 1.0 - s;
                    }
                }
            }
        }

        Point3D closest_point{ vertices[v0].get() + edge0 * s + edge1 * t };
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
            edge_distances[e] = point_segment_distance(
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
        std::tie( distance[0], closest[0] ) = point_segment_distance(
            point, Segment2D{ vertices[0], vertices[1] } );
        std::tie( distance[1], closest[1] ) = point_segment_distance(
            point, Segment2D{ vertices[1], vertices[2] } );
        std::tie( distance[2], closest[2] ) = point_segment_distance(
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
        if( const auto normal = triangle.new_normal() )
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
            if( circle.plane().normal().inexact_equal(
                    other_direction, global_epsilon )
                || circle.plane().normal().inexact_equal( other_direction * -1.,
                    global_epsilon ) ) // dummy_direction is aligned along plane
                                       // normal
            {
                other_direction.set_value( 1, 1.0 );
            }
            OPENGEODE_ASSERT( !circle.plane().normal().inexact_equal(
                                  other_direction, global_epsilon ),
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
    template std::tuple< double, Point1D > opengeode_geometry_api
        point_segment_distance( const Point1D&, const Segment1D& );
    template std::tuple< double, Point1D, Point1D > opengeode_geometry_api
        segment_segment_distance( const Segment1D&, const Segment1D& );

    template double opengeode_geometry_api point_point_distance(
        const Point2D&, const Point2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_segment_distance( const Point2D&, const Segment2D& );
    template double opengeode_geometry_api new_point_segment_distance(
        const Point2D&, const Segment2D& );
    template std::tuple< double, Point2D, Point2D > opengeode_geometry_api
        segment_segment_distance( const Segment2D&, const Segment2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_line_distance( const Point2D&, const InfiniteLine2D& );
    template double opengeode_geometry_api new_point_line_distance(
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
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_segment_distance( const Point3D&, const Segment3D& );
    template double opengeode_geometry_api new_point_segment_distance(
        const Point3D&, const Segment3D& );
    template std::tuple< double, Point3D, Point3D > opengeode_geometry_api
        segment_segment_distance( const Segment3D&, const Segment3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_line_distance( const Point3D&, const InfiniteLine3D& );
    template double opengeode_geometry_api new_point_line_distance(
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
