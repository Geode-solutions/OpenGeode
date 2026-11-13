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

#include <geode/mesh/core/mesh_element.hpp>

namespace geode
{
    void define_mesh_element( pybind11::module& module )
    {
        auto element = define_string_representation(
            pybind11::class_< MeshElement >( module, "MeshElement",
                "Element of a mesh identified by the unique id of the mesh "
                "and the index of the element in the mesh" ) );
        element
            .def( pybind11::init<>(),
                "Build an element with a default uuid and an invalid index "
                "(NO_ID)" )
            .def( pybind11::init< uuid, index_t >(), pybind11::arg( "mesh_id" ),
                pybind11::arg( "element_id" ),
                "Build the element element_id of the mesh identified by "
                "mesh_id" )
            .def( pybind11::self == pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self != pybind11::self, pybind11::arg( "other" ) )
            .def( pybind11::self < pybind11::self, pybind11::arg( "other" ) )
            .def( "__hash__", []( const MeshElement& value ) {
                return absl::HashOf( value );
            } );
        def_readwrite_named( element, "mesh_id", &MeshElement::mesh_id,
            "Unique id of the mesh" );
        def_readwrite_named( element, "element_id", &MeshElement::element_id,
            "Index of the element in the mesh" );
    }
} // namespace geode
