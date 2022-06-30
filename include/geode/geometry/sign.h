/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
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
