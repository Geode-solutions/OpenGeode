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

#include <geode/mesh/helpers/build_grid.h>

#include <geode/geometry/bounding_box.h>

namespace geode
{
    template < index_t dimension >
    LightRegularGrid< dimension >
        build_grid_from_bbox_target_length_and_maximum_cell_number(
            const BoundingBox< dimension >& bbox,
            double target_cell_length,
            index_t max_nb_cells )
    {
        const auto diagonal = bbox.max() - bbox.min();
        double numerator{ 1 };
        for( const auto d : LRange{ dimension } )
        {
            numerator *= diagonal.value( d );
        }
        const auto min_cell_length =
            std::pow( numerator / max_nb_cells, 1. / dimension );
        const auto cell_length =
            std::max( min_cell_length, target_cell_length );
        std::array< index_t, dimension > cell_numbers;
        std::array< double, dimension > cell_lengths;
        for( const auto d : LRange{ dimension } )
        {
            cell_numbers[d] = std::max( static_cast< index_t >( 1 ),
                static_cast< index_t >(
                    std::floor( diagonal.value( d ) / cell_length ) ) );
            cell_lengths[d] = diagonal.value( d ) / cell_numbers[d];
        }
        return { bbox.min(), std::move( cell_numbers ),
            std::move( cell_lengths ) };
    }

    template LightRegularGrid2D opengeode_mesh_api
        build_grid_from_bbox_target_length_and_maximum_cell_number(
            const BoundingBox2D&, double, index_t );
    template LightRegularGrid3D opengeode_mesh_api
        build_grid_from_bbox_target_length_and_maximum_cell_number(
            const BoundingBox3D&, double, index_t );
} // namespace geode