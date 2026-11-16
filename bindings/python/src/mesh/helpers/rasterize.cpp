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

#include <geode/mesh/helpers/rasterize.hpp>

#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>

#include <geode/mesh/core/grid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

namespace geode
{
    void define_rasterize( pybind11::module& module )
    {
        module.def( "rasterize_segment2D", &rasterize_segment< 2 >,
            pybind11::arg( "grid" ), pybind11::arg( "segment" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the indices of the cells of the 2D grid crossed by the "
            "segment; only its part inside the grid is rasterized" );
        module.def( "rasterize_segment3D", &rasterize_segment< 3 >,
            pybind11::arg( "grid" ), pybind11::arg( "segment" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the indices of the cells of the 3D grid crossed by the "
            "segment; only its part inside the grid is rasterized" );
        module.def( "conservative_rasterize_segment2D",
            &conservative_rasterize_segment< 2 >, pybind11::arg( "grid" ),
            pybind11::arg( "segment" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the indices of all the cells of the 2D grid touched by the "
            "segment; only its part inside the grid is rasterized" );
        module.def( "conservative_rasterize_segment3D",
            &conservative_rasterize_segment< 3 >, pybind11::arg( "grid" ),
            pybind11::arg( "segment" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the indices of all the cells of the 3D grid touched by the "
            "segment; only its part inside the grid is rasterized" );
        module.def( "conservative_rasterize_triangle2D",
            &conservative_rasterize_triangle< 2 >, pybind11::arg( "grid" ),
            pybind11::arg( "triangle" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the indices of all the cells of the 2D grid touched by the "
            "triangle; only its part inside the grid is rasterized" );
        module.def( "conservative_rasterize_triangle3D",
            &conservative_rasterize_triangle< 3 >, pybind11::arg( "grid" ),
            pybind11::arg( "triangle" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the indices of all the cells of the 3D grid touched by the "
            "triangle; only its part inside the grid is rasterized" );
        module.def( "rasterize_tetrahedron", &rasterize_tetrahedron,
            pybind11::arg( "grid" ), pybind11::arg( "tetrahedron" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the indices of the cells of the 3D grid occupied by the "
            "tetrahedron" );
        module.def( "rasterize_closed_surface", &rasterize_closed_surface,
            pybind11::arg( "grid" ), pybind11::arg( "closed_surface" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the indices of the cells of the 3D grid covered by the "
            "closed triangulated surface, its interior included" );
    }

} // namespace geode