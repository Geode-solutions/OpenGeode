/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <geode/mesh/helpers/private/regular_grid_shape_function.h>

#include <geode/geometry/point.h>

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
        const geode::RegularGrid< dimension >& grid,
        const geode::Point< dimension >& point,
        const geode::GridCellIndices< dimension >& cell_id )
    {
        auto local_coords = point - grid.origin();
        for( const auto d : geode::LRange{ dimension } )
        {
            local_coords.set_value( d,
                local_coords.value( d ) / grid.cell_length( d ) - cell_id[d] );
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
        double shape_function_value( const RegularGrid< dimension >& grid,
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

        template < index_t dimension >
        GridVertexIndices< dimension > cell_node_index(
            const GridCellIndices< dimension >& cell_id, local_index_t node_id )
        {
            auto node_index = cell_id;
            for( const auto d : LRange{ dimension } )
            {
                if( !node_is_on_axis_origin< dimension >( node_id, d ) )
                {
                    node_index[d] += 1;
                }
            }
            return node_index;
        }

        template double opengeode_mesh_api shape_function_value< 2 >(
            const RegularGrid< 2 >& grid,
            const GridVertexIndices< 2 >& cell_id,
            local_index_t node_id,
            const Point< 2 >& point );
        template GridVertexIndices< 2 > opengeode_mesh_api cell_node_index< 2 >(
            const GridCellIndices< 2 >& cell_id, local_index_t node_id );

        template double opengeode_mesh_api shape_function_value< 3 >(
            const RegularGrid< 3 >& grid,
            const GridVertexIndices< 3 >& cell_id,
            local_index_t node_id,
            const Point< 3 >& point );
        template GridVertexIndices< 3 > opengeode_mesh_api cell_node_index< 3 >(
            const GridCellIndices< 3 >& cell_id, local_index_t node_id );
    } // namespace detail
} // namespace geode