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

#include <geode/mesh/io/triangulated_surface_input.h>

#include <string_view>

#include <absl/strings/str_cat.h>

#include <geode/basic/detail/geode_input_impl.h>
#include <geode/basic/io.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/io/vertex_set_input.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TriangulatedSurface< dimension > >
        load_triangulated_surface(
            const MeshImpl& impl, std::string_view filename )
    {
        const auto type = absl::StrCat( "TriangulatedSurface", dimension, "D" );
        try
        {
            auto triangulated_surface = detail::geode_object_input_impl<
                TriangulatedSurfaceInputFactory< dimension > >(
                type, filename, impl );
            Logger::info( type, " has: ", triangulated_surface->nb_vertices(),
                " vertices, ", triangulated_surface->nb_polygons(),
                " triangles" );
            return triangulated_surface;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions<
                TriangulatedSurfaceInputFactory< dimension > >( type );
            Logger::info( "Other extensions are available in parent classes." );
            print_available_extensions< VertexSetInputFactory >( "VertexSet" );
            throw OpenGeodeException{
                "Cannot load TriangulatedSurface from file: ", filename
            };
        }
    }

    template < index_t dimension >
    std::unique_ptr< TriangulatedSurface< dimension > >
        load_triangulated_surface( std::string_view filename )
    {
        return load_triangulated_surface< dimension >(
            MeshFactory::default_impl(
                TriangulatedSurface< dimension >::type_name_static() ),
            filename );
    }

    template < index_t dimension >
    typename TriangulatedSurfaceInput< dimension >::MissingFiles
        check_triangulated_surface_missing_files( std::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            TriangulatedSurfaceInputFactory< dimension > >( filename );
        return input->check_missing_files();
    }

    template < index_t dimension >
    bool is_triangulated_surface_loadable( std::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            TriangulatedSurfaceInputFactory< dimension > >( filename );
        return input->is_loadable();
    }

    template std::unique_ptr< TriangulatedSurface< 2 > > opengeode_mesh_api
        load_triangulated_surface( const MeshImpl&, std::string_view );
    template std::unique_ptr< TriangulatedSurface< 3 > > opengeode_mesh_api
        load_triangulated_surface( const MeshImpl&, std::string_view );

    template std::unique_ptr< TriangulatedSurface< 2 > >
        opengeode_mesh_api load_triangulated_surface( std::string_view );
    template std::unique_ptr< TriangulatedSurface< 3 > >
        opengeode_mesh_api load_triangulated_surface( std::string_view );

    template TriangulatedSurfaceInput< 2 >::MissingFiles opengeode_mesh_api
        check_triangulated_surface_missing_files< 2 >( std::string_view );
    template TriangulatedSurfaceInput< 3 >::MissingFiles opengeode_mesh_api
        check_triangulated_surface_missing_files< 3 >( std::string_view );

    template bool opengeode_mesh_api is_triangulated_surface_loadable< 2 >(
        std::string_view );
    template bool opengeode_mesh_api is_triangulated_surface_loadable< 3 >(
        std::string_view );
} // namespace geode
