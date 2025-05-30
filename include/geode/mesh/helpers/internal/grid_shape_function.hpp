/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/grid.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
    FORWARD_DECLARATION_DIMENSION_CLASS( Grid );
} // namespace geode

namespace geode
{
    namespace internal
    {
        [[nodiscard]] bool opengeode_mesh_api local_cell_node_is_on_axis_origin(
            geode::local_index_t node_id, geode::local_index_t axis_id );

        template < index_t dimension >
        [[nodiscard]] double shape_function_value(
            const typename Grid< dimension >::CellIndices& cell_id,
            local_index_t node_id,
            const Point< dimension >& point_in_grid );

        template < index_t dimension >
        [[nodiscard]] double gradient_shape_function_value(
            const typename Grid< dimension >::CellIndices& cell_id,
            local_index_t node_id,
            const Point< dimension >& point_in_grid,
            local_index_t derivative_direction );
    } // namespace internal
} // namespace geode