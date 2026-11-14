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

#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/regular_grid_solid_builder.hpp>
#include <geode/mesh/builder/regular_grid_surface_builder.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>

#define PYTHON_REGULAR_GRID_BUILDER( Base, dimension )                         \
    const auto name##dimension =                                               \
        "RegularGridBuilder" + std::to_string( dimension ) + "D";              \
    pybind11::class_< RegularGridBuilder##dimension##D,                        \
        Base##Builder##dimension##D >( module, name##dimension.c_str(),        \
        "Builder to initialize and modify a regular grid" )                    \
        .def_static( "create",                                                 \
            geode::builder_creator(                                            \
                &RegularGridBuilder##dimension##D::create ),                   \
            pybind11::arg( "grid" ), pybind11::keep_alive< 0, 1 >(),           \
            "Return the builder to modify the regular grid, which keeps the "  \
            "grid alive" )                                                     \
        .def( "initialize_grid",                                               \
            static_cast< void ( RegularGridBuilder##dimension##D::* )(         \
                const Point##dimension##D&, std::array< index_t, dimension >,  \
                std::array< double, dimension > ) >(                           \
                &RegularGridBuilder##dimension##D::initialize_grid ),          \
            "Create the vertices and the cells of the grid from its origin, "  \
            "the number of cells and the cell length in each direction",       \
            pybind11::arg( "origin" ), pybind11::arg( "cells_number" ),        \
            pybind11::arg( "cells_length" ) )                                  \
        .def( "initialize_cartesian_grid",                                     \
            static_cast< void ( RegularGridBuilder##dimension##D::* )(         \
                const Point##dimension##D&, std::array< index_t, dimension >,  \
                double ) >(                                                    \
                &RegularGridBuilder##dimension##D::initialize_grid ),          \
            "Create the vertices and the cells of the grid from its origin, "  \
            "the number of cells in each direction and one cell length "       \
            "shared by all the directions",                                    \
            pybind11::arg( "origin" ), pybind11::arg( "cells_number" ),        \
            pybind11::arg( "cells_length" ) )                                  \
        .def( "initialize_grid_with_directions",                               \
            static_cast< void ( RegularGridBuilder##dimension##D::* )(         \
                Point##dimension##D, std::array< index_t, dimension >,         \
                std::array< Vector##dimension##D, dimension > ) >(             \
                &RegularGridBuilder##dimension##D::initialize_grid ),          \
            "Create the vertices and the cells of the grid from its origin, "  \
            "the number of cells in each direction and the vector of a cell "  \
            "along each direction",                                            \
            pybind11::arg( "origin" ), pybind11::arg( "cells_number" ),        \
            pybind11::arg( "cell_directions" ) )                               \
        .def( "update_origin",                                                 \
            &RegularGridBuilder##dimension##D::update_origin,                  \
            "Move the grid so that its origin is the given point, updating "   \
            "its vertices",                                                    \
            pybind11::arg( "origin" ) )

namespace geode
{
    void define_regular_grid_builder( pybind11::module& module )
    {
        PYTHON_REGULAR_GRID_BUILDER( SurfaceMesh, 2 );
        PYTHON_REGULAR_GRID_BUILDER( SolidMesh, 3 );
    }
} // namespace geode
