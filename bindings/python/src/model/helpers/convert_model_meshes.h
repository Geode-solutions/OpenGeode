/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <pybind11/operators.h>

#include <geode/model/helpers/convert_model_meshes.h>

namespace geode
{
    void define_convert_model_meshes( pybind11::module& module )
    {
        module
            .def( "convert_section_surface_meshes_into_triangulated_surfaces",
                ( void ( * )( Section& ) )
                    & convert_surface_meshes_into_triangulated_surfaces )
            .def( "convert_brep_surface_meshes_into_triangulated_surfaces",
                ( void ( * )( BRep& ) )
                    & convert_surface_meshes_into_triangulated_surfaces )
            .def( "convert_brep_block_meshes_into_tetrahedral_solids",
                &convert_block_meshes_into_tetrahedral_solids );
    }
} // namespace geode