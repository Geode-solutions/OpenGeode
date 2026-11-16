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
#include "../index_check.hpp"

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>

#define PYTHON_TETRAHEDRAL_SOLID( dimension )                                  \
    const auto name##dimension =                                               \
        "TetrahedralSolid" + std::to_string( dimension ) + "D";                \
    pybind11::class_< TetrahedralSolid##dimension##D, SolidMesh##dimension##D, \
        pybind11::smart_holder >( module, name##dimension.c_str(),             \
        "Solid mesh made of tetrahedra only" )                                 \
        .def_static( "create",                                                 \
            static_cast<                                                       \
                std::unique_ptr< TetrahedralSolid##dimension##D > ( * )() >(   \
                &TetrahedralSolid##dimension##D::create ),                     \
            "Create an empty tetrahedral solid with the default data "         \
            "structure" )                                                      \
        .def( "clone", &TetrahedralSolid##dimension##D::clone,                 \
            "Return a copy of the tetrahedral solid, with the same data "      \
            "structure, name and identifier" )                                 \
        .def( "tetrahedron",                                                   \
            geode::checked_snapshot( "tetrahedron",                            \
                &TetrahedralSolid##dimension##D::tetrahedron,                  \
                geode::PolyhedronCheck{} ),                                    \
            pybind11::arg( "tetrahedron_id" ),                                 \
            "Return the geometric tetrahedron of the given tetrahedron of "    \
            "the mesh, built from copies of the tetrahedron points: later "    \
            "changes of the mesh do not modify it" )                           \
        .def( "triangle",                                                      \
            geode::checked_snapshot( "triangle",                               \
                &TetrahedralSolid##dimension##D::triangle,                     \
                geode::PolyhedronFacetCheck{} ),                               \
            pybind11::arg( "facet" ),                                          \
            "Return the geometric triangle of the given tetrahedron facet of " \
            "the mesh, built from copies of the facet points: later changes "  \
            "of the mesh do not modify it" )

namespace geode
{
    void define_tetrahedral_solid( pybind11::module& module )
    {
        PYTHON_TETRAHEDRAL_SOLID( 3 );
    }
} // namespace geode
