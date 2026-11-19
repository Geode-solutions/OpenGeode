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

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/core/coordinate_reference_system.hpp>
#include <geode/mesh/core/coordinate_reference_system_manager.hpp>

#define PYTHON_CRS_MANAGER_BUILDER( dimension )                                \
    const auto name##dimension = "CoordinateReferenceSystemManagerBuilder"     \
                                 + std::to_string( dimension ) + "D";          \
    pybind11::class_< CoordinateReferenceSystemManagerBuilder##dimension##D >( \
        module, name##dimension.c_str(),                                       \
        "Builder to modify a coordinate reference system manager" )            \
        .def( pybind11::init<                                                  \
                  CoordinateReferenceSystemManager< dimension >& >(),          \
            pybind11::arg( "crs_manager" ), pybind11::keep_alive< 1, 2 >(),    \
            "Build the builder of a coordinate reference system manager, "     \
            "which is kept alive by the builder" )                             \
        .def(                                                                  \
            "register_coordinate_reference_system",                            \
            []( CoordinateReferenceSystemManagerBuilder< dimension >& manager, \
                std::string_view name,                                         \
                std::shared_ptr< CoordinateReferenceSystem< dimension > >&     \
                    crs ) {                                                    \
                manager.register_coordinate_reference_system(                  \
                    name, std::move( crs ) );                                  \
            },                                                                 \
            "Register a coordinate reference system under this name; raise "   \
            "an error if the name is already used",                            \
            pybind11::arg( "name" ), pybind11::arg( "crs" ) )                  \
        .def( "delete_coordinate_reference_system",                            \
            &CoordinateReferenceSystemManagerBuilder<                          \
                dimension >::delete_coordinate_reference_system,               \
            "Delete the coordinate reference system with this name (nothing "  \
            "happens if there is none); if it was active, no coordinate "      \
            "reference system is active anymore",                              \
            pybind11::arg( "name" ) )                                          \
        .def( "set_active_coordinate_reference_system",                        \
            &CoordinateReferenceSystemManagerBuilder<                          \
                dimension >::set_active_coordinate_reference_system,           \
            "Make the coordinate reference system with this name the active "  \
            "one; raise an error if there is none",                            \
            pybind11::arg( "name" ) )                                          \
        .def( "active_coordinate_reference_system",                            \
            &CoordinateReferenceSystemManagerBuilder<                          \
                dimension >::active_coordinate_reference_system,               \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the active coordinate reference system to modify it; "     \
            "raise an error if none is active" )                               \
        .def( "coordinate_reference_system",                                   \
            &CoordinateReferenceSystemManagerBuilder<                          \
                dimension >::coordinate_reference_system,                      \
            pybind11::arg( "name" ),                                           \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the coordinate reference system with this name to modify " \
            "it; raise an error if there is none" )

namespace geode
{
    void define_crs_manager_builder( pybind11::module& module )
    {
        PYTHON_CRS_MANAGER_BUILDER( 1 );
        PYTHON_CRS_MANAGER_BUILDER( 2 );
        PYTHON_CRS_MANAGER_BUILDER( 3 );
    }
} // namespace geode
