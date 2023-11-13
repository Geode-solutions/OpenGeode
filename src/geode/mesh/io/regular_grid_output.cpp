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

#include <geode/mesh/io/regular_grid_output.h>

#include <absl/strings/str_cat.h>
#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_output_impl.h>
#include <geode/basic/io.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/regular_grid_solid.h>
#include <geode/mesh/core/regular_grid_surface.h>
#include <geode/mesh/io/vertex_set_output.h>

namespace geode
{
    template < index_t dimension >
    void save_regular_grid( const RegularGrid< dimension >& regular_grid,
        absl::string_view filename )
    {
        const auto type = absl::StrCat( "RegularGrid", dimension, "D" );
        try
        {
            detail::geode_object_output_impl<
                RegularGridOutputFactory< dimension > >(
                type, regular_grid, filename );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< RegularGridOutputFactory< dimension > >(
                type );
            Logger::info( "Other extensions are available in parent clases." );
            print_available_extensions< VertexSetOutputFactory >( "VertexSet" );
            throw OpenGeodeException{ "Cannot save RegularGrid in file: ",
                filename };
        }
    }

    template < index_t dimension >
    bool is_regular_grid_saveable( const RegularGrid< dimension >& regular_grid,
        absl::string_view filename )
    {
        const auto output = detail::geode_object_output_writer<
            RegularGridOutputFactory< dimension > >( filename );
        return output->is_saveable( regular_grid );
    }

    template void opengeode_mesh_api save_regular_grid(
        const RegularGrid< 2 >&, absl::string_view );
    template void opengeode_mesh_api save_regular_grid(
        const RegularGrid< 3 >&, absl::string_view );

    template bool opengeode_mesh_api is_regular_grid_saveable(
        const RegularGrid< 2 >&, absl::string_view );
    template bool opengeode_mesh_api is_regular_grid_saveable(
        const RegularGrid< 3 >&, absl::string_view );
} // namespace geode
