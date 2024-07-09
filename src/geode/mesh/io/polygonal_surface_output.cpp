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

#include <geode/mesh/io/polygonal_surface_output.h>

#include <string>
#include <vector>

#include <absl/strings/str_cat.h>
#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_output_impl.h>
#include <geode/basic/io.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/io/vertex_set_output.h>

namespace geode
{
    template < index_t dimension >
    std::vector< std::string > save_polygonal_surface(
        const PolygonalSurface< dimension >& polygonal_surface,
        absl::string_view filename )
    {
        const auto type = absl::StrCat( "PolygonalSurface", dimension, "D" );
        try
        {
            return detail::geode_object_output_impl(
                polygonal_surface_output_factory< dimension >(), type,
                polygonal_surface, filename );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions(
                polygonal_surface_output_factory< dimension >(), type );
            Logger::info( "Other extensions are available in parent classes." );
            print_available_extensions(
                vertex_set_output_factory, "VertexSet" );
            throw OpenGeodeException{ "Cannot save PolygonalSurface in file: ",
                filename };
        }
    }

    template < index_t dimension >
    bool is_polygonal_surface_saveable(
        const PolygonalSurface< dimension >& polygonal_surface,
        absl::string_view filename )
    {
        const auto output = detail::geode_object_output_writer(
            polygonal_surface_output_factory< dimension >(), filename );
        return output->is_saveable( polygonal_surface );
    }

    template std::vector< std::string >
        opengeode_mesh_api save_polygonal_surface(
            const PolygonalSurface< 2 >&, absl::string_view );
    template std::vector< std::string >
        opengeode_mesh_api save_polygonal_surface(
            const PolygonalSurface< 3 >&, absl::string_view );

    template bool opengeode_mesh_api is_polygonal_surface_saveable(
        const PolygonalSurface< 2 >&, absl::string_view );
    template bool opengeode_mesh_api is_polygonal_surface_saveable(
        const PolygonalSurface< 3 >&, absl::string_view );
} // namespace geode
