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

#include <geode/geometry/position.h>

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/mensuration.h>
#include <geode/geometry/perpendicular.h>
#include <geode/geometry/private/position_from_sides.h>
#include <geode/geometry/private/predicates.h>

namespace geode
{
    Position point_segment_position_exact(
        const Point3D& point, const Segment3D& segment )
    {
        const auto& vertices = segment.vertices();
        if( !GEO::PCK::aligned_3d( point, vertices[0], vertices[1] ) )
        {
            return Position::outside;
        }
        const auto dot0 = GEO::PCK::dot_3d( vertices[0], point, vertices[1] );
        const auto dot1 = GEO::PCK::dot_3d( vertices[1], point, vertices[0] );
        return detail::point_segment_position(
            detail::side( dot0 ), detail::opposite_side( dot1 ) );
    }

    Position point_segment_position_exact(
        const Point2D& point, const Segment2D& segment )
    {
        const auto& vertices = segment.vertices();
        if( GEO::PCK::orient_2d( point, vertices[0], vertices[1] )
            != GEO::ZERO )
        {
            return Position::outside;
        }
        const auto dot0 = GEO::PCK::dot_2d( vertices[0], point, vertices[1] );
        const auto dot1 = GEO::PCK::dot_2d( vertices[1], point, vertices[0] );
        return detail::point_segment_position(
            detail::side( dot0 ), detail::opposite_side( dot1 ) );
    }

    Position point_triangle_position_exact(
        const Point2D& point, const Triangle2D& triangle )
    {
        const auto& vertices = triangle.vertices();
        const auto s0 =
            point_side_to_segment( point, { vertices[0], vertices[1] } );
        const auto s1 =
            point_side_to_segment( point, { vertices[1], vertices[2] } );
        const auto s2 =
            point_side_to_segment( point, { vertices[2], vertices[0] } );
        return detail::point_triangle_position( s0, s1, s2 );
    }

    Position compute_determinants( const Point3D& point,
        const Triangle3D& triangle,
        const Vector3D& third_vector )
    {
        const auto& vertices = triangle.vertices();
        const auto det0 = GEO::PCK::det_3d(
            point - vertices[0], point - vertices[1], third_vector );
        const auto det1 = GEO::PCK::det_3d(
            point - vertices[1], point - vertices[2], third_vector );
        const auto det2 = GEO::PCK::det_3d(
            point - vertices[2], point - vertices[0], third_vector );

        if( det0 == GEO::ZERO && det1 == GEO::ZERO && det2 == GEO::ZERO )
        {
            return compute_determinants( point, triangle,
                { { 2 * third_vector.value( 0 ), -3 * third_vector.value( 2 ),
                    third_vector.value( 1 ) } } );
        }
        return detail::point_triangle_position(
            detail::side( det0 ), detail::side( det1 ), detail::side( det2 ) );
    }

    Position point_triangle_position_exact(
        const Point3D& point, const Triangle3D& triangle )
    {
        const auto& vertices = triangle.vertices();
        if( GEO::PCK::orient_3d( point, vertices[0], vertices[1], vertices[2] )
            != GEO::ZERO )
        {
            return Position::outside;
        }

        return compute_determinants( point, triangle, { { 1.0, 1.0, 1.0 } } );
    }

    Position point_tetrahedron_position_exact(
        const Point3D& point, const Tetrahedron& tetra )
    {
        std::array< GEO::Sign, 4 > signs;
        const auto& vertices = tetra.vertices();
        for( const auto f : Range{ 4 } )
        {
            const auto& facet_vertices =
                Tetrahedron::tetrahedron_facet_vertex[f];
            signs[f] = GEO::PCK::orient_3d( vertices[facet_vertices[0]],
                vertices[facet_vertices[1]], vertices[facet_vertices[2]],
                point );
            if( signs[f] == GEO::Sign::NEGATIVE )
            {
                return Position::outside;
            }
        }
        return detail::point_tetrahedron_position( signs );
    }

