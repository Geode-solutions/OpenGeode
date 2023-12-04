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

#include <geode/mesh/io/vertex_set_output.h>

#include <string>
#include <vector>

#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_output_impl.h>
#include <geode/basic/io.h>

#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    std::vector< std::string > save_vertex_set(
        const VertexSet& vertex_set, absl::string_view filename )
    {
        constexpr auto type = "VertexSet";
        try
        {
            return detail::geode_object_output_impl< VertexSetOutputFactory >(
                type, vertex_set, filename );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< VertexSetOutputFactory >( type );
            throw OpenGeodeException{ "Cannot save VertexSet in file: ",
                filename };
        }
    }

    bool is_vertex_set_saveable(
        const VertexSet& vertex_set, absl::string_view filename )
    {
        const auto output =
            detail::geode_object_output_writer< VertexSetOutputFactory >(
                filename );
        return output->is_saveable( vertex_set );
    }
} // namespace geode
