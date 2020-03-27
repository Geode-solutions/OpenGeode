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

#include <geode/geometry/point.h>

#include <geode/geometry/basic_objects.h>
#include <geode/geometry/common.h>

namespace geode
{
    /*!
     * Return the projection of a point on a segment
     * @param[in] point the point to project
     * @param[in] segment the segment
     */
    template < index_t dimension >
    Point< dimension > point_segment_projection(
        const Point< dimension >& point, const Segment< dimension >& segment );

    /*!
     * Return the projection of a point on a line
     * @param[in] point the point to project
     * @param[in] line the line
     */
    template < index_t dimension >
    Point< dimension > point_line_projection( const Point< dimension >& point,
        const InfiniteLine< dimension >& line );

    /*!
     * Return the projection of a point on a triangle
     * @param[in] point the point to project
     * @param[in] segment the segment
     */
    template < index_t dimension >
    Point< dimension > point_triangle_projection(
        const Point< dimension >& point,
        const Triangle< dimension >& triangle );

} // namespace geode
