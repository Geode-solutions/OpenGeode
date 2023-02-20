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

#include <absl/types/span.h>

#include <geode/basic/attribute.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/grid.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGrid );
} // namespace geode

namespace geode
{
    /*!
     * API function for creating and computing the euclidean distance
     * map from raterized objects.
     * It computes the exact euclidean distance map.
     *
     * @param[in] grid Regular grid on which the euclidean distance map is
     * computed.
     * @param[in] grid_cell_ids Rasterization of every objects from which the
     * distance will be computed.
     * @param[in] distance_map_name Name of the attribute to store the map on
     * the \param grid.
     * @exception OpenGeodeException if the attribute named \param
     * distance_map_name cannot be accessed.
     * @return the created attribute
     */
    template < index_t dimension >
    std::shared_ptr< VariableAttribute< double > > euclidean_distance_transform(
        const RegularGrid< dimension >& grid,
        absl::Span< const typename Grid< dimension >::CellIndices >
            grid_cell_ids,
        absl::string_view distance_map_name );
} // namespace geode