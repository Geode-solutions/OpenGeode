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

#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>

#include <geode/model/mixin/core/block.hpp>

#define PYTHON_BLOCK( dimension )                                              \
    const auto name##dimension = "Block" + std::to_string( dimension ) + "D";  \
    pybind11::class_< Block##dimension##D, Component##dimension##D,            \
        pybind11::smart_holder >( module, name##dimension.c_str(),             \
        "Geometric component of a model described by a SolidMesh" )            \
        .def( "mesh", &Block##dimension##D::mesh< SolidMesh##dimension##D >,   \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the mesh of the block as a SolidMesh; modify it through "  \
            "the mesh builder given by the model builder "                     \
            "(block_mesh_builder)" )                                           \
        .def( "polyhedral_mesh",                                               \
            &Block##dimension##D::mesh< PolyhedralSolid##dimension##D >,       \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the mesh of the block as a PolyhedralSolid; raise a "      \
            "RuntimeError (bad cast) if the mesh is of another type" )         \
        .def( "tetrahedral_mesh",                                              \
            &Block##dimension##D::mesh< TetrahedralSolid##dimension##D >,      \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the mesh of the block as a TetrahedralSolid; raise a "     \
            "RuntimeError (bad cast) if the mesh is of another type" )         \
        .def( "hybrid_mesh",                                                   \
            &Block##dimension##D::mesh< HybridSolid##dimension##D >,           \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the mesh of the block as a HybridSolid; raise a "          \
            "RuntimeError (bad cast) if the mesh is of another type" )         \
        .def( "component_id", &Block##dimension##D::component_id,              \
            "Return the ComponentID (type and uuid) of the component" )        \
        .def_static( "component_type_static",                                  \
            &Block##dimension##D::component_type_static,                       \
            "Return the ComponentType of all the Blocks, without needing an "  \
            "instance" )

namespace geode
{
    void define_block( pybind11::module& module )
    {
        PYTHON_BLOCK( 3 );
    }
} // namespace geode
