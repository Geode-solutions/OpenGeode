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

#include <geode/mesh/helpers/private/regular_grid_shape_function.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/grid.h>

namespace
{
    template < geode::index_t dimension >
    bool node_is_on_axis_origin(
        geode::local_index_t node_id, geode::local_index_t axis_id )
    {
        /* returns ((node_id / pow(2,axis_id)) modulo 2) using binary
         * operators (faster). */
        return ( ( node_id / ( 1 << axis_id ) ) & 1 ) == 0;
    }

    template < geode::index_t dimension >
    geode::Point< dimension > local_point_coordinates(
        const geode::Grid< dimension >& grid,
        const geode::Point< dimension >& point,
        const geode::GridCellIndices< dimension >& cell_id )
    {
        auto local_coords = point - grid.origin();
        for( const auto d : geode::LRange{ dimension } )
        {
            local_coords.set_value(
                d, local_coords.value( d ) / grid.cell_length_in_direction( d )
                       - cell_id[d] );
            if( local_coords.value( d ) < 0 )
            {
                local_coords.set_value( d, 0 );
            }
            else if( local_coords.value( d ) > 1 )
            {
                local_coords.set_value( d, 1 );
            }
        }
        return local_coords;
    }
} // namespace

namespace geode
{
    namespace detail
    {
        template < index_t dimension >
        double shape_function_value( const Grid< dimension >& grid,
            const GridCellIndices< dimension >& cell_id,
            local_index_t node_id,
            const Point< dimension >& point )
        {
            const auto local_coords =
                local_point_coordinates< dimension >( grid, point, cell_id );
            double shape_function_value{ 1. };
            for( const auto d : LRange{ dimension } )
            {
                if( node_is_on_axis_origin< dimension >( node_id, d ) )
                {
                    shape_function_value *= 1 - local_coords.value( d );
                }
                else
                {
                    shape_function_value *= local_coords.value( d );
                }
            }
            return shape_function_value;
        }

        template double opengeode_mesh_api shape_function_value< 2 >(
            const Grid< 2 >& grid,
            const GridCellIndices< 2 >& cell_id,
            local_index_t node_id,
            const Point< 2 >& point );

        template double opengeode_mesh_api shape_function_value< 3 >(
            const Grid< 3 >& grid,
            const GridCellIndices< 3 >& cell_id,
            local_index_t node_id,
            const Point< 3 >& point );
    } // namespace detail
} // namespace geode