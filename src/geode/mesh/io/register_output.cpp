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

#include <geode/mesh/io/geode/register_output.hpp>

#include <geode/mesh/core/geode/geode_edged_curve.hpp>
#include <geode/mesh/core/geode/geode_graph.hpp>
#include <geode/mesh/core/geode/geode_hybrid_solid.hpp>
#include <geode/mesh/core/geode/geode_point_set.hpp>
#include <geode/mesh/core/geode/geode_polygonal_surface.hpp>
#include <geode/mesh/core/geode/geode_polyhedral_solid.hpp>
#include <geode/mesh/core/geode/geode_tetrahedral_solid.hpp>
#include <geode/mesh/core/geode/geode_triangulated_surface.hpp>
#include <geode/mesh/core/geode/geode_vertex_set.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/io/geode/geode_edged_curve_output.hpp>
#include <geode/mesh/io/geode/geode_graph_output.hpp>
#include <geode/mesh/io/geode/geode_hybrid_solid_output.hpp>
#include <geode/mesh/io/geode/geode_point_set_output.hpp>
#include <geode/mesh/io/geode/geode_polygonal_surface_output.hpp>
#include <geode/mesh/io/geode/geode_polyhedral_solid_output.hpp>
#include <geode/mesh/io/geode/geode_regular_grid_output.hpp>
#include <geode/mesh/io/geode/geode_tetrahedral_solid_output.hpp>
#include <geode/mesh/io/geode/geode_triangulated_surface_output.hpp>
#include <geode/mesh/io/geode/geode_vertex_set_output.hpp>
#include <geode/mesh/io/light_regular_grid_output.hpp>

#define BITSERY_OUTPUT_MESH_REGISTER_XD( Mesh, dimension )                     \
    geode::Mesh##OutputFactory##dimension##D::register_creator<                \
        geode::OpenGeode##Mesh##Output##dimension##D >(                        \
        geode::OpenGeode##Mesh##dimension##D ::native_extension_static()       \
            .data() )

#define LIGHT_REGULAR_GRID_OUTPUT_REGISTER_XD( dimension )                     \
    geode::LightRegularGridOutputFactory##dimension##D::register_creator<      \
        geode::LightRegularGridOutput##dimension##D >(                         \
        geode::LightRegularGrid##dimension##D ::native_extension_static()      \
            .data() )

#define BITSERY_OUTPUT_MESH_REGISTER_2D( Mesh )                                \
    BITSERY_OUTPUT_MESH_REGISTER_XD( Mesh, 2 )

#define BITSERY_OUTPUT_MESH_REGISTER_3D( Mesh )                                \
    BITSERY_OUTPUT_MESH_REGISTER_XD( Mesh, 3 )

#define BITSERY_OUTPUT_MESH_REGISTER_2D_3D( Mesh )                             \
    BITSERY_OUTPUT_MESH_REGISTER_2D( Mesh );                                   \
    BITSERY_OUTPUT_MESH_REGISTER_3D( Mesh )

namespace geode
{
    void register_geode_mesh_output()
    {
        VertexSetOutputFactory::register_creator< OpenGeodeVertexSetOutput >(
            OpenGeodeVertexSet::native_extension_static().data() );
        GraphOutputFactory::register_creator< OpenGeodeGraphOutput >(
            OpenGeodeGraph::native_extension_static().data() );

        BITSERY_OUTPUT_MESH_REGISTER_2D_3D( EdgedCurve );
        BITSERY_OUTPUT_MESH_REGISTER_2D_3D( PointSet );
        BITSERY_OUTPUT_MESH_REGISTER_2D_3D( PolygonalSurface );
        BITSERY_OUTPUT_MESH_REGISTER_2D_3D( RegularGrid );
        BITSERY_OUTPUT_MESH_REGISTER_2D_3D( TriangulatedSurface );
        BITSERY_OUTPUT_MESH_REGISTER_3D( HybridSolid );
        BITSERY_OUTPUT_MESH_REGISTER_3D( PolyhedralSolid );
        BITSERY_OUTPUT_MESH_REGISTER_3D( TetrahedralSolid );

        LIGHT_REGULAR_GRID_OUTPUT_REGISTER_XD( 2 );
        LIGHT_REGULAR_GRID_OUTPUT_REGISTER_XD( 3 );
    }
} // namespace geode
