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

#include <geode/mesh/io/light_regular_grid_input.hpp>

#include <fstream>
#include <string_view>

#include <absl/strings/str_cat.h>

#include <geode/basic/detail/geode_input_impl.hpp>
#include <geode/basic/factory.hpp>
#include <geode/basic/io.hpp>
#include <geode/basic/logger.hpp>

#include <geode/mesh/core/light_regular_grid.hpp>

namespace geode
{
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
    AdditionalFiles light_regular_grid_additional_files(
        std::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            LightRegularGridInputFactory< dimension > >( filename );
        return input->additional_files();
    }

    template < index_t dimension >
    Percentage is_light_regular_grid_loadable( std::string_view filename )
    {
        try
        {
            const auto input = detail::geode_object_input_reader<
                LightRegularGridInputFactory< dimension > >( filename );
            return input->is_loadable();
        }
        catch( ... )
        {
            return Percentage{ 0 };
        }
    }

    template < index_t dimension >
    index_t light_regular_grid_object_priority( std::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            LightRegularGridInputFactory< dimension > >( filename );
        return input->object_priority();
    }

    template LightRegularGrid< 2 > opengeode_mesh_api load_light_regular_grid(
        std::string_view );
    template LightRegularGrid< 3 > opengeode_mesh_api load_light_regular_grid(
        std::string_view );

    template class opengeode_mesh_api LightRegularGridInput< 2 >;
    template class opengeode_mesh_api LightRegularGridInput< 3 >;

    template AdditionalFiles opengeode_mesh_api
        light_regular_grid_additional_files< 2 >( std::string_view );
    template AdditionalFiles opengeode_mesh_api
        light_regular_grid_additional_files< 3 >( std::string_view );

    template Percentage opengeode_mesh_api is_light_regular_grid_loadable< 2 >(
        std::string_view );
    template Percentage opengeode_mesh_api is_light_regular_grid_loadable< 3 >(
        std::string_view );

    template index_t opengeode_mesh_api light_regular_grid_object_priority< 2 >(
        std::string_view );
    template index_t opengeode_mesh_api light_regular_grid_object_priority< 3 >(
        std::string_view );
} // namespace geode
