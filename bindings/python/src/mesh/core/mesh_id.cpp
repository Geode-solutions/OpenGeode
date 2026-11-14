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

#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/mesh_id.hpp>

namespace
{
    template < typename Type >
    void define_mesh_id( pybind11::module& module,
        const char* name,
        const char* class_doc,
        const char* init_doc )
    {
        geode::define_string_representation(
            pybind11::class_< Type >( module, name, class_doc ), &Type::get,
            [name]( const Type& id ) {
                return geode::python_call_repr(
                    name, { geode::python_repr( id.get() ) } );
            } )
            .def( pybind11::init< std::string >(), pybind11::arg( "value" ),
                init_doc )
            .def( "get", &Type::get, "Return the name as a string" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__", []( const Type& id ) {
                return std::hash< Type >{}( id );
            } );
    }
} // namespace

namespace geode
{
    void define_mesh_id( pybind11::module& module )
    {
        ::define_mesh_id< MeshImpl >( module, "MeshImpl",
            "Name of a mesh data structure, for example the one used to "
            "create a mesh",
            "Build the name of a data structure from a string" );
        ::define_mesh_id< MeshType >( module, "MeshType",
            "Name of a mesh type, for example the one of a PointSet or of a "
            "TriangulatedSurface",
            "Build the name of a mesh type from a string" );
        pybind11::class_< MeshFactory,
            std::unique_ptr< MeshFactory, pybind11::nodelete > >(
            module, "MeshFactory", "Registry of the mesh data structures" )
            .def_static( "type", &MeshFactory::type,
                "Return the mesh type implemented by the data structure; raise "
                "an error if the data structure is unknown",
                pybind11::arg( "key" ) )
            .def_static( "default_impl", &MeshFactory::default_impl,
                "Return the default data structure of the mesh type; raise an "
                "error if there is none",
                pybind11::arg( "type" ) );
    }
} // namespace geode
