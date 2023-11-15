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

#include <geode/mesh/io/graph_input.h>

#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_input_impl.h>
#include <geode/basic/io.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/graph.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/io/vertex_set_input.h>

namespace geode
{
    std::unique_ptr< Graph > load_graph(
        const MeshImpl& impl, absl::string_view filename )
    {
        constexpr auto TYPE = "Graph";
        try
        {
            auto graph = detail::geode_object_input_impl< GraphInputFactory >(
                TYPE, filename, impl );
            Logger::info( TYPE, " has: ", graph->nb_vertices(), " vertices, ",
                graph->nb_edges(), " edges" );
            return graph;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< GraphInputFactory >( TYPE );
            Logger::info( "Other extensions are available in parent classes." );
            print_available_extensions< VertexSetInputFactory >( "VertexSet" );
            throw OpenGeodeException{ "Cannot load Graph from file: ",
                filename };
        }
    }

    std::unique_ptr< Graph > load_graph( absl::string_view filename )
    {
        return load_graph(
            MeshFactory::default_impl( Graph::type_name_static() ), filename );
    }

    typename GraphInput::MissingFiles check_graph_missing_files(
        absl::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< GraphInputFactory >( filename );
        return input->check_missing_files();
    }
} // namespace geode
