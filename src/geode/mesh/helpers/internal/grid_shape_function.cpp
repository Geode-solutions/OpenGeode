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

#include <geode/mesh/helpers/internal/grid_shape_function.hpp>

#include <geode/geometry/coordinate_system.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/grid.hpp>

namespace
{
    template < geode::index_t dimension >
    double local_point_value( const geode::Point< dimension >& point_in_grid,
        const typename geode::Grid< dimension >::CellIndices& cell_id,
        geode::local_index_t direction )
    {
        auto local_coord =
            point_in_grid.value( direction ) - cell_id[direction];
        if( local_coord < 0 )
        {
            return 0.;
        }
        else if( local_coord > 1 )
        {
            return 1.;
        }
        return local_coord;
    }
} // namespace

namespace geode
{
    namespace internal
    {
        bool local_cell_node_is_on_axis_origin(
            geode::local_index_t node_id, geode::local_index_t axis_id )
        {
            /* returns ((node_id / pow(2,axis_id)) modulo 2) using binary
             * operators (faster). */
            return ( ( node_id / ( 1 << axis_id ) ) & 1 ) == 0;
        }

        template < index_t dimension >
        double shape_function_value(
            const typename Grid< dimension >::CellIndices& cell_id,
            local_index_t node_id,
            const Point< dimension >& point_in_grid )
        {
            double result{ 1. };
            for( const auto axis : LRange{ dimension } )
            {
                result *= local_cell_node_is_on_axis_origin( node_id, axis )
                              ? 1.
                                    - local_point_value< dimension >(
                                        point_in_grid, cell_id, axis )
                              : local_point_value< dimension >(
                                    point_in_grid, cell_id, axis );
            }
            return result;
        }

        template < index_t dimension >
        double gradient_shape_function_value(
            const typename Grid< dimension >::CellIndices& cell_id,
            local_index_t node_id,
            const Point< dimension >& point_in_grid,
            local_index_t direction )
        {
            double result{ 1. };
            for( const auto axis : LRange{ dimension } )
            {
                if( axis == direction )
                {
                    continue;
                }
                result *= local_cell_node_is_on_axis_origin( node_id, axis )
                              ? 1.
                                    - local_point_value< dimension >(
                                        point_in_grid, cell_id, axis )
                              : local_point_value< dimension >(
                                    point_in_grid, cell_id, axis );
            }
            if( local_cell_node_is_on_axis_origin( node_id, direction ) )
            {
                result *= -1.;
            }
            return result;
        }

        template double opengeode_mesh_api shape_function_value< 2 >(
            const Grid< 2 >::CellIndices&, local_index_t, const Point< 2 >& );
        template double opengeode_mesh_api shape_function_value< 3 >(
            const Grid< 3 >::CellIndices&, local_index_t, const Point< 3 >& );

        template double opengeode_mesh_api gradient_shape_function_value< 2 >(
            const typename Grid< 2 >::CellIndices&,
            local_index_t,
            const Point< 2 >&,
            local_index_t );
        template double opengeode_mesh_api gradient_shape_function_value< 3 >(
            const typename Grid< 3 >::CellIndices&,
            local_index_t,
            const Point< 3 >&,
            local_index_t );
    } // namespace internal
} // namespace geode