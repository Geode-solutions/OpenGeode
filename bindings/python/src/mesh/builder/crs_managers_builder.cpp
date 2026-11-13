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

#include "../../common.hpp"

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/builder/coordinate_reference_system_managers_builder.hpp>
#include <geode/mesh/core/coordinate_reference_system_managers.hpp>

#define PYTHON_CRS_MANAGERS_BUILDER( dimension )                               \
    const auto name##dimension = "CoordinateReferenceSystemManagersBuilder"    \
                                 + std::to_string( dimension ) + "D";          \
    pybind11::class_<                                                          \
        CoordinateReferenceSystemManagersBuilder##dimension##D >( module,      \
        name##dimension.c_str(),                                               \
        "Builder to modify the coordinate reference system managers of a "     \
        "mesh" )                                                               \
        .def( pybind11::init<                                                  \
                  CoordinateReferenceSystemManagers< dimension >& >(),         \
            pybind11::arg( "crs_managers" ), pybind11::keep_alive< 1, 2 >(),   \
            "Build the builder of coordinate reference system managers, "      \
            "which is kept alive by the builder" )                             \
        .def( "coordinate_reference_system_manager_builder1D",                 \
            &CoordinateReferenceSystemManagersBuilder##dimension##D::          \
                coordinate_reference_system_manager_builder1D,                 \
            pybind11::keep_alive< 0, 1 >(),                                    \
            "Return the builder of the 1D coordinate reference system "        \
            "manager, which keeps this builder alive" )                        \
        .def( "coordinate_reference_system_manager_builder2D",                 \
            &CoordinateReferenceSystemManagersBuilder##dimension##D::          \
                coordinate_reference_system_manager_builder2D,                 \
            pybind11::keep_alive< 0, 1 >(),                                    \
            "Return the builder of the 2D coordinate reference system "        \
            "manager, which keeps this builder alive" )                        \
        .def( "coordinate_reference_system_manager_builder3D",                 \
            &CoordinateReferenceSystemManagersBuilder##dimension##D::          \
                coordinate_reference_system_manager_builder3D,                 \
            pybind11::keep_alive< 0, 1 >(),                                    \
            "Return the builder of the 3D coordinate reference system "        \
            "manager, which keeps this builder alive" )                        \
        .def( "main_coordinate_reference_system_manager_builder",              \
            &CoordinateReferenceSystemManagersBuilder##dimension##D::          \
                main_coordinate_reference_system_manager_builder,              \
            pybind11::keep_alive< 0, 1 >(),                                    \
            "Return the builder of the coordinate reference system manager "   \
            "with the dimension of the mesh, which keeps this builder alive" ) \
        .def( "set_point",                                                     \
            &CoordinateReferenceSystemManagersBuilder##dimension##D::          \
                set_point,                                                     \
            "Set the coordinates of a vertex in the active coordinate "        \
            "reference system; the vertex must have been created before",      \
            pybind11::arg( "vertex" ), pybind11::arg( "point" ) )

namespace geode
{
    void define_crs_managers_builder( pybind11::module& module )
    {
        PYTHON_CRS_MANAGERS_BUILDER( 2 );
        PYTHON_CRS_MANAGERS_BUILDER( 3 );
    }
} // namespace geode
