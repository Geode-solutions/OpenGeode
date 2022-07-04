/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/mesh/common.h>
#include <geode/mesh/core/regular_grid.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Segment );
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
} // namespace geode

namespace geode
{
    template < index_t dimension >
    std::vector< GridCellIndices< dimension > > rasterize_segment(
        const RegularGrid< dimension >& grid,
        const Segment< dimension >& segment );

    template < index_t dimension >
    std::vector< GridCellIndices< dimension > > conservative_rasterize_segment(
        const RegularGrid< dimension >& grid,
        const Segment< dimension >& segment );

    template < index_t dimension >
    std::vector< GridCellIndices< dimension > > rasterize_triangle(
        const RegularGrid< dimension >& grid,
        const Triangle< dimension >& triangle );
} // namespace geode
