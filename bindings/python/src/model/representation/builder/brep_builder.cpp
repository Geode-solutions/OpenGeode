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
#include "../../model_vertex_check.hpp"

#include <geode/mesh/core/mesh_id.hpp>

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
            BlockCollectionsBuilder3D, PhysicalPropertiesBuilder,
            IdentifierBuilder >( module, "BRepBuilder",
            pybind11::dynamic_attr(),
            "Builder of a BRep: the only way to modify it. It also gives the "
            "builders of the component meshes and inherits the methods of the "
            "topology, component, collection and physical properties builders" )
            .def( pybind11::init< BRep& >(), pybind11::arg( "brep" ),
                pybind11::keep_alive< 1, 2 >(), remember_model(),
                "Build a builder of the BRep, which is kept alive by the "
                "builder" )
            .def( "copy", &BRepBuilder::copy, pybind11::arg( "brep" ),
                "Copy the components, relationships, unique vertices, geometry "
                "and physical properties of brep into the empty BRep, and "
                "return the ModelCopyMapping from the uuids of brep to the new "
                "ones. The name of brep is copied too if it has one. Raise an "
                "error if the BRep is not empty (use BRepConcatener to add to "
                "a non-empty BRep)" )
            .def( "add_corner",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_corner ),
                "Create a corner with the default mesh implementation, "
                "register it in the model (also for the unique vertices) and "
                "return its uuid" )
            .def( "add_line",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_line ),
                "Create a line with the default mesh implementation, register "
                "it in the model (also for the unique vertices) and return its "
                "uuid" )
            .def( "add_surface",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_surface ),
                "Create a surface with the default mesh implementation, "
                "register it in the model (also for the unique vertices) and "
                "return its uuid" )
            .def( "add_block",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_block ),
                "Create a block with the default mesh implementation, register "
                "it in the model (also for the unique vertices) and return its "
                "uuid" )
            .def( "add_corner",
                static_cast< const uuid& (BRepBuilder::*) ( const MeshImpl& ) >(
                    &BRepBuilder::add_corner ),
                pybind11::arg( "impl" ),
                "Create a corner whose mesh uses the given implementation, "
                "register it in the model (also for the unique vertices) and "
                "return its uuid" )
            .def( "add_line",
                static_cast< const uuid& (BRepBuilder::*) ( const MeshImpl& ) >(
                    &BRepBuilder::add_line ),
                pybind11::arg( "impl" ),
                "Create a line whose mesh uses the given implementation, "
                "register it in the model (also for the unique vertices) and "
                "return its uuid" )
            .def( "add_surface",
                static_cast< const uuid& (BRepBuilder::*) ( const MeshImpl& ) >(
                    &BRepBuilder::add_surface ),
                pybind11::arg( "impl" ),
                "Create a surface whose mesh uses the given implementation, "
                "register it in the model (also for the unique vertices) and "
                "return its uuid" )
            .def( "add_block",
                static_cast< const uuid& (BRepBuilder::*) ( const MeshImpl& ) >(
                    &BRepBuilder::add_block ),
                pybind11::arg( "impl" ),
                "Create a block whose mesh uses the given implementation, "
                "register it in the model (also for the unique vertices) and "
                "return its uuid" )
            .def( "add_model_boundary",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_model_boundary ),
                "Create a ModelBoundary, register it in the model and return "
                "its uuid" )
            .def( "add_corner_collection",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_corner_collection ),
                "Create a CornerCollection, register it in the model and "
                "return its uuid" )
            .def( "add_line_collection",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_line_collection ),
                "Create a LineCollection, register it in the model and return "
                "its uuid" )
            .def( "add_surface_collection",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_surface_collection ),
                "Create a SurfaceCollection, register it in the model and "
                "return its uuid" )
            .def( "add_block_collection",
                static_cast< const uuid& (BRepBuilder::*) () >(
                    &BRepBuilder::add_block_collection ),
                "Create a BlockCollection, register it in the model and return "
                "its uuid" )
            .def( "remove_corner", &BRepBuilder::remove_corner,
                pybind11::arg( "corner" ),
                "Remove the corner from the model with all its relationships "
                "and the identification of its vertices; the unique vertices "
                "left without any component vertex are kept "
                "(delete_isolated_vertices removes them)" )
            .def( "remove_line", &BRepBuilder::remove_line,
                pybind11::arg( "line" ),
                "Remove the line from the model with all its relationships and "
                "the identification of its vertices; the unique vertices left "
                "without any component vertex are kept "
                "(delete_isolated_vertices removes them)" )
            .def( "remove_surface", &BRepBuilder::remove_surface,
                pybind11::arg( "surface" ),
                "Remove the surface from the model with all its relationships "
                "and the identification of its vertices; the unique vertices "
                "left without any component vertex are kept "
                "(delete_isolated_vertices removes them)" )
            .def( "remove_block", &BRepBuilder::remove_block,
                pybind11::arg( "block" ),
                "Remove the block from the model with all its relationships "
                "and the identification of its vertices; the unique vertices "
                "left without any component vertex are kept "
                "(delete_isolated_vertices removes them)" )
            .def( "remove_model_boundary", &BRepBuilder::remove_model_boundary,
                pybind11::arg( "boundary" ),
                "Remove the ModelBoundary and its relationships; its items are "
                "kept" )
            .def( "remove_corner_collection",
                &BRepBuilder::remove_corner_collection,
                pybind11::arg( "collection" ),
                "Remove the CornerCollection and its relationships; its items "
                "are kept" )
            .def( "remove_line_collection",
                &BRepBuilder::remove_line_collection,
                pybind11::arg( "collection" ),
                "Remove the LineCollection and its relationships; its items "
                "are kept" )
            .def( "remove_surface_collection",
                &BRepBuilder::remove_surface_collection,
                pybind11::arg( "collection" ),
                "Remove the SurfaceCollection and its relationships; its items "
                "are kept" )
            .def( "remove_block_collection",
                &BRepBuilder::remove_block_collection,
                pybind11::arg( "collection" ),
                "Remove the BlockCollection and its relationships; its items "
                "are kept" )
            .def( "add_corner_line_boundary_relationship",
                &BRepBuilder::add_corner_line_boundary_relationship,
                pybind11::arg( "corner" ), pybind11::arg( "line" ),
                "Make the Corner a boundary of the Line" )
            .def( "add_line_surface_boundary_relationship",
                &BRepBuilder::add_line_surface_boundary_relationship,
                pybind11::arg( "line" ), pybind11::arg( "surface" ),
                "Make the Line a boundary of the Surface" )
            .def( "add_surface_block_boundary_relationship",
                &BRepBuilder::add_surface_block_boundary_relationship,
                pybind11::arg( "surface" ), pybind11::arg( "block" ),
                "Make the Surface a boundary of the Block" )
            .def( "add_corner_line_internal_relationship",
                &BRepBuilder::add_corner_line_internal_relationship,
                pybind11::arg( "corner" ), pybind11::arg( "line" ),
                "Make the Corner internal to the Line" )
            .def( "add_corner_surface_internal_relationship",
                &BRepBuilder::add_corner_surface_internal_relationship,
                pybind11::arg( "corner" ), pybind11::arg( "surface" ),
                "Make the Corner internal to the Surface" )
            .def( "add_line_surface_internal_relationship",
                &BRepBuilder::add_line_surface_internal_relationship,
                pybind11::arg( "line" ), pybind11::arg( "surface" ),
                "Make the Line internal to the Surface" )
            .def( "add_corner_block_internal_relationship",
                &BRepBuilder::add_corner_block_internal_relationship,
                pybind11::arg( "corner" ), pybind11::arg( "block" ),
                "Make the Corner internal to the Block" )
            .def( "add_line_block_internal_relationship",
                &BRepBuilder::add_line_block_internal_relationship,
                pybind11::arg( "line" ), pybind11::arg( "block" ),
                "Make the Line internal to the Block" )
            .def( "add_surface_block_internal_relationship",
                &BRepBuilder::add_surface_block_internal_relationship,
                pybind11::arg( "surface" ), pybind11::arg( "block" ),
                "Make the Surface internal to the Block" )
            .def( "add_surface_in_model_boundary",
                &BRepBuilder::add_surface_in_model_boundary,
                pybind11::arg( "surface" ), pybind11::arg( "boundary" ),
                "Add the Surface to the ModelBoundary" )
            .def( "add_corner_in_corner_collection",
                &BRepBuilder::add_corner_in_corner_collection,
                pybind11::arg( "corner" ), pybind11::arg( "collection" ),
                "Add the Corner to the CornerCollection" )
            .def( "add_line_in_line_collection",
                &BRepBuilder::add_line_in_line_collection,
                pybind11::arg( "line" ), pybind11::arg( "collection" ),
                "Add the Line to the LineCollection" )
            .def( "add_surface_in_surface_collection",
                &BRepBuilder::add_surface_in_surface_collection,
                pybind11::arg( "surface" ), pybind11::arg( "collection" ),
                "Add the Surface to the SurfaceCollection" )
            .def( "add_block_in_block_collection",
                &BRepBuilder::add_block_in_block_collection,
                pybind11::arg( "block" ), pybind11::arg( "collection" ),
                "Add the Block to the BlockCollection" );
    }
} // namespace geode
