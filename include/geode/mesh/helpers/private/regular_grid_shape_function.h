/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/mesh/common.h>
#include <geode/mesh/core/regular_grid.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
} // namespace geode

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        double shape_function_value( const RegularGrid< dimension >& grid,
            const GridCellIndices< dimension >& cell_id,
            local_index_t node_id,
            const Point< dimension >& point );

        template < index_t dimension >
        GridVertexIndices< dimension > cell_node_index(
            const GridCellIndices< dimension >& cell_id,
            local_index_t node_id );
    } // namespace detail
} // namespace geode