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

#include "../../basic/factory.h"
#include "../../basic/input.h"
#include "../../common.h"

#include <geode/mesh/core/vertex_set.h>
#include <geode/mesh/io/vertex_set_input.h>
#include <geode/mesh/io/vertex_set_output.h>

namespace geode
{
    void define_vertex_set_io( pybind11::module& module )
    {
        module.def( "save_vertex_set", &save_vertex_set );
        module.def(
            "load_vertex_set", static_cast< std::unique_ptr< VertexSet > ( * )(
                                   absl::string_view ) >( &load_vertex_set ) );
        module.def(
            "check_vertex_set_missing_files", &check_vertex_set_missing_files );
        module.def( "is_vertex_set_loadable", &is_vertex_set_loadable );
        module.def( "is_vertex_set_saveable", &is_vertex_set_saveable );
        PYTHON_INPUT_MESH_CLASS( std::unique_ptr< VertexSet >, "VertexSet" );
        PYTHON_FACTORY_CLASS( VertexSetInputFactory, vertex_set_input_factory );
        PYTHON_FACTORY_CLASS(
            VertexSetOutputFactory, vertex_set_output_factory );
    }
} // namespace geode
