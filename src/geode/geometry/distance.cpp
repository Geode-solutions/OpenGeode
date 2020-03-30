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

#include <geode/geometry/distance.h>

#include <geode/basic/logger.h>

#include <geode/geometry/basic_objects.h>
#include <geode/geometry/projection.h>
#include <geode/geometry/vector.h>

namespace
{
    constexpr double MAX_DOUBLE = std::numeric_limits< double >::max();
} // namespace

namespace geode
{
    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_segment_distance(
        const Point< dimension >& point, const Segment< dimension >& segment )
    {
        const auto nearest_p = point_segment_projection( point, segment );
        return std::make_tuple(
            Vector< dimension >{ point, nearest_p }.length(), nearest_p );
    }

    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_line_distance(
        const Point< dimension >& point, const InfiniteLine< dimension >& line )
    {
        const auto nearest_p = point_line_projection( point, line );
        return std::make_tuple(
            Vector< dimension >{ point, nearest_p }.length(), nearest_p );
    }

    template <>
    std::tuple< double, Point3D > point_triangle_distance(
        const Point3D& point, const Triangle3D& triangle )
    {
        Vector3D diff{ point, triangle.vertices()[0] };
        Vector3D edge0{ triangle.vertices()[0], triangle.vertices()[1] };
        Vector3D edge1{ triangle.vertices()[0], triangle.vertices()[2] };
        auto a00 = edge0.length2();
        auto a01 = edge0.dot( edge1 );
        auto a11 = edge1.length2();
        auto b0 = diff.dot( edge0 );
        auto b1 = diff.dot( edge1 );
        auto c = diff.length2();
        auto det = std::fabs( a00 * a11 - a01 * a01 );
        auto s = a01 * b1 - a11 * b0;
        auto t = a01 * b0 - a00 * b1;
        DEBUG( a00 );
        DEBUG( a01 );
        DEBUG( a11 );
        DEBUG( b0 );
        DEBUG( b1 );
        DEBUG( c );
        DEBUG( det );
        double sqrDistance;

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
                            sqrDistance = a00 + 2.0 * b0 + c;
                        }
                        else
                        {
                            s = -b0 / a00;
                            sqrDistance = b0 * s + c;
                        }
                    }
                    else
                    {
                        s = 0.0;
                        if( b1 >= 0.0 )
                        {
                            t = 0.0;
                            sqrDistance = c;
                        }
                        else if( -b1 >= a11 )
                        {
                            t = 1.0;
                            sqrDistance = a11 + 2.0 * b1 + c;
                        }
                        else
                        {
                            t = -b1 / a11;
                            sqrDistance = b1 * t + c;
                        }
                    }
                }
                else
                { // region 3
                    s = 0.0;
                    if( b1 >= 0.0 )
                    {
                        t = 0.0;
                        sqrDistance = c;
                    }
                    else if( -b1 >= a11 )
                    {
                        t = 1.0;
                        sqrDistance = a11 + 2.0 * b1 + c;
                    }
                    else
                    {
                        t = -b1 / a11;
                        sqrDistance = b1 * t + c;
                    }
                }
            }
            else if( t < 0.0 )
            { // region 5
                t = 0.0;
                if( b0 >= 0.0 )
                {
                    s = 0.0;
                    sqrDistance = c;
                }
                else if( -b0 >= a00 )
                {
                    s = 1.0;
                    sqrDistance = a00 + 2.0 * b0 + c;
                }
                else
                {
                    s = -b0 / a00;
                    sqrDistance = b0 * s + c;
                }
            }
            else
            { // region 0
                // minimum at interior point
                auto invDet = 1.0 / det;
                s *= invDet;
                t *= invDet;
                sqrDistance = s * ( a00 * s + a01 * t + 2.0 * b0 )
                              + t * ( a01 * s + a11 * t + 2.0 * b1 ) + c;
            }
        }
        else
        {
            double tmp0;
            double tmp1;
            double numer;
            double denom;

            if( s < 0.0 )
            { // region 2
                tmp0 = a01 + b0;
                tmp1 = a11 + b1;
                if( tmp1 > tmp0 )
                {
                    numer = tmp1 - tmp0;
                    denom = a00 - 2.0 * a01 + a11;
                    if( numer >= denom )
                    {
                        s = 1.0;
                        t = 0.0;
                        sqrDistance = a00 + 2.0 * b0 + c;
                    }
                    else
                    {
                        s = numer / denom;
                        t = 1.0 - s;
                        sqrDistance = s * ( a00 * s + a01 * t + 2.0 * b0 )
                                      + t * ( a01 * s + a11 * t + 2.0 * b1 )
                                      + c;
                    }
                }
                else
                {
                    s = 0.0;
                    if( tmp1 <= 0.0 )
                    {
                        t = 1.0;
                        sqrDistance = a11 + 2.0 * b1 + c;
                    }
                    else if( b1 >= 0.0 )
                    {
                        t = 0.0;
                        sqrDistance = c;
                    }
                    else
                    {
                        t = -b1 / a11;
                        sqrDistance = b1 * t + c;
                    }
                }
            }
            else if( t < 0.0 )
            { // region 6
                tmp0 = a01 + b1;
                tmp1 = a00 + b0;
                if( tmp1 > tmp0 )
                {
                    numer = tmp1 - tmp0;
                    denom = a00 - 2.0 * a01 + a11;
                    if( numer >= denom )
                    {
                        t = 1.0;
                        s = 0.0;
                        sqrDistance = a11 + 2.0 * b1 + c;
                    }
                    else
                    {
                        t = numer / denom;
                        s = 1.0 - t;
                        sqrDistance = s * ( a00 * s + a01 * t + 2.0 * b0 )
                                      + t * ( a01 * s + a11 * t + 2.0 * b1 )
                                      + c;
                    }
                }
                else
                {
                    t = 0.0;
                    if( tmp1 <= 0.0 )
                    {
                        s = 1.0;
                        sqrDistance = a00 + 2.0 * b0 + c;
                    }
                    else if( b0 >= 0.0 )
                    {
                        s = 0.0;
                        sqrDistance = c;
                    }
                    else
                    {
                        s = -b0 / a00;
                        sqrDistance = b0 * s + c;
                    }
                }
            }
            else
            { // region 1
                numer = a11 + b1 - a01 - b0;
                if( numer <= 0.0 )
                {
                    s = 0.0;
                    t = 1.0;
                    sqrDistance = a11 + 2.0 * b1 + c;
                }
                else
                {
                    denom = a00 - 2.0 * a01 + a11;
                    if( numer >= denom )
                    {
                        s = 1.0;
                        t = 0.0;
                        sqrDistance = a00 + 2.0 * b0 + c;
                    }
                    else
                    {
                        s = numer / denom;
                        t = 1.0 - s;
                        sqrDistance = s * ( a00 * s + a01 * t + 2.0 * b0 )
                                      + t * ( a01 * s + a11 * t + 2.0 * b1 )
                                      + c;
                    }
                }
            }
        }

        // Account for numerical round-off error.
        if( sqrDistance < 0.0 )
        {
            sqrDistance = 0.0;
        }

        Point3D closest_point{ triangle.vertices()[0].get() + edge0 * s
                               + edge1 * t };
        return std::make_tuple( std::sqrt( sqrDistance ), closest_point );
    }

    template <>
    std::tuple< double, Point2D > point_triangle_distance(
        const Point2D& point, const Triangle2D& triangle )
    {
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

    std::tuple< double, Point3D > point_tetra_distance(
        const Point3D& point, const Tetra& tetra )
    {
        auto dist = MAX_DOUBLE;
        Point3D nearest_p;
        bool inside{ true };
        for( const auto f : Range{ 4 } )
        {
            auto distance = MAX_DOUBLE;
            Point3D cur_p;
            std::tie( distance, cur_p ) = point_triangle_signed_distance( point,
                Triangle3D{ tetra.vertices()[Tetra::tetra_facet_vertex[f][0]],
                    tetra.vertices()[Tetra::tetra_facet_vertex[f][1]],
                    tetra.vertices()[Tetra::tetra_facet_vertex[f][2]] } );
            if( distance > 0 )
            {
                inside = false;
            }
            if( distance < dist && distance >= 0 )
            {
                dist = distance;
                nearest_p = cur_p;
            }
        }
        if( inside )
        {
            nearest_p = point;
            return std::make_tuple( 0.0, nearest_p );
        }
        return std::make_tuple( dist, nearest_p );
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
        const auto signed_distance =
            proj2point.dot( triangle.normal() ) <= 0 ? distance : -distance;
        return std::make_tuple( signed_distance, nearest_point );
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

    template std::tuple< double, Point2D > opengeode_geometry_api
        point_segment_distance( const Point2D&, const Segment2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_line_distance( const Point2D&, const InfiniteLine2D& );
    template std::tuple< double, Point2D > opengeode_geometry_api
        point_triangle_distance( const Point2D&, const Triangle2D& );

    template std::tuple< double, Point3D > opengeode_geometry_api
        point_segment_distance( const Point3D&, const Segment3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_line_distance( const Point3D&, const InfiniteLine3D& );
    template std::tuple< double, Point3D > opengeode_geometry_api
        point_triangle_distance( const Point3D&, const Triangle3D& );
} // namespace geode
