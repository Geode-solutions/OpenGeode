/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include "../../common.h"

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    void define_vertex_set( pybind11::module& module )
    {
        pybind11::class_< VertexSet, Identifier >( module, "VertexSet" )
            .def_static( "create",
                ( std::unique_ptr< VertexSet >( * )() ) & VertexSet::create )
            .def( "clone", &VertexSet::clone )
            .def( "native_extension", &VertexSet::native_extension )
            .def( "nb_vertices", &VertexSet::nb_vertices )
            .def( "vertex_attribute_manager",
                &VertexSet::vertex_attribute_manager,
                pybind11::return_value_policy::reference )
            .def( "impl_name", &VertexSet::impl_name );
    }
} // namespace geode
