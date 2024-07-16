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

#include "../../common.hpp"

#include <geode/mesh/builder/hybrid_solid_builder.hpp>
#include <geode/mesh/core/hybrid_solid.hpp>

#define PYTHON_HYBRID_SOLID_BUILDER( dimension )                               \
    const auto name##dimension =                                               \
        "HybridSolidBuilder" + std::to_string( dimension ) + "D";              \
    pybind11::class_< HybridSolidBuilder##dimension##D,                        \
        SolidMeshBuilder##dimension##D >( module, name##dimension.c_str() )    \
        .def_static( "create", &HybridSolidBuilder##dimension##D::create )     \
        .def( "create_tetrahedron",                                            \
            &HybridSolidBuilder##dimension##D::create_tetrahedron )            \
        .def( "create_hexahedron",                                             \
            &HybridSolidBuilder##dimension##D::create_hexahedron )             \
        .def(                                                                  \
            "create_prism", &HybridSolidBuilder##dimension##D::create_prism )  \
        .def( "create_pyramid",                                                \
            &HybridSolidBuilder##dimension##D::create_pyramid )

namespace geode
{
    void define_hybrid_solid_builder( pybind11::module& module )
    {
        PYTHON_HYBRID_SOLID_BUILDER( 3 );
    }
} // namespace geode
