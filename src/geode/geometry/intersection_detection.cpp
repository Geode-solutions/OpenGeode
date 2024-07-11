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

#include <geode/geometry/intersection_detection.h>

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/bounding_box.h>
#include <geode/geometry/intersection.h>
#include <geode/geometry/point.h>
#include <geode/geometry/position.h>
#include <geode/geometry/private/intersection_from_sides.h>
#include <geode/geometry/private/predicates.h>

namespace
{
    static constexpr std::array< geode::POSITION, 4 > vertex_id_to_position{
        geode::POSITION::vertex0, geode::POSITION::vertex1,
        geode::POSITION::vertex2, geode::POSITION::vertex0
    };

    std::array< geode::local_index_t, 2 > best_projection_axis(
        const geode::Triangle3D& triangle, const geode::Segment3D& segment )
    {
        geode::local_index_t largest_axis{ 2 };
        if( const auto normal = triangle.normal() )
        {
            for( const auto other_axis : geode::LRange{ 2 } )
            {
                if( std::fabs( normal->value( other_axis ) )
                    > std::fabs( normal->value( largest_axis ) ) )
                {
                    largest_axis = other_axis;
                }
            }
        }
        else
        {
            auto bbox = triangle.bounding_box();
            bbox.add_box( segment.bounding_box() );
            const auto diagonal = bbox.diagonal();
            for( const auto other_axis : geode::LRange{ 2 } )
            {
                if( std::fabs( diagonal.value( other_axis ) )
                    < std::fabs( diagonal.value( largest_axis ) ) )
                {
                    largest_axis = other_axis;
                }
            }
        }
        if( largest_axis == 0 )
        {
            return { 1, 2 };
        }
        if( largest_axis == 1 )
        {
            return { 2, 0 };
        }
        return { 0, 1 };
    }

    geode::SegmentTriangleIntersection
        segment_triangle_plane_intersection_detection(
            const geode::Segment3D& segment, const geode::Triangle3D& triangle )
    {
        const auto projection_axis = best_projection_axis( triangle, segment );
        const geode::Point2D segment_proj_p0{
            { segment.vertices()[0].get().value( projection_axis[0] ),
                segment.vertices()[0].get().value( projection_axis[1] ) }
        };
        const geode::Point2D segment_proj_p1{
            { segment.vertices()[1].get().value( projection_axis[0] ),
                segment.vertices()[1].get().value( projection_axis[1] ) }
        };
        const geode::Segment2D segment_projection{ segment_proj_p0,
            segment_proj_p1 };
        std::array< geode::Point2D, 3 > triangle_points_projection;
        for( const auto triangle_pt : geode::LRange{ 3 } )
        {
            triangle_points_projection[triangle_pt] =
                geode::Point2D{ { triangle.vertices()[triangle_pt].get().value(
                                      projection_axis[0] ),
                    triangle.vertices()[triangle_pt].get().value(
                        projection_axis[1] ) } };
        }
        geode::SegmentTriangleIntersection result{ geode::POSITION::outside,
            geode::POSITION::outside };
        for( const auto edge_v0 : geode::LRange{ 3 } )
        {
            const auto seg_edge_inter =
                geode::segment_segment_intersection_detection(
                    segment_projection,
                    { triangle_points_projection[edge_v0],
                        triangle_points_projection[edge_v0 == 2
                                                       ? 0
                                                       : edge_v0 + 1] } );
            if( seg_edge_inter.first == geode::POSITION::outside
                || seg_edge_inter.first == geode::POSITION::parallel )
            {
                continue;
            }
            if( ( seg_edge_inter.first == geode::POSITION::vertex0
                    || seg_edge_inter.first == geode::POSITION::vertex1 )
                && ( seg_edge_inter.second == geode::POSITION::vertex0
                     || seg_edge_inter.second == geode::POSITION::vertex1 ) )
            {
                result.first = seg_edge_inter.first;
                result.second =
                    vertex_id_to_position[seg_edge_inter.second
                                                  == geode::POSITION::vertex0
                                              ? edge_v0
                                              : edge_v0 + 1];
                continue;
            }
            return { geode::POSITION::parallel, geode::POSITION::parallel };
        }
        const geode::Triangle2D triangle_projection{
            triangle_points_projection[0], triangle_points_projection[1],
            triangle_points_projection[2]
        };
        if( geode::point_triangle_position(
                segment_proj_p0, triangle_projection )
                != geode::POSITION::outside
            && geode::point_triangle_position(
                   segment_proj_p1, triangle_projection )
                   != geode::POSITION::outside )
        {
            return { geode::POSITION::parallel, geode::POSITION::parallel };
        }
        return result;
    }
} // namespace

