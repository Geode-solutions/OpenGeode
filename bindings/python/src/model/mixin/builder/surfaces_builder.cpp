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
    pybind11::class_< SurfacesBuilder##dimension##D >( module,                 \
        name##dimension.c_str(),                                               \
        "Builder modifying the Surfaces of a model, in particular their "      \
        "meshes" )                                                             \
        .def(                                                                  \
            "surface_mesh_builder",                                            \
            []( SurfacesBuilder##dimension##D& builder,                        \
                const Surface##dimension##D& component ) {                     \
                return geode::builder_with_mesh(                               \
                    builder.surface_mesh_builder< SurfaceMesh< dimension > >(  \
                        component ),                                           \
                    component.mesh() );                                        \
            },                                                                 \
            pybind11::arg( "surface" ), pybind11::keep_alive< 0, 1 >(),        \
            "Return the SurfaceMeshBuilder modifying the mesh of the "         \
            "surface; it keeps the model builder alive" )                      \
        .def( "set_surface_name",                                              \
            &SurfacesBuilder##dimension##D::set_surface_name,                  \
            pybind11::arg( "surface" ), pybind11::arg( "name" ),               \
            "Set the name of the surface and of its mesh" )

namespace geode
{
    void define_surfaces_builder( pybind11::module& module )
    {
        PYTHON_SURFACES_BUILDER( 2 );
        PYTHON_SURFACES_BUILDER( 3 );
    }
} // namespace geode
