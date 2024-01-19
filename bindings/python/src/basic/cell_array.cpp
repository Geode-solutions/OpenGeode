/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include "../common.h"

#include <geode/basic/cell_array.h>

#define PYTHON_ARRAY( dimension )                                              \
    const auto name##dimension =                                               \
        "CellArray" + std::to_string( dimension ) + "D";                       \
    pybind11::class_< CellArray##dimension##D >(                               \
        module, name##dimension.c_str() )                                      \
        .def(                                                                  \
            "nb_cell_neighbors", &CellArray##dimension##D::nb_cell_neighbors ) \
        .def( "nb_cells", &CellArray##dimension##D::nb_cells )                 \
        .def( "nb_cells_in_direction",                                         \
            &CellArray##dimension##D::nb_cells_in_direction )                  \
        .def( "cell_index", &CellArray##dimension##D::cell_index )             \
        .def( "cell_indices", &CellArray##dimension##D::cell_indices )         \
        .def( "next_cell", &CellArray##dimension##D::next_cell )               \
        .def( "previous_cell", &CellArray##dimension##D::previous_cell )       \
        .def(                                                                  \
            "is_cell_on_border", &CellArray##dimension##D::is_cell_on_border )

namespace geode
{
    void define_cell_array( pybind11::module& module )
    {
        PYTHON_ARRAY( 2 );
        PYTHON_ARRAY( 3 );
    }
} // namespace geode
