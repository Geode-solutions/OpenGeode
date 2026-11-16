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

#include <geode/mesh/builder/vertex_set_builder.hpp>
#include <geode/mesh/core/vertex_set.hpp>

namespace geode
{
    void define_vertex_set_builder( pybind11::module& module )
    {
        pybind11::class_< VertexSetBuilder, IdentifierBuilder >( module,
            "VertexSetBuilder", pybind11::dynamic_attr(),
            "Builder to modify a vertex set" )
            .def_static( "create",
                geode::builder_creator( &VertexSetBuilder::create ),
                pybind11::arg( "mesh" ), pybind11::keep_alive< 0, 1 >(),
                "Return the builder to modify the vertex set, which keeps the "
                "mesh alive" )
            .def( "create_vertex", &VertexSetBuilder::create_vertex,
                "Create a vertex and return its index" )
            .def( "create_vertices", &VertexSetBuilder::create_vertices,
                "Create several vertices and return the index of the first one",
                pybind11::arg( "nb" ) )
            .def( "delete_vertices", &VertexSetBuilder::delete_vertices,
                "Delete the vertices whose flag is True and return the mapping "
                "from the old vertex indices to the new ones, NO_ID for a "
                "deleted vertex; to_delete has one flag per vertex",
                pybind11::arg( "to_delete" ) )
            .def( "permute_vertices", &VertexSetBuilder::permute_vertices,
                "Reorder the vertices so that the vertex at position i is the "
                "old vertex permutation[i], and return the mapping from the "
                "old vertex indices to the new ones",
                pybind11::arg( "permutation" ) );
    }
} // namespace geode
