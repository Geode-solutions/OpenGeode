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

#include <geode/basic/cell_array.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/grid.h>

#define PYTHON_GRID( dimension )                                               \
    const auto name##dimension = "Grid" + std::to_string( dimension ) + "D";   \
    pybind11::class_< Grid##dimension##D, CellArray##dimension##D >(           \
        module, name##dimension.c_str() )                                      \
        .def( "nb_cell_vertices", &Grid##dimension##D::nb_cell_vertices )      \
        .def( "origin", &Grid##dimension##D::origin )                          \
        .def( "cell_length_in_direction",                                      \
            &Grid##dimension##D::cell_length_in_direction )                    \
        .def( "cell_size", &Grid##dimension##D::cell_size )                    \
        .def( "nb_vertices_in_direction",                                      \
            &Grid##dimension##D::nb_vertices_in_direction )                    \
        .def( "vertex_index", &Grid##dimension##D::vertex_index )              \
        .def( "vertex_indices", &Grid##dimension##D::vertex_indices )          \
        .def( "next_vertex", &Grid##dimension##D::next_vertex )                \
        .def( "previous_vertex", &Grid##dimension##D::previous_vertex )        \
        .def( "is_grid_vertex_on_border",                                      \
            &Grid##dimension##D::is_grid_vertex_on_border )                    \
        .def( "cell_vertices", &Grid##dimension##D::cell_vertices )            \
        .def(                                                                  \
            "cell_vertex_indices", &Grid##dimension##D::cell_vertex_indices )  \
        .def( "cells", &Grid##dimension##D::cells )                            \
        .def( "contains", &Grid##dimension##D::contains )                      \
        .def( "closest_vertex", &Grid##dimension##D::closest_vertex )          \
        .def( "grid_bounding_box", &Grid##dimension##D::grid_bounding_box )

namespace geode
{
    void define_grid( pybind11::module& module )
    {
        PYTHON_GRID( 2 );
        PYTHON_GRID( 3 );
    }
} // namespace geode
