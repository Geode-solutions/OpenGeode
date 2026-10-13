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

#include <geode/geometry/position.hpp>

#include <cmath>

#include <absl/algorithm/container.h>

#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/internal/position_from_sides.hpp>
#include <geode/geometry/internal/predicates.hpp>
#include <geode/geometry/mensuration.hpp>
#include <geode/geometry/perpendicular.hpp>

namespace
{
    template < geode::index_t dimension >
    bool is_finite( const geode::Point< dimension >& point )
    {
        for( const auto d : geode::LRange{ dimension } )
        {
            if( !std::isfinite( point.value( d ) ) )
            {
                return false;
            }
        }
        return true;
    }

    geode::POSITION point_segment_position_exact(
        const geode::Point3D& point, const geode::Segment3D& segment )
    {
        const auto& vertices = segment.vertices();
        if( !GEO::PCK::aligned_3d( point, vertices[0], vertices[1] ) )
        {
            return geode::POSITION::outside;
        }
        const auto dot0 = GEO::PCK::dot_3d( vertices[0], point, vertices[1] );
        const auto dot1 = GEO::PCK::dot_3d( vertices[1], point, vertices[0] );
        return geode::internal::point_segment_position(
            geode::internal::side( dot0 ),
            geode::internal::opposite_side( dot1 ) );
    }

    geode::POSITION point_segment_position_exact(
        const geode::Point2D& point, const geode::Segment2D& segment )
    {
        const auto& vertices = segment.vertices();
        if( GEO::PCK::orient_2d( point, vertices[0], vertices[1] )
            != GEO::zero )
        {
            return geode::POSITION::outside;
        }
        const auto dot0 = GEO::PCK::dot_2d( vertices[0], point, vertices[1] );
        const auto dot1 = GEO::PCK::dot_2d( vertices[1], point, vertices[0] );
        return geode::internal::point_segment_position(
            geode::internal::side( dot0 ),
            geode::internal::opposite_side( dot1 ) );
    }

    geode::POSITION point_segment_position_exact(
        const geode::Point1D& point, const geode::Segment1D& segment )
    {
        const auto& vertices = segment.vertices();
        if( point == vertices[0] )
        {
            return geode::POSITION::vertex0;
        }
        if( point == vertices[1] )
        {
            return geode::POSITION::vertex1;
        }
        if( segment.bounding_box().contains( point ) )
        {
            return geode::POSITION::inside;
        }
        return geode::POSITION::outside;
    }

    template < geode::index_t dimension >
    geode::POSITION point_triangle_position_all_zero(
        const geode::Point< dimension >& point,
        const geode::Triangle< dimension >& triangle )
    {
        const auto& vertices = triangle.vertices();
        if( point == vertices[0] )
        {
            return geode::POSITION::parallel;
        }
        const auto smaller = point < vertices[0];

        if( smaller && vertices[1].get() <= point )
        {
            return geode::POSITION::parallel;
        }
        if( smaller && vertices[2].get() <= point )
        {
            return geode::POSITION::parallel;
        }
        if( !smaller && point <= vertices[1] )
        {
            return geode::POSITION::parallel;
        }
        if( !smaller && point <= vertices[2] )
        {
            return geode::POSITION::parallel;
        }
        return geode::POSITION::outside;
    }

    geode::POSITION point_triangle_position_exact(
        const geode::Point2D& point, const geode::Triangle2D& triangle )
    {
        const auto& vertices = triangle.vertices();
        const auto s0 = geode::internal::side(
            GEO::PCK::orient_2d( point, vertices[0], vertices[1] ) );
        const auto s1 = geode::internal::side(
            GEO::PCK::orient_2d( point, vertices[1], vertices[2] ) );
        if( s0 != s1 && s0 != geode::SIDE::zero && s1 != geode::SIDE::zero )
        {
            return geode::POSITION::outside;
        }
        const auto s2 = geode::internal::side(
            GEO::PCK::orient_2d( point, vertices[2], vertices[0] ) );
        if( s0 == s1 && s1 == s2 && s2 == geode::SIDE::zero )
        {
            return point_triangle_position_all_zero( point, triangle );
        }
        return geode::internal::point_triangle_position( s0, s1, s2 );
    }

