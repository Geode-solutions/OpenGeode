/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Segment );
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
    ALIAS_3D( Point );

    class Tetra;
} // namespace geode

namespace geode
{
    /*!
     * Compute the barycentric coordinates of a point with regards to a tetra.
     * @param[in] point the query point.
     * @param[in] tetra the tetra.
     * @return an array containing the parametric coordinates corresponding to
     * the tetra vertices.
     */
    std::array< double, 4 >
        opengeode_geometry_api tetra_barycentric_coordinates(
            const Point3D& point, const Tetra& tetra );

    /*!
     * Compute barycentric coordinates of a point with regards to a triangle.
     * @param[in] point the query point.
     * @param[in] triangle the triangle.
     * @return an array containing the parametric coordinates corresponding to
     * the triangle vertices.
     */
    template < index_t dimension >
    std::array< double, 3 > triangle_barycentric_coordinates(
        const Point< dimension >& point,
        const Triangle< dimension >& triangle );

    /*!
     * Compute barycentric coordinates of a point with regards to a segment.
     * @param[in] point the query point.
     * @param[in] segment the segment.
     * @return an array containing the parametric coordinates corresponding to
     * the segment vertices.
     */
    template < index_t dimension >
    std::array< double, 2 > segment_barycentric_coordinates(
        const Point< dimension >& point, const Segment< dimension >& segment );

} // namespace geode
