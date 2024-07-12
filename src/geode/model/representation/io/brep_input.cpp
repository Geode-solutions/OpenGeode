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

#include <geode/model/representation/io/brep_input.h>

#include <string_view>

#include <absl/strings/str_cat.h>

#include <geode/basic/detail/geode_input_impl.h>
#include <geode/basic/io.h>
#include <geode/basic/logger.h>

#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    BRep load_brep( std::string_view filename )
    {
        constexpr auto TYPE = "BRep";
        try
        {
            auto brep = detail::geode_object_input_impl< BRepInputFactory >(
                TYPE, filename );
            auto message = absl::StrCat( TYPE, " has: " );
            detail::add_to_message( message, brep.nb_blocks(), " Blocks, " );
            detail::add_to_message(
                message, brep.nb_surfaces(), " Surfaces, " );
            detail::add_to_message( message, brep.nb_lines(), " Lines, " );
            detail::add_to_message( message, brep.nb_corners(), " Corners, " );
            detail::add_to_message(
                message, brep.nb_model_boundaries(), " ModelBoundaries," );
            detail::add_to_message(
                message, brep.nb_corner_collections(), " CornerCollections," );
            detail::add_to_message(
                message, brep.nb_line_collections(), " LineCollections," );
            detail::add_to_message( message, brep.nb_surface_collections(),
                " SurfaceCollections," );
            detail::add_to_message(
                message, brep.nb_block_collections(), " BlockCollections," );
            Logger::info( message );
            return brep;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< BRepInputFactory >( TYPE );
            throw OpenGeodeException{ "Cannot load BRep from file: ",
                filename };
        }
    }

    typename BRepInput::MissingFiles check_brep_missing_files(
        std::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< BRepInputFactory >( filename );
        return input->check_missing_files();
    }

    bool is_brep_loadable( std::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< BRepInputFactory >( filename );
        return input->is_loadable();
    }
} // namespace geode
