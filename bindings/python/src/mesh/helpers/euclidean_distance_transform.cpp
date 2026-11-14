/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include "../../common.hpp"

#include <geode/mesh/helpers/euclidean_distance_transform.hpp>

namespace geode
{
    template < index_t dimension >
    std::shared_ptr< VariableAttribute< double > >
        checked_euclidean_distance_transform( const Grid< dimension >& grid,
            absl::Span< const typename Grid< dimension >::CellIndices >
                grid_cell_ids,
            std::string_view distance_map_name )
    {
        for( const auto& cell : grid_cell_ids )
        {
            check_grid_indices( "euclidean_distance_transform", "cell index",
                cell, [&grid]( index_t direction ) {
                    return grid.nb_cells_in_direction( direction );
                } );
        }
        pybind11::gil_scoped_release release;
        return euclidean_distance_transform< dimension >(
            grid, grid_cell_ids, distance_map_name );
    }

    void define_euclidean_distance_transform( pybind11::module& module )
    {
        module.def( "euclidean_distance_transform2D",
            &checked_euclidean_distance_transform< 2 >, pybind11::arg( "grid" ),
            pybind11::arg( "grid_cell_ids" ),
            pybind11::arg( "distance_map_name" ),
            "Compute the exact euclidean distance, in the units of the grid, "
            "from every cell of the 2D grid to the nearest cell of "
            "grid_cell_ids, store it in a new cell attribute named "
            "distance_map_name and return the attribute; cells keep the "
            "maximum double value if grid_cell_ids is empty. Raise "
            "IndexError if a cell of grid_cell_ids is out of the grid" );
        module.def( "euclidean_distance_transform3D",
            &checked_euclidean_distance_transform< 3 >, pybind11::arg( "grid" ),
            pybind11::arg( "grid_cell_ids" ),
            pybind11::arg( "distance_map_name" ),
            "Compute the exact euclidean distance, in the units of the grid, "
            "from every cell of the 3D grid to the nearest cell of "
            "grid_cell_ids, store it in a new cell attribute named "
            "distance_map_name and return the attribute; cells keep the "
            "maximum double value if grid_cell_ids is empty. Raise "
            "IndexError if a cell of grid_cell_ids is out of the grid" );
    }
} // namespace geode
