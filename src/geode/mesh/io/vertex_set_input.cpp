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

#include <geode/mesh/io/vertex_set_input.h>

#include <string_view>

#include <geode/basic/detail/geode_input_impl.h>
#include <geode/basic/io.h>

#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    std::unique_ptr< VertexSet > load_vertex_set( std::string_view filename )
    {
        return load_vertex_set(
            MeshFactory::default_impl( VertexSet::type_name_static() ),
            filename );
    }

    std::unique_ptr< VertexSet > load_vertex_set(
        const MeshImpl& impl, std::string_view filename )
    {
        constexpr auto TYPE = "VertexSet";
        try
        {
            auto vertex_set =
                detail::geode_object_input_impl< VertexSetInputFactory >(
                    TYPE, filename, impl );
            Logger::info(
                TYPE, " has: ", vertex_set->nb_vertices(), " vertices" );
            return vertex_set;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< VertexSetInputFactory >( TYPE );
            throw OpenGeodeException{ "Cannot load VertexSet from file: ",
                filename };
        }
    }

    typename VertexSetInput::MissingFiles check_vertex_set_missing_files(
        std::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< VertexSetInputFactory >(
                filename );
        return input->check_missing_files();
    }

    bool is_vertex_set_loadable( std::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< VertexSetInputFactory >(
                filename );
        return input->is_loadable();
    }
} // namespace geode
