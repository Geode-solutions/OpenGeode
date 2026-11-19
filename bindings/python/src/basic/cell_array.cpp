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

namespace geode
{
    template < index_t dimension >
    void check_cell_indices( std::string_view name,
        const CellArray< dimension >& array,
        const typename CellArray< dimension >::CellIndices& index )
    {
        check_grid_indices( name, "cell index", index, [&array]( index_t d ) {
            return array.nb_cells_in_direction( d );
        } );
    }

    template < index_t dimension >
    void check_cell_array_arguments( std::string_view name,
        const CellArray< dimension >& array,
        const typename CellArray< dimension >::CellIndices& index,
        index_t direction )
    {
        check_direction( name, direction, dimension );
        check_cell_indices( name, array, index );
    }
} // namespace geode

#define PYTHON_ARRAY( dimension )                                              \
    const auto name##dimension =                                               \
        "CellArray" + std::to_string( dimension ) + "D";                       \
    pybind11::class_< CellArray##dimension##D, pybind11::smart_holder >(       \
        module, name##dimension.c_str(),                                       \
        "Regular array of cells indexed by one integer per direction, "        \
        "also numbered by a single cell index" )                               \
        .def( "nb_cell_neighbors",                                             \
            &CellArray##dimension##D::nb_cell_neighbors,                       \
            "Return the number of neighbors of an inner cell (2 per "          \
            "direction)" )                                                     \
        .def( "nb_cells", &CellArray##dimension##D::nb_cells,                  \
            "Return the total number of cells" )                               \
        .def(                                                                  \
            "nb_cells_in_direction",                                           \
            []( const CellArray##dimension##D& array, index_t direction ) {    \
                check_direction(                                               \
                    "nb_cells_in_direction", direction, dimension );           \
                return array.nb_cells_in_direction( direction );               \
            },                                                                 \
            pybind11::arg( "direction" ),                                      \
            "Return the number of cells in the given direction. Raise "        \
            "IndexError if the direction is not in [0, dimension)" )           \
        .def(                                                                  \
            "cell_index",                                                      \
            []( const CellArray##dimension##D& array,                          \
                const CellArray##dimension##D::CellIndices& index ) {          \
                check_cell_indices( "cell_index", array, index );              \
                return array.cell_index( index );                              \
            },                                                                 \
            pybind11::arg( "index" ),                                          \
            "Return the cell index of the cell with the given indices (one "   \
            "per direction). Raise IndexError if one is not in [0, "           \
            "nb_cells_in_direction)" )                                         \
        .def(                                                                  \
            "cell_indices",                                                    \
            []( const CellArray##dimension##D& array, index_t index ) {        \
                check_index(                                                   \
                    "cell_indices", "cell", index, array.nb_cells() );         \
                return array.cell_indices( index );                            \
            },                                                                 \
            pybind11::arg( "index" ),                                          \
            "Return the indices (one per direction) of the cell with the "     \
            "given cell index. Raise IndexError if it is not in [0, "          \
            "nb_cells)" )                                                      \
        .def(                                                                  \
            "next_cell",                                                       \
            []( const CellArray##dimension##D& array,                          \
                const CellArray##dimension##D::CellIndices& index,             \
                index_t direction ) {                                          \
                check_cell_array_arguments(                                    \
                    "next_cell", array, index, direction );                    \
                return array.next_cell( index, direction );                    \
            },                                                                 \
            pybind11::arg( "index" ), pybind11::arg( "direction" ),            \
            "Return the indices of the next cell in the direction, or None "   \
            "if the cell is the last one in this direction. Raise IndexError " \
            "if the direction is not in [0, dimension) or an index is not in " \
            "[0, nb_cells_in_direction)" )                                     \
        .def(                                                                  \
            "previous_cell",                                                   \
            []( const CellArray##dimension##D& array,                          \
                const CellArray##dimension##D::CellIndices& index,             \
                index_t direction ) {                                          \
                check_cell_array_arguments(                                    \
                    "previous_cell", array, index, direction );                \
                return array.previous_cell( index, direction );                \
            },                                                                 \
            pybind11::arg( "index" ), pybind11::arg( "direction" ),            \
            "Return the indices of the previous cell in the direction, or "    \
            "None if the cell is the first one in this direction. Raise "      \
            "IndexError if the direction is not in [0, dimension) or an "      \
            "index is not in [0, nb_cells_in_direction)" )                     \
        .def(                                                                  \
            "is_cell_on_border",                                               \
            []( const CellArray##dimension##D& array,                          \
                const CellArray##dimension##D::CellIndices& cell_indices ) {   \
                check_cell_indices(                                            \
                    "is_cell_on_border", array, cell_indices );                \
                return array.is_cell_on_border( cell_indices );                \
            },                                                                 \
            pybind11::arg( "cell_indices" ),                                   \
            "Return True if the cell is on the border of the array. Raise "    \
            "IndexError if an index is not in [0, nb_cells_in_direction)" )

namespace geode
{
    void define_cell_array( pybind11::module& module )
    {
        PYTHON_ARRAY( 1 );
        PYTHON_ARRAY( 2 );
        PYTHON_ARRAY( 3 );
    }
} // namespace geode
