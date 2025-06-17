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

#include <geode/mesh/io/regular_grid_input.hpp>

#include <string_view>

#include <absl/strings/str_cat.h>

#include <geode/basic/detail/geode_input_impl.hpp>
#include <geode/basic/io.hpp>
#include <geode/basic/logger.hpp>

#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>
#include <geode/mesh/io/vertex_set_input.hpp>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< RegularGrid< dimension > > load_regular_grid(
        const MeshImpl& impl, std::string_view filename )
    {
        const auto type = absl::StrCat( "RegularGrid", dimension, "D" );
        try
        {
            auto grid = detail::geode_object_input_impl<
                RegularGridInputFactory< dimension > >( type, filename, impl );
            Logger::info( type, " has: ", grid->nb_cells(), " cells" );
            return grid;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< RegularGridInputFactory< dimension > >(
                type );
            Logger::info( "Other extensions are available in parent classes." );
            print_available_extensions< VertexSetInputFactory >( "VertexSet" );
            throw OpenGeodeException{ "Cannot load RegularGrid from file: ",
                filename };
        }
    }

    template < index_t dimension >
    std::unique_ptr< RegularGrid< dimension > > load_regular_grid(
        std::string_view filename )
    {
        return load_regular_grid< dimension >(
            MeshFactory::default_impl(
                RegularGrid< dimension >::type_name_static() ),
            filename );
    }

    template < index_t dimension >
    typename RegularGridInput< dimension >::MissingFiles
        check_regular_grid_missing_files( std::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            RegularGridInputFactory< dimension > >( filename );
        return input->check_missing_files();
    }

    template < index_t dimension >
    bool is_regular_grid_loadable( std::string_view filename )
    {
        try
        {
            const auto input = detail::geode_object_input_reader<
                RegularGridInputFactory< dimension > >( filename );
            return input->is_loadable();
        }
        catch( ... )
        {
            return false;
        }
    }

    template std::unique_ptr< RegularGrid< 2 > > opengeode_mesh_api
        load_regular_grid( const MeshImpl&, std::string_view );
    template std::unique_ptr< RegularGrid< 3 > > opengeode_mesh_api
        load_regular_grid( const MeshImpl&, std::string_view );

    template std::unique_ptr< RegularGrid< 2 > >
        opengeode_mesh_api load_regular_grid( std::string_view );
    template std::unique_ptr< RegularGrid< 3 > >
        opengeode_mesh_api load_regular_grid( std::string_view );

    template RegularGridInput< 2 >::MissingFiles opengeode_mesh_api
        check_regular_grid_missing_files< 2 >( std::string_view );
    template RegularGridInput< 3 >::MissingFiles opengeode_mesh_api
        check_regular_grid_missing_files< 3 >( std::string_view );

    template bool opengeode_mesh_api is_regular_grid_loadable< 2 >(
        std::string_view );
    template bool opengeode_mesh_api is_regular_grid_loadable< 3 >(
        std::string_view );
} // namespace geode
