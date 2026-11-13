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

#include <geode/geometry/bounding_box.hpp>

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/component_mensurations.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace geode
{
    void define_component_mensurations( pybind11::module& module )
    {
        module
            .def( "block_volume", &block_volume, pybind11::arg( "brep" ),
                pybind11::arg( "block" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return the volume of the Block: the sum of the volumes of its "
                "mesh polyhedra if it is meshed, otherwise the volume enclosed "
                "by its boundary Surfaces (0 if it has none)" )
            .def( "block_bounding_box", &block_bounding_box,
                pybind11::arg( "brep" ), pybind11::arg( "block" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return the bounding box of the Block mesh, or if the Block "
                "has no mesh vertex, of the meshes of its boundary Surfaces "
                "(an empty box if none has vertices)" )
            .def( "surface_area2D", &surface_area< 2 >,
                pybind11::arg( "surface" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return the area of the 2D Surface: the sum of the areas of "
                "its polygons" )
            .def( "surface_area3D", &surface_area< 3 >,
                pybind11::arg( "surface" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return the area of the 3D Surface: the sum of the areas of "
                "its polygons" );
    }
} // namespace geode
