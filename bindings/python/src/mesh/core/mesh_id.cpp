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
    void define_mesh_id( pybind11::module& module, const char* name )
    {
        pybind11::class_< Type >( module, name )
            .def( pybind11::init< std::string >() )
            .def( "get", &Type::get )
            .def( "__str__", &Type::get )
            .def( "__repr__", &Type::get )
            .def( pybind11::self == pybind11::self )
            .def( pybind11::self != pybind11::self )
            .def( "__hash__", []( const Type& id ) {
                return std::hash< Type >{}( id );
            } );
    }
} // namespace

namespace geode
{
    void define_mesh_id( pybind11::module& module )
    {
        ::define_mesh_id< MeshImpl >( module, "MeshImpl" );
        ::define_mesh_id< MeshType >( module, "MeshType" );
        pybind11::class_< MeshFactory,
            std::unique_ptr< MeshFactory, pybind11::nodelete > >(
            module, "MeshFactory" )
            .def_static( "type", &MeshFactory::type )
            .def_static( "default_impl", &MeshFactory::default_impl );
    }
} // namespace geode
