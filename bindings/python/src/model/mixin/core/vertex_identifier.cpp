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

#include <geode/model/mixin/core/vertex_identifier.hpp>

namespace geode
{
    void define_vertex_identifier( pybind11::module& module )
    {
        auto cls_ComponentMeshVertex =
            define_string_representation(
                pybind11::class_< ComponentMeshVertex >( module,
                    "ComponentMeshVertex",
                    "Identify a vertex of the mesh of a model component by the "
                    "component and the vertex index in its mesh" ),
                []( const ComponentMeshVertex& vertex ) {
                    return python_call_repr( "ComponentMeshVertex",
                        { "component_id=" + python_repr( vertex.component_id ),
                            "vertex_id=" + python_repr( vertex.vertex ) } );
                } )
                .def( pybind11::init< ComponentID, index_t >(),
                    pybind11::arg( "component_id" ),
                    pybind11::arg( "vertex_id" ),
                    "Build the identifier of the vertex vertex_id of the mesh "
                    "of the component" )
                .def(
                    pybind11::self == pybind11::self, pybind11::arg( "other" ) )
                .def( "__hash__",
                    []( const ComponentMeshVertex& value ) {
                        return absl::HashOf(
                            std::hash< ComponentID >{}( value.component_id ),
                            value.vertex );
                    } )

            ;
        def_readwrite_named( cls_ComponentMeshVertex, "component_id",
            &ComponentMeshVertex::component_id,
            "Component whose mesh holds the vertex" );
        def_readwrite_named( cls_ComponentMeshVertex, "vertex",
            &ComponentMeshVertex::vertex,
            "Index of the vertex in the mesh of the component" );

        pybind11::class_< VertexIdentifier, pybind11::smart_holder >( module,
            "VertexIdentifier",
            "Identification of the vertices of the component meshes as unique "
            "vertices: component vertices at the same location of the model "
            "(for instance the end of a Line and its Corner, or the vertices "
            "shared by two Surfaces) share one unique vertex. This is purely "
            "topological" )
            .def( pybind11::init<>(), "Build an empty VertexIdentifier" )
            .def( "nb_unique_vertices", &VertexIdentifier::nb_unique_vertices,
                "Return the number of unique vertices" )
            .def(
                "component_mesh_vertices",
                []( const VertexIdentifier& identifier,
                    index_t unique_vertex_id ) {
                    check_index( "component_mesh_vertices", "unique vertex",
                        unique_vertex_id, identifier.nb_unique_vertices() );
                    return identifier.component_mesh_vertices(
                        unique_vertex_id );
                },
                pybind11::arg( "unique_vertex_id" ),
                "Return the list of ComponentMeshVertex identified with the "
                "unique vertex; raise an IndexError if unique_vertex_id is not "
                "in [0, nb_unique_vertices)" )
            .def(
                "unique_vertex",
                []( TypedObject< VertexIdentifier > self,
                    const ComponentMeshVertex& component_vertex ) {
                    check_model_component_vertex(
                        "unique_vertex", self, component_vertex );
                    return self.cast< const VertexIdentifier& >().unique_vertex(
                        component_vertex );
                },
                pybind11::arg( "component_vertex" ),
                "Return the unique vertex of the component vertex, NO_ID if "
                "the vertex is not identified with any unique vertex. For a "
                "BRep or a Section, raise an OpenGeodeDataError if the "
                "component is not a mesh component of the model and an "
                "IndexError if the vertex is not in its mesh; a "
                "VertexIdentifier alone does not know the meshes: the "
                "component must be registered and the vertex must exist in "
                "its mesh, the result is undefined otherwise" );
    }
} // namespace geode
