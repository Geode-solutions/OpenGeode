/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include "../../common.hpp"

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/builder/coordinate_reference_system_managers_builder.hpp>
#include <geode/mesh/core/coordinate_reference_system_managers.hpp>

#define PYTHON_CRS_MANAGERS_BUILDER( dimension )                               \
    const auto name##dimension = "CoordinateReferenceSystemManagersBuilder"    \
                                 + std::to_string( dimension ) + "D";          \
    pybind11::class_<                                                          \
        CoordinateReferenceSystemManagersBuilder##dimension##D >(              \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init<                                                  \
            CoordinateReferenceSystemManagers< dimension >& >() )              \
        .def( "coordinate_reference_system_manager_builder1D",                 \
            &CoordinateReferenceSystemManagersBuilder##dimension##D::          \
                coordinate_reference_system_manager_builder1D )                \
        .def( "coordinate_reference_system_manager_builder2D",                 \
            &CoordinateReferenceSystemManagersBuilder##dimension##D::          \
                coordinate_reference_system_manager_builder2D )                \
        .def( "coordinate_reference_system_manager_builder3D",                 \
            &CoordinateReferenceSystemManagersBuilder##dimension##D::          \
                coordinate_reference_system_manager_builder3D )                \
        .def( "main_coordinate_reference_system_manager_builder",              \
            &CoordinateReferenceSystemManagersBuilder##dimension##D::          \
                main_coordinate_reference_system_manager_builder )             \
        .def( "set_point",                                                     \
            &CoordinateReferenceSystemManagersBuilder##dimension##D::          \
                set_point )

namespace geode
{
    void define_crs_managers_builder( pybind11::module& module )
    {
        PYTHON_CRS_MANAGERS_BUILDER( 2 );
        PYTHON_CRS_MANAGERS_BUILDER( 3 );
    }
} // namespace geode
