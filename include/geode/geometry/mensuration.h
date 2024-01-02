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

#include <geode/geometry/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
    ALIAS_2D_AND_3D( Triangle );
    ALIAS_3D( Vector );

    class Tetrahedron;
} // namespace geode

namespace geode
{
    /*!
     * Compute the (positive) area of a triangle
     * @param[in] triangle Triangle to compute area
     */
    template < index_t dimension >
    double triangle_area( const Triangle< dimension >& triangle );

    /*!
     * Compute the signed area of a triangle
     * @param[in] triangle Triangle to compute area
     * returned area is positive if the triangle vertices are ordered
     * counter-clockwise, negative if clockwise.
     */
    double opengeode_geometry_api triangle_signed_area(
        const Triangle2D& triangle );

    /*!
     * Compute the signed area of a 3D triangle
     * @param[in] triangle Triangle to compute area
     * @param[in] direction Direction used to determined the sign of
     * returned area: positive if the triangle normal is in the same direction
     * than direction, negative if the triangle normal is in the opposition
     * direction.
     */
    double opengeode_geometry_api triangle_signed_area(
        const Triangle3D& triangle, const Vector3D& direction );

    /*!
     * Compute the signed volume of a tetrahedron
     */
    double opengeode_geometry_api tetrahedron_signed_volume(
        const Tetrahedron& tetra );

    /*!
     * Compute the (positive) volume of a tetrahedron
     */
    double opengeode_geometry_api tetrahedron_volume(
        const Tetrahedron& tetra );
} // namespace geode
