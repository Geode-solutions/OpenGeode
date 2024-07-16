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

#include <geode/mesh/io/light_regular_grid_input.hpp>

#include <fstream>
#include <string_view>

#include <absl/strings/str_cat.h>

#include <geode/basic/detail/geode_input_impl.hpp>
#include <geode/basic/factory.hpp>
#include <geode/basic/io.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/core/bitsery_archive.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>

namespace geode
{
    template < index_t dimension >
    LightRegularGrid< dimension > LightRegularGridInput< dimension >::read()
    {
        std::ifstream file{ to_string( this->filename() ),
            std::ifstream::binary };
        OPENGEODE_EXCEPTION( file,
            "[LightRegularGridInput] Failed to open file: ",
            to_string( this->filename() ) );
        TContext context{};
        BitseryExtensions::register_deserialize_pcontext(
            std::get< 0 >( context ) );
        Deserializer archive{ context, file };
        Point< dimension > origin;
        std::array< index_t, dimension > cells_number;
        cells_number.fill( 1 );
        std::array< double, dimension > cells_length;
        cells_length.fill( 1 );
        LightRegularGrid< dimension > grid{ origin, cells_number,
            cells_length };
        archive.object( grid );
        const auto& adapter = archive.adapter();
        OPENGEODE_EXCEPTION( adapter.error() == bitsery::ReaderError::NoError
                                 && adapter.isCompletedSuccessfully()
                                 && std::get< 1 >( context ).isValid(),
            "[Bitsery::read] Error while reading file: ", this->filename() );
        return grid;
    }

    template < index_t dimension >
    LightRegularGrid< dimension > load_light_regular_grid(
        std::string_view filename )
    {
        const auto type = absl::StrCat( "LightRegularGrid", dimension, "D" );
        try
        {
            auto grid = detail::geode_object_input_impl<
                LightRegularGridInputFactory< dimension > >( type, filename );
            Logger::info( type, " has: ", grid.nb_cells(), " cells" );
            return grid;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions<
                LightRegularGridInputFactory< dimension > >( type );
            throw OpenGeodeException{
                "Cannot load LightRegularGrid from file: ", filename
            };
        }
    }

    template < index_t dimension >
    typename LightRegularGridInput< dimension >::MissingFiles
        check_light_regular_grid_missing_files( std::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            LightRegularGridInputFactory< dimension > >( filename );
        return input->check_missing_files();
    }

    template < index_t dimension >
    bool is_light_regular_grid_loadable( std::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            LightRegularGridInputFactory< dimension > >( filename );
        return input->is_loadable();
    }

    template LightRegularGrid< 2 > opengeode_mesh_api load_light_regular_grid(
        std::string_view );
    template LightRegularGrid< 3 > opengeode_mesh_api load_light_regular_grid(
        std::string_view );

    template class opengeode_mesh_api LightRegularGridInput< 2 >;
    template class opengeode_mesh_api LightRegularGridInput< 3 >;

    template LightRegularGridInput< 2 >::MissingFiles opengeode_mesh_api
        check_light_regular_grid_missing_files< 2 >( std::string_view );
    template LightRegularGridInput< 3 >::MissingFiles opengeode_mesh_api
        check_light_regular_grid_missing_files< 3 >( std::string_view );

    template bool opengeode_mesh_api is_light_regular_grid_loadable< 2 >(
        std::string_view );
    template bool opengeode_mesh_api is_light_regular_grid_loadable< 3 >(
        std::string_view );
} // namespace geode
