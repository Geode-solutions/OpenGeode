/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geometry/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( InfiniteLine );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Segment );
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
    ALIAS_2D( InfiniteLine );
    ALIAS_2D_AND_3D( Point );
    ALIAS_2D_AND_3D( Segment );
    ALIAS_3D( Triangle );
    class Plane;
    class Tetrahedron;
    enum struct Position;
    enum struct Side;
} // namespace geode

namespace geode
{
    /*!
     * Return the point side to a segment.
     */
    Side opengeode_geometry_api point_side_to_segment(
        const Point2D& point, const Segment2D& segment );

    /*!
     * Return the point side to a line.
     */
    Side opengeode_geometry_api point_side_to_line(
        const Point2D& point, const InfiniteLine2D& line );

    /*!
     * Return the point side to a plane.
     */
    Side opengeode_geometry_api point_side_to_plane(
        const Point3D& point, const Plane& plane );

    /*!
     * Return the point side to a 3D triangle.
     */
    Side opengeode_geometry_api point_side_to_triangle(
        const Point3D& point, const Triangle3D& triangle );

    /*!
     * Return the position of a point on a segment: inside, outside or on
     * segment vertex.
     */
    template < index_t dimension >
    Position point_segment_position(
        const Point< dimension >& point, const Segment< dimension >& segment );

    /*!
     * Return the position of a point in a triangle: inside, outside, on a
     * triangle vertex or an edge.
     */
    template < index_t dimension >
    Position point_triangle_position( const Point< dimension >& point,
        const Triangle< dimension >& triangle );

    /*!
     * Return the position of a point in a tetrahedron: inside, outside, on a
     * tetra vertex, an edge or a facet.
     */
    Position opengeode_geometry_api point_tetrahedron_position(
        const Point3D& point, const Tetrahedron& tetra );

    /*!
     * Return true if the three points are exactly aligned.
     */
    template < index_t dimension >
    bool are_points_aligned( const Point< dimension >& point0,
        const Point< dimension >& point1,
        const Point< dimension >& point2 );
} // namespace geode
