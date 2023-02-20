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
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
    ALIAS_2D_AND_3D( Triangle );
    class Tetrahedron;
    enum struct Side;
    using Sign = Side;
} // namespace geode

namespace geode
{
    /*!
     * Return the sign of a tetrahedron volume.
     */
    Sign opengeode_geometry_api tetrahedron_volume_sign(
        const Tetrahedron& tetra );

    /*!
     * Return the sign of a 2D triangle area.
     */
    Sign opengeode_geometry_api triangle_area_sign(
        const Triangle2D& triangle );

    /*!
     * Return the sign of a 3D triangle area aligned on X- Y- or Z-axis.
     */
    Sign opengeode_geometry_api triangle_area_sign(
        const Triangle3D& triangle, local_index_t axis );
} // namespace geode