    Position point_tetrahedron_position(
        const Point3D& point, const Tetrahedron& tetra )
    {
        const auto& vertices = tetra.vertices();
        for( const auto f : Range{ 4 } )
        {
            const auto& facet_vertices =
                Tetrahedron::tetrahedron_facet_vertex[f];
            const auto volume = tetrahedron_signed_volume(
                { vertices[facet_vertices[0]], vertices[facet_vertices[1]],
                    vertices[facet_vertices[2]], point } );
            if( volume < -global_epsilon )
            {
                return Position::outside;
            }
            if( volume < global_epsilon )
            {
                return point_tetrahedron_position_exact( point, tetra );
            }
        }
        return Position::inside;
    }

    template <>
    Position point_triangle_position(
        const Point2D& point, const Triangle2D& triangle )
    {
        const auto& vertices = triangle.vertices();
        const auto area1 =
            triangle_signed_area( { point, vertices[0], vertices[1] } );
        if( std::fabs( area1 ) < global_epsilon )
        {
            return point_triangle_position_exact( point, triangle );
        }
        const auto s1 = GEO::geo_sgn( area1 );
        const auto area2 =
            triangle_signed_area( { point, vertices[1], vertices[2] } );
        if( std::fabs( area2 ) < global_epsilon )
        {
            return point_triangle_position_exact( point, triangle );
        }
        const auto s2 = GEO::geo_sgn( area2 );
        const auto area3 =
            triangle_signed_area( { point, vertices[2], vertices[0] } );
        if( std::fabs( area3 ) < global_epsilon )
        {
            return point_triangle_position_exact( point, triangle );
        }
        const auto s3 = GEO::geo_sgn( area3 );
        if( s1 == s2 && s2 == s3 )
        {
            return Position::inside;
        }
        return Position::outside;
    }

    template <>
    Position point_triangle_position(
        const Point3D& point, const Triangle3D& triangle )
    {
        double distance;
        std::tie( distance, std::ignore ) =
            point_triangle_distance( point, triangle );
        if( distance < global_epsilon )
        {
            return point_triangle_position_exact( point, triangle );
        }
        return Position::outside;
    }

    template < index_t dimension >
    Position point_segment_position(
        const Point< dimension >& point, const Segment< dimension >& segment )
    {
        double distance;
        std::tie( distance, std::ignore ) =
            point_segment_distance( point, segment );
        if( distance > global_epsilon )
        {
            return Position::outside;
        }
        return point_segment_position_exact( point, segment );
    }

    Side point_side_to_segment( const Point2D& point, const Segment2D& segment )
    {
        const auto& vertices = segment.vertices();
        return detail::side(
            GEO::PCK::orient_2d( point, vertices[0], vertices[1] ) );
    }

    Side point_side_to_line( const Point2D& point, const InfiniteLine2D& line )
    {
        return point_side_to_segment(
            point, { line.origin(), line.origin() + line.direction() } );
    }

    Side point_side_to_plane( const Point3D& point, const Plane& plane )
    {
        const auto mult = point.value( 0 ) * plane.normal().value( 0 )
                          + point.value( 1 ) * plane.normal().value( 1 )
                          + point.value( 2 ) * plane.normal().value( 2 );
        const auto constant = -plane.plane_constant();
        if( mult > constant )
        {
            return Side::positive;
        }
        if( mult < constant )
        {
            return Side::negative;
        }
        return Side::zero;
    }

    Side point_side_to_triangle(
        const Point3D& point, const Triangle3D& triangle )
    {
        const auto& vertices = triangle.vertices();
        return detail::side( GEO::PCK::orient_3d(
            vertices[0], vertices[1], vertices[2], point ) );
    }

    template Position opengeode_geometry_api point_triangle_position(
        const Point2D&, const Triangle2D& );

    template Position opengeode_geometry_api point_segment_position(
        const Point2D&, const Segment2D& );

    template Position opengeode_geometry_api point_triangle_position(
        const Point3D&, const Triangle3D& );

    template Position opengeode_geometry_api point_segment_position(
        const Point3D&, const Segment3D& );

} // namespace geode
