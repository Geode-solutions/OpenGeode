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
                "(delete_isolated_vertices removes them)"
                ". The object passed, and every other Python object of this "
                "Corner or of its mesh (from corner(), corners(), mesh()...), "
                "refer to freed memory afterwards and must not be used: keep "
                "its uuid instead (has_corner)" )
            .def( "remove_line", &BRepBuilder::remove_line,
                pybind11::arg( "line" ),
                "Remove the line from the model with all its relationships and "
                "the identification of its vertices; the unique vertices left "
                "without any component vertex are kept "
                "(delete_isolated_vertices removes them)"
                ". The object passed, and every other Python object of this "
                "Line or of its mesh (from line(), lines(), mesh()...), refer "
                "to freed memory afterwards and must not be used: keep its "
                "uuid instead (has_line)" )
            .def( "remove_surface", &BRepBuilder::remove_surface,
                pybind11::arg( "surface" ),
                "Remove the surface from the model with all its relationships "
                "and the identification of its vertices; the unique vertices "
                "left without any component vertex are kept "
                "(delete_isolated_vertices removes them)"
                ". The object passed, and every other Python object of this "
                "Surface or of its mesh (from surface(), surfaces(), "
                "mesh()...), refer to freed memory afterwards and must not be "
                "used: keep its uuid instead (has_surface)" )
            .def( "remove_block", &BRepBuilder::remove_block,
                pybind11::arg( "block" ),
                "Remove the block from the model with all its relationships "
                "and the identification of its vertices; the unique vertices "
                "left without any component vertex are kept "
                "(delete_isolated_vertices removes them)"
                ". The object passed, and every other Python object of this "
                "Block or of its mesh (from block(), blocks(), mesh()...), "
                "refer to freed memory afterwards and must not be used: keep "
                "its uuid instead (has_block)" )
            .def(
                "remove_corners",
                []( BRepBuilder& builder, const std::vector< uuid >& ids ) {
                    builder.remove_corners( ids );
                },
                pybind11::arg( "ids" ),
                "Remove the Corners of the given uuids, as remove_corner does "
                "for "
                "each one, without needing their Python objects (a repeated "
                "uuid is removed once). Raise an error, before removing "
                "anything, if the model has no Corner with one of the uuids"
                ". The Python objects of the removed Corners and of their "
                "meshes (from corner(), corners(), mesh()...) refer to freed "
                "memory afterwards and must not be used" )
            .def(
                "remove_lines",
                []( BRepBuilder& builder, const std::vector< uuid >& ids ) {
                    builder.remove_lines( ids );
                },
                pybind11::arg( "ids" ),
                "Remove the Lines of the given uuids, as remove_line does for "
                "each one, without needing their Python objects (a repeated "
                "uuid is removed once). Raise an error, before removing "
                "anything, if the model has no Line with one of the uuids"
                ". The Python objects of the removed Lines and of their meshes "
                "(from line(), lines(), mesh()...) refer to freed memory "
                "afterwards and must not be used" )
            .def(
                "remove_surfaces",
                []( BRepBuilder& builder, const std::vector< uuid >& ids ) {
                    builder.remove_surfaces( ids );
                },
                pybind11::arg( "ids" ),
                "Remove the Surfaces of the given uuids, as remove_surface "
                "does for "
                "each one, without needing their Python objects (a repeated "
                "uuid is removed once). Raise an error, before removing "
                "anything, if the model has no Surface with one of the uuids"
                ". The Python objects of the removed Surfaces and of their "
                "meshes (from surface(), surfaces(), mesh()...) refer to freed "
                "memory afterwards and must not be used" )
            .def(
                "remove_blocks",
                []( BRepBuilder& builder, const std::vector< uuid >& ids ) {
                    builder.remove_blocks( ids );
                },
                pybind11::arg( "ids" ),
                "Remove the Blocks of the given uuids, as remove_block does "
                "for "
                "each one, without needing their Python objects (a repeated "
                "uuid is removed once). Raise an error, before removing "
                "anything, if the model has no Block with one of the uuids"
                ". The Python objects of the removed Blocks and of their "
                "meshes (from block(), blocks(), mesh()...) refer to freed "
                "memory afterwards and must not be used" )
            .def( "remove_model_boundary", &BRepBuilder::remove_model_boundary,
                pybind11::arg( "boundary" ),
                "Remove the ModelBoundary and its relationships; its items are "
                "kept"
                ". The object passed, and every other Python object of this "
                "ModelBoundary (from model_boundary(), model_boundaries()...), "
                "refer to freed memory afterwards and must not be used: keep "
                "its uuid instead (has_model_boundary)" )
            .def( "remove_corner_collection",
                &BRepBuilder::remove_corner_collection,
                pybind11::arg( "collection" ),
                "Remove the CornerCollection and its relationships; its items "
                "are kept"
                ". The object passed, and every other Python object of this "
                "CornerCollection (from corner_collection(), "
                "corner_collections()...), refer to freed memory afterwards "
                "and must not be used: keep its uuid instead "
                "(has_corner_collection)" )
            .def( "remove_line_collection",
                &BRepBuilder::remove_line_collection,
                pybind11::arg( "collection" ),
                "Remove the LineCollection and its relationships; its items "
                "are kept"
                ". The object passed, and every other Python object of this "
                "LineCollection (from line_collection(), "
                "line_collections()...), refer to freed memory afterwards and "
                "must not be used: keep its uuid instead "
                "(has_line_collection)" )
            .def( "remove_surface_collection",
                &BRepBuilder::remove_surface_collection,
                pybind11::arg( "collection" ),
                "Remove the SurfaceCollection and its relationships; its items "
                "are kept"
                ". The object passed, and every other Python object of this "
                "SurfaceCollection (from surface_collection(), "
                "surface_collections()...), refer to freed memory afterwards "
                "and must not be used: keep its uuid instead "
                "(has_surface_collection)" )
            .def( "remove_block_collection",
                &BRepBuilder::remove_block_collection,
                pybind11::arg( "collection" ),
                "Remove the BlockCollection and its relationships; its items "
                "are kept"
                ". The object passed, and every other Python object of this "
                "BlockCollection (from block_collection(), "
                "block_collections()...), refer to freed memory afterwards and "
                "must not be used: keep its uuid instead "
                "(has_block_collection)" )
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
