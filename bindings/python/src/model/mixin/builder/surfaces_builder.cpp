/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include "../../../common.hpp"

#include <geode/basic/uuid.hpp>

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/builder/surfaces_builder.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surfaces.hpp>

#define PYTHON_SURFACES_BUILDER( dimension )                                   \
    const auto name##dimension =                                               \
        "SurfacesBuilder" + std::to_string( dimension ) + "D";                 \
    pybind11::class_< SurfacesBuilder##dimension##D >(                         \
        module, name##dimension.c_str() )                                      \
        .def( "surface_mesh_builder",                                          \
            &SurfacesBuilder##dimension##D::surface_mesh_builder<              \
                SurfaceMesh< dimension > > )                                   \
        .def( "set_surface_name",                                              \
            &SurfacesBuilder##dimension##D::set_surface_name )

namespace geode
{
    void define_surfaces_builder( pybind11::module& module )
    {
        PYTHON_SURFACES_BUILDER( 2 );
        PYTHON_SURFACES_BUILDER( 3 );
    }
} // namespace geode
