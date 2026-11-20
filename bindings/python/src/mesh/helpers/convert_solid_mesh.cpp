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

#include <geode/geometry/octree.hpp>

#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/helpers/convert_solid_mesh.hpp>

namespace geode
{
    void define_convert_solid_mesh( pybind11::module& module )
    {
        module
            .def( "convert_solid_mesh_into_tetrahedral_solid",
                &convert_solid_mesh_into_tetrahedral_solid,
                pybind11::arg( "solid" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return the solid converted into a tetrahedral solid, or None "
                "if some polyhedron is not a tetrahedron" )
            .def( "convert_grid_into_tetrahedral_solid",
                &convert_grid_into_tetrahedral_solid, pybind11::arg( "grid" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return a tetrahedral solid obtained by splitting every cell "
                "of the 3D grid into tetrahedra" )
            .def( "convert_octree_into_hybrid_solid",
                &convert_octree_into_hybrid_solid, pybind11::arg( "octree" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Return a hybrid solid with one hexahedron per leaf of the 3D "
                "octree (in the order of Octree3D.leaves()), vertices shared "
                "between leaves at the same lattice position, and the cell "
                "attributes of the leaves transferred to the polyhedra. The "
                "mesh is non-conforming at level jumps (no hanging vertex in "
                "the coarser cells, missing adjacencies): it is meant for "
                "visualization and export" );
    }
} // namespace geode
