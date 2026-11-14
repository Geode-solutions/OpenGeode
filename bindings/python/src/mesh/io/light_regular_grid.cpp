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

#include <string>

#include "../../basic/factory.hpp"
#include "../../common.hpp"

#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/io/light_regular_grid_input.hpp>
#include <geode/mesh/io/light_regular_grid_output.hpp>

#define PYTHON_LIGHT_REGULAR_GRID_IO( dimension )                              \
    const auto save##dimension =                                               \
        "save_light_regular_grid" + std::to_string( dimension ) + "D";         \
    module.def( save##dimension.c_str(),                                       \
        &save_light_regular_grid< dimension >,                                 \
        pybind11::arg( "light_regular_grid" ), pybind11::arg( "filename" ),    \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Save the light regular grid in the file, whose extension selects "    \
        "the format, and return the list of the written files; raise an "      \
        "error if the format is not supported" );                              \
    const auto load##dimension =                                               \
        "load_light_regular_grid" + std::to_string( dimension ) + "D";         \
    module.def( load##dimension.c_str(),                                       \
        static_cast< std::vector< LightRegularGrid< dimension > > ( * )(       \
            std::string_view ) >( &load_light_regular_grid< dimension > ),     \
        pybind11::arg( "filename" ),                                           \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Load the light regular grid objects of the file, with the default "   \
        "data structure, and return them as a list; raise an error if the "    \
        "file cannot be read" );                                               \
    const auto priority##dimension = "light_regular_grid_object_priority"      \
                                     + std::to_string( dimension ) + "D";      \
    module.def( priority##dimension.c_str(),                                   \
        &light_regular_grid_object_priority< dimension >,                      \
        pybind11::arg( "filename" ),                                           \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Return the priority of the loader chosen for the file, used to "      \
        "select the geode object type when several can load it; raise an "     \
        "error if no loader handles the file" );                               \
    const auto loadable##dimension =                                           \
        "is_light_regular_grid_loadable" + std::to_string( dimension ) + "D";  \
    module.def( loadable##dimension.c_str(),                                   \
        &is_light_regular_grid_loadable< dimension >,                          \
        pybind11::arg( "filename" ),                                           \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Return a Percentage of how well the file can be loaded as a light "   \
        "regular grid, 0 if it cannot" );                                      \
    const auto check##dimension = "light_regular_grid_additional_files"        \
                                  + std::to_string( dimension ) + "D";         \
    module.def( check##dimension.c_str(),                                      \
        &light_regular_grid_additional_files< dimension >,                     \
        pybind11::arg( "filename" ),                                           \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Return the additional files used when loading the file; raise an "    \
        "error if no loader handles the file" );                               \
    const auto saveable##dimension =                                           \
        "is_light_regular_grid_saveable" + std::to_string( dimension ) + "D";  \
    module.def( saveable##dimension.c_str(),                                   \
        &is_light_regular_grid_saveable< dimension >,                          \
        pybind11::arg( "light_regular_grid" ), pybind11::arg( "filename" ),    \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Return True if the light regular grid can be saved in the file "      \
        "format given by the filename" );                                      \
    PYTHON_FACTORY_CLASS( LightRegularGridInputFactory##dimension##D );        \
    PYTHON_FACTORY_CLASS( LightRegularGridOutputFactory##dimension##D )

namespace geode
{
    void define_light_regular_grid_io( pybind11::module& module )
    {
        PYTHON_LIGHT_REGULAR_GRID_IO( 2 );
        PYTHON_LIGHT_REGULAR_GRID_IO( 3 );
    }
} // namespace geode
