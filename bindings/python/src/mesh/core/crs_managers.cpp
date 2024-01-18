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

#include "../../common.h"

#include <geode/geometry/point.h>

#include <geode/mesh/core/coordinate_reference_system_manager.h>
#include <geode/mesh/core/coordinate_reference_system_managers.h>

#define PYTHON_CRS_MANAGERS( dimension )                                       \
    const auto name##dimension = "CoordinateReferenceSystemManagers"           \
                                 + std::to_string( dimension ) + "D";          \
    pybind11::class_< CoordinateReferenceSystemManagers##dimension##D >(       \
        module, name##dimension.c_str() )                                      \
        .def( "coordinate_reference_system_manager1D",                         \
            static_cast< const CoordinateReferenceSystemManager1D& (           \
                CoordinateReferenceSystemManagers##dimension##D::*) ()         \
                    const >(                                                   \
                &CoordinateReferenceSystemManagers##dimension##D::             \
                    coordinate_reference_system_manager1D ),                   \
            pybind11::return_value_policy::reference )                         \
        .def( "coordinate_reference_system_manager2D",                         \
            static_cast< const CoordinateReferenceSystemManager2D& (           \
                CoordinateReferenceSystemManagers##dimension##D::*) ()         \
                    const >(                                                   \
                &CoordinateReferenceSystemManagers##dimension##D::             \
                    coordinate_reference_system_manager2D ),                   \
            pybind11::return_value_policy::reference )                         \
        .def( "coordinate_reference_system_manager3D",                         \
            static_cast< const CoordinateReferenceSystemManager3D& (           \
                CoordinateReferenceSystemManagers##dimension##D::*) ()         \
                    const >(                                                   \
                &CoordinateReferenceSystemManagers##dimension##D::             \
                    coordinate_reference_system_manager3D ),                   \
            pybind11::return_value_policy::reference )                         \
        .def( "main_coordinate_reference_system_manager",                      \
            static_cast<                                                       \
                const CoordinateReferenceSystemManager##dimension##D& (        \
                    CoordinateReferenceSystemManagers##dimension##D::*) ()     \
                    const >(                                                   \
                &CoordinateReferenceSystemManagers##dimension##D::             \
                    main_coordinate_reference_system_manager ),                \
            pybind11::return_value_policy::reference )                         \
        .def(                                                                  \
            "point", &CoordinateReferenceSystemManagers##dimension##D::point )

namespace geode
{
    void define_crs_managers( pybind11::module& module )
    {
        PYTHON_CRS_MANAGERS( 2 );
        PYTHON_CRS_MANAGERS( 3 );
    }
} // namespace geode