namespace geode
{
    SegmentSegmentIntersection segment_segment_intersection_detection(
        const Segment2D& segment0, const Segment2D& segment1 )
    {
        const auto s0_p0_side =
            point_side_to_segment( segment0.vertices()[0], segment1 );
        const auto s0_p1_side =
            point_side_to_segment( segment0.vertices()[1], segment1 );
        const auto s1_p0_side =
            point_side_to_segment( segment1.vertices()[0], segment0 );
        const auto s1_p1_side =
            point_side_to_segment( segment1.vertices()[1], segment0 );
        if( s0_p0_side == s0_p1_side || s1_p0_side == s1_p1_side )
        {
            if( s0_p0_side == SIDE::zero && s1_p0_side == SIDE::zero )
            {
                return std::make_pair( POSITION::parallel, POSITION::parallel );
            }
            return std::make_pair( POSITION::outside, POSITION::outside );
        }
        if( s0_p0_side == SIDE::zero )
        {
            if( s1_p0_side == SIDE::zero )
            {
                return std::make_pair( POSITION::vertex0, POSITION::vertex0 );
            }
            if( s1_p1_side == SIDE::zero )
            {
                return std::make_pair( POSITION::vertex0, POSITION::vertex1 );
            }
            return std::make_pair( POSITION::vertex0, POSITION::inside );
        }
        if( s0_p1_side == SIDE::zero )
        {
            if( s1_p0_side == SIDE::zero )
            {
                return std::make_pair( POSITION::vertex1, POSITION::vertex0 );
            }
            if( s1_p1_side == SIDE::zero )
            {
                return std::make_pair( POSITION::vertex1, POSITION::vertex1 );
            }
            return std::make_pair( POSITION::vertex1, POSITION::inside );
        }
        if( s1_p0_side == SIDE::zero )
        {
            return std::make_pair( POSITION::inside, POSITION::vertex0 );
        }
        if( s1_p1_side == SIDE::zero )
        {
            return std::make_pair( POSITION::inside, POSITION::vertex1 );
        }
        return std::make_pair( POSITION::inside, POSITION::inside );
    }

    SegmentSegmentIntersection colinear_segment_segment_intersection_detection(
        const Segment2D& segment0, const Segment2D& segment1 )
    {
        OPENGEODE_ASSERT(
            segment_segment_intersection_detection( segment0, segment1 )
                == std::make_pair( POSITION::parallel, POSITION::parallel ),
            "[colinear_segment_segment_intersection_detection] Given segments "
            "are not colinear" );
        const auto s0_p0_position =
            point_segment_position< 2 >( segment0.vertices()[0], segment1 );
        const auto s0_p1_position =
            point_segment_position< 2 >( segment0.vertices()[1], segment1 );
        const auto s1_p0_position =
            point_segment_position< 2 >( segment1.vertices()[0], segment0 );
        const auto s1_p1_position =
            point_segment_position< 2 >( segment1.vertices()[1], segment0 );
        if( s0_p0_position == POSITION::inside
            || s0_p1_position == POSITION::inside
            || s1_p0_position == POSITION::inside
            || s1_p1_position == POSITION::inside )
        {
            return std::make_pair( POSITION::parallel, POSITION::parallel );
        }
        if( s0_p0_position == POSITION::vertex0 )
        {
            if( s0_p1_position == POSITION::outside
                || s0_p1_position == POSITION::vertex0 )
            {
                return std::make_pair( POSITION::vertex0, POSITION::vertex0 );
            }
            else // s0_p1_position == POSITION::vertex1
            {
                return std::make_pair( POSITION::parallel, POSITION::parallel );
            }
        }
        if( s0_p0_position == POSITION::vertex1 )
        {
            if( s0_p1_position == POSITION::outside
                || s0_p1_position == POSITION::vertex1 )
            {
                return std::make_pair( POSITION::vertex0, POSITION::vertex1 );
            }
            else // s0_p1_position == POSITION::vertex0
            {
                return std::make_pair( POSITION::parallel, POSITION::parallel );
            }
        }
        if( s1_p0_position == POSITION::vertex0 )
        {
            if( s1_p1_position == POSITION::outside
                || s1_p1_position == POSITION::vertex0 )
            {
                return std::make_pair( POSITION::vertex0, POSITION::vertex0 );
            }
            else // s1_p1_position == POSITION::vertex1
            {
                return std::make_pair( POSITION::parallel, POSITION::parallel );
            }
        }
        if( s1_p0_position == POSITION::vertex1 )
        {
            if( s1_p1_position == POSITION::outside
                || s1_p1_position == POSITION::vertex1 )
            {
                return std::make_pair( POSITION::vertex1, POSITION::vertex0 );
            }
            else // s1_p1_position == POSITION::vertex0
            {
                return std::make_pair( POSITION::parallel, POSITION::parallel );
            }
        }
        return std::make_pair( POSITION::outside, POSITION::outside );
    }

