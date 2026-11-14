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

#include "../../model_mapping.hpp"

#include "../../../basic/mapping.hpp"

namespace geode
{
    void define_copy_mapping( pybind11::module& module )
    {
        define_model_mapping< BijectiveMapping< uuid > >( module,
            "ModelCopyMapping",
            "Mapping, for each component type, between the uuids of the "
            "components of a source model and the uuids of their copies",
            "BijectiveMappinguuid" );
    }
    void define_model_generic_mapping( pybind11::module& module )
    {
        define_model_mapping< GenericMapping< uuid > >( module,
            "ModelGenericMapping",
            "Mapping, for each component type, between the uuids of the "
            "components of a model before and after an operation, a component "
            "possibly corresponding to several others",
            "GenericMappinguuid" );
    }

    void define_model_mappings( pybind11::module& module )
    {
        define_generic_mapping< MeshElement >( module, "MeshElementMapping",
            "Many-to-many mapping between mesh elements (MeshElement) before "
            "and after an operation" );
        define_generic_mapping< MeshVertex >( module, "MeshVertexMapping",
            "Many-to-many mapping between mesh vertices (MeshVertex) before "
            "and after an operation" );
        define_generic_mapping< MeshElement, ComponentMeshElement >( module,
            "ComponentMeshElementMapping",
            "Many-to-many mapping from mesh elements (MeshElement, the input) "
            "to elements of model components (ComponentMeshElement, the "
            "output)" );
        define_generic_mapping< MeshVertex, ComponentMeshVertex >( module,
            "ComponentMeshVertexMapping",
            "Many-to-many mapping from mesh vertices (MeshVertex, the input) "
            "to vertices of model components (ComponentMeshVertex, the "
            "output)" );
        define_model_mapping< std::vector< uuid > >( module,
            "ModelComponentUuidsMapping",
            "List of component uuids for each component type: the C++ "
            "ModelAddedComponentMapping, ModelRemovedComponentMapping and "
            "ModelUnchangedComponentMapping",
            "list of the uuids" );
        define_model_mapping< MeshElementMapping >( module,
            "ModelMeshesElementMapping",
            "Mapping, for each component type, between the elements of the "
            "component meshes before and after an operation",
            "MeshElementMapping" );
        define_model_mapping< MeshVertexMapping >( module,
            "ModelMeshesVertexMapping",
            "Mapping, for each component type, between the vertices of the "
            "component meshes before and after an operation",
            "MeshVertexMapping" );
        pybind11::class_< ModelMappings >( module, "ModelMappings",
            "Mappings describing how a model was changed by an operation: "
            "components and mesh elements before and after it" )
            .def( pybind11::init<>(), "Build empty mappings" )
            .def_readonly( "component_mapping",
                &ModelMappings::component_mapping,
                "ModelGenericMapping between the uuids of the components "
                "before and after the operation, modifiable" )
            .def_readonly( "added_components", &ModelMappings::added_components,
                "ModelComponentUuidsMapping of the uuids of the components "
                "added by the operation, modifiable" )
            .def_readonly( "removed_components",
                &ModelMappings::removed_components,
                "ModelComponentUuidsMapping of the uuids of the components "
                "removed by the operation, modifiable" )
            .def_readonly( "unchanged_components",
                &ModelMappings::unchanged_components,
                "ModelComponentUuidsMapping of the uuids of the components "
                "left unchanged by the operation, modifiable" )
            .def_readonly( "mesh_element_mapping",
                &ModelMappings::mesh_element_mapping,
                "ModelMeshesElementMapping between the mesh elements before "
                "and after the operation, modifiable" )
            .def_readonly( "mesh_vertices_mapping",
                &ModelMappings::mesh_vertices_mapping,
                "ModelMeshesVertexMapping between the mesh vertices before and "
                "after the operation, modifiable" );
    }
} // namespace geode
