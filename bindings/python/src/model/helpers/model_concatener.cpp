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

#include <geode/model/helpers/model_concatener.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/mapping.hpp>
#include <geode/model/representation/core/section.hpp>

namespace geode
{
    void define_model_concatener( pybind11::module& module )
    {
        pybind11::class_< BRepConcatener >( module, "BRepConcatener",
            "Add the content of other BRep to a BRep" )
            .def( pybind11::init< BRep& >(), pybind11::arg( "model" ),
                pybind11::keep_alive< 1, 2 >(),
                "Build a concatener adding to the BRep, which is kept alive by "
                "it" )
            .def( "concatenate", &BRepConcatener::concatenate,
                pybind11::arg( "other_model" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Copy all the components of other_model into the BRep with new "
                "uuids, together with their geometry and relationships, and "
                "return the ModelCopyMapping from the uuids of other_model to "
                "the new ones; raise an OpenGeodeDataError if other_model is "
                "the BRep itself" );

        pybind11::class_< SectionConcatener >( module, "SectionConcatener",
            "Add the content of other Sections to a Section" )
            .def( pybind11::init< Section& >(), pybind11::arg( "model" ),
                pybind11::keep_alive< 1, 2 >(),
                "Build a concatener adding to the Section, which is kept alive "
                "by it" )
            .def( "concatenate", &SectionConcatener::concatenate,
                pybind11::arg( "other_model" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Copy all the components of other_model into the Section with "
                "new uuids, together with their geometry and relationships, "
                "and return the ModelCopyMapping from the uuids of other_model "
                "to the new ones; raise an OpenGeodeDataError if other_model "
                "is the Section itself" );
    }
} // namespace geode
