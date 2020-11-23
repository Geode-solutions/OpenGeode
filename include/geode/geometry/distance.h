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

#pragma once

#include <geode/geometry/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( InfiniteLine );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Segment );
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
    FORWARD_DECLARATION_DIMENSION_CLASS( Sphere );
    ALIAS_2D_AND_3D( Point );
    ALIAS_2D_AND_3D( Triangle );
    ALIAS_2D( InfiniteLine );

    class Plane;
    class Tetra;
} // namespace geode

namespace geode
{
    /*!
     * Compute the smallest distance between a point and a segment
     * @return a tuple containing:
     * - the smallest distance.
     * - the closest point on the segment.
     */
    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_segment_distance(
        const Point< dimension >& point, const Segment< dimension >& segment );

    /*!
     * Compute the smallest distance between two segments
     * @return a tuple containing:
     * - the smallest distance.
     * - the closest point on the first segment.
     * - the closest point on the second segment.
     */
    template < index_t dimension >
    std::tuple< double, Point< dimension >, Point< dimension > >
        segment_segment_distance( const Segment< dimension >& segment0,
            const Segment< dimension >& segment1 );

    /*!
     * Compute the smallest distance between a point and an infinite line
     * @return a tuple containing:
     * - the smallest distance.
     * - the closest point on the line.
     */
    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_line_distance(
        const Point< dimension >& point,
        const InfiniteLine< dimension >& line );

    /*!
     * Compute the smallest signed distance between a point and an infinite line
     * in 2D
     * @return a tuple containing:
     * - the smallest signed distance.
     * - the closest point on the line.
     */
    std::tuple< double, Point2D >
        opengeode_geometry_api point_line_signed_distance(
            const Point2D& point, const InfiniteLine2D& line );

    /*!
     * Compute the smallest distance between a point and a triangle
     * @return a tuple containing:
     * - the smallest distance.
     * - the closest point on the triangle.
     */
    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_triangle_distance(
        const Point< dimension >& point,
        const Triangle< dimension >& triangle );

    /*!
     * Compute the signed distance between a point and a triangle
     * @return a tuple containing:
     * - the smallest distance.
     * - the closest point on the triangle.
     * @details the sign is given by the triangle normal (positive if in the
     * same plane).
     */
    std::tuple< double, Point3D > point_triangle_signed_distance(
        const Point3D& point, const Triangle3D& triangle );

    /*!
     * Compute the distance between a point and a tetrahedron
     * @return a tuple containing:
     * - the smallest distance.
     * - the nearest point on the tetrahedron.
     */
    std::tuple< double, Point3D > opengeode_geometry_api point_tetra_distance(
        const Point3D& point, const Tetra& tetra );

    /*!
     * Compute the distance between a point and a plane
     * @return a tuple containing:
     * - the smallest distance.
     * - the nearest point on the plane.
     */
    std::tuple< double, Point3D > opengeode_geometry_api point_plane_distance(
        const Point3D& point, const Plane& plane );

    /*!
     * Compute the signed distance between a point and a plane
     * @return a tuple containing:
     * - the signed distance (sign is given by the plane normal direction).
     * - the nearest point on the plane.
     */
    std::tuple< double, Point3D > opengeode_geometry_api
        point_plane_signed_distance( const Point3D& point, const Plane& plane );

    /*!
     * Compute the smallest distance between a point and a sphere (circle in 2D)
     * @return a tuple containing:
     * - the smallest distance.
     * - the closest point on the sphere.
     */
    template < index_t dimension >
    std::tuple< double, Point< dimension > > point_sphere_distance(
        const Point< dimension >& point, const Sphere< dimension >& sphere );
} // namespace geode
