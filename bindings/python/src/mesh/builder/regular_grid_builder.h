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

#include <geode/mesh/builder/regular_grid_solid_builder.h>
#include <geode/mesh/builder/regular_grid_surface_builder.h>

#define PYTHON_REGULAR_GRID_BUILDER( Base, dimension )                         \
    const auto name##dimension =                                               \
        "RegularGridBuilder" + std::to_string( dimension ) + "D";              \
    pybind11::class_< RegularGridBuilder##dimension##D,                        \
        Base##Builder##dimension##D >( module, name##dimension.c_str() )       \
        .def_static( "create",                                                 \
            ( std::unique_ptr< RegularGridBuilder##dimension##D >( * )(        \
                RegularGrid< dimension >& ) )                                  \
                & RegularGridBuilder##dimension##D::create )                   \
        .def( "initialize_grid",                                               \
            ( void( RegularGridBuilder##dimension##D::* )(                     \
                const Point##dimension##D&, std::array< index_t, dimension >,  \
                std::array< double, dimension > ) )                            \
                & RegularGridBuilder##dimension##D::initialize_grid )          \
        .def( "update_origin",                                                 \
            &RegularGridBuilder##dimension##D::update_origin )

namespace geode
{
    void define_regular_grid_builder( pybind11::module& module )
    {
        PYTHON_REGULAR_GRID_BUILDER( SurfaceMesh, 2 );
        PYTHON_REGULAR_GRID_BUILDER( SolidMesh, 3 );
    }
} // namespace geode
