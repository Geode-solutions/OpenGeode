/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/regular_grid.h>

#define PYTHON_REGULAR_GRID( dimension )                                       \
    const auto name##dimension =                                               \
        "RegularGrid" + std::to_string( dimension ) + "D";                     \
    pybind11::class_< RegularGrid##dimension##D >(                             \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init< Point< dimension >,                              \
            std::array< index_t, dimension >,                                  \
            std::array< double, dimension > >() )                              \
        .def( pybind11::init< Point< dimension >,                              \
            std::array< index_t, dimension >, double >() )                     \
        .def(                                                                  \
            "native_extension", &RegularGrid##dimension##D::native_extension ) \
        .def(                                                                  \
            "nb_cell_vertices", &RegularGrid##dimension##D::nb_cell_vertices ) \
        .def( "origin", &RegularGrid##dimension##D::origin )                   \
        .def( "nb_cells", ( index_t( RegularGrid< dimension >::* )() const )   \
                              & RegularGrid##dimension##D::nb_cells )          \
        .def( "nb_cells_in_direction",                                         \
            ( index_t( RegularGrid< dimension >::* )( index_t ) const )        \
                & RegularGrid##dimension##D::nb_cells )                        \
        .def( "cell_length", &RegularGrid##dimension##D::cell_length )         \
        .def( "cell_size", &RegularGrid##dimension##D::cell_size )             \
        .def( "cell_index", &RegularGrid##dimension##D::cell_index )           \
        .def( "cell_indices", &RegularGrid##dimension##D::cell_indices )       \
        .def( "next_cell", &RegularGrid##dimension##D::next_cell )             \
        .def( "previous_cell", &RegularGrid##dimension##D::previous_cell )     \
        .def(                                                                  \
            "nb_vertices", ( index_t( RegularGrid< dimension >::* )() const )  \
                               & RegularGrid##dimension##D::nb_vertices )      \
        .def( "nb_vertices_in_direction",                                      \
            ( index_t( RegularGrid< dimension >::* )( index_t ) const )        \
                & RegularGrid##dimension##D::nb_vertices )                     \
        .def( "vertex_index", &RegularGrid##dimension##D::vertex_index )       \
        .def( "vertex_indices", &RegularGrid##dimension##D::vertex_indices )   \
        .def( "next_vertex", &RegularGrid##dimension##D::next_vertex )         \
        .def( "previous_vertex", &RegularGrid##dimension##D::previous_vertex ) \
        .def( "is_vertex_on_border",                                           \
            &RegularGrid##dimension##D::is_vertex_on_border )                  \
        .def( "cell_vertices", &RegularGrid##dimension##D::cell_vertices )     \
        .def( "cell_vertex_indices",                                           \
            &RegularGrid##dimension##D::cell_vertex_indices )                  \
        .def( "point", &RegularGrid##dimension##D::point )                     \
        .def( "cells", &RegularGrid##dimension##D::cells )                     \
        .def( "contains", &RegularGrid##dimension##D::contains )               \
        .def( "closest_vertex", &RegularGrid##dimension##D::closest_vertex )   \
        .def( "cell_barycenter", &RegularGrid##dimension##D::cell_barycenter ) \
        .def( "cell_attribute_manager",                                        \
            &RegularGrid##dimension##D::cell_attribute_manager,                \
            pybind11::return_value_policy::reference )                         \
        .def( "vertex_attribute_manager",                                      \
            &RegularGrid##dimension##D::vertex_attribute_manager,              \
            pybind11::return_value_policy::reference )                         \
        .def( "clone", &RegularGrid##dimension##D::clone )                     \
        .def( "bounding_box", &RegularGrid##dimension##D::bounding_box )

namespace geode
{
    void define_regular_grid( pybind11::module& module )
    {
        PYTHON_REGULAR_GRID( 2 );
        PYTHON_REGULAR_GRID( 3 );
    }
} // namespace geode
