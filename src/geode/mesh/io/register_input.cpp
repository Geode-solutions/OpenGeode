/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/mesh/io/detail/register_input.h>

#include <geode/mesh/core/geode_edged_curve.h>
#include <geode/mesh/core/geode_graph.h>
#include <geode/mesh/core/geode_point_set.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode_tetrahedral_solid.h>
#include <geode/mesh/core/geode_triangulated_surface.h>
#include <geode/mesh/io/detail/geode_edged_curve_input.h>
#include <geode/mesh/io/detail/geode_graph_input.h>
#include <geode/mesh/io/detail/geode_point_set_input.h>
#include <geode/mesh/io/detail/geode_polygonal_surface_input.h>
#include <geode/mesh/io/detail/geode_polyhedral_solid_input.h>
#include <geode/mesh/io/detail/geode_tetrahedral_solid_input.h>
#include <geode/mesh/io/detail/geode_triangulated_surface_input.h>

#define BITSERY_INPUT_MESH_REGISTER_2D( Mesh )                                 \
    geode::Mesh##InputFactory2D::register_creator<                             \
        geode::OpenGeode##Mesh##Input2D >(                                     \
        geode::OpenGeode##Mesh##2D ::native_extension_static().data() )

#define BITSERY_INPUT_MESH_REGISTER_3D( Mesh )                                 \
    geode::Mesh##InputFactory3D::register_creator<                             \
        geode::OpenGeode##Mesh##Input3D >(                                     \
        geode::OpenGeode##Mesh##3D ::native_extension_static().data() )

#define BITSERY_INPUT_MESH_REGISTER_2D_3D( Mesh )                              \
    BITSERY_INPUT_MESH_REGISTER_2D( Mesh );                                    \
    BITSERY_INPUT_MESH_REGISTER_3D( Mesh )

namespace geode
{
    void register_geode_mesh_input()
    {
        GraphInputFactory::register_creator< OpenGeodeGraphInput >(
            OpenGeodeGraph::native_extension_static().data() );

        BITSERY_INPUT_MESH_REGISTER_2D_3D( PointSet );
        BITSERY_INPUT_MESH_REGISTER_2D_3D( EdgedCurve );
        BITSERY_INPUT_MESH_REGISTER_2D_3D( PolygonalSurface );
        BITSERY_INPUT_MESH_REGISTER_2D_3D( TriangulatedSurface );
        BITSERY_INPUT_MESH_REGISTER_3D( PolyhedralSolid );
        BITSERY_INPUT_MESH_REGISTER_3D( TetrahedralSolid );
    }
} // namespace geode
