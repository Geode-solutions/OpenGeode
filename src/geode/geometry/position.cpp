/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/geometry/position.hpp>

#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/internal/position_from_sides.hpp>
#include <geode/geometry/internal/predicates.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/perpendicular.hpp>

namespace geode
{
    POSITION point_segment_position_exact(
        const Point3D& point, const Segment3D& segment )
    {
        const auto& vertices = segment.vertices();
        if( !GEO::PCK::aligned_3d( point, vertices[0], vertices[1] ) )
        {
            return POSITION::outside;
        }
        const auto dot0 = GEO::PCK::dot_3d( vertices[0], point, vertices[1] );
        const auto dot1 = GEO::PCK::dot_3d( vertices[1], point, vertices[0] );
        return internal::point_segment_position(
            internal::side( dot0 ), internal::opposite_side( dot1 ) );
    }

    POSITION point_segment_position_exact(
        const Point2D& point, const Segment2D& segment )
    {
        const auto& vertices = segment.vertices();
        if( GEO::PCK::orient_2d( point, vertices[0], vertices[1] )
            != GEO::zero )
        {
            return POSITION::outside;
        }
        const auto dot0 = GEO::PCK::dot_2d( vertices[0], point, vertices[1] );
        const auto dot1 = GEO::PCK::dot_2d( vertices[1], point, vertices[0] );
        return internal::point_segment_position(
            internal::side( dot0 ), internal::opposite_side( dot1 ) );
    }

    template < index_t dimension >
    POSITION point_triangle_position_all_zero(
        const Point< dimension >& point, const Triangle< dimension >& triangle )
    {
        const auto& vertices = triangle.vertices();
        if( point == vertices[0] )
        {
            return POSITION::parallel;
        }
        const auto smaller = point < vertices[0];

        if( smaller && vertices[1].get() <= point )
        {
            return POSITION::parallel;
        }
        if( smaller && vertices[2].get() <= point )
        {
            return POSITION::parallel;
        }
        if( !smaller && point <= vertices[1] )
        {
            return POSITION::parallel;
        }
        if( !smaller && point <= vertices[2] )
        {
            return POSITION::parallel;
        }
        return POSITION::outside;
    }

    POSITION point_triangle_position_exact(
        const Point2D& point, const Triangle2D& triangle )
    {
        const auto& vertices = triangle.vertices();
        const auto s0 =
            point_side_to_segment( point, { vertices[0], vertices[1] } );
        const auto s1 =
            point_side_to_segment( point, { vertices[1], vertices[2] } );
        const auto s2 =
            point_side_to_segment( point, { vertices[2], vertices[0] } );
        if( s0 == s1 && s1 == s2 && s2 == SIDE::zero )
        {
            return point_triangle_position_all_zero( point, triangle );
        }
        return internal::point_triangle_position( s0, s1, s2 );
    }

    POSITION compute_determinants( const Point3D& point,
        const Triangle3D& triangle,
        const Vector3D& third_vector )
    {
        const auto& vertices = triangle.vertices();
        const Vector3D v0_to_point{ vertices[0], point };
        const Vector3D v1_to_point{ vertices[1], point };
        const Vector3D v2_to_point{ vertices[2], point };
        const auto det0 =
            GEO::PCK::det_3d( v0_to_point, v1_to_point, third_vector );
        const auto det1 =
            GEO::PCK::det_3d( v1_to_point, v2_to_point, third_vector );
        const auto det2 =
            GEO::PCK::det_3d( v2_to_point, v0_to_point, third_vector );

        if( det0 == GEO::zero && det1 == GEO::zero && det2 == GEO::zero )
        {
            return point_triangle_position_all_zero( point, triangle );
        }
        return internal::point_triangle_position( internal::side( det0 ),
            internal::side( det1 ), internal::side( det2 ) );
    }

    POSITION point_triangle_position_exact(
        const Point3D& point, const Triangle3D& triangle )
    {
        const auto& vertices = triangle.vertices();
        if( GEO::PCK::orient_3d( point, vertices[0], vertices[1], vertices[2] )
            != GEO::zero )
        {
            return POSITION::outside;
        }

        return compute_determinants(
            point, triangle, Vector3D{ { 1.0, 1.0, 1.0 } } );
    }

    POSITION point_tetrahedron_position_exact(
        const Point3D& point, const Tetrahedron& tetra )
    {
        std::array< GEO::SIGN, 4 > signs;
        const auto& vertices = tetra.vertices();
        for( const auto f : Range{ 4 } )
        {
            const auto& facet_vertices =
                Tetrahedron::tetrahedron_facet_vertex[f];
            signs[f] = GEO::PCK::orient_3d( vertices[facet_vertices[0]],
                vertices[facet_vertices[1]], vertices[facet_vertices[2]],
                point );
            if( signs[f] == GEO::SIGN::negative )
            {
                return POSITION::outside;
            }
        }
        return internal::point_tetrahedron_position( signs );
    }

