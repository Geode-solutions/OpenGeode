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

#include <geode/model/helpers/create_coordinate_system.hpp>
#include <geode/model/helpers/model_coordinate_reference_system.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace geode
{
    void define_model_coordinate_reference_system( pybind11::module& module )
    {
        module
            .def( "brep_coordinate_reference_systems",
                &brep_coordinate_reference_systems, pybind11::arg( "brep" ),
                "Return the list of (CRSType, name) of the coordinate "
                "reference systems shared by all the component meshes of the "
                "BRep that have vertices; empty if none has vertices" )
            .def( "section_coordinate_reference_systems",
                &section_coordinate_reference_systems,
                pybind11::arg( "section" ),
                "Return the list of (CRSType, name) of the coordinate "
                "reference systems shared by all the component meshes of the "
                "Section that have vertices; empty if none has vertices" )
            .def( "brep_active_coordinate_reference_systems",
                &brep_active_coordinate_reference_systems,
                pybind11::arg( "brep" ),
                "Return the list of (CRSType, name) of the active coordinate "
                "reference systems of the component meshes of the BRep that "
                "have vertices, without duplicates: a single entry means a "
                "consistent model" )
            .def( "section_active_coordinate_reference_systems",
                &section_active_coordinate_reference_systems,
                pybind11::arg( "section" ),
                "Return the list of (CRSType, name) of the active coordinate "
                "reference systems of the component meshes of the Section that "
                "have vertices, without duplicates: a single entry means a "
                "consistent model" )
            .def( "create_brep_coordinate_system",
                &create_brep_coordinate_system, pybind11::arg( "model" ),
                pybind11::arg( "builder" ),
                pybind11::arg( "new_coordinate_system_name" ),
                pybind11::arg( "input" ), pybind11::arg( "output" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Register and activate a coordinate reference system named "
                "new_coordinate_system_name on the mesh of each component of "
                "the BRep, whose points are the mesh points with their first "
                "two coordinates converted from the input coordinate system to "
                "the output one" )
            .def( "create_section_coordinate_system",
                &create_section_coordinate_system, pybind11::arg( "model" ),
                pybind11::arg( "builder" ),
                pybind11::arg( "new_coordinate_system_name" ),
                pybind11::arg( "input" ), pybind11::arg( "output" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Register and activate a coordinate reference system named "
                "new_coordinate_system_name on the mesh of each component of "
                "the Section, whose points are the mesh points with their "
                "first two coordinates converted from the input coordinate "
                "system to the output one" )
            .def( "set_brep_active_coordinate_system",
                &set_brep_active_coordinate_system, pybind11::arg( "model" ),
                pybind11::arg( "builder" ),
                pybind11::arg( "coordinate_system_name" ),
                "Activate the coordinate reference system with the given name "
                "on the mesh of each component of the BRep" )
            .def( "set_section_active_coordinate_system",
                &set_section_active_coordinate_system, pybind11::arg( "model" ),
                pybind11::arg( "builder" ),
                pybind11::arg( "coordinate_system_name" ),
                "Activate the coordinate reference system with the given name "
                "on the mesh of each component of the Section" );
    }
} // namespace geode