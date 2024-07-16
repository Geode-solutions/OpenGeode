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

#include <geode/mesh/io/light_regular_grid_output.hpp>

#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include <absl/strings/str_cat.h>

#include <geode/basic/detail/geode_output_impl.hpp>
#include <geode/basic/io.hpp>
#include <geode/basic/logger.hpp>

#include <geode/mesh/core/light_regular_grid.hpp>

namespace geode
{
    template < index_t dimension >
    std::vector< std::string > LightRegularGridOutput< dimension >::write(
        const LightRegularGrid< dimension >& grid ) const
    {
        std::ofstream file{ to_string( this->filename() ),
            std::ofstream::binary };
        TContext context{};
        BitseryExtensions::register_serialize_pcontext(
            std::get< 0 >( context ) );
        Serializer archive{ context, file };
        archive.object( grid );
        archive.adapter().flush();
        OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
            "[Bitsery::write] Error while writing file: ", this->filename() );
        return { to_string( this->filename() ) };
    }

    template < index_t dimension >
    std::vector< std::string > save_light_regular_grid(
        const LightRegularGrid< dimension >& light_regular_grid,
        std::string_view filename )
    {
        const auto type = absl::StrCat( "LightRegularGrid", dimension, "D" );
        try
        {
            return detail::geode_object_output_impl<
                LightRegularGridOutputFactory< dimension > >(
                type, light_regular_grid, filename );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions<
                LightRegularGridOutputFactory< dimension > >( type );
            throw OpenGeodeException{ "Cannot save LightRegularGrid in file: ",
                filename };
        }
    }

    template < index_t dimension >
    bool is_light_regular_grid_saveable(
        const LightRegularGrid< dimension >& light_regular_grid,
        std::string_view filename )
    {
        const auto output = detail::geode_object_output_writer<
            LightRegularGridOutputFactory< dimension > >( filename );
        return output->is_saveable( light_regular_grid );
    }

    template std::vector< std::string >
        opengeode_mesh_api save_light_regular_grid(
            const LightRegularGrid< 2 >&, std::string_view );
    template std::vector< std::string >
        opengeode_mesh_api save_light_regular_grid(
            const LightRegularGrid< 3 >&, std::string_view );

    template class opengeode_mesh_api LightRegularGridOutput< 2 >;
    template class opengeode_mesh_api LightRegularGridOutput< 3 >;

    template bool opengeode_mesh_api is_light_regular_grid_saveable(
        const LightRegularGrid< 2 >&, std::string_view );
    template bool opengeode_mesh_api is_light_regular_grid_saveable(
        const LightRegularGrid< 3 >&, std::string_view );
} // namespace geode
