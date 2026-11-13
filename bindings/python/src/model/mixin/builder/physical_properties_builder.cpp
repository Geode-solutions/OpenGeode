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

#include <geode/basic/uuid.hpp>

#include <geode/model/mixin/builder/physical_properties_builder.hpp>

namespace geode
{
    void define_physical_properties_builder( pybind11::module& module )
    {
        pybind11::class_< PhysicalPropertiesBuilder >( module,
            "PhysicalPropertiesBuilder",
            "Builder modifying the PhysicalProperties of a model" )
            .def( pybind11::init< PhysicalProperties& >(),
                pybind11::arg( "physical_properties" ),
                pybind11::keep_alive< 1, 2 >(),
                "Build a builder of the given PhysicalProperties, kept alive "
                "by the builder" )
            .def( "set_physical_property",
                &PhysicalPropertiesBuilder::set_physical_property,
                pybind11::arg( "name" ), pybind11::arg( "component_type" ),
                pybind11::arg( "attribute_id" ),
                "Associate the physical property to the attribute attribute_id "
                "stored in the meshes of the components of the given type; "
                "replace the previous association of this property" )
            .def( "copy_physical_properties",
                &PhysicalPropertiesBuilder::copy_physical_properties,
                pybind11::arg( "other" ),
                "Replace all the physical properties by the ones of other" );
    }
} // namespace geode
