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

#include <geode/basic/attribute_manager.hpp>

#include <geode/mesh/core/vertex_set.hpp>

namespace geode
{
    void define_vertex_set( pybind11::module& module )
    {
        pybind11::class_< VertexSet, Identifier, pybind11::smart_holder >(
            module, "VertexSet", "Set of vertices, the base of every mesh" )
            .def(
                "__repr__",
                []( const VertexSet& mesh ) {
                    return identifier_repr( mesh, mesh.type_name().get(),
                        { { "nb_vertices",
                            python_repr( mesh.nb_vertices() ) } } );
                },
                "Return an unambiguous description of the mesh: its type, its "
                "id, its name if it has one and its number of vertices" )
            .def_static( "create",
                static_cast< std::unique_ptr< VertexSet > ( * )() >(
                    &VertexSet::create ),
                "Create an empty vertex set with the default data structure" )
            .def( "clone", &VertexSet::clone,
                "Return a copy of the vertex set, with the same data "
                "structure, name and identifier" )
            .def( "native_extension", &VertexSet::native_extension,
                "Return the file extension of the native format of the mesh" )
            .def( "impl_name", &VertexSet::impl_name,
                "Return the name of the data structure implementing the mesh" )
            .def( "type_name", &VertexSet::type_name,
                "Return the type of the mesh" )
            .def( "nb_vertices", &VertexSet::nb_vertices,
                "Return the number of vertices" )
            .def( "vertex_attribute_manager",
                &VertexSet::vertex_attribute_manager,
                pybind11::return_value_policy::reference_internal,
                "Return the manager of the attributes associated with the "
                "vertices" );
    }
} // namespace geode
