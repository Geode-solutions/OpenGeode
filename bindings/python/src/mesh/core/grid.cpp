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
#include <geode/basic/cell_array.hpp>

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/coordinate_system.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/core/grid.hpp>

#define PYTHON_GRID( dimension )                                               \
    const auto name##dimension = "Grid" + std::to_string( dimension ) + "D";   \
    pybind11::class_< Grid##dimension##D, CellArray##dimension##D,             \
        pybind11::smart_holder >( module, name##dimension.c_str(),             \
        "Grid of cells aligned on the axes of a coordinate system, with cell " \
        "and vertex indices given as one index per direction" )                \
        .def( "grid_coordinate_system",                                        \
            &Grid##dimension##D::grid_coordinate_system,                       \
            "Return the coordinate system of the grid: its origin and its "    \
            "cell directions" )                                                \
        .def( "nb_cell_vertices", &Grid##dimension##D::nb_cell_vertices,       \
            "Return the number of vertices of a cell: 4 in 2D, 8 in 3D" )      \
        .def( "cell_length_in_direction",                                      \
            &Grid##dimension##D::cell_length_in_direction,                     \
            "Return the length of a cell along the direction",                 \
            pybind11::arg( "direction" ) )                                     \
        .def( "cell_size", &Grid##dimension##D::cell_size,                     \
            "Return the size of a cell: its area in 2D, its volume in 3D, "    \
            "computed as the product of its lengths" )                         \
        .def( "grid_point", &Grid##dimension##D::grid_point,                   \
            "Return the coordinates of the grid vertex with the given "        \
            "indices",                                                         \
            pybind11::arg( "index" ) )                                         \
        .def( "nb_grid_vertices", &Grid##dimension##D::nb_grid_vertices,       \
            "Return the total number of grid vertices" )                       \
        .def( "nb_vertices_in_direction",                                      \
            &Grid##dimension##D::nb_vertices_in_direction,                     \
            "Return the number of grid vertices along the direction, i.e. "    \
            "the number of cells plus one",                                    \
            pybind11::arg( "direction" ) )                                     \
        .def( "nb_vertices_on_borders",                                        \
            &Grid##dimension##D::nb_vertices_on_borders,                       \
            "Return the number of grid vertices on the border of the grid" )   \
        .def( "vertex_index", &Grid##dimension##D::vertex_index,               \
            "Return the index of the grid vertex with the given indices",      \
            pybind11::arg( "index" ) )                                         \
        .def( "vertex_indices", &Grid##dimension##D::vertex_indices,           \
            "Return the indices along each direction of the grid vertex of "   \
            "the given index",                                                 \
            pybind11::arg( "index" ) )                                         \
        .def( "cell_vertices", &Grid##dimension##D::cell_vertices,             \
            "Return the indices of all the vertices of the cell",              \
            pybind11::arg( "cell_id" ) )                                       \
        .def( "cell_vertex_indices", &Grid##dimension##D::cell_vertex_indices, \
            "Return the indices of a vertex of the cell, vertex_id being the " \
            "local index of the vertex in the cell",                           \
            pybind11::arg( "cell_id" ), pybind11::arg( "vertex_id" ) )         \
        .def( "next_vertex", &Grid##dimension##D::next_vertex,                 \
            "Return the indices of the next grid vertex along the direction, " \
            "or None at the end of the grid",                                  \
            pybind11::arg( "index" ), pybind11::arg( "direction" ) )           \
        .def( "previous_vertex", &Grid##dimension##D::previous_vertex,         \
            "Return the indices of the previous grid vertex along the "        \
            "direction, or None at the start of the grid",                     \
            pybind11::arg( "index" ), pybind11::arg( "direction" ) )           \
        .def( "is_grid_vertex_on_border",                                      \
            &Grid##dimension##D::is_grid_vertex_on_border,                     \
            "Return True if the grid vertex is on the border of the grid",     \
            pybind11::arg( "index" ) )                                         \
        .def( "cell_barycenter", &Grid##dimension##D::cell_barycenter,         \
            "Return the barycenter of the cell", pybind11::arg( "cell_id" ) )  \
        .def( "contains", &Grid##dimension##D::contains,                       \
            "Return True if the point is inside the grid, up to a "            \
            "GLOBAL_EPSILON away from its bounding box",                       \
            pybind11::arg( "query" ) )                                         \
        .def( "closest_vertex", &Grid##dimension##D::closest_vertex,           \
            "Return the indices of the grid vertex closest to the point",      \
            pybind11::arg( "query" ) )                                         \
        .def( "cells", &Grid##dimension##D::cells,                             \
            "Return the indices of the cells containing the point: none if "   \
            "it is outside, several if it is near a cell limit",               \
            pybind11::arg( "query" ) )                                         \
        .def( "cell_attribute_manager",                                        \
            &Grid##dimension##D::cell_attribute_manager,                       \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the manager of the attributes associated with the cells" ) \
        .def( "grid_vertex_attribute_manager",                                 \
            &Grid##dimension##D::grid_vertex_attribute_manager,                \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the manager of the attributes associated with the grid "   \
            "vertices" )                                                       \
        .def( "grid_bounding_box", &Grid##dimension##D::grid_bounding_box,     \
            "Return the bounding box of the grid" )

namespace geode
{
    void define_grid( pybind11::module& module )
    {
        PYTHON_GRID( 2 );
        PYTHON_GRID( 3 );
    }
} // namespace geode
