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

#include "../../../common.hpp"

#include <geode/model/mixin/core/physical_properties.hpp>

namespace geode
{
    void define_physical_properties( pybind11::module& module )
    {
        pybind11::enum_< PHYSICAL_PROPERTY_NAME >(
            module, "PHYSICAL_PROPERTY_NAME" )
            .value( "porosity", PHYSICAL_PROPERTY_NAME::porosity )
            .value( "permeability", PHYSICAL_PROPERTY_NAME::permeability )
            .export_values();

        pybind11::class_< PhysicalProperties::Info >( module, "Info" )
            .def( pybind11::init< ComponentType, uuid >() )
            .def_readwrite(
                "component_type", &PhysicalProperties::Info::component_type )
            .def_readwrite(
                "attribute_id", &PhysicalProperties::Info::attribute_id );

        pybind11::class_< PhysicalProperties, pybind11::smart_holder >(
            module, "PhysicalProperties" )
            .def( "has_physical_property",
                &PhysicalProperties::has_physical_property )
            .def( "physical_property_info",
                &PhysicalProperties::physical_property_info,
                pybind11::return_value_policy::reference_internal );
    }
} // namespace geode
