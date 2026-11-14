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

#include <geode/mesh/core/graph.hpp>
#include <geode/mesh/io/graph_input.hpp>
#include <geode/mesh/io/graph_output.hpp>

namespace geode
{
    void define_graph_io( pybind11::module& module )
    {
        module.def( "save_graph", &save_graph, pybind11::arg( "graph" ),
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Save the graph in the file, whose extension selects the format, "
            "and return the list of the written files; raise an error if the "
            "format is not supported" );
        module.def( "load_graph",
            static_cast< std::vector< std::unique_ptr< Graph > > ( * )(
                std::string_view ) >( &load_graph ),
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Load the graph objects of the file, with the default data "
            "structure, and return them as a list; raise an error if the file "
            "cannot be read" );
        module.def( "graph_object_priority", &graph_object_priority,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the priority of the loader chosen for the file, used to "
            "select the geode object type when several can load it; raise an "
            "error if no loader handles the file" );
        module.def( "is_graph_loadable", &is_graph_loadable,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return a Percentage of how well the file can be loaded as a "
            "graph, 0 if it cannot" );
        module.def( "is_graph_saveable", &is_graph_saveable,
            pybind11::arg( "graph" ), pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return True if the graph can be saved in the file format given by "
            "the filename" );
        module.def( "graph_additional_files", &graph_additional_files,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the additional files used when loading the file; raise an "
            "error if no loader handles the file" );
        PYTHON_FACTORY_CLASS( GraphInputFactory );
        PYTHON_FACTORY_CLASS( GraphOutputFactory );
    }
} // namespace geode
