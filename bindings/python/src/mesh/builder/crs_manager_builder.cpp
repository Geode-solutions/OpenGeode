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

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.h>
#include <geode/mesh/core/coordinate_reference_system.h>
#include <geode/mesh/core/coordinate_reference_system_manager.h>

#define PYTHON_CRS_MANAGER_BUILDER( dimension )                                \
    const auto name##dimension = "CoordinateReferenceSystemManagerBuilder"     \
                                 + std::to_string( dimension ) + "D";          \
    pybind11::class_< CoordinateReferenceSystemManagerBuilder##dimension##D >( \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init<                                                  \
            CoordinateReferenceSystemManager< dimension >& >() )               \
        .def( "register_coordinate_reference_system",                          \
            []( CoordinateReferenceSystemManagerBuilder< dimension >& manager, \
                absl::string_view name,                                        \
                std::shared_ptr< CoordinateReferenceSystem< dimension > >&     \
                    crs ) {                                                    \
                manager.register_coordinate_reference_system(                  \
                    name, std::move( crs ) );                                  \
            } )                                                                \
        .def( "delete_coordinate_reference_system",                            \
            &CoordinateReferenceSystemManagerBuilder<                          \
                dimension >::delete_coordinate_reference_system )              \
        .def( "set_active_coordinate_reference_system",                        \
            &CoordinateReferenceSystemManagerBuilder<                          \
                dimension >::set_active_coordinate_reference_system )          \
        .def( "active_coordinate_reference_system",                            \
            &CoordinateReferenceSystemManagerBuilder<                          \
                dimension >::active_coordinate_reference_system )              \
        .def( "coordinate_reference_system",                                   \
            &CoordinateReferenceSystemManagerBuilder<                          \
                dimension >::coordinate_reference_system )

namespace geode
{
    void define_crs_manager_builder( pybind11::module& module )
    {
        PYTHON_CRS_MANAGER_BUILDER( 1 );
        PYTHON_CRS_MANAGER_BUILDER( 2 );
        PYTHON_CRS_MANAGER_BUILDER( 3 );
    }
} // namespace geode
