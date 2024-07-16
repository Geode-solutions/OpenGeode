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

#include <geode/model/helpers/convert_to_mesh.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/hybrid_solid.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    void define_convert_to_mesh( pybind11::module& module )
    {
        module
            .def( "convert_section_into_curve",
                []( const Section& section ) {
                    return std::get< 0 >(
                        convert_section_into_curve( section ) );
                } )
            .def( "convert_section_into_surface",
                []( const Section& section ) {
                    return std::get< 0 >(
                        convert_section_into_surface( section ) );
                } )
            .def( "convert_brep_into_curve",
                []( const BRep& brep ) {
                    return std::get< 0 >( convert_brep_into_curve( brep ) );
                } )
            .def( "convert_brep_into_surface",
                []( const BRep& brep ) {
                    return std::get< 0 >( convert_brep_into_surface( brep ) );
                } )
            .def( "convert_brep_into_solid", []( const BRep& brep ) {
                return std::get< 0 >( convert_brep_into_solid( brep ) );
            } );
    }
} // namespace geode