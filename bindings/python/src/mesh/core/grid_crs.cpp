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
        pybind11::class_< CRS, pybind11::smart_holder >( module,
            crs_name.c_str(),
            "Coordinate reference system of a light regular grid: the grid "
            "geometry (origin and directions) expressed in this reference "
            "system" )
            .def( pybind11::init< geode::CoordinateSystem< dimension > >(),
                "Build a grid coordinate reference system from the coordinate "
                "system (origin and directions) of the grid",
                pybind11::arg( "coordinate_system" ) )
            .def( "type_name", &CRS::type_name,
                "Return the type of the coordinate reference system" )
            .def_static( "type_name_static", &CRS::type_name_static,
                "Return the type of the grid coordinate reference system" )
            .def( "coordinate_system", &CRS::coordinate_system,
                pybind11::return_value_policy::reference_internal,
                "Return the grid coordinate system (origin and directions) "
                "expressed in this coordinate reference system" );

        using Manager =
            geode::GridCoordinateReferenceSystemManager< dimension >;
        const auto manager_name = absl::StrCat(
            "GridCoordinateReferenceSystemManager", dimension, "D" );
        pybind11::class_< Manager >( module, manager_name.c_str(),
            "Named grid coordinate reference systems of a light regular grid, "
            "one of which is active" )
            .def( "nb_coordinate_reference_systems",
                &Manager::nb_coordinate_reference_systems,
                "Return the number of registered coordinate reference systems" )
            .def( "find_coordinate_reference_system",
                &Manager::find_coordinate_reference_system,
                pybind11::arg( "name" ),
                pybind11::return_value_policy::reference_internal,
                "Return the coordinate reference system with this name; raise "
                "an error if there is none" )
            .def( "active_coordinate_reference_system",
                &Manager::active_coordinate_reference_system,
                pybind11::return_value_policy::reference_internal,
                "Return the active coordinate reference system; raise an error "
                "if none is active" )
            .def( "active_coordinate_reference_system_name",
                &Manager::active_coordinate_reference_system_name,
                "Return the name of the active coordinate reference system, "
                "empty if none is active" )
            .def( "coordinate_reference_system_names",
                &Manager::coordinate_reference_system_names,
                "Return the names of the registered coordinate reference "
                "systems, in no particular order" )
            .def( "coordinate_reference_system_exists",
                &Manager::coordinate_reference_system_exists,
                "Return True if a coordinate reference system has this name",
                pybind11::arg( "name" ) );
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