    POSITION point_tetrahedron_position(
        const Point3D& point, const Tetrahedron& tetra )
    {
        const auto& vertices = tetra.vertices();
        for( const auto f : Range{ 4 } )
        {
            const auto& facet_vertices =
                Tetrahedron::tetrahedron_facet_vertex[f];
            const Triangle3D facet{ vertices[facet_vertices[0]],
                vertices[facet_vertices[1]], vertices[facet_vertices[2]] };
            const auto signed_distance =
                std::get< 0 >( point_triangle_signed_distance( point, facet ) );
            if( signed_distance < -GLOBAL_EPSILON )
            {
                return POSITION::outside;
            }
            if( signed_distance < GLOBAL_EPSILON )
            {
                return point_tetrahedron_position_exact( point, tetra );
            }
        }
        return POSITION::inside;
    }

    template <>
    POSITION opengeode_geometry_api point_triangle_position(
        const Point2D& point, const Triangle2D& triangle )
    {
        const auto& vertices = triangle.vertices();
        const auto area1 =
            triangle_signed_area( { point, vertices[0], vertices[1] } );
        if( std::fabs( area1 ) < GLOBAL_EPSILON )
        {
            return point_triangle_position_exact( point, triangle );
        }
        const auto s1 = GEO::geo_sgn( area1 );
        const auto area2 =
            triangle_signed_area( { point, vertices[1], vertices[2] } );
        if( std::fabs( area2 ) < GLOBAL_EPSILON )
        {
            return point_triangle_position_exact( point, triangle );
        }
        const auto s2 = GEO::geo_sgn( area2 );
        const auto area3 =
            triangle_signed_area( { point, vertices[2], vertices[0] } );
        if( std::fabs( area3 ) < GLOBAL_EPSILON )
        {
            return point_triangle_position_exact( point, triangle );
        }
        const auto s3 = GEO::geo_sgn( area3 );
        if( s1 == s2 && s2 == s3 )
        {
            return POSITION::inside;
        }
        return POSITION::outside;
    }

    template <>
    POSITION opengeode_geometry_api point_triangle_position(
        const Point3D& point, const Triangle3D& triangle )
    {
        double distance;
        std::tie( distance, std::ignore ) =
            point_triangle_distance( point, triangle );
        if( distance < GLOBAL_EPSILON )
        {
            return point_triangle_position_exact( point, triangle );
        }
        return POSITION::outside;
    }

    template < index_t dimension >
    POSITION point_segment_position(
        const Point< dimension >& point, const Segment< dimension >& segment )
    {
        if( point_segment_distance( point, segment ) > GLOBAL_EPSILON )
        {
            return POSITION::outside;
        }
        return point_segment_position_exact( point, segment );
    }

    SIDE point_side_to_segment( const Point2D& point, const Segment2D& segment )
    {
        const auto& vertices = segment.vertices();
        return internal::side(
            GEO::PCK::orient_2d( point, vertices[0], vertices[1] ) );
    }

    SIDE point_side_to_line( const Point2D& point, const InfiniteLine2D& line )
    {
        return point_side_to_segment(
            point, { line.origin(), line.origin() + line.direction() } );
    }

    SIDE point_side_to_plane( const Point3D& point, const Plane& plane )
    {
        const auto mult = point.value( 0 ) * plane.normal().value( 0 )
                          + point.value( 1 ) * plane.normal().value( 1 )
                          + point.value( 2 ) * plane.normal().value( 2 );
        const auto constant = -plane.plane_constant();
        if( mult > constant )
        {
            return SIDE::positive;
        }
        if( mult < constant )
        {
            return SIDE::negative;
        }
        return SIDE::zero;
    }

    SIDE point_side_to_triangle(
        const Point3D& point, const Triangle3D& triangle )
    {
        const auto& vertices = triangle.vertices();
        return internal::side( GEO::PCK::orient_3d(
            vertices[0], vertices[1], vertices[2], point ) );
    }

    template <>
    bool opengeode_geometry_api are_points_aligned(
        const Point2D& point0, const Point2D& point1, const Point2D& point2 )
    {
        return GEO::PCK::orient_2d( point0, point1, point2 ) == GEO::SIGN::zero;
    }

    template <>
    bool opengeode_geometry_api are_points_aligned(
        const Point3D& point0, const Point3D& point1, const Point3D& point2 )
    {
        return GEO::PCK::aligned_3d( point0, point1, point2 );
    }

    template POSITION opengeode_geometry_api point_segment_position(
        const Point2D&, const Segment2D& );

    template POSITION opengeode_geometry_api point_segment_position(
        const Point3D&, const Segment3D& );
} // namespace geode
