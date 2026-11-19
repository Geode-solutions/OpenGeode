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

#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>

#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/detail/debug.hpp>

namespace geode
{
    void define_debug( pybind11::module& module )
    {
        module
            .def( "save_segment2D", &detail::save_segment< 2 >,
                pybind11::arg( "segment" ), pybind11::arg( "suffix" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Save the segment as the edged curve file "
                "segment_<suffix>.og_edc2d in the current directory; debugging "
                "helper" )
            .def( "save_segment3D", &detail::save_segment< 3 >,
                pybind11::arg( "segment" ), pybind11::arg( "suffix" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Save the segment as the edged curve file "
                "segment_<suffix>.og_edc3d in the current directory; debugging "
                "helper" )
            .def( "save_triangle2D", &detail::save_triangle< 2 >,
                pybind11::arg( "triangle" ), pybind11::arg( "suffix" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Save the triangle as the triangulated surface file "
                "triangle_<suffix>.og_tsf2d in the current directory; "
                "debugging helper" )
            .def( "save_triangle3D", &detail::save_triangle< 3 >,
                pybind11::arg( "triangle" ), pybind11::arg( "suffix" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Save the triangle as the triangulated surface file "
                "triangle_<suffix>.og_tsf3d in the current directory; "
                "debugging helper" )
            .def( "save_triangle2D", &detail::save_triangles< 2 >,
                pybind11::arg( "surface" ), pybind11::arg( "indices" ),
                pybind11::arg( "suffix" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Save each triangle of the surface listed in indices as a "
                "triangulated surface file triangle_<index>_<suffix>.og_tsf2d "
                "in the current directory; debugging helper" )
            .def( "save_triangle3D", &detail::save_triangles< 3 >,
                pybind11::arg( "surface" ), pybind11::arg( "indices" ),
                pybind11::arg( "suffix" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Save each triangle of the surface listed in indices as a "
                "triangulated surface file triangle_<index>_<suffix>.og_tsf3d "
                "in the current directory; debugging helper" )
            .def( "save_tetrahedron", &detail::save_tetrahedron,
                pybind11::arg( "tetrahedron" ), pybind11::arg( "suffix" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Save the tetrahedron as the triangulated surface file "
                "tet_<suffix>.og_tsf3d in the current directory; debugging "
                "helper" )
            .def( "save_tetrahedra", &detail::save_tetrahedra,
                pybind11::arg( "solid" ), pybind11::arg( "indices" ),
                pybind11::arg( "suffix" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Save each tetrahedron of the solid listed in indices as a "
                "triangulated surface file tet_<index>_<suffix>.og_tsf3d in "
                "the current directory; debugging helper" );
    }
} // namespace geode
