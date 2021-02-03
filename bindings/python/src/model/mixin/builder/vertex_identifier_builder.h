/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/model/mixin/builder/vertex_identifier_builder.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>

namespace geode
{
    void define_vertex_identifier_builder( pybind11::module& module )
    {
        pybind11::class_< VertexIdentifierBuilder >(
            module, "VertexIdentifierBuilder" )
            .def( pybind11::init< VertexIdentifier& >() )
            .def( "register_block3D_component",
                &VertexIdentifierBuilder::register_mesh_component< Block3D > )
            .def( "register_corner2D_component",
                &VertexIdentifierBuilder::register_mesh_component< Corner2D > )
            .def( "register_corner3D_component",
                &VertexIdentifierBuilder::register_mesh_component< Corner3D > )
            .def( "register_line2D_component",
                &VertexIdentifierBuilder::register_mesh_component< Line2D > )
            .def( "register_line3D_component",
                &VertexIdentifierBuilder::register_mesh_component< Line3D > )
            .def( "register_surface2D_component",
                &VertexIdentifierBuilder::register_mesh_component< Surface2D > )
            .def( "register_surface3D_component",
                &VertexIdentifierBuilder::register_mesh_component< Surface3D > )
            .def( "unregister_block3D_component",
                &VertexIdentifierBuilder::unregister_mesh_component< Block3D > )
            .def( "unregister_corner2D_component",
                &VertexIdentifierBuilder::unregister_mesh_component<
                    Corner2D > )
            .def( "unregister_corner3D_component",
                &VertexIdentifierBuilder::unregister_mesh_component<
                    Corner3D > )
            .def( "unregister_line2D_component",
                &VertexIdentifierBuilder::unregister_mesh_component< Line2D > )
            .def( "unregister_line3D_component",
                &VertexIdentifierBuilder::unregister_mesh_component< Line3D > )
            .def( "unregister_surface2D_component",
                &VertexIdentifierBuilder::unregister_mesh_component<
                    Surface2D > )
            .def( "unregister_surface3D_component",
                &VertexIdentifierBuilder::unregister_mesh_component<
                    Surface3D > )
            .def( "create_unique_vertex",
                &VertexIdentifierBuilder::create_unique_vertex )
            .def( "create_unique_vertices",
                &VertexIdentifierBuilder::create_unique_vertices )
            .def( "set_unique_vertex",
                &VertexIdentifierBuilder::set_unique_vertex )
            .def( "delete_isolated_vertices",
                &VertexIdentifierBuilder::delete_isolated_vertices );
    }
} // namespace geode
