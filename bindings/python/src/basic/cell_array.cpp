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

#include "../common.hpp"

#include <geode/basic/cell_array.hpp>
#include <geode/basic/range.hpp>

#define PYTHON_ARRAY( dimension )                                              \
    const auto name##dimension =                                               \
        "CellArray" + std::to_string( dimension ) + "D";                       \
    pybind11::class_< CellArray##dimension##D, pybind11::smart_holder >(       \
        module, name##dimension.c_str(),                                       \
        "Regular array of cells indexed by one integer per direction, "        \
        "also numbered by a single cell index" )                               \
        .def( "nb_cell_neighbors",                                             \
            &CellArray##dimension##D::nb_cell_neighbors,                       \
            "Return the maximum number of neighbors of a cell" )               \
        .def( "nb_cells", &CellArray##dimension##D::nb_cells,                  \
            "Return the total number of cells" )                               \
        .def( "nb_cells_in_direction",                                         \
            &CellArray##dimension##D::nb_cells_in_direction,                   \
            pybind11::arg( "direction" ),                                      \
            "Return the number of cells in the given direction" )              \
        .def(                                                                  \
            "cell_index",                                                      \
            []( const CellArray##dimension##D& array,                          \
                const CellArray##dimension##D::CellIndices& index ) {          \
                for( const auto d : LRange{ dimension } )                      \
                {                                                              \
                    OpenGeodeBasicException::check_exception(                  \
                        index[d] < array.nb_cells_in_direction( d ), nullptr,  \
                        OpenGeodeException::TYPE::data,                        \
                        "[CellArray::cell_index] Invalid index ", index[d],    \
                        " in direction ", d );                                 \
                }                                                              \
                return array.cell_index( index );                              \
            },                                                                 \
            pybind11::arg( "index" ),                                          \
            "Return the cell index of the cell with the given indices (one "   \
            "per direction). Raise an error if one is out of range" )          \
        .def(                                                                  \
            "cell_indices",                                                    \
            []( const CellArray##dimension##D& array, index_t index ) {        \
                OpenGeodeBasicException::check_exception(                      \
                    index < array.nb_cells(), nullptr,                         \
                    OpenGeodeException::TYPE::data,                            \
                    "[CellArray::cell_indices] Invalid index ", index );       \
                return array.cell_indices( index );                            \
            },                                                                 \
            pybind11::arg( "index" ),                                          \
            "Return the indices (one per direction) of the cell with the "     \
            "given cell index. Raise an error if it is out of range" )         \
        .def( "next_cell", &CellArray##dimension##D::next_cell,                \
            pybind11::arg( "index" ), pybind11::arg( "direction" ),            \
            "Return the indices of the next cell in the direction, or None "   \
            "if there is none" )                                               \
        .def( "previous_cell", &CellArray##dimension##D::previous_cell,        \
            pybind11::arg( "index" ), pybind11::arg( "direction" ),            \
            "Return the indices of the previous cell in the direction, or "    \
            "None if there is none" )                                          \
        .def( "is_cell_on_border",                                             \
            &CellArray##dimension##D::is_cell_on_border,                       \
            pybind11::arg( "cell_indices" ),                                   \
            "Return True if the cell is on the border of the array" )

namespace geode
{
    void define_cell_array( pybind11::module& module )
    {
        PYTHON_ARRAY( 1 );
        PYTHON_ARRAY( 2 );
        PYTHON_ARRAY( 3 );
    }
} // namespace geode
