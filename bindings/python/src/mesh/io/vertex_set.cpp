/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include "../../basic/factory.hpp"
#include "../../common.hpp"

#include <geode/mesh/core/vertex_set.hpp>
#include <geode/mesh/io/vertex_set_input.hpp>
#include <geode/mesh/io/vertex_set_output.hpp>

namespace geode
{
    void define_vertex_set_io( pybind11::module& module )
    {
        module.def( "save_vertex_set", &save_vertex_set,
            pybind11::arg( "vertex_set" ), pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Save the vertex set in the file, whose extension selects the "
            "format, and return the list of the written files; raise an error "
            "if the format is not supported" );
        module.def( "load_vertex_set",
            static_cast< std::vector< std::unique_ptr< VertexSet > > ( * )(
                std::string_view ) >( &load_vertex_set ),
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Load the vertex set objects of the file, with the default data "
            "structure, and return them as a list; raise an error if the file "
            "cannot be read" );
        module.def( "vertex_set_object_priority", &vertex_set_object_priority,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the priority of the loader chosen for the file, used to "
            "select the geode object type when several can load it; raise an "
            "error if no loader handles the file" );
        module.def( "is_vertex_set_loadable", &is_vertex_set_loadable,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return a Percentage of how well the file can be loaded as a "
            "vertex set, 0 if it cannot" );
        module.def( "is_vertex_set_saveable", &is_vertex_set_saveable,
            pybind11::arg( "vertex_set" ), pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return True if the vertex set can be saved in the file format "
            "given by the filename" );
        module.def( "vertex_set_additional_files", &vertex_set_additional_files,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the additional files used when loading the file; raise an "
            "error if no loader handles the file" );
        PYTHON_FACTORY_CLASS( VertexSetInputFactory );
        PYTHON_FACTORY_CLASS( VertexSetOutputFactory );
    }
} // namespace geode
