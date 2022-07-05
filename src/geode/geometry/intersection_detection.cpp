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

#include <geode/geometry/intersection_detection.h>

#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/intersection.h>
#include <geode/geometry/position.h>
#include <geode/geometry/private/intersection_from_sides.h>
#include <geode/geometry/private/predicates.h>

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
            if( s0_p0_side == Side::zero && s1_p0_side == Side::zero )
            {
                return std::make_pair( Position::parallel, Position::parallel );
            }
            return std::make_pair( Position::outside, Position::outside );
        }
        if( s0_p0_side == Side::zero )
        {
            if( s1_p0_side == Side::zero )
            {
                return std::make_pair( Position::vertex0, Position::vertex0 );
            }
            if( s1_p1_side == Side::zero )
            {
                return std::make_pair( Position::vertex0, Position::vertex1 );
            }
            return std::make_pair( Position::vertex0, Position::inside );
        }
        if( s0_p1_side == Side::zero )
        {
            if( s1_p0_side == Side::zero )
            {
                return std::make_pair( Position::vertex1, Position::vertex0 );
            }
            if( s1_p1_side == Side::zero )
            {
                return std::make_pair( Position::vertex1, Position::vertex1 );
            }
            return std::make_pair( Position::vertex1, Position::inside );
        }
        if( s1_p0_side == Side::zero )
        {
            return std::make_pair( Position::inside, Position::vertex0 );
        }
        if( s1_p1_side == Side::zero )
        {
            return std::make_pair( Position::inside, Position::vertex1 );
        }
        return std::make_pair( Position::inside, Position::inside );
    }

    SegmentSegmentIntersection colinear_segment_segment_intersection_detection(
        const Segment2D& segment0, const Segment2D& segment1 )
    {
        OPENGEODE_ASSERT(
            segment_segment_intersection_detection( segment0, segment1 )
                == std::make_pair( Position::parallel, Position::parallel ),
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
        if( s0_p0_position == Position::inside
            || s0_p1_position == Position::inside
            || s1_p0_position == Position::inside
            || s1_p1_position == Position::inside )
        {
            return std::make_pair( Position::parallel, Position::parallel );
        }
        if( s0_p0_position == Position::vertex0 )
        {
            if( s0_p1_position == Position::outside
                || s0_p1_position == Position::vertex0 )
            {
                return std::make_pair( Position::vertex0, Position::vertex0 );
            }
            else // s0_p1_position == Position::vertex1
            {
                return std::make_pair( Position::parallel, Position::parallel );
            }
        }
        if( s0_p0_position == Position::vertex1 )
        {
            if( s0_p1_position == Position::outside
                || s0_p1_position == Position::vertex1 )
            {
                return std::make_pair( Position::vertex0, Position::vertex1 );
            }
            else // s0_p1_position == Position::vertex0
            {
                return std::make_pair( Position::parallel, Position::parallel );
            }
        }
        if( s1_p0_position == Position::vertex0 )
        {
            if( s1_p1_position == Position::outside
                || s1_p1_position == Position::vertex0 )
            {
                return std::make_pair( Position::vertex0, Position::vertex0 );
            }
            else // s1_p1_position == Position::vertex1
            {
                return std::make_pair( Position::parallel, Position::parallel );
            }
        }
        if( s1_p0_position == Position::vertex1 )
        {
            if( s1_p1_position == Position::outside
                || s1_p1_position == Position::vertex1 )
            {
                return std::make_pair( Position::vertex1, Position::vertex0 );
            }
            else // s1_p1_position == Position::vertex0
            {
                return std::make_pair( Position::parallel, Position::parallel );
            }
        }
        return std::make_pair( Position::outside, Position::outside );
    }

    Position segment_line_intersection_detection(
        const Segment2D& segment, const InfiniteLine2D& line )
    {
        const auto s0_side = point_side_to_line( segment.vertices()[0], line );
        const auto s1_side = point_side_to_line( segment.vertices()[1], line );
        if( s0_side == s1_side )
        {
            if( s0_side == Side::zero )
            {
                return Position::parallel;
            }
            return Position::outside;
        }
        if( s0_side == Side::zero )
        {
            return Position::vertex0;
        }
        if( s1_side == Side::zero )
        {
            return Position::vertex1;
        }
        return Position::inside;
    }

    Position line_triangle_intersection_detection(
        const InfiniteLine3D& line, const Triangle3D& triangle )
    {
        const auto v0_translated =
            triangle.vertices()[0].get() + line.direction();
        if( GEO::PCK::orient_3d( v0_translated, triangle.vertices()[0],
                triangle.vertices()[1], triangle.vertices()[2] )
            == GEO::ZERO )
        {
            if( point_side_to_triangle( line.origin(), triangle )
                == Side::zero )
            {
                return geode::Position::parallel;
            }
            return Position::outside;
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
            if( side0 == Side::zero )
            {
                return { Position::parallel, Position::parallel };
            }
            return { Position::outside, Position::outside };
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
        if( triangle_position == Position::outside )
        {
            return { Position::outside, Position::outside };
        }
        return { detail::segment_intersection_detection( side0, side1 ),
            triangle_position };
    }

    Position segment_plane_intersection_detection(
        const Segment3D& segment, const Plane& plane )
    {
        const auto side0 = point_side_to_plane( segment.vertices()[0], plane );
        const auto side1 = point_side_to_plane( segment.vertices()[1], plane );
        return detail::segment_intersection_detection( side0, side1 );
    }
} // namespace geode
