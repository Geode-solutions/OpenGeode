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

#pragma once

#include <optional>

#include <absl/container/inlined_vector.h>

#include <geode/geometry/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( InfiniteLine );
    FORWARD_DECLARATION_DIMENSION_CLASS( OwnerInfiniteLine );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Segment );
    FORWARD_DECLARATION_DIMENSION_CLASS( Sphere );
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
    ALIAS_2D_AND_3D( InfiniteLine );
    ALIAS_2D_AND_3D( OwnerInfiniteLine );
    ALIAS_2D_AND_3D( Point );
    ALIAS_2D_AND_3D( Segment );
    ALIAS_3D( Triangle );
    class Circle;
    class Cylinder;
    class Plane;
} // namespace geode

namespace geode
{
    enum struct INTERSECTION_TYPE
    {
        none,
        intersect,
        parallel,
        incorrect
    };

    template < typename Intersection >
    struct CorrectnessInfo
    {
        using Correctness = std::pair< bool, Intersection >;

        CorrectnessInfo(
            Correctness& first_correctness, Correctness& second_correctness )
            : first{ std::move( first_correctness ) },
              second{ std::move( second_correctness ) }
        {
        }

        Correctness first;
        Correctness second;
    };

    template < typename Intersection >
    struct IntersectionResult
    {
        IntersectionResult( Intersection intersection,
            CorrectnessInfo< Intersection > correctness_info )
            : result( std::move( intersection ) ),
              type( INTERSECTION_TYPE::intersect ),
              correctness( std::move( correctness_info ) )
        {
            if( !correctness->first.first || !correctness->second.first )
            {
                type = INTERSECTION_TYPE::incorrect;
            }
        }
        IntersectionResult( INTERSECTION_TYPE intersection_type )
            : type( intersection_type )
        {
        }

        [[nodiscard]] bool has_intersection() const
        {
            return type == INTERSECTION_TYPE::intersect;
        }

        operator bool() const
        {
            return has_intersection();
        }

        std::optional< Intersection > result;
        INTERSECTION_TYPE type;
        std::optional< CorrectnessInfo< Intersection > > correctness;
    };

    /*!
     * Compute the intersection between a plane and an infinite line
     * @return an optional of the intersection.
     */
    [[nodiscard]] IntersectionResult< Point3D >
        opengeode_geometry_api line_plane_intersection(
            const InfiniteLine3D& line, const Plane& plane );

    /*!
     * Compute the intersection(s) between a sphere and an infinite line
     * @return an optional of the intersection points.
     */
    template < index_t dimension >
    [[nodiscard]] IntersectionResult<
        absl::InlinedVector< Point< dimension >, 2 > >
        line_sphere_intersection( const InfiniteLine< dimension >& line,
            const Sphere< dimension >& sphere );

    /*!
     * Compute the intersection(s) between a (n-1)-sphere and a segment in
     * n-dimension space.
     * @return an optional of the intersection points.
     */
    template < index_t dimension >
    [[nodiscard]] IntersectionResult<
        absl::InlinedVector< Point< dimension >, 2 > >
        segment_sphere_intersection( const Segment< dimension >& segment,
            const Sphere< dimension >& sphere );

    /*!
     * Compute the intersection between a plane and a segment
     * @return an optional of the intersection point.
     * @warning if the segment is included in the plane nothing is returned
     */
    [[nodiscard]] IntersectionResult< Point3D >
        opengeode_geometry_api segment_plane_intersection(
            const Segment3D& segment, const Plane& plane );

    /*!
     * Compute the intersection of a segment and a triangle
     * @return an optional of the intersection point.
     * @warning if the segment is included in the triangle plane nothing is
     * returned
     */
    [[nodiscard]] IntersectionResult< Point3D >
        opengeode_geometry_api segment_triangle_intersection(
            const Segment3D& segment, const Triangle3D& triangle );

    /*!
     * Compute the intersection of a line and a triangle
     * @return an optional of the intersection point.
     * @warning if the segment is included in the triangle plane nothing is
     * returned
     */
    [[nodiscard]] IntersectionResult< Point3D >
        opengeode_geometry_api line_triangle_intersection(
            const InfiniteLine3D& line, const Triangle3D& triangle );

    /*!
     * Compute the intersection between two infinite lines
     * @return an optional of the intersection point.
     */
    [[nodiscard]] IntersectionResult< Point2D >
        opengeode_geometry_api line_line_intersection(
            const InfiniteLine2D& line0, const InfiniteLine2D& line1 );

    /*!
     * Compute the intersection between two segments
     * @return an optional of the intersection point.
     */
    [[nodiscard]] IntersectionResult< Point2D >
        opengeode_geometry_api segment_segment_intersection(
            const Segment2D& segment0, const Segment2D& segment1 );

    /*!
     * Compute the intersection between a segment and an infinite line
     * @return an optional of the intersection point.
     */
    [[nodiscard]] IntersectionResult< Point2D >
        opengeode_geometry_api segment_line_intersection(
            const Segment2D& segment, const InfiniteLine2D& line );

    /*!
     * Compute the intersection between a segment and a cylinder
     * @return an optional of the intersection points.
     */
    [[nodiscard]] IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api segment_cylinder_intersection(
            const Segment3D& segment, const Cylinder& cylinder );

    /*!
     * Compute the intersection between a line and a cylinder
     * @return an optional of the intersection points.
     */
    [[nodiscard]] IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api line_cylinder_intersection(
            const InfiniteLine3D& line, const Cylinder& cylinder );

    /*!
     * Compute the intersection between a triangle and a circle
     * @return an optional of the intersection points.
     */
    [[nodiscard]] IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api triangle_circle_intersection(
            const Triangle3D& triangle, const Circle& circle );

    /*!
     * Compute the intersection between a plane and a circle
     * @return an optional of the intersection points.
     */
    [[nodiscard]] IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api plane_circle_intersection(
            const Plane& plane, const Circle& circle );

    /*!
     * Compute the intersection between two planes
     * @return an optional of the intersection line.
     */
    [[nodiscard]] IntersectionResult< OwnerInfiniteLine3D >
        opengeode_geometry_api plane_plane_intersection(
            const Plane& plane0, const Plane& plane1 );
} // namespace geode