    POSITION segment_line_intersection_detection(
        const Segment2D& segment, const InfiniteLine2D& line )
    {
        const auto s0_side = point_side_to_line( segment.vertices()[0], line );
        const auto s1_side = point_side_to_line( segment.vertices()[1], line );
        if( s0_side == s1_side )
        {
            if( s0_side == SIDE::zero )
            {
                return POSITION::parallel;
            }
            return POSITION::outside;
        }
        if( s0_side == SIDE::zero )
        {
            return POSITION::vertex0;
        }
        if( s1_side == SIDE::zero )
        {
            return POSITION::vertex1;
        }
        return POSITION::inside;
    }

    POSITION line_triangle_intersection_detection(
        const InfiniteLine3D& line, const Triangle3D& triangle )
    {
        const auto v0_translated =
            triangle.vertices()[0].get() + line.direction();
        if( GEO::PCK::orient_3d( v0_translated, triangle.vertices()[0],
                triangle.vertices()[1], triangle.vertices()[2] )
            == GEO::zero )
        {
            if( point_side_to_triangle( line.origin(), triangle )
                == SIDE::zero )
            {
                return geode::POSITION::parallel;
            }
            return POSITION::outside;
        }

        const auto other = line.origin() + line.direction();
        const auto sign01 = GEO::PCK::orient_3d( line.origin(),
            triangle.vertices()[0], triangle.vertices()[1], other );

        const auto sign12 = GEO::PCK::orient_3d( line.origin(),
            triangle.vertices()[1], triangle.vertices()[2], other );

        const auto sign20 = GEO::PCK::orient_3d( line.origin(),
            triangle.vertices()[2], triangle.vertices()[0], other );

        return detail::triangle_intersection_detection( detail::side( sign01 ),
            detail::side( sign12 ), detail::side( sign20 ) );
    }

    SegmentTriangleIntersection segment_triangle_intersection_detection(
        const Segment3D& segment, const Triangle3D& triangle )
    {
        const auto side0 =
            point_side_to_triangle( segment.vertices()[0], triangle );
        const auto side1 =
            point_side_to_triangle( segment.vertices()[1], triangle );

        if( side0 == side1 )
        {
            if( side0 == SIDE::zero )
            {
                return segment_triangle_plane_intersection_detection(
                    segment, triangle );
            }
            return { POSITION::outside, POSITION::outside };
        }

        const auto sign01 =
            GEO::PCK::orient_3d( segment.vertices()[0], triangle.vertices()[0],
                triangle.vertices()[1], segment.vertices()[1] );

        const auto sign12 =
            GEO::PCK::orient_3d( segment.vertices()[0], triangle.vertices()[1],
                triangle.vertices()[2], segment.vertices()[1] );

        const auto sign20 =
            GEO::PCK::orient_3d( segment.vertices()[0], triangle.vertices()[2],
                triangle.vertices()[0], segment.vertices()[1] );

        const auto triangle_position =
            detail::triangle_intersection_detection( detail::side( sign01 ),
                detail::side( sign12 ), detail::side( sign20 ) );
        if( triangle_position == POSITION::outside )
        {
            return { POSITION::outside, POSITION::outside };
        }
        return { detail::segment_intersection_detection( side0, side1 ),
            triangle_position };
    }

    POSITION segment_plane_intersection_detection(
        const Segment3D& segment, const Plane& plane )
    {
        const auto side0 = point_side_to_plane( segment.vertices()[0], plane );
        const auto side1 = point_side_to_plane( segment.vertices()[1], plane );
        return detail::segment_intersection_detection( side0, side1 );
    }
} // namespace geode
