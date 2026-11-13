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

#include <geode/mesh/core/mesh_id.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/section.hpp>

namespace geode
{
    void define_section_builder( pybind11::module& module )
    {
        pybind11::class_< SectionBuilder, TopologyBuilder, CornersBuilder2D,
            LinesBuilder2D, SurfacesBuilder2D, ModelBoundariesBuilder2D,
            CornerCollectionsBuilder2D, LineCollectionsBuilder2D,
            SurfaceCollectionsBuilder2D, IdentifierBuilder >( module,
            "SectionBuilder",
            "Builder of a Section: the only way to modify it. It also gives "
            "the builders of the component meshes and inherits the methods of "
            "the topology, component and collection builders" )
            .def( pybind11::init< Section& >(), pybind11::arg( "section" ),
                pybind11::keep_alive< 1, 2 >(),
                "Build a builder of the Section, which is kept alive by the "
                "builder" )
            .def( "copy", &SectionBuilder::copy, pybind11::arg( "section" ),
                "Copy the components, relationships, unique vertices and "
                "geometry of section into the empty Section, and return the "
                "ModelCopyMapping from the uuids of section to the new ones; "
                "raise an error if the Section is not empty (use "
                "SectionConcatener to add to a non-empty Section)" )
            .def( "add_corner",
                static_cast< const uuid& (SectionBuilder::*) () >(
                    &SectionBuilder::add_corner ),
                "Create a corner with the default mesh implementation, "
                "register it in the model (also for the unique vertices) and "
                "return its uuid" )
            .def( "add_line",
                static_cast< const uuid& (SectionBuilder::*) () >(
                    &SectionBuilder::add_line ),
                "Create a line with the default mesh implementation, register "
                "it in the model (also for the unique vertices) and return its "
                "uuid" )
            .def( "add_surface",
                static_cast< const uuid& (SectionBuilder::*) () >(
                    &SectionBuilder::add_surface ),
                "Create a surface with the default mesh implementation, "
                "register it in the model (also for the unique vertices) and "
                "return its uuid" )
            .def( "add_corner",
                static_cast< const uuid& (
                    SectionBuilder::*) ( const MeshImpl& ) >(
                    &SectionBuilder::add_corner ),
                pybind11::arg( "impl" ),
                "Create a corner whose mesh uses the given implementation, "
                "register it in the model (also for the unique vertices) and "
                "return its uuid" )
            .def( "add_line",
                static_cast< const uuid& (
                    SectionBuilder::*) ( const MeshImpl& ) >(
                    &SectionBuilder::add_line ),
                pybind11::arg( "impl" ),
                "Create a line whose mesh uses the given implementation, "
                "register it in the model (also for the unique vertices) and "
                "return its uuid" )
            .def( "add_surface",
                static_cast< const uuid& (
                    SectionBuilder::*) ( const MeshImpl& ) >(
                    &SectionBuilder::add_surface ),
                pybind11::arg( "impl" ),
                "Create a surface whose mesh uses the given implementation, "
                "register it in the model (also for the unique vertices) and "
                "return its uuid" )
            .def( "add_model_boundary",
                static_cast< const uuid& (SectionBuilder::*) () >(
                    &SectionBuilder::add_model_boundary ),
                "Create a ModelBoundary, register it in the model and return "
                "its uuid" )
            .def( "add_corner_collection",
                static_cast< const uuid& (SectionBuilder::*) () >(
                    &SectionBuilder::add_corner_collection ),
                "Create a CornerCollection, register it in the model and "
                "return its uuid" )
            .def( "add_line_collection",
                static_cast< const uuid& (SectionBuilder::*) () >(
                    &SectionBuilder::add_line_collection ),
                "Create a LineCollection, register it in the model and return "
                "its uuid" )
            .def( "add_surface_collection",
                static_cast< const uuid& (SectionBuilder::*) () >(
                    &SectionBuilder::add_surface_collection ),
                "Create a SurfaceCollection, register it in the model and "
                "return its uuid" )
            .def( "remove_corner", &SectionBuilder::remove_corner,
                pybind11::arg( "corner" ),
                "Remove the corner from the model with all its relationships "
                "and the identification of its vertices; the unique vertices "
                "left without any component vertex are kept "
                "(delete_isolated_vertices removes them)" )
            .def( "remove_line", &SectionBuilder::remove_line,
                pybind11::arg( "line" ),
                "Remove the line from the model with all its relationships and "
                "the identification of its vertices; the unique vertices left "
                "without any component vertex are kept "
                "(delete_isolated_vertices removes them)" )
            .def( "remove_surface", &SectionBuilder::remove_surface,
                pybind11::arg( "surface" ),
                "Remove the surface from the model with all its relationships "
                "and the identification of its vertices; the unique vertices "
                "left without any component vertex are kept "
                "(delete_isolated_vertices removes them)" )
            .def( "remove_model_boundary",
                &SectionBuilder::remove_model_boundary,
                pybind11::arg( "boundary" ),
                "Remove the ModelBoundary and its relationships; its items are "
                "kept" )
            .def( "remove_corner_collection",
                &SectionBuilder::remove_corner_collection,
                pybind11::arg( "collection" ),
                "Remove the CornerCollection and its relationships; its items "
                "are kept" )
            .def( "remove_line_collection",
                &SectionBuilder::remove_line_collection,
                pybind11::arg( "collection" ),
                "Remove the LineCollection and its relationships; its items "
                "are kept" )
            .def( "remove_surface_collection",
                &SectionBuilder::remove_surface_collection,
                pybind11::arg( "collection" ),
                "Remove the SurfaceCollection and its relationships; its items "
                "are kept" )
            .def( "add_corner_line_boundary_relationship",
                &SectionBuilder::add_corner_line_boundary_relationship,
                pybind11::arg( "corner" ), pybind11::arg( "line" ),
                "Make the Corner a boundary of the Line" )
            .def( "add_line_surface_boundary_relationship",
                &SectionBuilder::add_line_surface_boundary_relationship,
                pybind11::arg( "line" ), pybind11::arg( "surface" ),
                "Make the Line a boundary of the Surface" )
            .def( "add_corner_line_internal_relationship",
                &SectionBuilder::add_corner_line_internal_relationship,
                pybind11::arg( "corner" ), pybind11::arg( "line" ),
                "Make the Corner internal to the Line" )
            .def( "add_corner_surface_internal_relationship",
                &SectionBuilder::add_corner_surface_internal_relationship,
                pybind11::arg( "corner" ), pybind11::arg( "surface" ),
                "Make the Corner internal to the Surface" )
            .def( "add_line_surface_internal_relationship",
                &SectionBuilder::add_line_surface_internal_relationship,
                pybind11::arg( "line" ), pybind11::arg( "surface" ),
                "Make the Line internal to the Surface" )
            .def( "add_line_in_model_boundary",
                &SectionBuilder::add_line_in_model_boundary,
                pybind11::arg( "line" ), pybind11::arg( "boundary" ),
                "Add the Line to the ModelBoundary" )
            .def( "add_corner_in_corner_collection",
                &SectionBuilder::add_corner_in_corner_collection,
                pybind11::arg( "corner" ), pybind11::arg( "collection" ),
                "Add the Corner to the CornerCollection" )
            .def( "add_line_in_line_collection",
                &SectionBuilder::add_line_in_line_collection,
                pybind11::arg( "line" ), pybind11::arg( "collection" ),
                "Add the Line to the LineCollection" )
            .def( "add_surface_in_surface_collection",
                &SectionBuilder::add_surface_in_surface_collection,
                pybind11::arg( "surface" ), pybind11::arg( "collection" ),
                "Add the Surface to the SurfaceCollection" );
    }
} // namespace geode
