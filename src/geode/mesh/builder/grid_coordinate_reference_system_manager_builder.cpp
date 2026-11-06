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

#include <geode/mesh/builder/grid_coordinate_reference_system_manager_builder.hpp>

#include <geode/geometry/coordinate_system.hpp>

#include <geode/mesh/builder/grid_builder.hpp>
#include <geode/mesh/core/grid_coordinate_reference_system.hpp>
#include <geode/mesh/core/grid_coordinate_reference_system_manager.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>

namespace geode
{
    template < index_t dimension >
    void GridCoordinateReferenceSystemManagerBuilder<
        dimension >::register_coordinate_reference_system( std::string_view
                                                               name,
        std::shared_ptr< GridCoordinateReferenceSystem< dimension > >&& crs )
    {
        grid_
            .modifiable_grid_coordinate_reference_system_manager(
                typename LightRegularGrid<
                    dimension >::GridCRSManagerBuilderKey{} )
            .register_coordinate_reference_system( name, std::move( crs ),
                typename GridCoordinateReferenceSystemManager<
                    dimension >::GridCRSManagerKey{} );
    }

    template < index_t dimension >
    void GridCoordinateReferenceSystemManagerBuilder<
        dimension >::delete_coordinate_reference_system( std::string_view name )
    {
        grid_
            .modifiable_grid_coordinate_reference_system_manager(
                typename LightRegularGrid<
                    dimension >::GridCRSManagerBuilderKey{} )
            .delete_coordinate_reference_system(
                name, typename GridCoordinateReferenceSystemManager<
                          dimension >::GridCRSManagerKey{} );
    }

    template < index_t dimension >
    void GridCoordinateReferenceSystemManagerBuilder< dimension >::
        set_active_coordinate_reference_system( std::string_view name )
    {
        auto& crs_manager =
            grid_.modifiable_grid_coordinate_reference_system_manager(
                typename LightRegularGrid<
                    dimension >::GridCRSManagerBuilderKey{} );
        // Copy before applying: grid modifications update the active CRS
        const auto coordinate_system =
            crs_manager.find_coordinate_reference_system( name )
                .coordinate_system();
        crs_manager.set_active_coordinate_reference_system(
            name, typename GridCoordinateReferenceSystemManager<
                      dimension >::GridCRSManagerKey{} );
        GridBuilder< dimension > builder{ grid_ };
        builder.set_grid_origin( coordinate_system.origin() );
        std::array< Vector< dimension >, dimension > directions;
        for( const auto d : LRange{ dimension } )
        {
            directions[d] = coordinate_system.direction( d );
        }
        builder.set_grid_directions( std::move( directions ) );
    }

    template class opengeode_mesh_api
        GridCoordinateReferenceSystemManagerBuilder< 2 >;
    template class opengeode_mesh_api
        GridCoordinateReferenceSystemManagerBuilder< 3 >;
} // namespace geode
