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

#include <geode/model/mixin/core/vertex_identifier.h>

namespace geode
{
    void define_vertex_identifier( pybind11::module& module )
    {
        pybind11::class_< VertexIdentifier >( module, "VertexIdentifier" )
            .def( pybind11::init<>() )
            .def( "nb_unique_vertices", &VertexIdentifier::nb_unique_vertices )
            .def( "mesh_component_vertices",
                ( const std::vector< MeshComponentVertex >& (
                    VertexIdentifier::*) ( index_t ) const )
                    & VertexIdentifier::mesh_component_vertices )
            .def( "filtered_mesh_component_vertices_by_type",
                ( std::vector< MeshComponentVertex >( VertexIdentifier::* )(
                    index_t, const ComponentType& ) const )
                    & VertexIdentifier::mesh_component_vertices )
            .def( "filtered_mesh_component_vertices_by_id",
                ( std::vector< index_t >( VertexIdentifier::* )(
                    index_t, const uuid& ) const )
                    & VertexIdentifier::mesh_component_vertices )
            .def( "unique_vertex", &VertexIdentifier::unique_vertex );

        pybind11::class_< MeshComponentVertex >( module, "MeshComponentVertex" )
            .def( pybind11::init< ComponentID, index_t >() )
            .def( pybind11::self == pybind11::self )
            .def_readwrite( "component_id", &MeshComponentVertex::component_id )
            .def_readwrite( "vertex", &MeshComponentVertex::vertex );
    }
} // namespace geode
