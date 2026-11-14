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

#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/model/helpers/convert_brep_section.hpp>

namespace geode
{
    void define_convert_brep_section( pybind11::module& module )
    {
        module.def( "convert_brep_into_section", &convert_brep_into_section,
            pybind11::arg( "brep" ), pybind11::arg( "axis_to_remove" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return (Section, ModelCopyMapping): the Corners, Lines and "
            "Surfaces of the BRep with their relationships and unique "
            "vertices, projected by removing the coordinate axis_to_remove (0, "
            "1 or 2); the Blocks are not converted. The mapping goes from the "
            "BRep uuids to the Section ones" );
        module.def( "convert_section_into_brep", &convert_section_into_brep,
            pybind11::arg( "section" ), pybind11::arg( "axis_to_add" ),
            pybind11::arg( "axis_coordinate" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return (BRep, ModelCopyMapping): the Corners, Lines and Surfaces "
            "of the Section with their relationships and unique vertices, "
            "embedded in 3D by inserting the coordinate axis_to_add (0, 1 or "
            "2) set to axis_coordinate for all points. The mapping goes from "
            "the Section uuids to the BRep ones" );
        auto cls_SectionExtruderOptions =
            pybind11::class_< SectionExtruderOptions >( module,
                "SectionExtruderOptions",
                "Options of extrude_section_to_brep: the axis along which the "
                "Section is extruded and the two coordinates, on this axis, of "
                "the extrusion ends" )
                .def( pybind11::init<>(),
                    "Build options with no axis to extrude (it must be set), "
                    "and min and max coordinates 0" )

            ;
        def_readwrite_named( cls_SectionExtruderOptions, "axis_to_extrude",
            &SectionExtruderOptions::axis_to_extrude,
            "Axis (0, 1 or 2) along which the Section is extruded; it must be "
            "set" );
        def_readwrite_named( cls_SectionExtruderOptions, "min_coordinate",
            &SectionExtruderOptions::min_coordinate,
            "Coordinate on the extrusion axis of the first end of the "
            "extrusion" );
        def_readwrite_named( cls_SectionExtruderOptions, "max_coordinate",
            &SectionExtruderOptions::max_coordinate,
            "Coordinate on the extrusion axis of the second end of the "
            "extrusion" );
        module.def( "extrude_section_to_brep", &extrude_section_to_brep,
            pybind11::arg( "section" ), pybind11::arg( "options" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return a BRep obtained by extruding the Section along "
            "options.axis_to_extrude between options.min_coordinate and "
            "options.max_coordinate: the Section is copied at both ends and "
            "each Corner, Line and Surface is swept into a Line, a Surface and "
            "a Block (hybrid solid mesh). Raise an error if a Surface has a "
            "polygon other than a triangle or a quadrangle" );
    }
} // namespace geode
