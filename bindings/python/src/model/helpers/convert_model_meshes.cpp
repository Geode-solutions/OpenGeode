/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/model/helpers/convert_model_meshes.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    void define_convert_model_meshes( pybind11::module& module )
    {
        module
            .def( "triangulate_section_surface_meshes",
                static_cast< void ( * )( Section& ) >(
                    &triangulate_surface_meshes ) )
            .def( "triangulate_brep_surface_meshes",
                static_cast< void ( * )( BRep& ) >(
                    &triangulate_surface_meshes ) )
            .def( "convert_section_surface_meshes_into_triangulated_surfaces",
                static_cast< void ( * )( Section& ) >(
                    &convert_surface_meshes_into_triangulated_surfaces ) )
            .def( "convert_brep_surface_meshes_into_triangulated_surfaces",
                static_cast< void ( * )( BRep& ) >(
                    &convert_surface_meshes_into_triangulated_surfaces ) )
            .def( "convert_brep_block_meshes_into_tetrahedral_solids",
                static_cast< void ( * )( BRep& ) >(
                    &convert_block_meshes_into_tetrahedral_solids ) );
    }
} // namespace geode