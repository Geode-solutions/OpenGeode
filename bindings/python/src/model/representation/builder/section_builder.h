/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/model/representation/builder/section_builder.h>

namespace geode
{
    void define_section_builder( pybind11::module& module )
    {
        pybind11::class_< SectionBuilder, TopologyBuilder, CornersBuilder2D,
            LinesBuilder2D, SurfacesBuilder2D, ModelBoundariesBuilder2D,
            IdentifierBuilder >( module, "SectionBuilder" )
            .def( pybind11::init< Section& >() )
            .def( "copy", &SectionBuilder::copy )
            .def( "add_corner", ( const uuid& (SectionBuilder::*) () )
                                    & SectionBuilder::add_corner )
            .def( "add_line", ( const uuid& (SectionBuilder::*) () )
                                  & SectionBuilder::add_line )
            .def( "add_surface", ( const uuid& (SectionBuilder::*) () )
                                     & SectionBuilder::add_surface )
            .def( "add_model_boundary", &SectionBuilder::add_model_boundary )
            .def( "remove_corner", &SectionBuilder::remove_corner )
            .def( "remove_line", &SectionBuilder::remove_line )
            .def( "remove_surface", &SectionBuilder::remove_surface )
            .def( "remove_model_boundary",
                &SectionBuilder::remove_model_boundary )
            .def( "add_corner_line_boundary_relationship",
                &SectionBuilder::add_corner_line_boundary_relationship )
            .def( "add_line_surface_boundary_relationship",
                &SectionBuilder::add_line_surface_boundary_relationship )
            .def( "add_corner_surface_internal_relationship",
                &SectionBuilder::add_corner_surface_internal_relationship )
            .def( "add_line_surface_internal_relationship",
                &SectionBuilder::add_line_surface_internal_relationship )
            .def( "add_line_in_model_boundary",
                &SectionBuilder::add_line_in_model_boundary );
    }
} // namespace geode