    geode::POSITION point_degenerated_triangle_position_exact(
        const geode::Point3D& point, const geode::Triangle3D& triangle )
    {
        const auto& vertices = triangle.vertices();
        for( const auto e : geode::LRange{ 3 } )
        {
            const geode::Point3D& vertex0 = vertices[e];
            const geode::Point3D& vertex1 = vertices[e == 2 ? 0 : e + 1];
            if( point == vertex0 )
            {
                return geode::POSITION::parallel;
            }
            if( vertex0 != vertex1
                && point_segment_position_exact( point, { vertex0, vertex1 } )
                       != geode::POSITION::outside )
            {
                return geode::POSITION::parallel;
            }
        }
        return geode::POSITION::outside;
    }

    geode::POSITION point_triangle_position_exact(
        const geode::Point3D& point, const geode::Triangle3D& triangle )
    {
        const auto& vertices = triangle.vertices();
        if( GEO::PCK::orient_3d( point, vertices[0], vertices[1], vertices[2] )
            != GEO::zero )
        {
            return geode::POSITION::outside;
        }
        // The point is exactly on the triangle plane: project along the axis
        // where the triangle is the largest (dropping a coordinate is exact)
        const auto normal = geode::Vector3D{ vertices[0], vertices[1] }.cross(
            geode::Vector3D{ vertices[0], vertices[2] } );
        std::array< geode::local_index_t, 3 > axes{ 0, 1, 2 };
        absl::c_stable_sort( axes,
            [&normal]( geode::local_index_t lhs, geode::local_index_t rhs ) {
                return std::fabs( normal.value( lhs ) )
                       > std::fabs( normal.value( rhs ) );
            } );
        for( const auto axis : axes )
        {
            const auto projected0 = vertices[0].get().project_point( axis );
            const auto projected1 = vertices[1].get().project_point( axis );
            const auto projected2 = vertices[2].get().project_point( axis );
            if( GEO::PCK::orient_2d( projected0, projected1, projected2 )
                == GEO::zero )
            {
                continue;
            }
            return point_triangle_position_exact( point.project_point( axis ),
                { projected0, projected1, projected2 } );
        }
        return point_degenerated_triangle_position_exact( point, triangle );
    }

    geode::POSITION point_tetrahedron_position_exact(
        const geode::Point3D& point, const geode::Tetrahedron& tetra )
    {
        std::array< GEO::SIGN, 4 > signs;
        const auto& vertices = tetra.vertices();
        for( const auto f : geode::Range{ 4 } )
        {
            const auto& facet_vertices =
                geode::Tetrahedron::tetrahedron_facet_vertex[f];
            signs[f] = GEO::PCK::orient_3d( vertices[facet_vertices[0]],
                vertices[facet_vertices[1]], vertices[facet_vertices[2]],
                point );
            if( signs[f] == GEO::SIGN::negative )
            {
                return geode::POSITION::outside;
            }
        }
        return geode::internal::point_tetrahedron_position( signs );
    }

} // namespace

namespace geode
{
    POSITION point_tetrahedron_position(
        const Point3D& point, const Tetrahedron& tetra )
    {
        if( !is_finite( point ) )
        {
            return POSITION::outside;
        }
        return point_tetrahedron_position_exact( point, tetra );
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
        // A zero sign here comes from a NaN area (non finite input)
        if( s1 != s2 || s1 == GEO::zero )
        {
            return POSITION::outside;
        }
        const auto area3 =
            triangle_signed_area( { point, vertices[2], vertices[0] } );
        if( std::fabs( area3 ) < GLOBAL_EPSILON )
        {
            return point_triangle_position_exact( point, triangle );
        }
        if( s2 == GEO::geo_sgn( area3 ) )
        {
            return POSITION::inside;
        }
        return POSITION::outside;
    }

    template <>
    POSITION opengeode_geometry_api point_triangle_position(
        const Point3D& point, const Triangle3D& triangle )
    {
        if( !is_finite( point ) )
        {
            return POSITION::outside;
        }
        return point_triangle_position_exact( point, triangle );
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
        if( point == vertices[0].get() || point == vertices[1].get()
            || point == vertices[2].get() )
        {
            return SIDE::zero;
        }
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
        const Point1D&, const Segment1D& );
    template POSITION opengeode_geometry_api point_segment_position(
        const Point2D&, const Segment2D& );
    template POSITION opengeode_geometry_api point_segment_position(
        const Point3D&, const Segment3D& );
} // namespace geode
