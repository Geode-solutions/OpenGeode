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
        pybind11::native_enum< PHYSICAL_PROPERTY_NAME >( module,
            "PhysicalPropertyName", "enum.Enum",
            "Names of the physical properties that can be associated to the "
            "components of a model (porosity, permeability, boundary "
            "conditions, mechanical parameters...)" )
            .value( "porosity", PHYSICAL_PROPERTY_NAME::porosity )
            .value( "permeability", PHYSICAL_PROPERTY_NAME::permeability )
            .value(
                "boundary_pressure", PHYSICAL_PROPERTY_NAME::boundary_pressure )
            .value( "boundary_temperature",
                PHYSICAL_PROPERTY_NAME::boundary_temperature )
            .value( "boundary_oil_fraction",
                PHYSICAL_PROPERTY_NAME::boundary_oil_fraction )
            .value( "boundary_gas_fraction",
                PHYSICAL_PROPERTY_NAME::boundary_gas_fraction )
            .value( "boundary_water_fraction",
                PHYSICAL_PROPERTY_NAME::boundary_water_fraction )
            .value( "bulk_modulus", PHYSICAL_PROPERTY_NAME::bulk_modulus )
            .value( "shear_modulus", PHYSICAL_PROPERTY_NAME::shear_modulus )
            .value( "density", PHYSICAL_PROPERTY_NAME::density )
            .value(
                "biot_coefficient", PHYSICAL_PROPERTY_NAME::biot_coefficient )
            .value( "grain_density", PHYSICAL_PROPERTY_NAME::grain_density )
            .value( "thermal_expansion_coefficient",
                PHYSICAL_PROPERTY_NAME::thermal_expansion_coefficient )
            .value( "initial_stress", PHYSICAL_PROPERTY_NAME::initial_stress )
            .value( "initial_shear_stress",
                PHYSICAL_PROPERTY_NAME::initial_shear_stress )
            .value( "boundary_displacement",
                PHYSICAL_PROPERTY_NAME::boundary_displacement )
            .value(
                "boundary_traction", PHYSICAL_PROPERTY_NAME::boundary_traction )
            .value( "boundary_normal_traction",
                PHYSICAL_PROPERTY_NAME::boundary_normal_traction )
            .finalize();

        auto cls_Info =
            pybind11::class_< PhysicalProperties::Info >( module, "Info",
                "Where the values of a physical property are stored: the type "
                "of the components carrying them and the uuid of the attribute "
                "holding the values inside the component meshes" )
                .def( pybind11::init< ComponentType, uuid >(),
                    pybind11::arg( "component_type" ),
                    pybind11::arg( "attribute_id" ),
                    "Build the description of a physical property stored in "
                    "the attribute attribute_id of the meshes of the "
                    "components of the given type" )

            ;
        def_readwrite_named( cls_Info, "component_type",
            &PhysicalProperties::Info::component_type,
            "Type of the components carrying the physical property values" );
        def_readwrite_named( cls_Info, "attribute_id",
            &PhysicalProperties::Info::attribute_id,
            "Uuid of the attribute storing the physical property values inside "
            "the component meshes" );

        pybind11::class_< PhysicalProperties, pybind11::smart_holder >( module,
            "PhysicalProperties",
            "Association between physical property names and the attributes of "
            "the model component meshes holding their values" )
            .def( "has_physical_property",
                &PhysicalProperties::has_physical_property,
                pybind11::arg( "name" ),
                "Return True if the physical property has been set" )
            .def( "physical_property_info",
                &PhysicalProperties::physical_property_info,
                pybind11::arg( "name" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Info of the physical property; raise an error if "
                "it has not been set (check with has_physical_property)" );
    }
} // namespace geode
