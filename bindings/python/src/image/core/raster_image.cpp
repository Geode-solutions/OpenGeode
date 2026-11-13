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

#include <geode/image/core/raster_image.hpp>
#include <geode/image/core/rgb_color.hpp>

#define PYTHON_RASTER_IMAGE( dimension )                                       \
    const auto name##dimension =                                               \
        "RasterImage" + std::to_string( dimension ) + "D";                     \
    pybind11::class_< RasterImage##dimension##D, CellArray##dimension##D,      \
        Identifier, pybind11::smart_holder >( module, name##dimension.c_str(), \
        "Regular grid of colored cells (pixels or voxels)" )                   \
        .def(                                                                  \
            "__repr__",                                                        \
            []( const RasterImage##dimension##D& object ) {                    \
                return identifier_repr( object, python_type_name( object ),    \
                    { { "nb_cells", python_repr( object.nb_cells() ) } } );    \
            },                                                                 \
            "Return an unambiguous description of the image: its type, its "   \
            "id, its name if it has one and its number of cells" )             \
        .def( pybind11::init< std::array< index_t, dimension > >(),            \
            pybind11::arg( "cells_number" ),                                   \
            "Build a raster image with the given number of cells in each "     \
            "direction" )                                                      \
        .def( "native_extension",                                              \
            &RasterImage##dimension##D::native_extension,                      \
            "Return the extension of the native OpenGeode file format of the " \
            "raster image" )                                                   \
        .def(                                                                  \
            "color",                                                           \
            []( const RasterImage##dimension##D& raster, index_t index ) {     \
                check_raster_cell( raster, index );                            \
                return raster.color( index );                                  \
            },                                                                 \
            pybind11::arg( "index" ),                                          \
            "Return the color of the cell with the given cell index. Raise "   \
            "an error if the index is out of range" )                          \
        .def(                                                                  \
            "set_color",                                                       \
            []( RasterImage##dimension##D& raster, index_t index,              \
                RGBColor color ) {                                             \
                check_raster_cell( raster, index );                            \
                raster.set_color( index, std::move( color ) );                 \
            },                                                                 \
            pybind11::arg( "index" ), pybind11::arg( "color" ),                \
            "Set the color of the cell with the given cell index. Raise an "   \
            "error if the index is out of range" )                             \
        .def( "clone", &RasterImage##dimension##D::clone,                      \
            "Return a copy of the raster image" )

namespace geode
{
    template < index_t dimension >
    void check_raster_cell(
        const RasterImage< dimension >& raster, index_t index )
    {
        OpenGeodeImageException::check_exception( index < raster.nb_cells(),
            nullptr, OpenGeodeException::TYPE::data,
            "[RasterImage] Invalid cell index ", index, " (", raster.nb_cells(),
            " cells)" );
    }

    void define_raster_image( pybind11::module& module )
    {
        PYTHON_RASTER_IMAGE( 1 );
        PYTHON_RASTER_IMAGE( 2 );
        PYTHON_RASTER_IMAGE( 3 );
    }
} // namespace geode
