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

#include <geode/basic/logger.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/helpers/aabb_solid_helpers.hpp>

#include <geode/tests/common.hpp>

bool inexact_equal( double nb1, double nb2, double eps )
{
    return std::abs( nb1 - nb2 ) < eps;
}

void build_test_solid( geode::TetrahedralSolid3D& solid )
{
    auto builder = geode::TetrahedralSolidBuilder3D::create( solid );
    builder->create_vertices( 8 );
    builder->set_point( 0, geode::Point3D{ { 0, 0, 0 } } );
    builder->set_point( 1, geode::Point3D{ { 1, 0, 0 } } );
    builder->set_point( 2, geode::Point3D{ { 1, 1, 0 } } );
    builder->set_point( 3, geode::Point3D{ { 0, 1, 0 } } );
    builder->set_point( 4, geode::Point3D{ { 0, 0, 1 } } );
    builder->set_point( 5, geode::Point3D{ { 1, 0, 1 } } );
    builder->set_point( 6, geode::Point3D{ { 1, 1, 1 } } );
    builder->set_point( 7, geode::Point3D{ { 0, 1, 1 } } );
    builder->create_tetrahedron( { 0, 4, 1, 3 } );
    builder->create_tetrahedron( { 1, 2, 3, 6 } );
    builder->create_tetrahedron( { 1, 4, 5, 6 } );
    builder->create_tetrahedron( { 3, 7, 4, 6 } );
    builder->create_tetrahedron( { 1, 4, 6, 3 } );
    builder->compute_polyhedron_adjacencies();
}

void check_solid_tree( const geode::AABBTree3D& tree,
    const geode::DistanceToTetrahedron3D& distance_action )
{
    geode::index_t tetrahedron_box_id;
    double distance;
    std::tie( tetrahedron_box_id, distance ) = tree.closest_element_box(
        geode::Point3D{ { 0, 0, 0 } }, distance_action );
    OPENGEODE_EXCEPTION(
        tetrahedron_box_id == 0, "Wrong tetrahedron id on query 1" );
    OPENGEODE_EXCEPTION(
        inexact_equal( distance, 0., 1e-7 ), "Wrong distance on query 1" );

    std::tie( tetrahedron_box_id, distance ) = tree.closest_element_box(
        geode::Point3D{ { 1, 0, 1 } }, distance_action );
    OPENGEODE_EXCEPTION(
        tetrahedron_box_id == 2, "Wrong tetrahedron id on query 2" );
    OPENGEODE_EXCEPTION(
        inexact_equal( distance, 0., 1e-7 ), "Wrong distance on query 2" );

    std::tie( tetrahedron_box_id, distance ) = tree.closest_element_box(
        geode::Point3D{ { 0.5, 0.5, 0.5 } }, distance_action );
    OPENGEODE_EXCEPTION(
        tetrahedron_box_id == 4, "Wrong tetrahedron id on query 3" );
    OPENGEODE_EXCEPTION(
        inexact_equal( distance, 0., 1e-7 ), "Wrong distance on query 3" );

    std::tie( tetrahedron_box_id, distance ) = tree.closest_element_box(
        geode::Point3D{ { -0.5, 0.5, 0.5 } }, distance_action );
    OPENGEODE_EXCEPTION(
        inexact_equal( distance, 0.5, 1e-7 ), "Wrong distance on query 4" );
}

void test_SolidAABB()
{
    geode::Logger::info( "TEST", " TetrahedralSolid AABB Helper3D" );

    auto t_solid = geode::TetrahedralSolid3D::create();
    build_test_solid( *t_solid );

    const auto aabb_tree = create_aabb_tree( *t_solid );
    geode::DistanceToTetrahedron3D distance_action( *t_solid );

    check_solid_tree( aabb_tree, distance_action );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    test_SolidAABB();
}

OPENGEODE_TEST( "aabb-triangulated-surfacce-helpers" )
