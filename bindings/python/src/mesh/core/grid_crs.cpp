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

#include <geode/geometry/coordinate_system.hpp>

#include <geode/mesh/core/grid_coordinate_reference_system.hpp>
#include <geode/mesh/core/grid_coordinate_reference_system_manager.hpp>

namespace
{
    template < geode::index_t dimension >
    void define_python_grid_crs( pybind11::module& module )
    {
        using CRS = geode::GridCoordinateReferenceSystem< dimension >;
        const auto crs_name =
            absl::StrCat( "GridCoordinateReferenceSystem", dimension, "D" );
        pybind11::class_< CRS, pybind11::smart_holder >(
            module, crs_name.c_str() )
            .def( pybind11::init< geode::CoordinateSystem< dimension > >() )
            .def( "type_name", &CRS::type_name )
            .def_static( "type_name_static", &CRS::type_name_static )
            .def( "coordinate_system", &CRS::coordinate_system,
                pybind11::return_value_policy::reference_internal );

        using Manager =
            geode::GridCoordinateReferenceSystemManager< dimension >;
        const auto manager_name = absl::StrCat(
            "GridCoordinateReferenceSystemManager", dimension, "D" );
        pybind11::class_< Manager >( module, manager_name.c_str() )
            .def( "nb_coordinate_reference_systems",
                &Manager::nb_coordinate_reference_systems )
            .def( "find_coordinate_reference_system",
                &Manager::find_coordinate_reference_system,
                pybind11::return_value_policy::reference_internal )
            .def( "active_coordinate_reference_system",
                &Manager::active_coordinate_reference_system,
                pybind11::return_value_policy::reference_internal )
            .def( "active_coordinate_reference_system_name",
                &Manager::active_coordinate_reference_system_name )
            .def( "coordinate_reference_system_names",
                &Manager::coordinate_reference_system_names )
            .def( "coordinate_reference_system_exists",
                &Manager::coordinate_reference_system_exists );
    }
} // namespace

namespace geode
{
    void define_grid_crs( pybind11::module& module )
    {
        define_python_grid_crs< 2 >( module );
        define_python_grid_crs< 3 >( module );
    }
} // namespace geode
