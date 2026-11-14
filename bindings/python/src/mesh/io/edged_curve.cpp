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

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/io/edged_curve_input.hpp>
#include <geode/mesh/io/edged_curve_output.hpp>

#define PYTHON_EDGED_CURVE_IO( dimension )                                     \
    const auto save##dimension =                                               \
        "save_edged_curve" + std::to_string( dimension ) + "D";                \
    module.def( save##dimension.c_str(), &save_edged_curve< dimension >,       \
        pybind11::arg( "edged_curve" ), pybind11::arg( "filename" ),           \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Save the edged curve in the file, whose extension selects the "       \
        "format, and return the list of the written files; raise an error if " \
        "the format is not supported" );                                       \
    const auto load##dimension =                                               \
        "load_edged_curve" + std::to_string( dimension ) + "D";                \
    module.def( load##dimension.c_str(),                                       \
        static_cast<                                                           \
            std::vector< std::unique_ptr< EdgedCurve< dimension > > > ( * )(   \
                std::string_view ) >( &load_edged_curve< dimension > ),        \
        pybind11::arg( "filename" ),                                           \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Load the edged curve objects of the file, with the default data "     \
        "structure, and return them as a list; raise an error if the file "    \
        "cannot be read" );                                                    \
    const auto priority##dimension =                                           \
        "edged_curve_object_priority" + std::to_string( dimension ) + "D";     \
    module.def( priority##dimension.c_str(),                                   \
        &edged_curve_object_priority< dimension >,                             \
        pybind11::arg( "filename" ),                                           \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Return the priority of the loader chosen for the file, used to "      \
        "select the geode object type when several can load it; raise an "     \
        "error if no loader handles the file" );                               \
    const auto loadable##dimension =                                           \
        "is_edged_curve_loadable" + std::to_string( dimension ) + "D";         \
    module.def( loadable##dimension.c_str(),                                   \
        &is_edged_curve_loadable< dimension >, pybind11::arg( "filename" ),    \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Return a Percentage of how well the file can be loaded as an edged "  \
        "curve, 0 if it cannot" );                                             \
    const auto check##dimension =                                              \
        "edged_curve_additional_files" + std::to_string( dimension ) + "D";    \
    module.def( check##dimension.c_str(),                                      \
        &edged_curve_additional_files< dimension >,                            \
        pybind11::arg( "filename" ),                                           \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Return the additional files used when loading the file; raise an "    \
        "error if no loader handles the file" );                               \
    const auto saveable##dimension =                                           \
        "is_edged_curve_saveable" + std::to_string( dimension ) + "D";         \
    module.def( saveable##dimension.c_str(),                                   \
        &is_edged_curve_saveable< dimension >, pybind11::arg( "edged_curve" ), \
        pybind11::arg( "filename" ),                                           \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        "Return True if the edged curve can be saved in the file format "      \
        "given by the filename" );                                             \
    PYTHON_FACTORY_CLASS( EdgedCurveInputFactory##dimension##D );              \
    PYTHON_FACTORY_CLASS( EdgedCurveOutputFactory##dimension##D )

namespace geode
{
    void define_edged_curve_io( pybind11::module& module )
    {
        PYTHON_EDGED_CURVE_IO( 2 );
        PYTHON_EDGED_CURVE_IO( 3 );
    }
} // namespace geode
