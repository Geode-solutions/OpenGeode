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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_edges.hpp>
#include <geode/mesh/core/solid_facets.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/helpers/remove_vertex_duplication.hpp>

#include <geode/tests/common.hpp>

void test_point_set()
{
    auto point_set = geode::PointSet2D::create();
    auto builder = geode::PointSetBuilder2D::create( *point_set );
    builder->create_vertices( 5 );
    const geode::Point2D point0{ { 0, 0 } };
    const geode::Point2D point1{ { 1, 0 } };
    builder->set_point( 0, point0 );
    builder->set_point( 1, point1 );
    builder->set_point( 2, point1 );
    builder->set_point( 3, point0 );
    builder->set_point( 4, point1 );
    geode::remove_vertex_duplication( *point_set, *builder );
    OPENGEODE_EXCEPTION( point_set->nb_vertices() == 2,
        "[Test] PointSet should have 2 vertices after process" );
    OPENGEODE_EXCEPTION( point_set->point( 0 ) == point0,
        "[Test] Point 0 coordinates have not been correctly set" );
    OPENGEODE_EXCEPTION( point_set->point( 1 ) == point1,
        "[Test] Point 1 coordinates have not been correctly set" );
}

void test_edged_curve()
{
    auto curve = geode::EdgedCurve2D::create();
    auto builder = geode::EdgedCurveBuilder2D::create( *curve );
    const geode::Point2D point0{ { 0, 0 } };
    const geode::Point2D point1{ { 1, 0 } };
    const geode::Point2D point2{ { 1, 1 } };
    builder->create_vertices( 4 );
    builder->set_point( 0, point0 );
    builder->set_point( 1, point1 );
    builder->set_point( 2, point1 );
    builder->set_point( 3, point2 );
    builder->create_edge( 0, 1 );
    builder->create_edge( 2, 3 );
    geode::remove_vertex_duplication( *curve, *builder );
    OPENGEODE_EXCEPTION( curve->nb_vertices() == 3,
        "[Test] EdgedCurve should have 3 vertices after process" );
    OPENGEODE_EXCEPTION( curve->nb_edges() == 2,
        "[Test] EdgedCurve should have 2 edges after process" );
    OPENGEODE_EXCEPTION( curve->point( 0 ) == point0,
        "[Test] Point 0 coordinates have not been correctly set" );
    OPENGEODE_EXCEPTION( curve->point( 1 ) == point1,
        "[Test] Point 1 coordinates have not been correctly set" );
    OPENGEODE_EXCEPTION( curve->point( 2 ) == point2,
        "[Test] Point 2 coordinates have not been correctly set" );
}

void test_surface_mesh()
{
    auto surface = geode::SurfaceMesh2D::create();
    auto builder = geode::SurfaceMeshBuilder2D::create( *surface );
    const geode::Point2D point0{ { 0, 0 } };
    const geode::Point2D point1{ { 1, 0 } };
    const geode::Point2D point2{ { 1, 1 } };
    const geode::Point2D point3{ { 2, 0 } };
    const geode::Point2D point4{ { 2, 1 } };
    const geode::Point2D point5{ { 3, 2 } };
    const geode::Point2D point6{ { 3, 1 } };
    const geode::Point2D point7{ { 4, 2 } };
    const geode::Point2D point8{ { 3, 0 } };
    const geode::Point2D point9{ { 4, 0 } };
    const geode::Point2D point10{ { 4, 3 } };
    builder->create_vertices( 15 );
    builder->set_point( 0, point0 );
    builder->set_point( 1, point1 );
    builder->set_point( 2, point2 );
    builder->set_point( 3, point3 );
    builder->set_point( 4, point4 );
    builder->set_point( 5, point5 );
    builder->set_point( 6, point6 );
    builder->set_point( 7, point7 );
    builder->set_point( 8, point8 );
    builder->set_point( 9, point9 );
    builder->set_point( 10, point10 );
    builder->set_point( 11, point3 );
    builder->set_point( 12, point5 );
    builder->set_point( 13, point5 );
    builder->set_point( 14, point7 );

    builder->create_polygon( { 0, 2, 1 } );
    builder->create_polygon( { 1, 3, 2 } );
    builder->create_polygon( { 2, 4, 11 } );
    builder->create_polygon( { 4, 6, 5 } );
    builder->create_polygon( { 12, 14, 6 } );
    builder->create_polygon( { 13, 7, 10 } );
    builder->create_polygon( { 6, 8, 9, 14 } );
    builder->create_polygon( { 9, 7, 10 } );
    builder->create_polygon( { 2, 4, 12 } );
    builder->compute_polygon_adjacencies();
    surface->enable_edges();
    geode::remove_vertex_duplication( *surface, *builder );
    OPENGEODE_EXCEPTION( surface->nb_vertices() == 11,
        "[Test] SurfaceMesh should have 12 vertices after process" );
    OPENGEODE_EXCEPTION( surface->nb_polygons() == 9,
        "[Test] SurfaceMesh should have 9 polygons after process" );
    OPENGEODE_EXCEPTION( surface->edges().nb_edges() == 19,
        "[Test] SurfaceMesh should have 18 unique edges after process" );
}

void test_solid_mesh()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    const geode::Point3D point0{ { 0, 0, 0 } };
    const geode::Point3D point1{ { 1, 0, 0 } };
    const geode::Point3D point2{ { 1, 1, 0 } };
    const geode::Point3D point3{ { 2, 0, 0 } };
    const geode::Point3D point4{ { 2, 1, 0 } };
    builder->create_vertices( 8 );
    builder->set_point( 0, point0 );
    builder->set_point( 1, point1 );
    builder->set_point( 2, point2 );
    builder->set_point( 3, point3 );
    builder->set_point( 4, point4 );
    builder->set_point( 5, point1 );
    builder->set_point( 6, point0 );
    builder->set_point( 7, point2 );
    builder->create_tetrahedron( { 0, 2, 1, 3 } );
    builder->create_tetrahedron( { 4, 5, 6, 7 } );
    builder->compute_polyhedron_adjacencies();
    solid->enable_edges();
    solid->enable_facets();
    geode::remove_vertex_duplication( *solid, *builder );
    OPENGEODE_EXCEPTION( solid->nb_vertices() == 5,
        "[Test] SolidMesh should have 5 vertices after process" );
    OPENGEODE_EXCEPTION( solid->nb_polyhedra() == 2,
        "[Test] SolidMesh should have 2 polyhedra after process" );
    OPENGEODE_EXCEPTION( solid->edges().nb_edges() == 9,
        "[Test] SolidMesh should have 9 unique edges after process" );
    OPENGEODE_EXCEPTION( solid->facets().nb_facets() == 7,
        "[Test] SolidMesh should have 7 unique facets after process" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    geode::Logger::set_level( geode::Logger::LEVEL::debug );
    test_point_set();
    test_edged_curve();
    test_surface_mesh();
    test_solid_mesh();
}

OPENGEODE_TEST( "remove-vertex-duplication" )