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

#include "../../common.h"

#include <geode/mesh/core/coordinate_reference_system.h>
#include <geode/mesh/core/coordinate_reference_system_manager.h>

#define PYTHON_CRS_MANAGER( dimension )                                        \
    const auto name##dimension = "CoordinateReferenceSystemManager"            \
                                 + std::to_string( dimension ) + "D";          \
    pybind11::class_< CoordinateReferenceSystemManager##dimension##D >(        \
        module, name##dimension.c_str() )                                      \
        .def( "nb_coordinate_reference_systems",                               \
            &CoordinateReferenceSystemManager<                                 \
                dimension >::nb_coordinate_reference_systems )                 \
        .def( "find_coordinate_reference_system",                              \
            &CoordinateReferenceSystemManager<                                 \
                dimension >::find_coordinate_reference_system,                 \
            pybind11::return_value_policy::reference )                         \
        .def( "active_coordinate_reference_system",                            \
            &CoordinateReferenceSystemManager<                                 \
                dimension >::active_coordinate_reference_system,               \
            pybind11::return_value_policy::reference )                         \
        .def( "active_coordinate_reference_system_name",                       \
            &CoordinateReferenceSystemManager<                                 \
                dimension >::active_coordinate_reference_system_name )         \
        .def( "coordinate_reference_system_names",                             \
            &CoordinateReferenceSystemManager<                                 \
                dimension >::coordinate_reference_system_names )               \
        .def( "coordinate_reference_system_exists",                            \
            &CoordinateReferenceSystemManager<                                 \
                dimension >::coordinate_reference_system_exists )

namespace geode
{
    void define_crs_manager( pybind11::module& module )
    {
        PYTHON_CRS_MANAGER( 2 );
        PYTHON_CRS_MANAGER( 3 );
    }
} // namespace geode
