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

#include "../../common.hpp"

#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/model/helpers/convert_brep_section.hpp>

namespace geode
{
    void define_convert_brep_section( pybind11::module& module )
    {
        module.def( "convert_brep_into_section", &convert_brep_into_section );
        module.def( "convert_section_into_brep", &convert_section_into_brep );
        pybind11::class_< SectionExtruderOptions >(
            module, "SectionExtruderOptions" )
            .def( pybind11::init<>() )
            .def_readwrite(
                "axis_to_extrude", &SectionExtruderOptions::axis_to_extrude )
            .def_readwrite(
                "min_coordinate", &SectionExtruderOptions::min_coordinate )
            .def_readwrite(
                "max_coordinate", &SectionExtruderOptions::max_coordinate );
        module.def( "extrude_section_to_brep", &extrude_section_to_brep );
    }
} // namespace geode
