/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/io/regular_grid_input.h>

#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/regular_grid.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< RegularGrid< dimension > > load_regular_grid(
        absl::string_view filename )
    {
        try
        {
            auto input = RegularGridInputFactory< dimension >::create(
                to_string( extension_from_filename( filename ) ), filename );
            input->read();
            auto grid = input->regular_grid();

            Logger::info(
                "RegularGrid", dimension, "D loaded from ", filename );
            Logger::info( "RegularGrid", dimension, "D has: ", grid->nb_cells(),
                " cells" );
            return grid;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot load RegularGrid from file: ",
                filename };
        }
    }

    template < index_t dimension >
    RegularGridInput< dimension >::RegularGridInput(
        absl::string_view filename )
        : Input( filename )
    {
    }

    template std::unique_ptr< RegularGrid< 2 > >
        opengeode_mesh_api load_regular_grid( absl::string_view );
    template std::unique_ptr< RegularGrid< 3 > >
        opengeode_mesh_api load_regular_grid( absl::string_view );

    template class opengeode_mesh_api RegularGridInput< 2 >;
    template class opengeode_mesh_api RegularGridInput< 3 >;
} // namespace geode
