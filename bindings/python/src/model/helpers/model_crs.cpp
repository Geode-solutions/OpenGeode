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
                &brep_coordinate_reference_systems )
            .def( "section_coordinate_reference_systems",
                &section_coordinate_reference_systems )
            .def( "brep_active_coordinate_reference_systems",
                &brep_active_coordinate_reference_systems )
            .def( "section_active_coordinate_reference_systems",
                &section_active_coordinate_reference_systems )
            .def( "create_brep_coordinate_system",
                &create_brep_coordinate_system )
            .def( "create_section_coordinate_system",
                &create_section_coordinate_system )
            .def( "set_brep_active_coordinate_system",
                &set_brep_active_coordinate_system )
            .def( "set_section_active_coordinate_system",
                &set_section_active_coordinate_system );
    }
} // namespace geode