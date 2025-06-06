/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace geode
{
    void define_brep_builder( pybind11::module& module )
    {
        pybind11::class_< BRepBuilder, TopologyBuilder, CornersBuilder3D,
            LinesBuilder3D, SurfacesBuilder3D, BlocksBuilder3D,
            ModelBoundariesBuilder3D, CornerCollectionsBuilder3D,
            LineCollectionsBuilder3D, SurfaceCollectionsBuilder3D,
            BlockCollectionsBuilder3D, IdentifierBuilder >(
            module, "BRepBuilder" )
            .def( pybind11::init< BRep& >() )
            .def( "copy", &BRepBuilder::copy )
            .def( "add_corner", static_cast< const uuid& (BRepBuilder::*) () >(
                                    &BRepBuilder::add_corner ) )
            .def( "add_line", static_cast< const uuid& (BRepBuilder::*) () >(
                                  &BRepBuilder::add_line ) )
            .def( "add_surface", static_cast< const uuid& (BRepBuilder::*) () >(
                                     &BRepBuilder::add_surface ) )
            .def( "add_block", static_cast< const uuid& (BRepBuilder::*) () >(
                                   &BRepBuilder::add_block ) )
            .def( "add_model_boundary",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_model_boundary ) )
            .def( "add_corner_collection",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_corner_collection ) )
            .def( "add_line_collection",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_line_collection ) )
            .def( "add_surface_collection",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_surface_collection ) )
            .def( "add_block_collection",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_block_collection ) )
            .def( "remove_corner", &BRepBuilder::remove_corner )
            .def( "remove_line", &BRepBuilder::remove_line )
            .def( "remove_surface", &BRepBuilder::remove_surface )
            .def( "remove_block", &BRepBuilder::remove_block )
            .def( "remove_model_boundary", &BRepBuilder::remove_model_boundary )
            .def( "remove_corner_collection",
                &BRepBuilder::remove_corner_collection )
            .def(
                "remove_line_collection", &BRepBuilder::remove_line_collection )
            .def( "remove_surface_collection",
                &BRepBuilder::remove_surface_collection )
            .def( "remove_block_collection",
                &BRepBuilder::remove_block_collection )
            .def( "add_corner_line_boundary_relationship",
                &BRepBuilder::add_corner_line_boundary_relationship )
            .def( "add_line_surface_boundary_relationship",
                &BRepBuilder::add_line_surface_boundary_relationship )
            .def( "add_surface_block_boundary_relationship",
                &BRepBuilder::add_surface_block_boundary_relationship )
            .def( "add_corner_surface_internal_relationship",
                &BRepBuilder::add_corner_surface_internal_relationship )
            .def( "add_line_surface_internal_relationship",
                &BRepBuilder::add_line_surface_internal_relationship )
            .def( "add_corner_block_internal_relationship",
                &BRepBuilder::add_corner_block_internal_relationship )
            .def( "add_line_block_internal_relationship",
                &BRepBuilder::add_line_block_internal_relationship )
            .def( "add_surface_block_internal_relationship",
                &BRepBuilder::add_surface_block_internal_relationship )
            .def( "add_surface_in_model_boundary",
                &BRepBuilder::add_surface_in_model_boundary )
            .def( "add_corner_in_corner_collection",
                &BRepBuilder::add_corner_in_corner_collection )
            .def( "add_line_in_line_collection",
                &BRepBuilder::add_line_in_line_collection )
            .def( "add_surface_in_surface_collection",
                &BRepBuilder::add_surface_in_surface_collection )
            .def( "add_block_in_block_collection",
                &BRepBuilder::add_block_in_block_collection );
    }
} // namespace geode
