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
#include <geode/geometry/perpendicular.h>
#include <geode/geometry/projection.h>
#include <geode/geometry/signed_mensuration.h>
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
        const auto signed_area_1 =
            geode::triangle_signed_area( geode::Triangle2D{
                triangle.vertices()[0], triangle.vertices()[1], point } );
        const auto signed_area_2 =
            geode::triangle_signed_area( geode::Triangle2D{
                triangle.vertices()[1], triangle.vertices()[2], point } );
        const auto signed_area_3 =
            geode::triangle_signed_area( geode::Triangle2D{
                triangle.vertices()[2], triangle.vertices()[0], point } );

        return ( signed_area_1 <= 0. && signed_area_2 <= 0.
                   && signed_area_3 <= 0 )
               || ( signed_area_1 >= 0. && signed_area_2 >= 0.
                    && signed_area_3 >= 0 );
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    double point_point_distance(
        const Point< dimension >& point0, const Point< dimension >& point1 )
    {
        return Vector< dimension >{ point0, point1 }.length();
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
    std::tuple< double, Point< dimension >, Point< dimension > >
        segment_segment_distance( const Segment< dimension >& segment0,
            const Segment< dimension >& segment1 )
    {
        /* Algorithm and code found on
         * http://geomalgorithms.com/a07-_distance.html
         */
        const Vector< dimension > u{ segment0.vertices()[0],
            segment0.vertices()[1] };
        const Vector< dimension > v{ segment1.vertices()[0],
            segment1.vertices()[1] };
        const Vector< dimension > w{ segment1.vertices()[0],
            segment0.vertices()[0] };
        const auto a = u.dot( u ); // always >= 0
        const auto b = u.dot( v );
        const auto c = v.dot( v ); // always >= 0
        const auto d = u.dot( w );
        const auto e = v.dot( w );
        const auto D = a * c - b * b;
        auto sc = D;
        auto sN = D;
        auto sD = D; // sc = sN / sD, default sD = D >= 0
        auto tc = D;
        auto tN = D;
        auto tD = D; // tc = tN / tD, default tD = D >= 0

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
        const Point< dimension > closest0 =
            segment0.vertices()[0].get() + u * sc;
        const Point< dimension > closest1 =
            segment1.vertices()[0].get() + v * tc;
        const Vector< dimension > dP{ closest0,
            closest1 }; // =  segment0(sc) - segment1(tc)

        return std::make_tuple( dP.length(), closest0, closest1 );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_line_distance(
        const Point< dimension >& point, const InfiniteLine< dimension >& line )
    {
        const auto nearest_p = point_line_projection( point, line );
        return std::make_tuple(
            point_point_distance( point, nearest_p ), nearest_p );
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

    template <>
    std::tuple< double, Point3D > point_triangle_distance(
        const Point3D& point, const Triangle3D& triangle )
    {
        const Vector3D diff{ point, triangle.vertices()[0] };
        const Vector3D edge0{ triangle.vertices()[0], triangle.vertices()[1] };
        const Vector3D edge1{ triangle.vertices()[0], triangle.vertices()[2] };
        const auto a00 = edge0.length2();
        const auto a01 = edge0.dot( edge1 );
        const auto a11 = edge1.length2();
        const auto b0 = diff.dot( edge0 );
        const auto b1 = diff.dot( edge1 );
        const auto det = std::fabs( a00 * a11 - a01 * a01 );
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

        Point3D closest_point{ triangle.vertices()[0].get() + edge0 * s
                               + edge1 * t };
        const auto distance = point_point_distance( point, closest_point );
        return std::make_tuple( distance, std::move( closest_point ) );
    }

    template <>
    std::tuple< double, Point2D > point_triangle_distance(
        const Point2D& point, const Triangle2D& triangle )
    {
        if( may_point_be_in_triangle( point, triangle ) )
        {
            return std::make_tuple( 0.0, point );
        }
        std::array< Point2D, 3 > closest;
        std::array< double, 3 > distance;
        std::tie( distance[0], closest[0] ) = point_segment_distance( point,
            Segment2D{ triangle.vertices()[0], triangle.vertices()[1] } );
        std::tie( distance[1], closest[1] ) = point_segment_distance( point,
            Segment2D{ triangle.vertices()[1], triangle.vertices()[2] } );
        std::tie( distance[2], closest[2] ) = point_segment_distance( point,
            Segment2D{ triangle.vertices()[2], triangle.vertices()[0] } );
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
        for( const auto f : Range{ 4 } )
        {
            double distance;
            Point3D cur_p;
            std::tie( distance, cur_p ) = point_triangle_signed_distance( point,
                Triangle3D{
                    tetra.vertices()[Tetrahedron::tetrahedron_facet_vertex[f]
                                                                          [0]],
                    tetra.vertices()[Tetrahedron::tetrahedron_facet_vertex[f]
                                                                          [1]],
                    tetra.vertices()
                        [Tetrahedron::tetrahedron_facet_vertex[f][2]] } );
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
        const Point2D&, const Point2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_segment_distance( const Point2D&, const Segment2D& );
    template std::tuple< double, Point2D, Point2D > opengeode_geometry_api
        segment_segment_distance( const Segment2D&, const Segment2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_line_distance( const Point2D&, const InfiniteLine2D& );
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
    template std::tuple< double, Point3D, Point3D > opengeode_geometry_api
        segment_segment_distance( const Segment3D&, const Segment3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_line_distance( const Point3D&, const InfiniteLine3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_triangle_distance( const Point3D&, const Triangle3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_sphere_distance( const Point3D&, const Sphere3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_sphere_signed_distance( const Point3D&, const Sphere3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_ball_distance( const Point3D&, const Ball3D& );
} // namespace geode
