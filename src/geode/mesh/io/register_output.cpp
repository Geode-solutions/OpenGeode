/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/mesh/io/detail/register_output.h>

#include <geode/mesh/core/geode_edged_curve.h>
#include <geode/mesh/core/geode_graph.h>
#include <geode/mesh/core/geode_point_set.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode_tetrahedral_solid.h>
#include <geode/mesh/core/geode_triangulated_surface.h>
#include <geode/mesh/io/detail/geode_edged_curve_output.h>
#include <geode/mesh/io/detail/geode_graph_output.h>
#include <geode/mesh/io/detail/geode_point_set_output.h>
#include <geode/mesh/io/detail/geode_polygonal_surface_output.h>
#include <geode/mesh/io/detail/geode_polyhedral_solid_output.h>
#include <geode/mesh/io/detail/geode_tetrahedral_solid_output.h>
#include <geode/mesh/io/detail/geode_triangulated_surface_output.h>

#define BITSERY_OUTPUT_MESH_REGISTER_2D( Mesh )                                \
    geode::Mesh##OutputFactory2D::register_creator<                            \
        geode::OpenGeode##Mesh##Output2D >(                                    \
        geode::OpenGeode##Mesh##2D ::native_extension_static() )

#define BITSERY_OUTPUT_MESH_REGISTER_3D( Mesh )                                \
    geode::Mesh##OutputFactory3D::register_creator<                            \
        geode::OpenGeode##Mesh##Output3D >(                                    \
        geode::OpenGeode##Mesh##3D ::native_extension_static() )

#define BITSERY_OUTPUT_MESH_REGISTER_2D_3D( Mesh )                             \
    BITSERY_OUTPUT_MESH_REGISTER_2D( Mesh );                                   \
    BITSERY_OUTPUT_MESH_REGISTER_3D( Mesh )

namespace geode
{
    void register_geode_mesh_output()
    {
        GraphOutputFactory::register_creator< OpenGeodeGraphOutput >(
            OpenGeodeGraph::native_extension_static() );

        BITSERY_OUTPUT_MESH_REGISTER_2D_3D( PointSet );
        BITSERY_OUTPUT_MESH_REGISTER_2D_3D( EdgedCurve );
        BITSERY_OUTPUT_MESH_REGISTER_2D_3D( PolygonalSurface );
        BITSERY_OUTPUT_MESH_REGISTER_2D_3D( TriangulatedSurface );
        BITSERY_OUTPUT_MESH_REGISTER_3D( PolyhedralSolid );
        BITSERY_OUTPUT_MESH_REGISTER_3D( TetrahedralSolid );
    }
} // namespace geode
