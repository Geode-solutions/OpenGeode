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

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/graph.hpp>
#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/polyhedral_solid.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/core/vertex_set.hpp>

namespace
{
    template < typename Mesh >
    bool cast_to( const geode::VertexSet& mesh, pybind11::object& result )
    {
        const auto* derived = dynamic_cast< const Mesh* >( &mesh );
        if( derived == nullptr )
        {
            return false;
        }
        result =
            pybind11::cast( derived, pybind11::return_value_policy::reference );
        return true;
    }

    template < typename... Meshes >
    pybind11::object most_derived( const geode::VertexSet& mesh )
    {
        pybind11::object result;
        if( ( cast_to< Meshes >( mesh, result ) || ... ) )
        {
            return result;
        }
        return pybind11::cast(
            &mesh, pybind11::return_value_policy::reference );
    }
} // namespace

namespace geode
{
    void define_bound_interface( pybind11::module& module )
    {
        // The meshes of the model components are returned by their C++
        // static type (e.g. SurfaceMesh3D): pybind11 cannot downcast them to
        // their implementation class (OpenGeodeTriangulatedSurface3D), which
        // is not bound. Used by the generic functions of basic.py.
        module.def( "_bound_interface",
            &most_derived< TriangulatedSurface2D, TriangulatedSurface3D,
                PolygonalSurface2D, PolygonalSurface3D, RegularGrid2D,
                RegularGrid3D, TetrahedralSolid3D, PolyhedralSolid3D,
                HybridSolid3D, SurfaceMesh2D, SurfaceMesh3D, SolidMesh3D,
                EdgedCurve2D, EdgedCurve3D, Graph, PointSet2D, PointSet3D >,
            pybind11::arg( "mesh" ), pybind11::keep_alive< 0, 1 >(),
            "Return the mesh as its most derived bound class (e.g. a "
            "SurfaceMesh3D that is a triangulated surface as a "
            "TriangulatedSurface3D), the same C++ object kept alive by the "
            "result" );
    }
} // namespace geode
