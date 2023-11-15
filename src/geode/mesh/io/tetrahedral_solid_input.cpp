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

#include <geode/mesh/io/tetrahedral_solid_input.h>

#include <absl/strings/str_cat.h>
#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_input_impl.h>
#include <geode/basic/io.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/io/vertex_set_input.h>

namespace geode
{
    template < index_t dimension >
    std::unique_ptr< TetrahedralSolid< dimension > > load_tetrahedral_solid(
        const MeshImpl& impl, absl::string_view filename )
    {
        const auto type = absl::StrCat( "TetrahedralSolid", dimension, "D" );
        try
        {
            auto tetrahedral_solid = detail::geode_object_input_impl<
                TetrahedralSolidInputFactory< dimension > >(
                type, filename, impl );
            Logger::info( type, " has: ", tetrahedral_solid->nb_vertices(),
                " vertices, ", tetrahedral_solid->nb_polyhedra(),
                " tetrahedra" );
            return tetrahedral_solid;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions<
                TetrahedralSolidInputFactory< dimension > >( type );
            Logger::info( "Other extensions are available in parent classes." );
            print_available_extensions< VertexSetInputFactory >( "VertexSet" );
            throw OpenGeodeException{
                "Cannot load TetrahedralSolid from file: ", filename
            };
        }
    }

    template < index_t dimension >
    std::unique_ptr< TetrahedralSolid< dimension > > load_tetrahedral_solid(
        absl::string_view filename )
    {
        return load_tetrahedral_solid< dimension >(
            MeshFactory::default_impl(
                TetrahedralSolid< dimension >::type_name_static() ),
            filename );
    }

    template < index_t dimension >
    typename TetrahedralSolidInput< dimension >::MissingFiles
        check_tetrahedral_solid_missing_files( absl::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            TetrahedralSolidInputFactory< dimension > >( filename );
        return input->check_missing_files();
    }

    template std::unique_ptr< TetrahedralSolid< 3 > > opengeode_mesh_api
        load_tetrahedral_solid( const MeshImpl&, absl::string_view );

    template std::unique_ptr< TetrahedralSolid< 3 > >
        opengeode_mesh_api load_tetrahedral_solid( absl::string_view );

    template TetrahedralSolidInput< 3 >::MissingFiles opengeode_mesh_api
        check_tetrahedral_solid_missing_files< 3 >( absl::string_view );
} // namespace geode
