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

#include <geode/mesh/helpers/convert_grid.hpp>

#include <geode/basic/identifier_builder.hpp>

#include <geode/geometry/coordinate_system.hpp>

#include <geode/image/core/raster_image.hpp>
#include <geode/image/core/rgb_color.hpp>

#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/helpers/internal/copy.hpp>

namespace geode
{
    template < index_t dimension >
    LightRegularGrid< dimension > convert_raster_image_into_grid(
        const RasterImage< dimension >& raster,
        const CoordinateSystem< dimension >& coordinate_system )
    {
        std::array< index_t, dimension > cells_number;
        std::array< Vector< dimension >, dimension > cells_directions;
        for( const auto dim : geode::LRange{ dimension } )
        {
            cells_number[dim] = raster.nb_cells_in_direction( dim );
            cells_directions[dim] = coordinate_system.direction( dim );
        }
        LightRegularGrid< dimension > grid{ coordinate_system.origin(),
            cells_number, cells_directions };
        IdentifierBuilder builder{ grid };
        internal::copy_meta_info( raster, builder );
        auto color = grid.cell_attribute_manager()
                         .template find_or_create_attribute< VariableAttribute,
                             RGBColor >( "RGB_data", {} );
        for( const auto cell_id : geode::Range{ raster.nb_cells() } )
        {
            color->set_value( cell_id, raster.color( cell_id ) );
        }
        return grid;
    }

    template LightRegularGrid< 2 >
        opengeode_mesh_api convert_raster_image_into_grid(
            const RasterImage< 2 >&, const CoordinateSystem< 2 >& );
    template LightRegularGrid< 3 >
        opengeode_mesh_api convert_raster_image_into_grid(
            const RasterImage< 3 >&, const CoordinateSystem< 3 >& );
} // namespace geode