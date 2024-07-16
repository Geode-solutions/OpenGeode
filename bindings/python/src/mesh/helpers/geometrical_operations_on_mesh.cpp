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

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/hybrid_solid_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/polygonal_surface_builder.hpp>
#include <geode/mesh/builder/polyhedral_solid_builder.hpp>
#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/geometrical_operations_on_mesh.hpp>

#define PYTHON_TRANSLATE( mesh, mesh_name, dimension )                         \
    const auto translate_name##mesh##dimension = absl::StrCat(                 \
        "translate_", mesh_name, std::to_string( dimension ), "D" );           \
    module.def( translate_name##mesh##dimension.c_str(),                       \
        &translate_mesh< mesh, mesh##Builder, dimension > )

#define PYTHON_RESCALE( mesh, mesh_name, dimension )                           \
    const auto rescale_name##mesh##dimension = absl::StrCat(                   \
        "rescale_", mesh_name, std::to_string( dimension ), "D" );             \
    module.def( rescale_name##mesh##dimension.c_str(),                         \
        &rescale_mesh< mesh, mesh##Builder, dimension > )

namespace geode
{
    void define_geometrical_operations_on_mesh( pybind11::module& module )
    {
        PYTHON_TRANSLATE( PointSet, "point_set", 2 );
        PYTHON_TRANSLATE( PointSet, "point_set", 3 );
        PYTHON_TRANSLATE( EdgedCurve, "edged_curve", 2 );
        PYTHON_TRANSLATE( EdgedCurve, "edged_curve", 3 );
        PYTHON_TRANSLATE( TriangulatedSurface, "triangulated_surface", 2 );
        PYTHON_TRANSLATE( TriangulatedSurface, "triangulated_surface", 3 );
        PYTHON_TRANSLATE( PolygonalSurface, "polygonal_surface", 2 );
        PYTHON_TRANSLATE( PolygonalSurface, "polygonal_surface", 3 );
        PYTHON_TRANSLATE( SurfaceMesh, "surface_mesh", 2 );
        PYTHON_TRANSLATE( SurfaceMesh, "surface_mesh", 3 );
        PYTHON_TRANSLATE( HybridSolid, "hybrid_solid", 3 );
        PYTHON_TRANSLATE( PolyhedralSolid, "polyhedral_solid", 3 );
        PYTHON_TRANSLATE( TetrahedralSolid, "tetrahedral_solid", 3 );
        PYTHON_TRANSLATE( SolidMesh, "solid_mesh", 3 );

        PYTHON_RESCALE( PointSet, "point_set", 2 );
        PYTHON_RESCALE( PointSet, "point_set", 3 );
        PYTHON_RESCALE( EdgedCurve, "edged_curve", 2 );
        PYTHON_RESCALE( EdgedCurve, "edged_curve", 3 );
        PYTHON_RESCALE( TriangulatedSurface, "triangulated_surface", 2 );
        PYTHON_RESCALE( TriangulatedSurface, "triangulated_surface", 3 );
        PYTHON_RESCALE( PolygonalSurface, "polygonal_surface", 2 );
        PYTHON_RESCALE( PolygonalSurface, "polygonal_surface", 3 );
        PYTHON_RESCALE( SurfaceMesh, "surface_mesh", 2 );
        PYTHON_RESCALE( SurfaceMesh, "surface_mesh", 3 );
        PYTHON_RESCALE( HybridSolid, "hybrid_solid", 3 );
        PYTHON_RESCALE( PolyhedralSolid, "polyhedral_solid", 3 );
        PYTHON_RESCALE( TetrahedralSolid, "tetrahedral_solid", 3 );
        PYTHON_RESCALE( SolidMesh, "solid_mesh", 3 );
    }
} // namespace geode