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

#include <geode/mesh/io/regular_grid_output.h>

#include <geode/basic/timer.h>

#include <geode/mesh/core/regular_grid.h>

namespace geode
{
    template < index_t dimension >
    void save_regular_grid( const RegularGrid< dimension >& regular_grid,
        absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                to_string( extension_from_filename( filename ) );
            OPENGEODE_EXCEPTION(
                RegularGridOutputFactory< dimension >::has_creator( extension ),
                "Unknown extension: ", extension );
            RegularGridOutputFactory< dimension >::create( extension, filename )
                ->write( regular_grid );
            Logger::info( "RegularGrid", dimension, "D saved in ", filename,
                " in ", timer.duration() );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot save RegularGrid in file: ",
                filename };
        }
    }

    template void opengeode_mesh_api save_regular_grid(
        const RegularGrid< 2 >&, absl::string_view );
    template void opengeode_mesh_api save_regular_grid(
        const RegularGrid< 3 >&, absl::string_view );
} // namespace geode
