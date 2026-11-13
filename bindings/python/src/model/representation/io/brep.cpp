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

#include "../../../basic/factory.hpp"
#include "../../../common.hpp"

#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>
#include <geode/model/representation/io/brep_output.hpp>
#include <geode/model/representation/io/brep_time_series_input.hpp>

namespace geode
{
    void define_brep_io( pybind11::module& module )
    {
        module.def( "save_brep", &save_brep, pybind11::arg( "brep" ),
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Save the BRep in the file, whose extension selects the format, "
            "and return the list of the written files; raise an error if the "
            "format is not supported" );
        module.def( "load_brep", &load_brep, pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Load the BRep objects of the file, with the default data "
            "structure, and return them as a list; raise an error if the file "
            "cannot be read" );
        module.def( "brep_object_priority", &brep_object_priority,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the priority of the loader chosen for the file, used to "
            "select the geode object type when several can load it; raise an "
            "error if no loader handles the file" );
        module.def( "is_brep_loadable", &is_brep_loadable,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return a Percentage of how well the file can be loaded as a BRep, "
            "0 if it cannot" );
        module.def( "is_brep_saveable", &is_brep_saveable,
            pybind11::arg( "brep" ), pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return True if the BRep can be saved in the file format given by "
            "the filename" );
        module.def( "brep_additional_files", &brep_additional_files,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the AdditionalFiles (mandatory and optional) used when "
            "loading the file; raise an error if no loader handles the file" );
        module.def( "load_brep_time_series", &load_brep_time_series,
            pybind11::arg( "brep" ), pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Load the time series of the file onto the BRep: each series is "
            "stored on the Block meshes (vertex and polyhedron attributes) as "
            "time step attributes; the loader is chosen from the file "
            "extension and an error is raised if none handles it" );
        module.def( "is_brep_time_series_loadable",
            &is_brep_time_series_loadable, pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return a Percentage of how well the file can be loaded as BRep "
            "time series, 0 if it cannot" );
        module.def( "brep_time_series_additional_files",
            &brep_time_series_additional_files, pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the additional files used when loading the time series "
            "file; raise an error if no loader handles the file" );
        PYTHON_FACTORY_CLASS( BRepInputFactory );
        PYTHON_FACTORY_CLASS( BRepOutputFactory );
        PYTHON_FACTORY_CLASS( BRepTimeSeriesInputFactory );
    }
} // namespace geode
