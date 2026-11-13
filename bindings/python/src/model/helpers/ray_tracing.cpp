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

#include <pybind11/native_enum.h>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/model/helpers/ray_tracing.hpp>

namespace geode
{
    void define_model_ray_tracing( pybind11::module& module )
    {
        pybind11::native_enum< RayTracingResult::POSITION >( module, "POSITION",
            "enum.Enum",
            "Position of a point relative to a block: outside, inside or "
            "on_border (on one of its boundary surfaces)" )
            .value( "outside", RayTracingResult::POSITION::outside )
            .value( "inside", RayTracingResult::POSITION::inside )
            .value( "on_border", RayTracingResult::POSITION::on_border )
            .finalize();

        auto cls_RayTracingResult =
            pybind11::class_< RayTracingResult >( module, "RayTracingResult",
                "Result of a point-in-block query: its position is outside, "
                "inside or on_border; the result is True (bool) only when the "
                "position is inside" )
                .def( pybind11::init<>(),
                    "Build a result whose position is outside" )
                .def( "__bool__", &RayTracingResult::operator bool );
        def_readwrite_named( cls_RayTracingResult, "position",
            &RayTracingResult::position,
            "POSITION of the point relative to the block: outside, inside or "
            "on_border" );

        pybind11::class_< BRepRayTracing >( module, "BRepRayTracing",
            "Locate points in the Blocks of a BRep by casting rays against the "
            "boundary Surfaces; the acceleration structures of the Surfaces "
            "are built on first use and kept, so create a new object after "
            "modifying the BRep" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "brep" ),
                pybind11::keep_alive< 1, 2 >(),
                "Build a ray tracing of the BRep, which is kept alive by the "
                "object" )
            .def( "is_point_inside_block",
                &BRepRayTracing::is_point_inside_block,
                pybind11::arg( "point" ), pybind11::arg( "block" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return the RayTracingResult of the point in the Block, "
                "deduced from the number of intersections of rays with its "
                "boundary Surfaces (they must enclose the Block); the result "
                "is True only for 'inside', not for 'on_border'. Raise an "
                "error if the position remains ambiguous" )
            .def( "block_containing_point",
                &BRepRayTracing::block_containing_point,
                pybind11::arg( "point" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return the uuid of the first Block of the BRep that strictly "
                "contains the point (not on its border), or None if there is "
                "none" );
    }
} // namespace geode