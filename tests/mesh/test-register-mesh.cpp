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

#include <geode/basic/logger.h>

#include <geode/mesh/core/detail/register_mesh.h>

#include <geode/mesh/core/geode_edged_curve.h>
#include <geode/mesh/core/geode_graph.h>
#include <geode/mesh/core/geode_point_set.h>
#include <geode/mesh/core/geode_polygonal_surface.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/core/geode_tetrahedral_solid.h>
#include <geode/mesh/core/geode_triangulated_surface.h>

template < typename GeodeFactory >
void check_register( const geode::MeshType& key )
{
    OPENGEODE_EXCEPTION( GeodeFactory::has_creator( key ),
        std::string( "No creator for " + key.get() + " is not correct" ) );
}

int main()
{
    using namespace geode;

    try
    {
        /* To initialize the mesh library */
        register_geode_mesh();

        /* Run checks */
        check_register< GraphFactory >( OpenGeodeGraph::type_name_static() );
        check_register< PointSetFactory2D >(
            OpenGeodePointSet2D::type_name_static() );
        check_register< PointSetFactory3D >(
            OpenGeodePointSet3D::type_name_static() );
        check_register< EdgedCurveFactory2D >(
            OpenGeodeEdgedCurve2D::type_name_static() );
        check_register< EdgedCurveFactory3D >(
            OpenGeodeEdgedCurve3D::type_name_static() );
        check_register< PolygonalSurfaceFactory2D >(
            OpenGeodePolygonalSurface2D::type_name_static() );
        check_register< PolygonalSurfaceFactory3D >(
            OpenGeodePolygonalSurface3D::type_name_static() );
        check_register< TriangulatedSurfaceFactory2D >(
            OpenGeodeTriangulatedSurface2D::type_name_static() );
        check_register< TriangulatedSurfaceFactory3D >(
            OpenGeodeTriangulatedSurface3D::type_name_static() );
        check_register< PolyhedralSolidFactory3D >(
            OpenGeodePolyhedralSolid3D::type_name_static() );
        check_register< TetrahedralSolidFactory3D >(
            OpenGeodeTetrahedralSolid3D::type_name_static() );

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
