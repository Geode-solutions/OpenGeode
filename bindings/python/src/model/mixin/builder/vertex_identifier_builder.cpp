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

#include <geode/model/mixin/builder/vertex_identifier_builder.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>

namespace geode
{
    void define_vertex_identifier_builder( pybind11::module& module )
    {
        pybind11::class_< VertexIdentifierBuilder >( module,
            "VertexIdentifierBuilder",
            "Builder modifying the VertexIdentifier of a model" )
            .def( pybind11::init< VertexIdentifier& >(),
                pybind11::arg( "vertex_identifier" ),
                pybind11::keep_alive< 1, 2 >(),
                "Build a builder of the given VertexIdentifier, kept alive by "
                "the builder" )
            .def( "register_block3D_component",
                &VertexIdentifierBuilder::register_mesh_component< Block3D >,
                pybind11::arg( "component" ),
                "Register the Block 3D so that its vertices can be identified "
                "with unique vertices, and create the attribute storing the "
                "identification on its mesh; raise an error if it is already "
                "registered, which is the case of the components created "
                "through the model builder (add_xxx)" )
            .def( "register_corner2D_component",
                &VertexIdentifierBuilder::register_mesh_component< Corner2D >,
                pybind11::arg( "component" ),
                "Register the Corner 2D so that its vertices can be identified "
                "with unique vertices, and create the attribute storing the "
                "identification on its mesh; raise an error if it is already "
                "registered, which is the case of the components created "
                "through the model builder (add_xxx)" )
            .def( "register_corner3D_component",
                &VertexIdentifierBuilder::register_mesh_component< Corner3D >,
                pybind11::arg( "component" ),
                "Register the Corner 3D so that its vertices can be identified "
                "with unique vertices, and create the attribute storing the "
                "identification on its mesh; raise an error if it is already "
                "registered, which is the case of the components created "
                "through the model builder (add_xxx)" )
            .def( "register_line2D_component",
                &VertexIdentifierBuilder::register_mesh_component< Line2D >,
                pybind11::arg( "component" ),
                "Register the Line 2D so that its vertices can be identified "
                "with unique vertices, and create the attribute storing the "
                "identification on its mesh; raise an error if it is already "
                "registered, which is the case of the components created "
                "through the model builder (add_xxx)" )
            .def( "register_line3D_component",
                &VertexIdentifierBuilder::register_mesh_component< Line3D >,
                pybind11::arg( "component" ),
                "Register the Line 3D so that its vertices can be identified "
                "with unique vertices, and create the attribute storing the "
                "identification on its mesh; raise an error if it is already "
                "registered, which is the case of the components created "
                "through the model builder (add_xxx)" )
            .def( "register_surface2D_component",
                &VertexIdentifierBuilder::register_mesh_component< Surface2D >,
                pybind11::arg( "component" ),
                "Register the Surface 2D so that its vertices can be "
                "identified with unique vertices, and create the attribute "
                "storing the identification on its mesh; raise an error if it "
                "is already registered, which is the case of the components "
                "created through the model builder (add_xxx)" )
            .def( "register_surface3D_component",
                &VertexIdentifierBuilder::register_mesh_component< Surface3D >,
                pybind11::arg( "component" ),
                "Register the Surface 3D so that its vertices can be "
                "identified with unique vertices, and create the attribute "
                "storing the identification on its mesh; raise an error if it "
                "is already registered, which is the case of the components "
                "created through the model builder (add_xxx)" )
            .def( "unregister_block3D_component",
                &VertexIdentifierBuilder::unregister_mesh_component< Block3D >,
                pybind11::arg( "component" ),
                "Unregister the Block 3D: its vertices are removed from their "
                "unique vertices, which are kept even if no component vertex "
                "remains, and the attribute storing the identification is "
                "deleted from its mesh; raise an error if it is not "
                "registered" )
            .def( "unregister_corner2D_component",
                &VertexIdentifierBuilder::unregister_mesh_component< Corner2D >,
                pybind11::arg( "component" ),
                "Unregister the Corner 2D: its vertices are removed from their "
                "unique vertices, which are kept even if no component vertex "
                "remains, and the attribute storing the identification is "
                "deleted from its mesh; raise an error if it is not "
                "registered" )
            .def( "unregister_corner3D_component",
                &VertexIdentifierBuilder::unregister_mesh_component< Corner3D >,
                pybind11::arg( "component" ),
                "Unregister the Corner 3D: its vertices are removed from their "
                "unique vertices, which are kept even if no component vertex "
                "remains, and the attribute storing the identification is "
                "deleted from its mesh; raise an error if it is not "
                "registered" )
            .def( "unregister_line2D_component",
                &VertexIdentifierBuilder::unregister_mesh_component< Line2D >,
                pybind11::arg( "component" ),
                "Unregister the Line 2D: its vertices are removed from their "
                "unique vertices, which are kept even if no component vertex "
                "remains, and the attribute storing the identification is "
                "deleted from its mesh; raise an error if it is not "
                "registered" )
            .def( "unregister_line3D_component",
                &VertexIdentifierBuilder::unregister_mesh_component< Line3D >,
                pybind11::arg( "component" ),
                "Unregister the Line 3D: its vertices are removed from their "
                "unique vertices, which are kept even if no component vertex "
                "remains, and the attribute storing the identification is "
                "deleted from its mesh; raise an error if it is not "
                "registered" )
            .def( "unregister_surface2D_component",
                &VertexIdentifierBuilder::unregister_mesh_component<
                    Surface2D >,
                pybind11::arg( "component" ),
                "Unregister the Surface 2D: its vertices are removed from "
                "their unique vertices, which are kept even if no component "
                "vertex remains, and the attribute storing the identification "
                "is deleted from its mesh; raise an error if it is not "
                "registered" )
            .def( "unregister_surface3D_component",
                &VertexIdentifierBuilder::unregister_mesh_component<
                    Surface3D >,
                pybind11::arg( "component" ),
                "Unregister the Surface 3D: its vertices are removed from "
                "their unique vertices, which are kept even if no component "
                "vertex remains, and the attribute storing the identification "
                "is deleted from its mesh; raise an error if it is not "
                "registered" )
            .def( "create_unique_vertex",
                &VertexIdentifierBuilder::create_unique_vertex,
                "Create an unique vertex identified with no component vertex "
                "and return its index" )
            .def( "create_unique_vertices",
                &VertexIdentifierBuilder::create_unique_vertices,
                pybind11::arg( "nb" ),
                "Create nb unique vertices identified with no component vertex "
                "and return the index of the first one" )
            .def( "set_unique_vertex",
                &VertexIdentifierBuilder::set_unique_vertex,
                pybind11::arg( "component_vertex_id" ),
                pybind11::arg( "unique_vertex_id" ),
                "Identify the component vertex with the unique vertex, after "
                "detaching it from the unique vertex it was identified with, "
                "if any. The component must be registered, the vertex must "
                "exist in its mesh and the unique vertex must exist (none of "
                "this is checked)" )
            .def( "update_unique_vertices",
                &VertexIdentifierBuilder::update_unique_vertices,
                pybind11::arg( "component_id" ), pybind11::arg( "old2new" ),
                "Update the identification after the mesh of the component "
                "changed: the vertex i of the former mesh becomes old2new[i], "
                "and is detached from its unique vertex if old2new[i] is "
                "NO_ID. old2new needs one value per former vertex of the mesh" )
            .def( "delete_isolated_vertices",
                &VertexIdentifierBuilder::delete_isolated_vertices,
                "Delete the unique vertices identified with no component "
                "vertex and return the mapping from the old to the new unique "
                "vertex indices (NO_ID for the deleted ones, identity if there "
                "was none)" );
    }
} // namespace geode
