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

#include "../../../common.hpp"

#include <geode/mesh/core/point_set.hpp>

#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/external_time_series.hpp>

namespace geode
{
    void define_external_time_series( pybind11::module& module )
    {
        pybind11::class_< ExternalTimeSeries >( module, "ExternalTimeSeries",
            "Time series computed outside of a BRep (e.g. by a simulator), "
            "stored apart from the model and written on it on demand with "
            "apply_to()" )
            .def( pybind11::init<>(), "Build empty time series" )
            .def_static( "native_extension_static",
                &ExternalTimeSeries::native_extension_static,
                "Return the extension of the native file of time series" )
            .def( "native_extension", &ExternalTimeSeries::native_extension,
                "Return the extension of the native file of time series" )
            .def( "set_unique_vertices",
                &ExternalTimeSeries::set_unique_vertices,
                pybind11::arg( "vertices" ),
                "Set the simulation unique vertices, a PointSet3D carrying the "
                "vertex values as vertex attributes; the point set is taken "
                "over, do not use it afterwards. Raise an error if it is None" )
            .def( "set_cell_barycenters",
                &ExternalTimeSeries::set_cell_barycenters,
                pybind11::arg( "barycenters" ),
                "Set the simulation cell barycenters, a PointSet3D carrying "
                "the cell values as vertex attributes; the point set is taken "
                "over, do not use it afterwards. Raise an error if it is None" )
            .def( "apply_to", &ExternalTimeSeries::apply_to,
                pybind11::arg( "model" ),
                "Write the transferable attributes on the Block meshes of the "
                "BRep: each unique vertex on the closest BRep unique vertex "
                "(on all its Block vertices), each cell barycenter on the "
                "Block polyhedron with the closest barycenter. Raise an error "
                "if a point is farther than the tolerance (GLOBAL_EPSILON "
                "relative to the largest coordinate when larger than 1) from "
                "its closest Block element, if several points match the same "
                "Block element, or if a point is within the tolerance of "
                "several Block elements" );
    }
} // namespace geode
